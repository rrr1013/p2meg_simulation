#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4Event;

class EventAction : public G4UserEventAction {
public:
    EventAction();                    // ← 引数なし
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event*) override;
    void EndOfEventAction(const G4Event*) override;

private:
    // ヒットコレクション ID
    G4int fNaIHCID;
    G4int fPSHCID;
    // Digits
    G4int fNaIDCID;
    G4int fPSDCID;

    // しきい値・coincidence 窓
    G4double fEthNaI;
    G4double fEthPS;
    G4double fCoincDT;
};

#endif
