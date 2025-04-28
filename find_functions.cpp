#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::tooling;

// AST Visitor：遍历函数声明
class FindFunctionsVisitor : public RecursiveASTVisitor<FindFunctionsVisitor> {
public:
    explicit FindFunctionsVisitor(ASTContext *Context) : Context(Context) {}

    bool VisitFunctionDecl(FunctionDecl *FuncDecl) {
        if (FuncDecl->isThisDeclarationADefinition()) {
            llvm::outs() << "Found function: "
                         << FuncDecl->getNameInfo().getName().getAsString()
                         << "\n";
        }
        return true;
    }

private:
    ASTContext *Context;
};

// AST Consumer：处理每个AST
class FindFunctionsConsumer : public ASTConsumer {
public:
    explicit FindFunctionsConsumer(ASTContext *Context)
        : Visitor(Context) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    FindFunctionsVisitor Visitor;
};

// FrontendAction：为每个输入文件创建Consumer
class FindFunctionsAction : public ASTFrontendAction {
public:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   StringRef InFile) override {
        return std::make_unique<FindFunctionsConsumer>(&CI.getASTContext());
    }
};

// 命令行选项分类
static llvm::cl::OptionCategory MyToolCategory("find-functions options");

int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedParser.get();

    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    return Tool.run(newFrontendActionFactory<FindFunctionsAction>().get());
}
