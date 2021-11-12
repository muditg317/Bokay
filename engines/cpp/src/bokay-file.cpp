#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>
#include "bokay-file.hpp"

BokayFile::BokayFile(std::string filePath) {
  std::string line;
  std::ifstream myfile(filePath);
  if (myfile.is_open()) {
    while (std::getline(myfile,line)) {
      lines.push_back(line);
    }
    myfile.close();
  } else {
    std::cout << "Unable to open file " << filePath << std::endl;
    throw new std::runtime_error("Unable to open file ");
  }
}

int BokayFile::getLineCount() {
  return lines.size();
}

std::string BokayFile::getLine(int i) {
  return lines.at(i);
}

std::vector<std::string> BokayFile::getLines() {
  return lines;
}