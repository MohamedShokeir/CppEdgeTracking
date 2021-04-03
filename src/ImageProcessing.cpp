#include "ImageProcessing.h"
// #include "Parser.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace cv;

typedef uchar Pixel;

template <typename T> T QueueManager<T>::receive() {
  std::unique_lock<std::mutex> lck(_mtx);
  _condition.wait(lck, [this] {
    return !_queue.empty();
  }); // pass unique lock to condition variable

  // remove last vector element from queue
  T msg = std::move(_queue.back());
  _queue.pop_back();

  return msg; // will not be copied due to return value optimization (RVO) in
              // C++
}

template <typename T> void QueueManager<T>::send(T &&img) {
  std::lock_guard<std::mutex> lck(_mtx);
  _queue.push_back(std::move(img));
  _condition
      .notify_one(); // notify client after pushing new Vehicle into vector
}

// Constructor
ImageProcessing::ImageProcessing(const std::string &imgName, const float &force,
                                 std::shared_ptr<QueueManager<bool>> queue,
                                 std::shared_ptr<std::mutex> mutex)
    : _imgName(imgName), _force(force), _queue(queue), _mtx(mutex) {
  _imgName.erase(std::remove(_imgName.begin(), _imgName.end(), '/'),
                 _imgName.end());
  _imgName.erase(std::remove(_imgName.begin(), _imgName.end(), '.'),
                 _imgName.end());
  _img = std::make_shared<Mat>(imread(imgName, COLOR_BGR2GRAY));
  if (_img->empty()) {
    std::cout << "-- Error! Could not read the image: " << imgName << std::endl;
  }
};

// Copy Constructor
ImageProcessing::ImageProcessing(const ImageProcessing &source) {
  _img = source._img;
  _mtx = source._mtx;
  _force = source._force;
  _Lmin = source._Lmin;
  _Rmin = source._Rmin;
  _queue = source._queue;
  _Lpoints = source._Lpoints;
  _Lxpoints = source._Lxpoints;
  _Rpoints = source._Rpoints;
  _Rxpoints = source._Rxpoints;
  _imgName = source._imgName;
  _distances = source._distances;
  _binaryImg = source._binaryImg;
  _min_distance = source._min_distance;
  *diameter_force_curvatureRadius = *source.diameter_force_curvatureRadius;

  // std::cout << "Copy Constructor " << std::endl;
}

// Copy Assignment Operator
ImageProcessing &ImageProcessing::operator=(const ImageProcessing &source) {
  if (this == &source)
    return *this;

  _img = source._img;
  _mtx = source._mtx;
  _force = source._force;
  _Lmin = source._Lmin;
  _Rmin = source._Rmin;
  _queue = source._queue;
  _Lpoints = source._Lpoints;
  _Lxpoints = source._Lxpoints;
  _Rpoints = source._Rpoints;
  _Rxpoints = source._Rxpoints;
  _imgName = source._imgName;
  _distances = source._distances;
  _binaryImg = source._binaryImg;
  _min_distance = source._min_distance;
  *diameter_force_curvatureRadius = *source.diameter_force_curvatureRadius;

  // std::cout << "Copy Assignement Operator" << std::endl;
  return *this;
}

// Move Constructor
ImageProcessing::ImageProcessing(ImageProcessing &&source) {
  _img = source._img;
  _mtx = source._mtx;
  _force = source._force;
  _Lmin = source._Lmin;
  _Rmin = source._Rmin;
  _queue = source._queue;
  _Lpoints = source._Lpoints;
  _Lxpoints = source._Lxpoints;
  _Rpoints = source._Rpoints;
  _Rxpoints = source._Rxpoints;
  _imgName = source._imgName;
  _distances = source._distances;
  _binaryImg = source._binaryImg;
  _min_distance = source._min_distance;
  *diameter_force_curvatureRadius = *source.diameter_force_curvatureRadius;

  // std::cout << "Move Constructor " << std::endl;
}

// Move Assignment Operator
ImageProcessing &ImageProcessing::operator=(ImageProcessing &&source) {
  if (this == &source)
    return *this;

  _img = source._img;
  _mtx = source._mtx;
  _force = source._force;
  _Lmin = source._Lmin;
  _Rmin = source._Rmin;
  _queue = source._queue;
  _Lpoints = source._Lpoints;
  _Lxpoints = source._Lxpoints;
  _Rpoints = source._Rpoints;
  _Rxpoints = source._Rxpoints;
  _imgName = source._imgName;
  _distances = source._distances;
  _binaryImg = source._binaryImg;
  _min_distance = source._min_distance;
  *diameter_force_curvatureRadius = *source.diameter_force_curvatureRadius;

  _img = nullptr;

  // std::cout << "Move Assignement Operator" << std::endl;
  return *this;
}

Mat ImageProcessing::GetImage() { return *_img; }

Mat ImageProcessing::GetBinaryImage() { return _binaryImg; }

std::string ImageProcessing::GetImageName() { return _imgName; }

