#include "LoopAnalyzer.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Error.h"

using namespace clang;
using namespace clang::tooling;

// 🔥 自定义 Category
static llvm::cl::OptionCategory ArmIntrinCategory("arm-intrin-suggester options");

int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ArmIntrinCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedParser.get();

    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
    return Tool.run(newFrontendActionFactory<LoopFrontendAction>().get());
}
