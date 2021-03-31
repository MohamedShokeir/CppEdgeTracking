#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include "Interpolation.h"
#include "Parser.h"
#include "QueueManager.h"

#include <future>
#include <memory>
#include <mutex>
#include <opencv2/core.hpp>    // Core functionality
#include <opencv2/highgui.hpp> // Image Processing
#include <opencv2/imgproc.hpp> // High-level GUI
#include <thread>

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
  ImageProcessing() {}
  ~ImageProcessing() {}
  ImageProcessing(const std::string &imgName, const float &force);

  ImageProcessing(const ImageProcessing &source); // copy ctr
  ImageProcessing &
  operator=(const ImageProcessing &source);  // copy assignment operator
  ImageProcessing(ImageProcessing &&source); // move ctr
  ImageProcessing &
  operator=(ImageProcessing &&source); // move assignment operator

  // Getters
  Mat GetImage();
  Mat GetBinaryImage();
  std::string GetImageName();
  float *GetMinimumPixelDiameterAndForce(bool show, bool save);

  void ShowImage(Mat &src);
  void MorphologyOperations(int operationCode = 2);
  void GlobalThresholding();
  void ContourDetection();
  template <typename T>
  std::vector<T> LinearInterpolation(const std::vector<T> &vec, std::size_t k);
  void ComputeMinimumDiameter();
  void ComputeMinimumEdges();
  void DrawMinimumDiameter(bool &show, bool &save);

  // template <typename T> T MatToVector(Mat mat);
  // template <typename T> Mat_<T> VectorToMat(std::vector<std::vector<T>>
  // &inVec);
  uchar MatToVector(Mat mat);
  Mat_<uchar> VectorToMat(std::vector<std::vector<uchar>> &inVec);

  std::string TypeToStr(int &type);

protected:
  std::shared_ptr<Mat> _img;
  Mat _binaryImg;

private:
  Point _Lmin;
  Point _Rmin;
  std::vector<Point> _Lpoints;
  std::vector<Point> _Rpoints;
  std::vector<int> _Lxpoints;
  std::vector<int> _Rxpoints;
  std::vector<int> _distances;

  std::string _imgName;

  int _min_distance;

  float _force;
  float diameter_force[2];

  ThresholdType _threshType;

  QueueManager<int> _queue;

  bool _user_closed_window = false;

  std::mutex _mtx;
  std::condition_variable _cond;
};

#endif
