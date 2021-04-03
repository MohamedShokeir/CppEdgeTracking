#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

// #include "Interpolation.h"
// #include "Parser.h"

#include <deque>
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

template <class T> class QueueManager {
public:
  T receive();
  void send(T &&msg);

private:
  std::deque<T> _queue;
  std::condition_variable _condition;
  std::mutex _mtx;
};

class ImageProcessing {
public:
  ImageProcessing() {}
  ~ImageProcessing() {}
  ImageProcessing(const std::string &imgName, const float &force,
                  std::shared_ptr<QueueManager<bool>> queue,
                  std::shared_ptr<std::mutex> mutex);

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

  // Methods
  void ShowImage(Mat &src);
  void MorphologyOperations(int operationCode = 2);
  void GlobalThresholding();
  void ContourDetection();
  void ComputeMinimumDiameter();
  void ComputeMinimumEdges();
  void DrawResults();
  void WaitForScreen();

  // template <typename T> T MatToVector(Mat mat);
  // template <typename T> Mat_<T> VectorToMat(std::vector<std::vector<T>>
  // &inVec);
  uchar MatToVector(Mat mat);
  Mat_<uchar> VectorToMat(std::vector<std::vector<uchar>> &inVec);

  // Main method
  float *Process(const bool show, const bool save);

  std::string TypeToStr(int &type);

  // static QueueManager<bool> _queue;
  std::shared_ptr<QueueManager<bool>> _queue;

protected:
  std::shared_ptr<Mat> _img;
  Mat _binaryImg;

private:
  Point2d _Lmin;
  Point2d _Rmin;

  std::vector<Point> _Lpoints;
  std::vector<Point> _Rpoints;
  std::vector<double> _Lxpoints;
  std::vector<double> _Lypoints;
  std::vector<double> _Rxpoints;
  std::vector<double> _Rypoints;
  std::vector<int> _distances;

  std::string _imgName;

  int _min_distance;
  bool _screen_busy;
  float _force;
  float _LCurvatureRadius;
  float _RCurvatureRadius;
  float diameter_force_curvatureRadius[4];

  ThresholdType _threshType;

  std::shared_ptr<std::mutex> _mtx;
};

#endif
