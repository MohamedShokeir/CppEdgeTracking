#include "ImageProcessing.h"
#include "Parser.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

using namespace cv;

int main(int argc, char **argv) {

  // Program input file:
  std::string inputFile;
  if (argc > 1) {
    inputFile = argv[1];
  } else {
    throw std::runtime_error("-- Error! No file found");
  }

  //---------------
  // Start program:
  //---------------
  std::shared_ptr<FileParser> parser =
      std::make_shared<FileParser>(inputFile); // Parse input txt file
  parser->ReadCsvFile();                       // Read csv file

  std::vector<std::pair<std::string, float>> images =
      parser->GetImagesVector(); // Get the image-force vector
  std::vector<std::pair<float, float>>
      output; // Instantiate the output stress-deformation vector

  bool show = parser->GetBoolShow(); // Show output image
  bool save = parser->GetBoolSave(); // Save output image

  float area = parser->GetArea(); // Get the sample's cross section area
  int phi0; // Instantiate initial cross section diameter in pixels
  float deformation{
      0.f};          // Instantiate calculated deformation = (phi-phi0)/phi0
  float stress{0.f}; // Instantiate calculated stress = force/area

  //---------------------
  // Without concurrency:
  //---------------------
  // Iterate on elements of image-force vector
  // for (std::vector<std::pair<std::string, float>>::const_iterator iter =
  //          images.begin();
  //      iter != images.end(); ++iter) {

  //   ImageProcessing img(iter->first, iter->second);

  //   float deformation{0.f};
  //   float stress{0.f};

  //   if (iter == images.begin()) {
  //     phi0 = (float)img.GetMinimumPixelDistance(
  //         show, save); // Set the reference minimum
  //                      // cross section diameter
  //     deformation = 0.f;
  //   } else {
  //     deformation =
  //         (float)abs(img.GetMinimumPixelDistance(show, save) - phi0) / phi0;
  //     stress = (float)(iter->second) / area;
  //   }

  //   output.push_back(
  //       std::make_pair(deformation, stress)); // Store stress-deformation
  // }

  //------------------
  // With concurrency:
  //------------------
  std::vector<std::future<float *>> futures;

  for (std::vector<std::pair<std::string, float>>::const_iterator iter =
           images.begin();
       iter != images.end(); ++iter) {

    ImageProcessing img(iter->first, iter->second); // Create an instance

    if (iter == images.begin()) {
      phi0 = img.GetMinimumPixelDiameterAndForce(
          show, save)[0]; // Set the reference minimum cross section diameter
      output.push_back(std::make_pair(0.f, 0.f)); // Store stress-deformation
    } else {
      futures.emplace_back(std::async(
          std::launch::async, &ImageProcessing::GetMinimumPixelDiameterAndForce,
          img, show, save));
    }
  }

  std::for_each(futures.begin(), futures.end(), [&](std::future<float *> &ftr) {
    ftr.wait();
    auto res = ftr.get();
    deformation = abs(res[0] - phi0) / phi0;
    stress = (res[1]) / area;

    output.push_back(
        std::make_pair(deformation, stress)); // Store stress - deformation
  });

  parser->WriteOutputFile(output); // Write output file

  //---------------
  // End of program
  //---------------
} // end of main