void ImageProcessing::ShowImage(Mat &src) {
  std::unique_lock<std::mutex> lck(*_mtx);
  imshow("Press any key to exit", src);
  waitKey(0); // Wait for a keystroke in the window
  destroyAllWindows();
  lck.unlock();
}

void ImageProcessing::MorphologyOperations(int operationCode) {
  /*
  Opening: MORPH_OPEN : 2
  Closing: MORPH_CLOSE: 3
  Gradient: MORPH_GRADIENT: 4
  Top Hat: MORPH_TOPHAT: 5
  Black Hat: MORPH_BLACKHAT: 6
  */
  const int morph_size = 2;
  Mat element = getStructuringElement(
      MORPH_RECT, Size(2 * morph_size + 1, 2 * morph_size + 1),
      Point(morph_size, morph_size));
  // Opening
  morphologyEx(*_img, *_img, MORPH_OPEN, element, Point(-1, -1), 2);
}

void ImageProcessing::GlobalThresholding() {
  Mat *tmpImg = new Mat;
  uchar thresh = threshold(*_img, *tmpImg, 0, 255, THRESH_OTSU);
  delete tmpImg;
  // std::lock_guard<std::mutex> lck(*_mtx);
  std::cout << "-- Computed threshold = " << (int)thresh << std::endl;
  // lck.unlock();

  _img->copyTo(
      _binaryImg); // Just to make sure the Mat objects are of the same size.

  _binaryImg.forEach<Pixel>([thresh, this](Pixel &p, const int *position) {
    p = (p > thresh) ? 255 : 0;
    // std::cout << format("[%d,%d]= %d \n", position[0], position[1], (int)p)
    //           << std::endl;
  });
}

void ImageProcessing::ContourDetection() {
  for (int x = 0; x < _binaryImg.rows; x++) {
    Point2d left, right;
    bool b_left, b_right;
    uchar *ptr = _binaryImg.ptr<uchar>(x);

    // iterate rows of the image matrix to find contour
    for (int y = 0; y < _binaryImg.cols - 1; y++) {
      const int current = ptr[y];
      const int next = ptr[y + 1];
      if (current - next < 0) { // detect left contour
        right = Point2d(y, x);
        b_right = true; // left contour detected !

      } else if (current - next > 0) { // detect right contour
        left = Point2d(y, x);
        b_left = true; // right contour detected !
      }
    } // end of one row
    if (b_left && b_right) {
      _Rpoints.push_back(right); // push back all 2D points of the right contour
      _Rxpoints.push_back(right.x);
      _Rypoints.push_back(right.y);
      _Lpoints.push_back(left); // push back all 2D points of the left contour
      _Lxpoints.push_back(left.x);
      _Lypoints.push_back(left.y);
      _distances.push_back(abs(right.x - left.x)); // push back all distances
    }
    b_left = false;
    b_right = false; // reset both booleans
  }                  // end of column
}

void ImageProcessing::ComputeMinimumDiameter() {
  // Calculate minimum diameter:
  _min_distance = *std::min_element(_distances.begin(), _distances.end());
  // std::lock_guard<std::mutex> lck(*_mtx); // Lock the terminal for printing
  std::cout << "-- Minimum distance in pixels: " << _min_distance << '\n'
            << std::endl;
}

void ImageProcessing::ComputeMinimumEdges() {
  std::vector<Point>::iterator other =
      _Rpoints.begin(); // to iterate on the _Lpoints and _Rpoints vectors
                        // simultaneously

  // draw the minmum diameter
  auto res = std::find_if(_Lpoints.begin(), _Lpoints.end(),
                          [this, &other](Point &point) {
                            auto local_distance = abs(point.x - (*other).x);
                            if (local_distance == _min_distance) {
                              _Lmin = point;
                              _Rmin = *other;
                              return true;
                            }
                            other++;
                            return false;
                          });
}

