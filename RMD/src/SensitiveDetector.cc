#include "SensitiveDetector.hh"

#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

//
// --- SimpleHit implementation -----------------------------------
//
SimpleHit::SimpleHit()
: fEdep(0.)
{}
SimpleHit::~SimpleHit() {}


//
// --- SensitiveDetector implementation ---------------------------
//
SensitiveDetector::SensitiveDetector(const G4String& name)
: G4VSensitiveDetector(name)
{
  collectionName.insert("HitsCollection");  // ★これが必須
}
SensitiveDetector::~SensitiveDetector() {}


// イベントの最初に一度呼ばれる
void SensitiveDetector::Initialize(G4HCofThisEvent* hce) {
    //G4cout << "[DEBUG] SD " << SensitiveDetectorName
    //   << " collectionName size = " << collectionName.size() << G4endl;

    fHitsCollection =
        new SimpleHitsCollection(SensitiveDetectorName, "HitsCollection");

    if (fHCID < 0) {
        fHCID = GetCollectionID(0);   // 0番目のコレクションID
    }

    hce->AddHitsCollection(fHCID, fHitsCollection);
}


// ステップごとに呼ばれ、ヒットが作成される
G4bool SensitiveDetector::ProcessHits(G4Step* step,
                                      G4TouchableHistory*) {

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.) return false;

    // ★ ヒットした volume の copyNo（角度ビン番号）を取得
    auto touch = step->GetPreStepPoint()->GetTouchable();
    G4int copyNo = touch->GetCopyNumber();

    // ★ 時刻も取得
    G4double time = step->GetPreStepPoint()->GetGlobalTime();

    // ヒット作成
    auto hit = new SimpleHit();
    hit->AddEdep(edep);

    // ★ 角度ビン番号と時刻を保存
    hit->SetCopyNo(copyNo);
    hit->SetTime(time);

    fHitsCollection->insert(hit);

    return true;
}
