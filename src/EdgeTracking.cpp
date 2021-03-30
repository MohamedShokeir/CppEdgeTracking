#include "ImageProcessing.h"
#include "Parser.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include <filesystem>
#include <iostream>
#include <utility>

namespace fs = std::__fs::filesystem;

using namespace cv;

namespace fs = std::__fs::filesystem;

int main(int argc, char **argv) {

  // Program input file:
  std::string inputFile;
  // std::string fileName;
  // std::string csvName;
  if (argc > 1) {
    inputFile = argv[1];
    // fileName = argv[1];
    // csvName = argv[2];
  } else {
    throw std::runtime_error("-- Error! No file found");
  }

  // ArgumentParser testing :
  // ArgumentParser input(inputFile);
  // input.ParseInputFile();

  // ImageProcessing testing:
  // ImageProcessing img(fileName);
  // img.MorphologyOperations();
  // img.GlobalThresholding();
  // auto mat = img.GetBinaryImage();
  // img.ContourDetection();
  // img.ComputeMinimumDiameter();
  // img.DrawMinimumDiameter();

  // FileParser testing:
  // FileParser parser(inputFile);
  // parser.ReadCsvFile();
  // std::vector<std::pair<float, float>> dataset{std::make_pair(0., 0.),
  //                                              std::make_pair(100., 0.1)};
  // parser.WriteOutputFile(dataset);

  //-----------------
  // Program testing:
  //-----------------

  // Parse input file:
  FileParser parser(inputFile);
  // Read csv file:
  parser.ReadCsvFile();
  // Get the vector of images
  std::vector<std::pair<std::string, float>> images = parser.GetImagesVector();
  std::vector<std::pair<float, float>> output;

  // Get the sample's area:
  float area = parser.GetArea();

  // Initialize initial diameter in pixels:
  int phi0;

  for (std::vector<std::pair<std::string, float>>::const_iterator iter =
           images.begin();
       iter != images.end(); ++iter) {

    ImageProcessing img(iter->first, iter->second);

    float deformation{0.f};
    float stress{0.f};

    if (iter == images.begin()) {
      phi0 = (float)img.GetMinimumPixelDistance();
      deformation = 0.f;
    } else {
      deformation = (float)abs(img.GetMinimumPixelDistance() - phi0) / phi0;
      stress = (float)(iter->second) / area;
    }
    output.push_back(std::make_pair(deformation, stress));
  }

  parser.WriteOutputFile(output);

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