void ImageProcessing::DrawResults() {
  cvtColor(*_img, *_img, COLOR_GRAY2BGR); // Transform image to a rgb image
  polylines(*_img, _Lpoints, false, Scalar(0, 0, 255), 10, LINE_AA,
            0); // draw leftcontour
  polylines(*_img, _Rpoints, false, Scalar(0, 0, 255), 10, LINE_AA,
            0); // draw right contour
  line(*_img, _Lmin, _Rmin, Scalar(0, 0, 255), 10,
       LINE_8); // draw horizontal line at the minimum diameter

  // Fit circle on left and right detected arcs
  Point2f center1(0., 0.);
  Point2f center2(0., 0.);
  approxPolyDP(_Rpoints, _Rpoints, 3, true);
  approxPolyDP(_Lpoints, _Lpoints, 3, true);

  minEnclosingCircle(_Rpoints, center1, _RCurvatureRadius);
  minEnclosingCircle(_Rpoints, center2, _LCurvatureRadius);

  // std::ostringstream ss1;
  // ss1 << _RCurvatureRadius * 0.00444;
  // std::string stringRradius(ss1.str());

  // std::ostringstream ss2;
  // ss2 << "Radius = ";
  // ss2 << _LCurvatureRadius * 0.00444 << "mm";
  // std::string stringLradius(ss2.str());

  Point2f Rcenter(_Rmin.x + _RCurvatureRadius, _Rmin.y);
  Point2f Lcenter(_Lmin.x - _LCurvatureRadius, _Lmin.y);
  circle(*_img, Lcenter, _LCurvatureRadius, Scalar(255, 255, 255),
         2); // Draw the circle that fits the left contour
  // putText(*tmp, stringRradius, _Rmin, FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0,
  // 0), 2, LINE_AA);
  circle(*_img, Rcenter, _RCurvatureRadius, Scalar(255, 255, 255),
         2); // Draw the circle that fits the right contour
  // putText(*tmp, stringLradius, _Lmin, FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 0,
  // 0), 2, LINE_AA);

  circle(*_img, _Lmin, 18, Scalar(0, 255, 0),
         -1); // draw a point on the left of the horizontal line at the minimum
              // diameter
  circle(*_img, _Rmin, 18, Scalar(0, 255, 0),
         -1); // draw a point on the right of the horizontal line at the minimum
              // diameter
}

void ImageProcessing::WaitForScreen() {
  while (true) {
    bool ready = ImageProcessing::_queue->receive();
    if (ready)
      return;
  }
}

float *ImageProcessing::Process(const bool show, const bool save) {
  MorphologyOperations();
  std::unique_lock<std::mutex> lck(*_mtx); // to lock the screen for cout-ing
  GlobalThresholding();
  ContourDetection();
  ComputeMinimumDiameter();
  lck.unlock();
  ComputeMinimumEdges();
  DrawResults();

  if (show) {
    WaitForScreen();
    namedWindow(GetImageName(), WINDOW_AUTOSIZE);
    imshow(_imgName, *_img);
    waitKey(0); // Wait for a keystroke in the window
    waitKey(1);
    destroyAllWindows();
  }

  bool _screen_free = true;
  _queue->send(std::move(_screen_free));

  if (save) {
    // std::unique_lock<std::mutex> lck(*_mtx); // Lock the terminal for
    // printing
    const std::string out = "ET/" + _imgName + ".png";
    imwrite(out, *_img); // Save the frame into a file
    // lck.unlock();
  }

  diameter_force_curvatureRadius[0] = (float)_min_distance;
  diameter_force_curvatureRadius[1] = (float)_force;
  diameter_force_curvatureRadius[2] = (float)_LCurvatureRadius;
  diameter_force_curvatureRadius[3] = (float)_RCurvatureRadius;
  return diameter_force_curvatureRadius;
}

/*template <typename T> T ImageProcessing::MatToVector(Mat mat) {
  T **array = new T *[mat.rows];
  for (int i = 0; i < mat.rows; ++i)
    array[i] = new T[mat.cols * mat.channels()];

  for (int i = 0; i < mat.rows; ++i)
    array[i] = mat.ptr<T>(i);
}*/

uchar ImageProcessing::MatToVector(Mat mat) {
  uchar **array = new uchar *[mat.rows];
  for (int i = 0; i < mat.rows; ++i)
    array[i] = new uchar[mat.cols * mat.channels()];

  for (int i = 0; i < mat.rows; ++i)
    array[i] = mat.ptr<uchar>(i);

  std::cout << (int)*array[0] << std::endl;

  return **array;
}

/*template <typename T>
Mat_<T> ImageProcessing::VectorToMat(std::vector<std::vector<T>> &inVec) {
  int rows = static_cast<int>(inVec.size());
  int cols = static_cast<int>(inVec[0].size());

  Mat_<T> resmat(rows, cols);
  for (int i = 0; i < rows; i++) {
    resmat.row(i) = Mat(inVec[i]).t();
  }
  return resmat;
}*/

Mat_<uchar>
ImageProcessing::VectorToMat(std::vector<std::vector<uchar>> &inVec) {
  int rows = static_cast<int>(inVec.size());
  int cols = static_cast<int>(inVec[0].size());

  Mat_<uchar> resmat(rows, cols);
  for (int i = 0; i < rows; i++) {
    resmat.row(i) = Mat(inVec[i]).t();
  }
  return resmat;
}

// TypeToStr returns the type of the input image and the size of its matrix
std::string ImageProcessing::TypeToStr(int &type) {
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch (depth) {
  case CV_8U:
    r = "8U";
    break;
  case CV_8S:
    r = "8S";
    break;
  case CV_16U:
    r = "16U";
    break;
  case CV_16S:
    r = "16S";
    break;
  case CV_32S:
    r = "32S";
    break;
  case CV_32F:
    r = "32F";
    break;
  case CV_64F:
    r = "64F";
    break;
  default:
    r = "User";
    break;
  }

  r += "C";
  r += (chans + '0');

  printf("Matrix: %s %dx%d \n", r.c_str(), _img->cols, _img->rows);
  return r;
}
