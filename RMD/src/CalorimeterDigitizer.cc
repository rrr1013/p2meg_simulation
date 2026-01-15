#include "CalorimeterDigitizer.hh"
#include "CalorimeterDigit.hh"
#include "SensitiveDetector.hh"

#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

#include <map>
#include <limits>

CalorimeterDigitizer::CalorimeterDigitizer(
  const G4String& name,
  const G4String& hitsCollectionPath)
: G4VDigitizerModule(name),
  fHitsCollectionPath(hitsCollectionPath),
  fHCID(-1)
{
  collectionName.push_back("DigitsCollection");
}

void CalorimeterDigitizer::Digitize()
{
  auto* digiMan = G4DigiManager::GetDMpointer();

  // --- HitsCollection ID（Digitizerごとに保持）
  if (fHCID < 0) {
    fHCID =
      G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollectionPath);
  }
  if (fHCID < 0) return;

  // --- HitsCollection を DigiManager から直接取得（★正解）
  auto* hits =
    static_cast<const SimpleHitsCollection*>(
      digiMan->GetHitsCollection(fHCID)
    );
  if (!hits) return;

  // --- channel(copyNo) ごとに集約
  std::map<G4int, G4double> energySum;
  std::map<G4int, G4double> timeMin;

  for (size_t i = 0; i < hits->entries(); ++i) {
    auto* hit = (*hits)[i];
    G4int ch = hit->GetCopyNo();

    energySum[ch] += hit->GetEdep();

    if (timeMin.find(ch) == timeMin.end())
      timeMin[ch] = hit->GetTime();
    else
      timeMin[ch] = std::min(timeMin[ch], hit->GetTime());
  }

  // --- DigitsCollection 作成
  auto* digiCollection =
    new CalorimeterDigitsCollection(GetName(), "DigitsCollection");

  for (auto& [ch, E] : energySum) {
    auto* digi = new CalorimeterDigit();
    digi->SetChannelID(ch);
    digi->SetEnergy(E);
    digi->SetTime(timeMin[ch]);
    digiCollection->insert(digi);
  }

  StoreDigiCollection(digiCollection);
}
