#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include <map>
#include <string>

using namespace clang;
using namespace clang::ast_matchers;

using namespace clang::tooling;

int num_functions;

std::map<std::string, std::pair<int,int>> name_to_args;

DeclarationMatcher functions = functionDecl().bind("fxn");

class FunctionCallback : public MatchFinder::MatchCallback {
public:
  ASTContext* context;

  void dump_map() {
    for (std::pair<std::string, std::pair<int,int>> entry : name_to_args) {
      printf("%s has %d to %d args\n", entry.first.c_str(), entry.second.first, entry.second.second);
    }
  }

  void run(const MatchFinder::MatchResult& result) override {
    context = result.Context;

    const FunctionDecl* func = result.Nodes.getNodeAs<FunctionDecl>("fxn");

    if (!func || !context->getSourceManager().isWrittenInMainFile(func->getBeginLoc())) {
      // printf("ERROR: invalid function decl/location\n");
      return;
    }

    std::string func_name = func->getNameAsString();
    unsigned int num_params = func->getNumParams();
    unsigned int min_params = func->getMinRequiredArguments();

    // printf("%s has %d to %d args\n", func_name.c_str(), min_params, num_params);

    // if it doesn't exist, add it
    if (name_to_args.find(func_name) == name_to_args.end()) {
      name_to_args[func_name] = {min_params, num_params};
    }
    else { // if it does exist, possibly update
      if (name_to_args[func_name].first > min_params) {
        name_to_args[func_name].first = min_params;
      }
      if (name_to_args[func_name].second < num_params) {
        name_to_args[func_name].second = num_params;
      }
    }
  }

  void onEndOfTranslationUnit() override {
    printf("std::map<std::string, std::pair<int,int>> name_to_args = {\n");

    for (std::pair<std::string, std::pair<int,int>> entry : name_to_args) {
      printf("{\"%s\", {%d, %d}},\n", entry.first.c_str(), entry.second.first, entry.second.second);
    }

    printf("};\n");
    // dump_map();
  }
};

static llvm::cl::OptionCategory
    RewriteCategory("Automatic code rewriting options");


int main(int argc, const char **argv) {
  auto ExpectedParser =
      CommonOptionsParser::create(argc, argv, RewriteCategory);
  if (!ExpectedParser) {
    // Fail gracefully for unsupported options.
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser &OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  MatchFinder func_finder;
  FunctionCallback func_callback;

  func_finder.addMatcher(functions, &func_callback);

  int retval = Tool.run(newFrontendActionFactory(&func_finder).get());

  return retval;
}
