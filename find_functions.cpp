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

// === For 循环处理器（升级版）===
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

        if (const auto *compound = dyn_cast<CompoundStmt>(body)) {
            for (const Stmt *stmt : compound->body()) {
                if (const auto *binOp = dyn_cast<BinaryOperator>(stmt)) {
                    if (binOp->isAssignmentOp() || binOp->isCompoundAssignmentOp()) {
                        const Expr *rhs = binOp->getRHS()->IgnoreParenCasts();
                        const Expr *lhs = binOp->getLHS()->IgnoreParenCasts();

                        // 尝试获取左值数组元素类型
                        QualType elementType;
                        if (const auto *arraySub = dyn_cast<ArraySubscriptExpr>(lhs)) {
                            elementType = arraySub->getBase()->IgnoreParenCasts()->getType()->getPointeeType();
                        }

                        if (elementType.isNull())
                            continue;

                        std::string typeStr = elementType.getAsString();
                        std::string typeHint;

                        if (typeStr.find("int32_t") != std::string::npos) {
                            typeHint = "_s32";
                        } else if (typeStr.find("float") != std::string::npos) {
                            typeHint = "_f32";
                        } else if (typeStr.find("uint16_t") != std::string::npos) {
                            typeHint = "_u16";
                        } else if (typeStr.find("uint8_t") != std::string::npos) {
                            typeHint = "_u8";
                        } else {
                            continue; // 不支持的类型
                        }

                        SourceManager &SM = Context->getSourceManager();
                        SourceLocation loc = forStmt->getBeginLoc();

                        if (binOp->isAssignmentOp()) {
                            if (const auto *innerBinOp = dyn_cast<BinaryOperator>(rhs)) {
                                switch (innerBinOp->getOpcode()) {
                                    case BO_Add:
                                        llvm::outs() << "=== 找到加法循环 (" << typeStr << ") ===\n";
                                        llvm::outs() << "文件: " << SM.getFilename(loc) << "\n";
                                        llvm::outs() << "行号: " << SM.getSpellingLineNumber(loc) << "\n";
                                        llvm::outs() << "建议: 使用 ARM NEON vaddq" << typeHint << " 加速\n";
                                        llvm::outs() << "========================\n\n";
                                        break;
                                    case BO_Sub:
                                        llvm::outs() << "=== 找到减法循环 (" << typeStr << ") ===\n";
                                        llvm::outs() << "文件: " << SM.getFilename(loc) << "\n";
                                        llvm::outs() << "行号: " << SM.getSpellingLineNumber(loc) << "\n";
                                        llvm::outs() << "建议: 使用 ARM NEON vsubq" << typeHint << " 加速\n";
                                        llvm::outs() << "========================\n\n";
                                        break;
                                    case BO_Mul:
                                        llvm::outs() << "=== 找到乘法循环 (" << typeStr << ") ===\n";
                                        llvm::outs() << "文件: " << SM.getFilename(loc) << "\n";
                                        llvm::outs() << "行号: " << SM.getSpellingLineNumber(loc) << "\n";
                                        llvm::outs() << "建议: 使用 ARM NEON vmulq" << typeHint << " 加速\n";
                                        llvm::outs() << "========================\n\n";
                                        break;
                                    default:
                                        break;
                                }
                            }
                        } else if (binOp->isCompoundAssignmentOp()) {
                            if (binOp->getOpcode() == BO_AddAssign) {
                                if (const auto *innerBinOp = dyn_cast<BinaryOperator>(rhs)) {
                                    if (innerBinOp->getOpcode() == BO_Mul) {
                                        llvm::outs() << "=== 找到点乘累加循环 (" << typeStr << ") ===\n";
                                        llvm::outs() << "文件: " << SM.getFilename(loc) << "\n";
                                        llvm::outs() << "行号: " << SM.getSpellingLineNumber(loc) << "\n";
                                        llvm::outs() << "建议: 使用 ARM NEON vmlaq" << typeHint << " 加速\n";
                                        llvm::outs() << "========================\n\n";
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
};

// === Memcpy/ Memset处理器（上阶段已经实现）===
class MemcpyHandler : public MatchFinder::MatchCallback {
public:
    virtual void run(const MatchFinder::MatchResult &Result) {
        const CallExpr *call = Result.Nodes.getNodeAs<CallExpr>("memcpyCall");
        if (!call)
            return;

        ASTContext *Context = Result.Context;
        const FunctionDecl *callee = call->getDirectCallee();
        if (!callee)
            return;

        StringRef funcName = callee->getName();

        if (funcName == "memcpy" || funcName == "memset") {
            SourceManager &SM = Context->getSourceManager();
            SourceLocation loc = call->getBeginLoc();

            llvm::outs() << "=== 找到可优化的内存函数调用 ===\n";
            llvm::outs() << "文件: " << SM.getFilename(loc) << "\n";
            llvm::outs() << "行号: " << SM.getSpellingLineNumber(loc) << "\n";

            if (funcName == "memcpy") {
                llvm::outs() << "建议: 用 ARM NEON vld1q_u8/vst1q_u8 加速 memcpy\n";
            } else if (funcName == "memset") {
                llvm::outs() << "建议: 用 ARM SIMD 指令加速 memset\n";
            }
            llvm::outs() << "========================\n\n";
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

    ForLoopHandler forHandler;
    MemcpyHandler memcpyHandler;
    MatchFinder Finder;

    Finder.addMatcher(forStmt().bind("forLoop"), &forHandler);

    StatementMatcher MemcpyMatcher = callExpr(
        callee(functionDecl(hasName("memcpy")).bind("func"))
    ).bind("memcpyCall");

    StatementMatcher MemsetMatcher = callExpr(
        callee(functionDecl(hasName("memset")).bind("func"))
    ).bind("memcpyCall");

    Finder.addMatcher(MemcpyMatcher, &memcpyHandler);
    Finder.addMatcher(MemsetMatcher, &memcpyHandler);

    return Tool.run(newFrontendActionFactory(&Finder).get());
}
