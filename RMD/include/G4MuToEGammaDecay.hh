#ifndef G4MuToEGammaDecay_hh
#define G4MuToEGammaDecay_hh

#include "G4VDecayChannel.hh"
#include "G4ThreeVector.hh"

class G4MuToEGammaDecay : public G4VDecayChannel
{
public:
  G4MuToEGammaDecay(const G4String& parentName, G4double br);
  ~G4MuToEGammaDecay() override;

  G4DecayProducts* DecayIt(G4double) override;

private:
  G4double GenerateCosTheta(G4double Pmu) const;
};

#endif
