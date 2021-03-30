#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <deque>
#include <memory>
#include <opencv2/core.hpp>    // Core functionality
#include <opencv2/highgui.hpp> // Image Processing
#include <opencv2/imgproc.hpp> // High-level GUI

using namespace cv;

enum ThresholdType {
  Binary = THRESH_BINARY,
  BinaryInv = THRESH_BINARY_INV,
  Trunc = THRESH_TRUNC,
  Tozero = THRESH_TOZERO,
  Otsu = THRESH_OTSU,
  Triangle = THRESH_TRIANGLE
};

class ImageProcessing {
public:
  ImageProcessing();
  ImageProcessing(const std::string &imgName, const float &force);
  ImageProcessing(const std::string &imgName, ThresholdType threshType);

  // Getters
  Mat GetImage();
  Mat GetBinaryImage();
  int GetMinimumPixelDistance();

  void ShowImage(Mat &src);
  void MorphologyOperations(int operationCode = 2);
  void GlobalThresholding();
  void ContourDetection();
  void ComputeMinimumDiameter();
  void DrawMinimumDiameter();

  // template <typename T> T MatToVector(Mat mat);
  // template <typename T> Mat_<T> VectorToMat(std::vector<std::vector<T>>
  // &inVec);
  uchar MatToVector(Mat mat);
  Mat_<uchar> VectorToMat(std::vector<std::vector<uchar>> &inVec);

  std::string TypeToStr(int &type);

protected:
  Mat _img;
  Mat _binaryImg;

private:
  std::vector<Point2d> _Lpoints;
  std::vector<Point2d> _Rpoints;
  std::vector<int> _distances;

  std::string _imgName;

  int _min_distance;

  float _force;

  ThresholdType _threshType;
};

#endif
