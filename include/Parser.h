#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <vector>

class ArgumentParser {
public:
  ArgumentParser() {}
  ArgumentParser(std::string inputFile);

  // Setter:
  // void SetLocalPath();

  void ParseInputFile();

protected:
  std::string _name;
  std::string _cameraDir;
  int _Fcol;
  int _Imgcol;
  float _diameter;
  char _delimiter;

private:
  std::string _inputFile;
};

class FileParser : public ArgumentParser {
public:
  FileParser(std::string inputFile);

  // Getters
  float GetArea();
  std::vector<std::pair<std::string, float>> GetImagesVector();

  void ReadCsvFile();
  void WriteOutputFile(std::vector<std::pair<float, float>> &output);

private:
  std::string _csvFile;
  std::string _outFile;
  std::vector<std::pair<std::string, float>> _images;
};

#endif