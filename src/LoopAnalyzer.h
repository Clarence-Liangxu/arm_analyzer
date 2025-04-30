#pragma once

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"

using namespace clang::ast_matchers;

class LoopHandler : public MatchFinder::MatchCallback {
public:
    virtual void run(const MatchFinder::MatchResult &Result) override;
};

class LoopFrontendAction : public clang::ASTFrontendAction {
public:
    void EndSourceFileAction() override;
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef file) override;

private:
    LoopHandler Handler;
};