#include "args-parser.hpp"
#include "compiler.hpp"

#include <exception>
#include <iostream>
#include <iomanip>

// #define GLOG_CUSTOM_PREFIX_SUPPORT
#include <glog/logging.h>
// potential missing log detection regex: (?<!LOG.*)<<(?!.*<<)

/* This function writes a prefix that matches glog's default format.
 * (The third parameter can be used to receive user-supplied data, and is
 * NULL by default.)
 */
void CustomPrefix(std::ostream &s, const LogMessageInfo &l, void* userData) {
  bool testing = *((bool *) userData);
  // if (testing) {
  //   return;
  // }
  s 
   << "["
   << std::setfill(' ') << std::setw(16) << l.filename;
  if (!testing) {
    s
     << ':'
     << std::setw(4) << l.line_number;
  }
  s
   << "] "
   << std::setw(7) << std::left << l.severity
   << " -";
}

// #define TESTING
#ifdef TESTING
#include <regex>
#define REGEX "(\\n)"
int main(int argc, char *argv[]) {
  
  google::InitGoogleLogging(argv[0], &CustomPrefix);
  FLAGS_logtostderr = true;

  std::string comment = "\n\n";
  LOG(INFO) << "the string: `" << comment << "`\n";
  std::regex re{REGEX};
  LOG(INFO) << "the regex: `" REGEX "`\n";
  std::smatch match_result;
  bool hasMatch = std::regex_search(comment.cbegin(), comment.cend(), match_result, re);
  LOG(INFO) << "hasMatch: " << hasMatch ;
  LOG(INFO) << "size: " << match_result.size() ;
  LOG(INFO) << "length: " << match_result.length() ;
}
#else
int main(int argc, char *argv[]) {

  // Add jank option to detect when in testing mode
  bool testing = false;
  if (argc > 1) {
    if (std::string(argv[argc-1]) == "TESTING_ENGINE") {
      testing = true;
      argc--;
    }
  }

  google::InitGoogleLogging(argv[0], &CustomPrefix, &testing);
  google::InstallFailureSignalHandler();
  FLAGS_logtostderr = true;

  Options options;
  ParseResult ret = parseCommandLine(argc, argv, options);
  if (ret != ParseResult::PARSING_SUCCESS) {
    LOG(ERROR) << "Command line usage error!\n\tCode: " << static_cast<int>(ret);
    return static_cast<int>(ret);
  }

  Compiler compiler(options);

  try {
    CompilerResult result = compiler.run();
    // if (result != COMPILATION_SUCCESS) {
    return static_cast<int>(result);
    // }
  } catch (std::exception &e) {
    LOG(ERROR) << "Compiler error!\n\tError: " << e.what();
    return 1;
  }
}
#endif