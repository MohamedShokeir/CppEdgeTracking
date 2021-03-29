#include "FileParser.h"
#include "ImageProcessing.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include <deque>
#include <iostream>
#include <utility>

using namespace cv;

int main(int argc, char **argv) {

  std::string fileName;
  std::string csvName;
  if (argc > 1) {
    fileName = argv[1];
    csvName = argv[2];
  } else {
    std::cout << "-- Error! No input image" << std::endl;
    exit(1);
  }

  // ImageProcessing testin:
  // ImageProcessing img(fileName);
  // img.MorphologyOperations();
  // img.GlobalThresholding();
  // auto mat = img.GetBinaryImage();
  // img.ContourDetection();
  // img.ComputeMinimumDiameter();
  // img.DrawMinimumDiameter();

  // FileParser Testing
  FileParser parser(csvName, 3, 1, ',');
  parser.ReadCsvFile();
  std::vector<std::pair<float, float>> dataset{std::make_pair(0., 0.),
                                               std::make_pair(100., 0.1)};
  parser.WriteOutputFile(dataset);

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