#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

// 命令行选项定义
static cl::OptionCategory ToolCategory("arm-intrinsic-replacer options");

// === 核心处理器：负责处理匹配到的 for 循环 ===
class ForLoopHandler : public MatchFinder::MatchCallback {
public:
    virtual void run(const MatchFinder::MatchResult &Result) {
        const ForStmt *forStmt = Result.Nodes.getNodeAs<ForStmt>("forLoop");

        if (!forStmt)
            return;

        ASTContext *Context = Result.Context;
        const Stmt *body = forStmt->getBody();
        if (!body)
            return;

        // 尝试找到 body 里面的二元赋值：dst[i] = src1[i] + src2[i];
        if (const auto *compound = dyn_cast<CompoundStmt>(body)) {
            for (const Stmt *stmt : compound->body()) {
                if (const auto *binOp = dyn_cast<BinaryOperator>(stmt)) {
                    if (binOp->isAssignmentOp()) {
                        const Expr *rhs = binOp->getRHS()->IgnoreParenCasts();
                        if (const auto *innerBinOp = dyn_cast<BinaryOperator>(rhs)) {
                            if (innerBinOp->getOpcode() == BO_Add) {
                                // 成功匹配到形如 dst[i] = src1[i] + src2[i];
                                SourceManager &SM = Context->getSourceManager();
                                SourceLocation loc = forStmt->getBeginLoc();

                                llvm::outs() << "=== 找到可优化的循环 ===\n";
                                llvm::outs() << "文件: " << SM.getFilename(loc) << "\n";
                                llvm::outs() << "行号: " << SM.getSpellingLineNumber(loc) << "\n";
                                llvm::outs() << "建议: 可以使用 ARM NEON 指令 vaddq_s32 来替换此加法循环\n";
                                llvm::outs() << "========================\n\n";
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
};

// === 主程序入口 ===
int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedParser.get();

    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    ForLoopHandler Handler;
    MatchFinder Finder;

    // 匹配 for 循环
    StatementMatcher ForLoopMatcher = forStmt().bind("forLoop");

    Finder.addMatcher(ForLoopMatcher, &Handler);

    return Tool.run(newFrontendActionFactory(&Finder).get());
}
