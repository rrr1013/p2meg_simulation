// DetectorConstruction.hh
#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    void SetTargetSize(G4double x, G4double y, G4double z);
    virtual ~DetectorConstruction();

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

private:
    G4double fTargetX;
    G4double fTargetY;
    G4double fTargetZ;

    G4LogicalVolume* fPSLV  = nullptr;   // ★保持
    G4LogicalVolume* fNaILV = nullptr;   // ★保持
};

#endif
