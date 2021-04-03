#include "Parser.h"
#include "ImageProcessing.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#define PI 3.14159265359

ArgumentParser::ArgumentParser(std::string inputFile) : _inputFile(inputFile) {
  const char *dirname = "ET/";
  int outDir = mkdir(dirname, 0777);

  if (!outDir) // check if directory is created or not
    std::cout << "-- Results directory created" << std::endl;

  std::ofstream outFile(dirname);
}

bool ArgumentParser::GetBoolShow() { return _show; }
bool ArgumentParser::GetBoolSave() { return _save; }
float ArgumentParser::GetPixelRatio() { return _pixelRatio; }

void ArgumentParser::ParseInputFile() {
  std::string line;
  std::ifstream filestream(_inputFile);
  std::string key;
  std::string value;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) { // Read data, line by line
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Name")
          _name = value;
        else if (key == "Camera_directory")
          _cameraDir = "./" + value;
        else if (key == "Diameter")
          _diameter = std::stof(value);
        else if (key == "Force")
          _Fcol = std::stof(value);
        else if (key == "Image")
          _Imgcol = std::stof(value);
        else if (key == "Delimiter")
          _delimiter = value[0];
        else if (key == "Show")
          _show = (value == "true");
        else if (key == "Save")
          _save = (value == "true");
        else if (key == "Pixel_mm_ratio")
          _pixelRatio = std::stof(value);
      }
    }
  }
  filestream.close(); // Close file
}

FileParser::FileParser(std::string inputFile) : ArgumentParser{inputFile} {
  ParseInputFile();
  _csvFile = _cameraDir + "/" + _name + ".csv";
  _outFile = _name + ".res";
}

float FileParser::GetArea() { return PI * _diameter * _diameter / 4; }

std::vector<std::pair<std::string, float>> FileParser::GetImagesVector() {
  return _images;
}

void FileParser::ReadCsvFile() {

  std::string line;
  std::string img_name;
  float force{0.f};

  std::ifstream filestream(_csvFile);

  if (!filestream.is_open()) // Make sure the file is open
    throw std::runtime_error("-- Error! Could not open file");
  else if (filestream.is_open()) {
    if (filestream.good()) {          // Make sure the file can be read
      std::getline(filestream, line); // Extract the header of the file
      std::stringstream ss(line);
    }
    while (std::getline(filestream, line)) { // Read data, line by line
      std::stringstream ss(line);
      int colIdx = 0;

      while (std::getline(ss, line, _delimiter)) { // Get columns
        if (colIdx == _Fcol) {
          force = std::stof(line);
        }
        if (colIdx == _Imgcol) {
          img_name = line;
        }
        ++colIdx;
      }
      _images.push_back(std::make_pair(img_name, force));
    }
    filestream.close(); // Close file
  }
  // for (std::vector<std::pair<std::string, float>>::const_iterator iter =
  //          _images.begin();
  //      iter != _images.end(); ++iter) {
  //   std::cout << "First: " << iter->first << ", Second: " << iter->second
  //             << std::endl;
}

void FileParser::WriteOutputFile(std::vector<std::array<float, 4>> &output) {

  const char *dirname = "ET/";
  // int outDir = mkdir(dirname, 0777);

  // if (!outDir) // check if directory is created or not
  //   std::cout << "-- Results directory created" << std::endl;

  std::ofstream outFile(dirname + _outFile); // Create output filestream object

  outFile << "#\t1-phi/phi0(mm/mm)\t2-F/S0(MPa)\t3-Curvature radius "
             "left(mm)\t4-Curvature radius right(mm)\n"; // Write header

  outFile << std::fixed; // set fixed precisioin
  std::for_each(
      output.begin(), output.end(), [&outFile](std::array<float, 4> &a) {
        outFile << a[0] << "\t" << a[1] << "\t" << a[2] << "\t" << a[3] << "\n";
      });

  outFile.close(); // Close the file
}
