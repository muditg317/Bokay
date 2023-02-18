#include "compiler.hpp"
#include "utilities.hpp"
#include "args-parser.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "ast-builder.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/range/algorithm/count.hpp>
#include <fmt/format.h>
#include <glog/logging.h>

static std::string readFile(boost::filesystem::path filePath) {
  assert(boost::filesystem::exists(filePath) && "File must exist to be read! Check before calling this method!");
  std::fstream fileStream(filePath.string(), std::ios::in);
  std::stringstream buffer;
  buffer << fileStream.rdbuf();
  return buffer.str();
}

Compiler::Compiler(Options &options) :
    sourceFile(options.sourceFile),
    outputTemps(options.outputTemps)
{
  sourceName = sourceFile.stem().string();
  // validate(options);
  if (options.outputTemps) {
    tempFileDir = boost::filesystem::path(options.tempFileDir);
  }
  if (options.customOutputPath) {
    outputPath = boost::filesystem::path(options.outputPath);
  } else { // create default output path
    outputPath = boost::filesystem::path(sourceFile);
    // if (outputPath.has_extension()) {
    outputPath.replace_extension(boost::filesystem::path("out"));
    LOG(INFO) << "Using default output path... " << outputPath ;
    // } else {
    //   outputPath.extension
    // }
  }
}

bool Compiler::validate_options(void) {
  if (!boost::filesystem::exists(sourceFile)) { // doesn't exist
    LOG(ERROR) << "Source file (" << sourceFile << ") does not exist!" ;
    return false;
  }
  if (!tempFileDir.empty() && !boost::filesystem::exists(tempFileDir)) { // doesn't exist
    LOG(WARNING) << "Directory for temp files doesn't exist! Creating..." ;
    bool created = boost::filesystem::create_directories(tempFileDir);
    if (!created) return false;
    LOG(INFO) << tempFileDir << " created!" ;
  } else { // temp dir exist
    if (!boost::filesystem::is_directory(tempFileDir)) { // exists but not a dir
      LOG(ERROR) << tempFileDir << " already exists as a file! Exiting..." ;
      return false;
    }
  }
  auto outputParent = outputPath.parent_path();
  if (!boost::filesystem::exists(outputParent)) { // parent of outputPath needs to be created
    LOG(WARNING) << "Creating parent directory for output executable..." ;
    bool created = boost::filesystem::create_directories(outputParent);
    if (!created) return false;
    LOG(INFO) << "\tDone: " << outputParent << " created!" ;
    return false;
  }
  return true;
}

template<class Stage>
typename Stage::Base::OutputType &Compiler::runStage(
    typename Stage::Base::InputType &input,
    typename Stage::Base::OutputType *&output
  ) const {

  const Stage &stage = std::get<Stage>(stages);

  typename Stage::Base::ErrorType errorCode = stage(input, output);

  if (errorCode != Stage::Base::SUCCESS_CODE) {
    LOG(ERROR) << "Stage " << Stage::Base::NAME << " failed with error code " << static_cast<int>(errorCode) ;
    // throw compiler_code_map::type<Stage::NAME_>;
    throw failure_code_for<Stage>;
  }

  #ifdef DEBUG
  stage.debugCallback(*output);
  #endif

  if (outputTemps) {
    boost::filesystem::path tempFile = tempFileDir / fmt::format("{}.{}", sourceName, Stage::Base::TMP_OUT_EXT);
    LOG(INFO) << "Writing " << Stage::Base::NAME << " outputs to temp file: " << tempFile;
    stage.writeOutput(*output, tempFile);
  }

  return *output;
}

