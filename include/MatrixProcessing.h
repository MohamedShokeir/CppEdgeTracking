#ifndef MATRIXPROCESSING_H
#define MATRIXPROCESSING_H

#include "ImageProcessing.h"

#include <opencv2/core.hpp>    // Core functionality
#include <opencv2/highgui.hpp> // Image Processing
#include <opencv2/imgproc.hpp> // High-level GUI

using namespace cv;

class Matrix : public ImageProcessing {
  Matrix();
};

#endif