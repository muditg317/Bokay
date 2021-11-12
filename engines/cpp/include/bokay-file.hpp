#ifndef BOKAY_FILE_HPP
#define BOKAY_FILE_HPP
#include <string>
#include <fstream>
#include <vector>
#include <exception>

class BokayFile {
  public:
    BokayFile(std::string filePath);
    int getLineCount();
    std::vector<std::string> getLines();
    std::string getLine(int i);
  private:
    std::vector<std::string> lines;
};

#endif