#include "ImageProcessing.h"
#include "Parser.h"

#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include <filesystem>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>

// namespace fs = std::filesystem;

using namespace cv;

int main(int argc, char **argv) {

  // Program input file:
  std::string inputFile;
  if (argc > 1) {
    inputFile = argv[1];
  } else {
    throw std::runtime_error("-- Error! No file found");
  }

  //-----------------
  // Program testing:
  //-----------------

  std::shared_ptr<FileParser> parser =
      std::make_shared<FileParser>(inputFile); // Parse input txt file
  parser->ReadCsvFile();                       // Read csv file

  std::vector<std::pair<std::string, float>> images =
      parser->GetImagesVector(); // Get the image-force vector
  std::vector<std::pair<float, float>>
      output; // Instantiate the output stress-deformation vector

  bool show = parser->GetBoolShow(); // Show output image
  bool save = parser->GetBoolSave(); // Save output image

  float area = parser->GetArea(); // Get the sample's cross section area
  int phi0; // Instantiate initial cross section diameter in pixels

  // Iterate on elements of image-force vector
  for (std::vector<std::pair<std::string, float>>::const_iterator iter =
           images.begin();
       iter != images.end(); ++iter) {

    ImageProcessing img(iter->first, iter->second);

    float deformation{0.f};
    float stress{0.f};

    if (iter == images.begin()) {
      phi0 = (float)img.GetMinimumPixelDistance(
          show, save); // Set the reference minimum
                       // cross section diameter
      deformation = 0.f;
    } else {
      deformation =
          (float)abs(img.GetMinimumPixelDistance(show, save) - phi0) / phi0;
      stress = (float)(iter->second) / area;
    }

    output.push_back(
        std::make_pair(deformation, stress)); // Store stress-deformation
  }

  parser->WriteOutputFile(output); // Write output file

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