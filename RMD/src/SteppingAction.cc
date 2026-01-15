#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

SteppingAction::SteppingAction() {}
SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    // ステップ開始点と終了点
    auto pre  = step->GetPreStepPoint();
    auto post = step->GetPostStepPoint();

    // 例：ターゲットから粒子が出た瞬間を検知したい場合
    if (pre->GetPhysicalVolume()  &&
        post->GetPhysicalVolume() &&
        pre->GetPhysicalVolume()->GetName() == "TargetPV" &&
        post->GetPhysicalVolume()->GetName() != "TargetPV")
    {
        auto momentum = post->GetMomentum();
        auto kineticE = post->GetKineticEnergy();

        //G4cout << "[SteppingAction] Particle exited target: "
        //       << "Momentum = " << momentum.mag()/MeV << " MeV/c, "
        //       << "Energy = " << kineticE/MeV << " MeV"
        //       << G4endl;
    }
}
