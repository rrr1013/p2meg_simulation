#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"
#include "globals.hh"

//
// --- Hit class --------------------------------------------------
//
class SimpleHit : public G4VHit {
    public:
        SimpleHit();
        virtual ~SimpleHit();

        void AddEdep(G4double e) { fEdep += e; }
        G4double GetEdep() const { return fEdep; }

        void SetCopyNo(G4int c) { fCopyNo = c; }
        G4int GetCopyNo() const { return fCopyNo; }

        void SetTime(G4double t) { fTime = t; }
        G4double GetTime() const { return fTime; }

    private:
        G4double fEdep;
        G4int    fCopyNo = -1;   // ★ 角度ビンを保存
        G4double fTime   = 0.;   // ★ ヒット時刻を保存
};

using SimpleHitsCollection = G4THitsCollection<SimpleHit>;


//
// --- SensitiveDetector class ------------------------------------
//
class SensitiveDetector : public G4VSensitiveDetector {
public:
    SensitiveDetector(const G4String& name);
    virtual ~SensitiveDetector();

    virtual void Initialize(G4HCofThisEvent*);
    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    
private:
    SimpleHitsCollection* fHitsCollection = nullptr;
    G4int fHCID = -1;
};

#endif
