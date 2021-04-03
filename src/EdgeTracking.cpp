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
  std::vector<std::array<float, 4>>
      output; // Instantiate the output stress-deformation vector

  bool show = parser->GetBoolShow(); // Show output image
  bool save = parser->GetBoolSave(); // Save output image
  float pixelRatio = parser->GetPixelRatio();

  float area = parser->GetArea(); // Get the sample's cross section area
  int phi0; // Instantiate initial cross section diameter in pixels
  float &&deformation{
      0.f};            // Instantiate calculated deformation = (phi-phi0)/phi0
  float &&stress{0.f}; // Instantiate calculated stress = force/area
  float &&LcurvatureRadius{0.f}; // Instantiate left curvature radius
  float &&RcurvatureRadius{0.f}; // Instantiate right curvature radius

  //---------------------
  // Without concurrency:
  //---------------------
  // // Iterate on elements of image-force vector
  // for (std::vector<std::pair<std::string, float>>::const_iterator iter =
  //          images.begin();
  //      iter != images.end(); ++iter) {

  //   ImageProcessing img(iter->first, iter->second);

  //   float deformation{0.f};
  //   float stress{0.f};

  //   if (iter == images.begin()) {
  //     phi0 = img.GetMinimumPixelDiameterAndForce(
  //         show, save)[0]; // Set the reference minimum cross section diameter
  //     deformation = 0.f;
  //   } else {
  //     deformation =
  //         (float)abs(img.GetMinimumPixelDiameterAndForce(show, save)[0] -
  //                    phi0) /
  //         phi0;
  //     stress = (float)(iter->second) / area;
  //   }
  //   output.push_back(
  //       std::make_pair(deformation, stress)); // Store stress-deformation
  // }

  //------------------
  // With concurrency:
  //------------------
  std::vector<std::future<float *>> futures;
  std::shared_ptr<QueueManager<bool>> queue(new QueueManager<bool>);
  std::shared_ptr<std::mutex> mutex(
      new std::mutex); // To lock the screen when cout-ing

  for (std::vector<std::pair<std::string, float>>::const_iterator iter =
           images.begin();
       iter != images.end(); ++iter) {

    ImageProcessing img(iter->first, iter->second, queue,
                        mutex); // Create an instance

    if (iter == images.begin()) {
      if (show) {
        // Send screen_is_free to queue manager to print to screen first image
        bool screen_free = true;
        queue->send(std::move(screen_free));
      }
      auto res = img.Process(
          show, save); // Set the reference minimum cross section diameter
      phi0 = res[0];
      LcurvatureRadius = res[2] * pixelRatio;
      RcurvatureRadius = res[3] * pixelRatio;
      output.emplace_back(
          std::array<float, 4>{0.f, 0.f, LcurvatureRadius,
                               RcurvatureRadius}); // Store stress - deformation
    } else {
      if (show)
        futures.emplace_back(std::async(
            std::launch::deferred, &ImageProcessing::Process, img, show, save));
      else
        futures.emplace_back(std::async(
            std::launch::async, &ImageProcessing::Process, img, show, save));
    }
  }

  std::for_each(futures.begin(), futures.end(), [&](std::future<float *> &ftr) {
    ftr.wait();
    auto res = ftr.get();
    deformation = abs(res[0] - phi0) / phi0;
    stress = (res[1]) / area;
    LcurvatureRadius = res[2] * pixelRatio;
    RcurvatureRadius = res[3] * pixelRatio;

    // Add results of to the ouput vector
    output.emplace_back(
        std::array<float, 4>{deformation, stress, LcurvatureRadius,
                             RcurvatureRadius}); // Store stress - deformation
  });

  parser->WriteOutputFile(output); // Write output file
  //---------------
  // End of program
  //---------------
} // end of main