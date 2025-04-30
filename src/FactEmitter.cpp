#include "clang/AST/Stmt.h"
#include "clang/AST/Type.h"
#include "clang/Basic/SourceManager.h"
#include <fstream>
#include <string>

using namespace clang;

// 通用的 emitFact
#include "FactEmitter.h"

std::string opToFactFile(const std::string &op) {
    if (op == "add") return "addition.facts";
    if (op == "sub") return "sub.facts";
    if (op == "mul") return "mul.facts";
    if (op == "div") return "div.facts";
    if (op == "fma") return "fma.facts";
    if (op == "fabs") return "fabs.facts";
    if (op == "neg") return "neg.facts";
    return "unknown.facts";  // fallback
}

void emitFact(const std::string &op, const clang::ForStmt *FS, const std::string &type, const clang::SourceManager &SM) {
    std::string filename = opToFactFile(op);  // ✅ 使用映射规则
    std::ofstream out("souffle/facts/" + filename, std::ios::app);
    auto loc = FS->getForLoc();
    std::string file = SM.getFilename(loc).str();
    unsigned line = SM.getSpellingLineNumber(loc);

    out << file << "\t" << line << "\t" << type << "\n";
    out.close();
}

// 辅助检测数据类型（float32/int32等）
std::string detectType(clang::QualType qt) {
    const clang::Type *typePtr = qt.getTypePtrOrNull();
    if (!typePtr)
        return "unknown";

    if (const auto *bt = llvm::dyn_cast<clang::BuiltinType>(typePtr)) {
        switch (bt->getKind()) {
            case clang::BuiltinType::Float:
                return "f32";
            case clang::BuiltinType::Double:
                return "f64";
            case clang::BuiltinType::Int:
                return "s32";
            case clang::BuiltinType::Short:
                return "s16";
            case clang::BuiltinType::UChar:
                return "u8";
            default:
                return "unknown";
        }
    }
    return "unknown";
}