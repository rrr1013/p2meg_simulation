#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "RunAction.hh"

#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4MuonPlus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

namespace B4c {

SteppingAction::SteppingAction(const DetectorConstruction* detector)
  : fDet(detector)
{}

SteppingAction::~SteppingAction() = default;

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  const auto track = step->GetTrack();
  const auto particle = track->GetDefinition();

  // --- RunActionの取得（各スレッド内で動く） ---
  auto* baseRun = G4RunManager::GetRunManager()->GetUserRunAction();
  auto* run = const_cast<B4c::RunAction*>( dynamic_cast<const B4c::RunAction*>(baseRun) );

  if (!run) return;

  // --- 1. 一次ミューオンをカウント ---
  if (particle == G4MuonPlus::Definition() &&
      track->GetParentID() == 0 &&
      track->GetCurrentStepNumber() == 1) {
    run->CountPrimaryMu();
  }

  // --- 2. ミューオンが停止した場合を検出 ---
  if (particle == G4MuonPlus::Definition()) {

    auto status = track->GetTrackStatus();

    // 停止判定：fStopAndKill のみに限定
    if (status == fStopAndKill) {
      bool inTarget = false;

      if (auto* postPV = step->GetPostStepPoint()->GetPhysicalVolume()) {
        // 物理ボリューム名で判定
        if (postPV->GetName() == "NaI_phys") {
          inTarget = true;
        } else if (fDet) {
          // ロジカル一致で保険
          auto* detLV  = fDet->GetNaILogical();
          auto* postLV = postPV->GetLogicalVolume();
          if (detLV && postLV && detLV == postLV) inTarget = true;
        }
      }

      if (inTarget) {
        run->CountStoppedMu();
        G4cout << "[DEBUG] Mu+ stopped in target at "
               << track->GetPosition() / cm << " cm" << G4endl;
      }
    }
  }

  // --- 3. 他粒子の追加処理があればここ ---
}

}  // namespace B4c