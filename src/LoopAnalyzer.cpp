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
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

void LoopHandler::run(const MatchFinder::MatchResult &Result) {
    if (const ForStmt *FS = Result.Nodes.getNodeAs<ForStmt>("forLoop")) {
        const ASTContext *Context = Result.Context;
        const SourceManager &SM = Context->getSourceManager();
        const Stmt *Body = FS->getBody();
        if (!Body) return;

        llvm::errs() << "🌀 Matched a for-loop at "
                     << SM.getFilename(FS->getForLoc()) << ":"
                     << SM.getSpellingLineNumber(FS->getForLoc()) << "\n";

        if (const auto *CS = llvm::dyn_cast<CompoundStmt>(Body)) {
            for (const Stmt *SubStmt : CS->body()) {
                if (!SubStmt) continue;

                if (const auto *Assign = llvm::dyn_cast<BinaryOperator>(SubStmt)) {
                    if (Assign->isAssignmentOp()) {
                        const Expr *RHS = Assign->getRHS()->IgnoreParenImpCasts();

                        // fmaf
                        if (const auto *Call = llvm::dyn_cast<CallExpr>(RHS)) {
                            if (const FunctionDecl *FD = Call->getDirectCallee()) {
                                std::string name = FD->getNameInfo().getAsString();
                                if (name == "fmaf") {
                                    std::string type = detectType(Call->getType());
                                    llvm::errs() << "  ➤ Detected function call: fmaf (type: " << type << ")\n";
                                    emitFact("fma", FS, type, SM);
                                } else if (name == "fabsf") {
                                    std::string type = detectType(Call->getType());
                                    llvm::errs() << "  ➤ Detected function call: fabsf (type: " << type << ")\n";
                                    emitFact("fabs", FS, type, SM);
                                }
                            }
                        }

                        // + - * /
                        if (const auto *InnerBO = llvm::dyn_cast<BinaryOperator>(RHS)) {
                            std::string op;
                            switch (InnerBO->getOpcode()) {
                                case BO_Add: op = "addition"; break;
                                case BO_Sub: op = "subtraction"; break;
                                case BO_Mul: op = "multiplication"; break;
                                case BO_Div: op = "division"; break;
                                default: break;
                            }

                            if (!op.empty()) {
                                std::string type = detectType(InnerBO->getType());
                                llvm::errs() << "  ➤ Detected binary op: " << op << " (type: " << type << ")\n";
                                emitFact(op, FS, type, SM);
                            }
                        }

                        // -x
                        if (const auto *UO = llvm::dyn_cast<UnaryOperator>(RHS)) {
                            if (UO->getOpcode() == UO_Minus) {
                                std::string type = detectType(UO->getType());
                                llvm::errs() << "  ➤ Detected unary negation: -x (type: " << type << ")\n";
                                emitFact("negation", FS, type, SM);
                            }
                        }
                    }
                }
            }
        }
    }
}

void LoopFrontendAction::EndSourceFileAction() {}

std::unique_ptr<clang::ASTConsumer> LoopFrontendAction::CreateASTConsumer(CompilerInstance &CI, StringRef file) {
    Matcher.addMatcher(
        forStmt().bind("forLoop"),
        &Handler
    );
    return Matcher.newASTConsumer();
}
