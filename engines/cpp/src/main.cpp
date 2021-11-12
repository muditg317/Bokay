#include <iostream>
#include <vector>
#include <string>
// #include <boost/program_options.h>
#include "bokay-file.hpp"

// using namespace std;

int main(int argc, char *argv[]) {
  std::cout << argc << " args:\t";
  for (int i = 0; i < argc; i++) {
    std::cout << argv[i] << '\t';
  }
  std::cout << std::endl;

  BokayFile inputFile(argv[1]);

  for (int i = 0, size = inputFile.getLineCount(); i < size; i++) {
    std::cout << inputFile.getLine(i) << '\n';
  }
}