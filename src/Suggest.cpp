#include "Suggest.h"

std::string suggestNeon(const std::string &op, const std::string &type) {
    if (op == "add") {
        if (type == "f32") return "vaddq_f32";
        if (type == "f64") return "vaddq_f64";
        if (type == "s32") return "vaddq_s32";
        if (type == "s16") return "vaddq_s16";
        if (type == "u8")  return "vaddq_u8";
    } else if (op == "sub") {
        if (type == "f32") return "vsubq_f32";
        if (type == "s32") return "vsubq_s32";
    } else if (op == "mul") {
        if (type == "f32") return "vmulq_f32";
        if (type == "f64") return "vmulq_f64";
        if (type == "s32") return "vmulq_s32";
    } else if (op == "div") {
        if (type == "f32") return "vmulq_f32(a, vrecpeq_f32(b)) // approximate div";
    } else if (op == "fma") {
        if (type == "f32") return "vfmaq_f32(c, a, b)";
    } else if (op == "fabs") {
        if (type == "f32") return "vabsq_f32";
    } else if (op == "neg") {
        if (type == "f32") return "vnegq_f32";
    }
    return "UNKNOWN";
}