template<class ... Stages>
typename std::tuple_element_t<sizeof...(Stages)-1, std::tuple<Stages...>>::Base::OutputType &Compiler::runStages(
  typename std::tuple_element_t<0, std::tuple<Stages...>>::Base::InputType &input
) const {
  static_assert(std::is_same_v<std::tuple<Stages...>, StagesTuple>, "Stages must be the same as the stages in the compiler!");

  InputPtrTuple inputs;
  std::get<StageIndex<FirstStage>>(inputs) = &input;
  LastStage::Base::OutputType *output = nullptr;

  auto applyInputsFromOutputs = [&]<std::size_t... index_seq>(std::index_sequence<index_seq...>) {
    (runStage<std::tuple_element_t<index_seq, StagesTuple>>(*std::get<StageIndex<std::tuple_element_t<index_seq, StagesTuple>>>(inputs), std::get<StageIndex<next_stage_for<std::tuple_element_t<index_seq, StagesTuple>>>>(inputs)), ...);
  };
  applyInputsFromOutputs(std::make_index_sequence<sizeof...(Stages)-1>{});

  runStage<LastStage>(*std::get<StageIndex<LastStage>>(inputs), output);
  // (runStage<Stages>(*std::get<StageIndex<Stages>>(inputs), std::is_same_v<Stages, LastStage> ? output : std::get<StageIndex<next_stage_for<Stages>>>(inputs)), ...);

  return *output;

  // OutputPtrTuple outputs;

  // index_seq for all but last
  // using stage_except_last_indices = std::make_index_sequence<sizeof...(Stages)-1>;
  // auto index_seq = stage_except_last_indices{};

  // InputPtrRefTuple inputs;// = std::make_tuple(std::ref(&input));

  // auto applyInputsFromOutputs = [&]<std::size_t... index_seq>(std::index_sequence<index_seq...>){
  //   ((std::get<TupleIndexOf_v<next_stage_for<std::tuple_element_t<index_seq, StagesTuple>>, StagesTuple>>(inputs) = std::ref(std::get<TupleIndexOf_v<std::tuple_element_t<index_seq, StagesTuple>, StagesTuple>>(outputs))), ...);
  // };
  // applyInputsFromOutputs(stage_except_last_indices{}); // apply inputs from outputs

  // // ((std::get<TupleIndexOf_v<next_stage_for<std::tuple_element_t<index_seq, StagesTuple>>, StagesTuple>>(inputs) = std::ref(std::get<index_seq>(outputs))), ...);

  // // auto applyInputsFromOutputs = [&]<class S1, class S2, class ... index_seq>() {
  // //   std::get<TupleIndexOf_v<S2, StagesTuple>>(inputs) = std::get<TupleIndexOf_v<S1, StagesTuple>>(outputs);
  // //   if constexpr (sizeof...(Ss) > 0) {
  // //     applyInputsFromOutputs.operator()<S2, Ss...>();
  // //   }
  // // };
  // // std::get<TupleIndexOf_v<Lexer, StagesTuple>>(inputs) = &input;
  // // (std::get<TupleIndexOf_v<Stages, StagesTuple>>(inputs) = std::get<TupleIndexOf_v<Stages, StagesTuple>>(outputs), ...

  // ((std::get<TupleIndexOf_v<Stages, StagesTuple>>(outputs) = runStage<Stages>(std::get<TupleIndexOf_v<Stages, StagesTuple>>(inputs))), ...);
}

CompilerResult Compiler::run(void) {
  if (!validate_options()) {
    return CompilerResult::INVALID_COMPILATION_OPTIONS;
  }

  fileContents = readFile(sourceFile);

  int lines = boost::range::count(fileContents, '\n') + 1;
  DLOG(INFO) << "File contains " << lines << " lines." ;
  int ind = fileContents.find('\n');
  DLOG(INFO) << "Line 1: " << fileContents.substr(0, ind != std::string::npos ? ind : fileContents.size()) ;

  
  try {
    // std::vector<Token> *tokens_ptr;
    // std::vector<Token> tokens = runStage<Lexer>(fileContents, tokens_ptr);
    // ParseTree *ptree_ptr;
    // ParseTree ptree = runStage<Parser>(tokens, ptree_ptr);
    // ASTRootNode *astRoot_ptr;
    // ASTRootNode astRoot = runStage<ASTBuilder>(ptree, astRoot_ptr);

    runStages<Lexer, Parser, ASTBuilder>(fileContents);

  } catch (CompilerResult e) {
    return e;
  }

  return CompilerResult::COMPILATION_SUCCESS;
}