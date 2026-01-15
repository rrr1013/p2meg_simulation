#ifndef CalorimeterDigitizer_h
#define CalorimeterDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "globals.hh"

class CalorimeterDigitizer : public G4VDigitizerModule {
public:
  CalorimeterDigitizer(const G4String& name,
                       const G4String& hitsCollectionPath);
  ~CalorimeterDigitizer() override = default;

  void Digitize() override;

private:
  G4String fHitsCollectionPath;
  G4int    fHCID;
};

#endif
