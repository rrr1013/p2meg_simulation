#include "RunAction.hh"

#include "G4AccumulableManager.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

namespace B4c {

RunAction::RunAction()
{
  auto* ana = G4AnalysisManager::Instance();
  ana->SetVerboseLevel(1);
  ana->SetNtupleMerging(true);

  // ここに既存の H1/Ntuple 作成があればそのまま残す

  // ★ Geant4 11 の新API推奨：Register()
  auto* acc = G4AccumulableManager::Instance();
  acc->Register(fNMu);
  acc->Register(fNMuStopped);
}

RunAction::~RunAction() = default;

void RunAction::BeginOfRunAction(const G4Run*)
{
  auto* acc = G4AccumulableManager::Instance();
  acc->Reset();

  // 既存の OpenFile() などがあればそのまま
}

void RunAction::EndOfRunAction(const G4Run*)
{
  auto* acc = G4AccumulableManager::Instance();
  acc->Merge();

  const auto nMu        = fNMu.GetValue();
  const auto nMuStopped = fNMuStopped.GetValue();
  const double frac = (nMu > 0) ? double(nMuStopped)/double(nMu) : 0.0;

  // ★ Workerスレッドでは出さず、マスターのみ出力
  if (IsMaster()) {
    G4cout << "==== Muon stopping summary ====" << G4endl;
    G4cout << "  Primary mu+ count     : " << nMu << G4endl;
    G4cout << "  Stopped mu+ in target : " << nMuStopped << G4endl;
    G4cout << "  Fraction stopped      : " << frac << G4endl;
    G4cout << "===============================" << G4endl;
  }
}

} // namespace B4c