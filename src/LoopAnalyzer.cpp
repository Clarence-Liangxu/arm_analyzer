#include "LoopAnalyzer.h"
#include "FactEmitter.h"
#include "Suggest.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/OperationKinds.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

void LoopHandler::run(const MatchFinder::MatchResult &Result) {
    return;
    if (const ForStmt *FS = Result.Nodes.getNodeAs<ForStmt>("forLoop")) {
        const ASTContext *Context = Result.Context;
        const SourceManager &SM = Context->getSourceManager();
        const Stmt *Body = FS->getBody();

        if (!Body) return;

        // 🔒 只处理 CompoundStmt 类型的循环体
        if (const auto *CS = llvm::dyn_cast<CompoundStmt>(Body)) {
            for (const Stmt *SubStmt : CS->body()) {
                if (!SubStmt) continue;

                // 检测二元操作符（+ - * /）
                if (const auto *BO = llvm::dyn_cast<BinaryOperator>(SubStmt)) {
                    std::string op;

                    switch (BO->getOpcode()) {
                        case BO_Add: op = "add"; break;
                        case BO_Sub: op = "sub"; break;
                        case BO_Mul: op = "mul"; break;
                        case BO_Div: op = "div"; break;
                        default: break;
                    }

                    if (!op.empty()) {
                        //llvm::errs() << "Detected op=" << op << " type=" << detectType(BO->getType()) << "\n";
                        //std::string type = detectType(BO->getType());
                        //emitFact(op, FS, type, SM);
                        
                    }
                }

                // 检测函数调用（fmaf）
                if (const auto *ExprNode = llvm::dyn_cast<Expr>(SubStmt)) {
                    if (const auto *Call = llvm::dyn_cast<CallExpr>(ExprNode)) {
                        if (const FunctionDecl *FD = Call->getDirectCallee()) {
                            std::string FuncName = FD->getNameInfo().getAsString();
                            if (FuncName == "fmaf") {
                                //llvm::errs() << "Detected op=" << op << " type=" << detectType(BO->getType()) << "\n";
                                //std::string type = detectType(Call->getType());
                                //emitFact("fma", FS, type, SM);
                            }
                        }
                    }
                }
            }
        } else {
            // 🔒 非 CompoundStmt 的 for body（单条语句），也尝试处理一次
            const Stmt *SubStmt = Body;

            if (!SubStmt) return;

            if (const auto *BO = llvm::dyn_cast<BinaryOperator>(SubStmt)) {
                std::string op;

                switch (BO->getOpcode()) {
                    case BO_Add: op = "add"; break;
                    case BO_Sub: op = "sub"; break;
                    case BO_Mul: op = "mul"; break;
                    case BO_Div: op = "div"; break;
                    default: break;
                }

                if (!op.empty()) {
                    //std::string type = detectType(BO->getType());
                    //emitFact(op, FS, type, SM);
                }
            }

            if (const auto *ExprNode = llvm::dyn_cast<Expr>(SubStmt)) {
                if (const auto *Call = llvm::dyn_cast<CallExpr>(ExprNode)) {
                    if (const FunctionDecl *FD = Call->getDirectCallee()) {
                        std::string FuncName = FD->getNameInfo().getAsString();
                        if (FuncName == "fmaf") {
                            //std::string type = detectType(Call->getType());
                            //emitFact("fma", FS, type, SM);
                        }
                    }
                }
            }
        }
    }
}

void LoopFrontendAction::EndSourceFileAction() {}

std::unique_ptr<clang::ASTConsumer> LoopFrontendAction::CreateASTConsumer(CompilerInstance &CI, StringRef file) {
    auto Matcher = std::make_unique<MatchFinder>();

    Matcher->addMatcher(
        forStmt(hasLoopInit(declStmt(hasSingleDecl(
            varDecl(hasInitializer(anything()))
        )))).bind("forLoop"),
        &Handler
    );

    return Matcher->newASTConsumer();
}