#include "args-parser.hpp"

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

ParseResult parseCommandLine(int argc, char *argv[], Options &options) {
  try {
    std::string configFile;
    // Declare a group of options that will be 
    // allowed only on command line
    po::options_description generic("Generic options");
    generic.add_options()
        (OPT_W_ALIAS(VERSION), "print version string")
        (OPT_W_ALIAS(HELP), "produce help message")
        (OPT_W_ALIAS(CONFIG), po::value<std::string>(&configFile),//->default_value("multiple_sources.cfg"),
                    "name of a file of a configuration.")
        ;
    
    // std::cout << "generic options\n" << generic << std::endl;

    // Declare a group of options that will be 
    // allowed both on command line and in
    // config file
    po::options_description config("Configuration");
    config.add_options()
        // ("optimization", po::value<int>(&options.optimization)->default_value(10), 
        //       "optimization level")
        (OPT_W_ALIAS(TEMP_FILE_DIR), po::value<std::string>(&options.tempFileDir)->composing(), 
              "temporary file output directory")
        (OPT_W_ALIAS(OUTPUT), po::value<std::string>(&options.outputPath), 
              "output file path")
        // ("include-path,I", 
        //     po::value< vector<string> >()->composing(), 
        //     "include path")
        ;

    // std::cout << "config options\n" << config << std::endl;

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    po::options_description hidden("Hidden options");
    hidden.add_options()
        (OPT_W_ALIAS(INPUT_FILE), po::value<std::string>(&options.sourceFile), "input file")
        ;

    // std::cout << "hidden options\n" << hidden << std::endl;

    po::options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);

    // std::cout << "cmd options\n" << cmdline_options << std::endl;

    po::options_description config_file_options;
    config_file_options.add(config).add(hidden);

    // std::cout << "config file options\n" << config_file_options << std::endl;

    po::options_description visible("Allowed options");
    visible.add(generic).add(config);

    // std::cout << "visible options\n" << visible << std::endl;

    po::positional_options_description p;
    p.add(INPUT_FILE, 1); // max of 1 input file

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    po::notify(vm);

    // std::cout << cmdline_options << std::endl;

    if (vm.count(CONFIG)) {
      std::ifstream ifs(configFile.c_str());
      if (!ifs) {
        std::cout << "can not open config file: " << configFile << std::endl;
        return ParseResult::EXIT_COMPILATION;
      } else {
        po::store(po::parse_config_file(ifs, config_file_options), vm);
        po::notify(vm);
      }
    }

    if (vm.count(HELP)) {
      std::cout << visible<< std::endl;
      return ParseResult::EXIT_COMPILATION;
    }

    if (vm.count(VERSION)) {
      std::cout << "Bokay CPP Compiler, version 0.0.1\n";
      return ParseResult::EXIT_COMPILATION;
    }

    if (!vm.count(INPUT_FILE)) {
      std::cout << "Must provide a file to be compiled!" << std::endl;
      return ParseResult::INVALID_ARGUMENTS;
    }
    std::cout << "Input source file: " << options.sourceFile << std::endl;

    if ((options.customOutputPath = vm.count(OUTPUT))) {
      std::cout << "Output path: " << options.outputPath << std::endl;
    } else {
      std::cout << "No output path provided." << std::endl;
      // options.outputPath = options.sourceFile + ".out";
    }


    if ((options.outputTemps = vm.count(TEMP_FILE_DIR))) {
      std::cout << "Temp file output to: " << options.tempFileDir << std::endl;
    }

  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return ParseResult::INVALID_ARGUMENTS;
  }
  return ParseResult::PARSING_SUCCESS;
}