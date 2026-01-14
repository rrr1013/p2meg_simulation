#ifndef B4c_DetectorConstruction_h
#define B4c_DetectorConstruction_h 1
#include "G4VUserDetectorConstruction.hh"
class G4LogicalVolume; class G4VPhysicalVolume;
namespace B4c {
class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction(); ~DetectorConstruction() override;
  G4VPhysicalVolume* Construct() override;
  void ConstructSDandField() override;
  G4LogicalVolume* GetNaILogical() const { return fTargetLog; } // 互換のためこの名前
private:
  void DefineMaterials();
  G4LogicalVolume* fTargetLog = nullptr;
  const bool fCheckOverlaps = true;
};
} // namespace B4c
#endif