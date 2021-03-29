#include "ImageProcessing.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include <cassert>
#include <deque>
#include <iostream>
#include <vector>

typedef uchar Pixel;

ImageProcessing::ImageProcessing(const std::string &imgName) {
  _img = imread(imgName, COLOR_BGR2GRAY);
  if (_img.empty()) {
    std::cout << "-- Error! Could not read the image: " << imgName << std::endl;
  }
};

ImageProcessing::ImageProcessing(const std::string &imgName,
                                 ThresholdType threshType) {
  _img = imread(imgName, COLOR_BGR2GRAY);
  if (_img.empty()) {
    std::cout << "-- Error! Could not read the image: " << imgName << std::endl;
  }
  try {
    _threshType = threshType;
  } catch (...) {
    throw std::invalid_argument(
        "-- Error! Valid threshold types are: Binary, BinaryInv, Trunc, "
        "Tozero, Otsu, Triangle ");
  }
};

Mat ImageProcessing::GetImage() { return _img; }

Mat ImageProcessing::GetBinaryImage() { return _binaryImg; }

void ImageProcessing::ShowImage(Mat &src) {
  imshow("Press any key to exit", src);
  waitKey(0); // Wait for a keystroke in the window
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
  morphologyEx(_img, _img, MORPH_OPEN, element, Point(-1, -1), 2);
}

void ImageProcessing::GlobalThresholding() {
  Mat tmpImg = _img.clone();
  uchar thresh = threshold(_img, tmpImg, 0, 255, THRESH_OTSU);
  std::cout << "-- Computed threshold = " << (int)thresh << std::endl;

  _img.copyTo(
      _binaryImg); // Just to make sure the Mat objects are of the same size.

  _binaryImg.forEach<Pixel>([thresh, this](Pixel &p, const int *position) {
    p = (p > thresh) ? 255 : 0;
    // std::cout << format("[%d,%d]= %d \n", position[0], position[1], (int)p)
    //           << std::endl;
  });
}

void ImageProcessing::ContourDetection() {
  // _contour = Mat::ones(_img.size(), _img.type()) * 255;

  for (int x = 0; x < _binaryImg.rows; x++) {
    Point2d left, right;
    bool b_left, b_right;
    uchar *ptr = _binaryImg.ptr<uchar>(x);

    // iterate rows of the image matrix to find contour
    for (int y = 0; y < _binaryImg.cols - 1; y++) {
      const int current = ptr[y];
      const int next = ptr[y + 1];
      if (current - next < 0) { // detect left contour
        left = Point2d(y, x);
        b_left = true; // left contour detected !

      } else if (current - next > 0) { // detect right contour
        right = Point2d(y, x);
        b_right = true; // right contour detected !
      }
    } // end of one row
    if (b_left && b_right) {
      _distances.push_back(abs(right.x - left.x)); // push back all distances
      _Rpoints.push_back(right); // push back all 2D points of the right contour
      _Lpoints.push_back(left);  // push back all 2D points of the left contour
    }
    b_left = false;
    b_right = false; // reset both booleans
  }                  // end of column
}

void ImageProcessing::ComputeMinimumDiameter() {
  // find minumum diameter
  _min_distance = *std::min_element(_distances.begin(), _distances.end());
  std::cout << "-- Minimum distance in pixels: " << _min_distance << std::endl;
}

void ImageProcessing::DrawMinimumDiameter() {
  Mat tmp; // new image to sketch the contour and miminum diameter

  std::vector<Point2d>::iterator other =
      _Rpoints.begin(); // to iterate on the _Lpoints and _Rpoints vectors
                        // simultaneously

  // draw the minmum diameter
  auto res = std::find_if(
      _Lpoints.begin(), _Lpoints.end(), [this, &tmp, &other](Point2d &point) {
        int local_distance = point.x - (*other).x;

        if (local_distance == _min_distance) {
          cvtColor(_img, tmp, COLOR_GRAY2BGR);
          circle(tmp, point, 15, Scalar(0, 255, 0), -1);  // draw left circle
          circle(tmp, *other, 15, Scalar(0, 255, 0), -1); // draw right circle
          line(tmp, point, *other, Scalar(0, 0, 255), 2,
               LINE_8); // draw horizontal line at the minimum diameter
          imshow("Press any key to exit", tmp);
          waitKey(0); // Wait for a keystroke in the window
          return true;
        }
        other++;
        return false;
      });
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

  printf("Matrix: %s %dx%d \n", r.c_str(), _img.cols, _img.rows);
  return r;
}