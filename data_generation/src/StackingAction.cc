#include "StackingAction.hh"

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"

// ------------------------------------------------------------
// コンストラクタ
// fYMax は「zx 平面からどれだけ外れてよいか」を決める
// ------------------------------------------------------------
StackingAction::StackingAction()
: G4UserStackingAction(),
  fYMax(std::sin(0.2))   // ★必要に応じて調整
{
}

// ------------------------------------------------------------
// 新しいトラックが生成されるたびに呼ばれる
// ------------------------------------------------------------
G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track* track)
{
    // ParentID == 1 のみを見る
    if (track->GetParentID() != 1) {
        return fUrgent;
    }

    // --- ガンマ線のみ対象 ---
    if (track->GetDefinition() != G4Gamma::Definition()) {
        return fUrgent;
    }

    // --- 崩壊で生成されたもののみ ---
    const auto* proc = track->GetCreatorProcess();
    if (!proc) {
        return fUrgent;
    }

    if (proc->GetProcessName() != "Decay") {
        return fUrgent;
    }

    // --- 運動量方向 ---
    const G4ThreeVector& dir =
        track->GetMomentumDirection();

    // --- zx 平面から大きく外れたら kill ---
    // 検出器が zx 平面方向にしか無いので，
    // y 成分が大きいイベントは絶対に検出されない
    if (std::abs(dir.y()) > fYMax) {
        return fKill;   // tracking せず次のイベントへ
    }

    // --- それ以外は通常通り ---
    return fUrgent;
}
