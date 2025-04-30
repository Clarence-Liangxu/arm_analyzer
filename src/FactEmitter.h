#pragma once
#include "clang/AST/Stmt.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include <string>

void emitFact(const std::string &op, const clang::ForStmt *FS, const std::string &type, const clang::SourceManager &SM);
std::string detectType(clang::QualType qt);
