#include "ImageProcessing.h"
#include "Parser.h"
#include "QueueManager.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

// namespace fs = std::filesystem;

using namespace cv;

class Vehicle {
public:
  Vehicle(float id) : _id(id) {}
  float getID() { return _id; }

private:
  float _id;
};

int main(int argc, char **argv) {

  // Program input file:
  std::string inputFile;
  if (argc > 1) {
    inputFile = argv[1];
  } else {
    throw std::runtime_error("-- Error! No file found");
  }

  //-----------------
  // Program testing:
  //-----------------

  unsigned int nCores = std::thread::hardware_concurrency();
  std::cout << "---------------------------------------------" << std::endl;
  std::cout << "This machine supports concurrency with " << nCores
            << " cores available" << std::endl;
  std::cout << "---------------------------------------------" << std::endl;

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

  // Without concurrency

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

  // Concurrency

  // create threads
  // std::vector<std::thread> threads;

  std::vector<std::future<std::shared_ptr<float> *>> futures;

  for (std::vector<std::pair<std::string, float>>::const_iterator iter =
           images.begin();
       iter != images.end(); ++iter) {

    ImageProcessing img(iter->first, iter->second);

    // create new thread from a Lambda
    // threads.emplace_back([iter]() {
    //   // perform work
    //   std::cout << "Hello from Worker thread #" << iter->first << std::endl;
    // });

    // Vehicle v(iter->second);
    // futures.emplace_back(std::async(std::launch::async, &Vehicle::getID, v));

    if (iter == images.begin()) {
      phi0 = *img.GetMinimumPixelDiameterAndForce(
          std::move(show), std::move(save))[0];   // Set the reference minimum
                                                  // cross section diameter
      output.push_back(std::make_pair(0.f, 0.f)); // Store stress-deformation
    } else {
      futures.emplace_back(std::async(
          std::launch::async, &ImageProcessing::GetMinimumPixelDiameterAndForce,
          img, std::move(show), std::move(save)));
    }
  }

  // // call join on all thread objects using a range-based loop
  // for (auto &t : threads)
  //   t.join();
  std::for_each(futures.begin(), futures.end(),
                [&](std::future<std::shared_ptr<float> *> &ftr) {
                  ftr.wait();
                  auto res = ftr.get();
                  // std::cout << "future result: " << *res[0] << " " << *res[1]
                  //           << std::endl;
                  deformation = abs(*res[0] - phi0) / phi0;
                  stress = (*res[1]) / area;

                  output.push_back(std::make_pair(
                      deformation, stress)); // Store stress-deformation
                });

  parser->WriteOutputFile(output); // Write output file

  //---------------
  // End of program
  //---------------
} // end of main

/*
  // Compute distance to first black pixel, starting from the center of the
  image

   mat.setTo(128, mat == 0);

    mat.at<uchar>(mat.rows / 2, mat.cols / 2) = 0; // Set center point to
    zero

    Mat dist;
    distanceTransform(mat, dist, DIST_L2,
                      0); // Can be tweaked for desired accuracy

    Scalar val = mean(dist, mat == 255);
    double mean = val[0];

    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    minMaxLoc(dist, &minVal, &maxVal, &minLoc, &maxLoc);
    std::cout << minVal << ' ' << maxVal << ' ' << ' ' << minLoc << ' '
    << maxLoc;
  */