#ifndef B4c_TargetConfig_h
#define B4c_TargetConfig_h 1
#include "G4SystemOfUnits.hh"

namespace B4c { namespace TargetConfig {

static constexpr const char* kWorldMaterial  = "G4_AIR";   // or "G4_AIR"
static constexpr const char* kTargetMaterial = "G4_Ag";         // 例: G4_C, G4_Si, G4_Cu ...
static constexpr double kTargetHalfX = 25.*mm;
static constexpr double kTargetHalfY = 25.*mm;

// ★ 追加（全長で指定する）：ここを書き換えるだけで厚さを変えられる
static constexpr double kTargetFullZ = 200.*um;

// 半長は自動で決まる（G4Boxは半長指定）
static constexpr double kTargetHalfZ = 0.5*kTargetFullZ;

static constexpr double kTargetPosX = 0.;
static constexpr double kTargetPosY = 0.;
static constexpr double kTargetPosZ = 0.;
static constexpr double kWorldHalf  = 0.5*m;

} } // namespace
#endif
