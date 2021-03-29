#include "FileParser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <utility>

FileParser::FileParser(std::string cameraDir, int Fcol, int Imgcol,
                       char delimiter)
    : _cameraDir(cameraDir), _Fcol(Fcol), _Imgcol(Imgcol),
      _delimiter(delimiter) {}

void FileParser::ReadCsvFile() {

  std::string line;
  std::string img_name;
  float force{0.f};

  std::ifstream filestream(_cameraDir);

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
          //   std::cout << "Image force: " << line << std::endl;
        }
        if (colIdx == _Imgcol) {
          img_name = line;
          //   std::cout << "Image name: " << line << std::endl;
        }
        ++colIdx;
      }
      _images.push_back(std::make_pair(img_name, force));
    }
    filestream.close(); // Close file
  }
  for (std::vector<std::pair<std::string, float>>::const_iterator iter =
           _images.begin();
       iter != _images.end(); ++iter) {
    std::cout << "First: " << iter->first << ", Second: " << iter->second
              << std::endl;
  }
}

void FileParser::WriteOutputFile(std::vector<std::pair<float, float>> &output) {

  const char *dirname = "ET";
  int outDir = mkdir(dirname, 0777);

  if (!outDir) // check if directory is created or not
    std::cout << "-- Results directory created" << std::endl;

  std::string filename = "/out.res";
  std::ofstream outFile(dirname + filename); // Create output filestream object

  outFile << "1-Phi/Phi0(mm/mm)\t2-F/S0(MPa)\n"; // Write header

  outFile << std::fixed; // set fixed precisioin

  std::for_each(output.begin(), output.end(),
                [&outFile](std::pair<float, float> &i) {
                  outFile << i.first << "\t" << i.second << "\n";
                });

  //   for (int i = 0; i < output.size(); ++i) {
  //     outFile << output.at(i).first << "\t" << output.at(i).second << "\n";
  //   }
  outFile.close(); // Close the file
}
