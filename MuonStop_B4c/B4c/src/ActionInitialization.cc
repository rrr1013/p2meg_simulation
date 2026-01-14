#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"

#include "G4RunManager.hh"

namespace B4c {

ActionInitialization::ActionInitialization() = default;
ActionInitialization::~ActionInitialization() = default;

//=========================================
// マルチスレッド版のMasterスレッド初期化
//=========================================
void ActionInitialization::BuildForMaster() const
{
  // マスターは RunAction だけを持てばOK（粒子ガンやステップは不要）
  SetUserAction(new B4c::RunAction());
}

//=========================================
// 通常のスレッド用 (Worker thread用)
//=========================================
void ActionInitialization::Build() const
{
  // DetectorConstruction の取得
  auto* det = static_cast<const DetectorConstruction*>(
      G4RunManager::GetRunManager()->GetUserDetectorConstruction());

  // 各UserAction登録
  SetUserAction(new B4c::PrimaryGeneratorAction());
  SetUserAction(new B4c::RunAction());
  SetUserAction(new B4c::SteppingAction(det));
}

}  // namespace B4c