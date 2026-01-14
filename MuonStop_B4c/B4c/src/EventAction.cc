#include "EventAction.hh"
#include "EdepAccumulator.hh"
#include "AnalysisIds.hh"

#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

namespace B4c {

void EventAction::BeginOfEventAction(const G4Event*)
{
  EdepAccumulator::edepNaI = 0.0;
}

void EventAction::EndOfEventAction(const G4Event*)
{
  auto ana = G4AnalysisManager::Instance();

  // 念のため：このスレッドで未作成なら作る（怠惰生成）
  if (AnalysisIds::h1EdepNaI < 0) {
    AnalysisIds::h1EdepNaI =
      ana->CreateH1("Edep_NaI",
                    "Energy deposited in NaI;E_{dep} [MeV];Events",
                    600, 0., 60.);
  }

  ana->FillH1(AnalysisIds::h1EdepNaI, EdepAccumulator::edepNaI / MeV);
}

} // namespace B4c