#include "args-parser.hpp"

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <glog/logging.h>
// namespace boost::program_options = boost::program_options;

ParseResult parseCommandLine(int argc, char *argv[], Options &options) {
  // google::Init
  try {
    std::string configFile;
    // Declare a group of options that will be 
    // allowed only on command line
    boost::program_options::options_description generic("Generic options");
    generic.add_options()
        (OPT_W_ALIAS(ARG_VERSION), "print version string")
        (OPT_W_ALIAS(ARG_HELP), "produce help message")
        (OPT_W_ALIAS(ARG_CONFIG), boost::program_options::value<std::string>(&configFile),//->default_value("multiple_sources.cfg"),
                    "name of a file of a configuration.")
        ;
    
    // DLOG(INFO) << "generic options\n" << generic ;

    // Declare a group of options that will be 
    // allowed both on command line and in
    // config file
    boost::program_options::options_description config("Configuration");
    config.add_options()
        // ("optimization", boost::program_options::value<int>(&options.optimization)->default_value(10), 
        //       "optimization level")
        (OPT_W_ALIAS(ARG_TEMP_FILE_DIR), boost::program_options::value<std::string>(&options.tempFileDir)->composing(), 
              "temporary file output directory")
        (OPT_W_ALIAS(ARG_OUTPUT), boost::program_options::value<std::string>(&options.outputPath), 
              "output file path")
        // ("include-path,I", 
        //     boost::program_options::value< vector<string> >()->composing(), 
        //     "include path")
        ;

    // DLOG(INFO) << "config options\n" << config ;

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    boost::program_options::options_description hidden("Hidden options");
    hidden.add_options()
        (OPT_W_ALIAS(ARG_INPUT_FILE), boost::program_options::value<std::string>(&options.sourceFile), "input file")
        ;

    // DLOG(INFO) << "hidden options\n" << hidden ;

    boost::program_options::options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);

    // DLOG(INFO) << "cmd options\n" << cmdline_options ;

    boost::program_options::options_description config_file_options;
    config_file_options.add(config).add(hidden);

    // DLOG(INFO) << "config file options\n" << config_file_options ;

    boost::program_options::options_description visible("Allowed options");
    visible.add(generic).add(config);

    DLOG(INFO) << "visible options\n" << visible ;

    boost::program_options::positional_options_description p;
    p.add(ARG_INPUT_FILE, 1); // max of 1 input file

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    boost::program_options::notify(vm);

    // // DLOG(INFO) << cmdline_options ;

    if (vm.count(ARG_CONFIG)) {
      std::ifstream ifs(configFile.c_str());
      if (!ifs) {
        LOG(ERROR) << "can not open config file: " << configFile ;
        return ParseResult::EXIT_COMPILATION;
      } else {
        boost::program_options::store(boost::program_options::parse_config_file(ifs, config_file_options), vm);
        boost::program_options::notify(vm);
      }
    }

    if (vm.count(ARG_HELP)) {
      LOG(INFO) << visible;
      return ParseResult::EXIT_COMPILATION;
    }

    if (vm.count(ARG_VERSION)) {
      LOG(INFO) << "Bokay CPP Compiler, version 0.0.1\n";
      return ParseResult::EXIT_COMPILATION;
    }

    if (!vm.count(ARG_INPUT_FILE)) {
      LOG(ERROR) << "Must provide a file to be compiled!" ;
      return ParseResult::INVALID_ARGUMENTS;
    }
    LOG(INFO) << "Input source file: " << options.sourceFile ;

    if ((options.customOutputPath = vm.count(ARG_OUTPUT))) {
      LOG(INFO) << "Output path: " << options.outputPath ;
    } else {
      LOG(WARNING) << "No output path provided." ;
      // options.outputPath = options.sourceFile + ".out";
    }


    if ((options.outputTemps = vm.count(ARG_TEMP_FILE_DIR))) {
      LOG(INFO) << "Temp file output to: " << options.tempFileDir ;
    }

  } catch (std::exception &e) {
    LOG(ERROR) << "Command line parsing error: " << e.what();
    return ParseResult::INVALID_ARGUMENTS;
  }
  return ParseResult::PARSING_SUCCESS;
}