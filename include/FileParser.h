#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <vector>

class FileParser {
public:
  void ReadFile();

private:
  std::vector<std::string> _images;
};

#endif