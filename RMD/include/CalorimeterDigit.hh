#ifndef CalorimeterDigit_h
#define CalorimeterDigit_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "globals.hh"

class CalorimeterDigit : public G4VDigi {
public:
  CalorimeterDigit();
  ~CalorimeterDigit() override = default;

  void SetChannelID(G4int v) { fChannelID = v; }
  void SetEnergy(G4double v) { fEnergy = v; }
  void SetTime(G4double v)   { fTime = v; }

  G4int    GetChannelID() const { return fChannelID; }
  G4double GetEnergy()    const { return fEnergy; }
  G4double GetTime()      const { return fTime; }

private:
  G4int    fChannelID;  // = copyNo
  G4double fEnergy;     // MeV
  G4double fTime;       // ns
};

using CalorimeterDigitsCollection =
    G4TDigiCollection<CalorimeterDigit>;

#endif
