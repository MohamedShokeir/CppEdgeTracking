#ifndef PARSER_H
#define PARSER_H

#include <array>
#include <string>
#include <vector>

class ArgumentParser {
public:
  ArgumentParser() {}
  ArgumentParser(std::string inputFile);

  // Getter
  bool GetBoolShow();
  bool GetBoolSave();
  float GetPixelRatio();

  void ParseInputFile();

protected:
  std::string _name;
  std::string _cameraDir;
  int _Fcol;
  int _Imgcol;
  float _diameter;
  float _pixelRatio;
  char _delimiter;

private:
  bool _show;
  bool _save;

  std::string _inputFile;
};

class FileParser : public ArgumentParser {
public:
  FileParser(std::string inputFile);

  // Getters
  float GetArea();
  std::vector<std::pair<std::string, float>> GetImagesVector();

  void ReadCsvFile();
  void WriteOutputFile(std::vector<std::array<float, 4>> &output);

private:
  std::string _csvFile;
  std::string _outFile;
  std::vector<std::pair<std::string, float>> _images;
};

#endif