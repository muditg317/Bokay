#include <iostream>
#include <vector>
#include <string>
// #include <boost/program_options.h>
#include "bokay-file.hpp"

// using namespace std;
enum EngineCommand {COMPILE};
std::string EngineCommands[] = {"compile"};

int main(int argc, char *argv[]) {
  std::cout << argc << " args:\t";
  for (int i = 0; i < argc; i++) {
    std::cout << argv[i] << '\t';
  }
  std::cout << std::endl;

  if (!EngineCommands[COMPILE].compare(argv[1])) { // trying to compile
    BokayFile inputFile(argv[2]);

    for (int i = 0, size = inputFile.getLineCount(); i < size; i++) {
      std::cout << inputFile.getLine(i) << '\n';
    }
  } else {
    std::cout << "run command: " << argv[1] << std::endl;
  }

}