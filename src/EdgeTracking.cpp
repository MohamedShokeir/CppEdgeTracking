#include "FileParser.h"
#include "ImageProcessing.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <deque>
#include <iostream>
using namespace cv;

int main(int argc, char **argv) {

  std::string fileName;
  if (argc > 1)
    fileName = argv[1];
  else
    std::cout << "-- Error! No input image" << std::endl;

  ImageProcessing img(fileName);
  // auto mat = img._img;
  img.MorphologyOperations();
  img.GlobalThresholding();
  auto mat = img.GetBinaryImage();
  img.ContourDetection();
  img.ComputeMinimumDiameter();
  img.DrawMinimumDiameter();
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