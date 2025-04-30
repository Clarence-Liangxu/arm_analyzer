#include "FactEmitter.h"
#include <fstream>
#include <filesystem>
#include <sstream>

std::string detectType(clang::QualType qt) {
    const clang::Type *typePtr = qt.getTypePtrOrNull();
    if (!typePtr) return "unknown";

    if (const auto *bt = llvm::dyn_cast<clang::BuiltinType>(typePtr)) {
        switch (bt->getKind()) {
            case clang::BuiltinType::Float: return "f32";
            case clang::BuiltinType::Double: return "f64";
            case clang::BuiltinType::Int: return "s32";
            case clang::BuiltinType::Short: return "s16";
            case clang::BuiltinType::UChar: return "u8";
            default: return "unknown";
        }
    }
    return "unknown";
}

void emitFact(const std::string &op, const clang::ForStmt *FS, const std::string &type, const clang::SourceManager &SM) {
    std::filesystem::create_directories("souffle/facts");
    std::string filename = "souffle/facts/" + op + ".facts";

    std::ofstream out(filename, std::ios::app);
    if (!out.is_open()) {
        llvm::errs() << "❌ Failed to open fact file: " << filename << "\n";
        return;
    }

    auto loc = FS->getForLoc();
    std::string fullpath = SM.getFilename(loc).str();
    std::string file = std::filesystem::path(fullpath).filename().string();

    if (file.empty())
        file = "unknown.c";

    unsigned line = SM.getSpellingLineNumber(loc);
    std::ostringstream row;
    row << file << "\t" << line << "\t" << type << "\n";

    llvm::errs() << "📥 Writing fact line: " << row.str();
    llvm::errs() << "📥 Writing fact: " << std::filesystem::absolute(filename) << " — " << row.str() << "\n";
    out << row.str();
    out.flush();              // ✅ 强制写入
    out.close();              // ✅ 关闭触发写入
}
