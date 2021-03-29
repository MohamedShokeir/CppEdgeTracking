#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <string>
#include <vector>

class FileParser {
public:
  FileParser();
  FileParser(std::string cameraDir, int Fcol, int Imgcol, char delimiter);

  void ReadCsvFile();
  void WriteOutputFile(std::vector<std::pair<float, float>> &output);

private:
  int _Fcol;
  int _Imgcol;
  char _delimiter;

  std::string _cameraDir;
  //   std::string _outDir;

  std::vector<std::pair<std::string, float>> _images;
};

#endif