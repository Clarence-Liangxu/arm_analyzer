#include "FactEmitter.h"
#include <fstream>
#include <filesystem>

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

    auto loc = FS->getForLoc();
    std::string file = SM.getFilename(loc).str();
    unsigned line = SM.getSpellingLineNumber(loc);

    out << file << "\t" << line << "\t" << type << "\n";
    out.close();
}
