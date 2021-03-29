#include "opencv2/core/utility.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

int main() {
  std::string image_path = "img/J47.tif";
  Mat img = imread(image_path, IMREAD_COLOR);
  if (img.empty()) {
    std::cout << "Could not read the image: " << image_path << std::endl;
    return 1;
  }
  imshow("Display window", img);
  int k = waitKey(0); // Wait for a keystroke in the window
  if (k == 's') {
    imwrite("img/J47test.tif", img);
  }
  return 0;
}