#ifndef B4c_SteppingAction_h
#define B4c_SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class G4Step;
class G4LogicalVolume;

namespace B4c {

class DetectorConstruction;

class SteppingAction : public G4UserSteppingAction {
public:
  explicit SteppingAction(const DetectorConstruction* det);
  ~SteppingAction() override;

  void UserSteppingAction(const G4Step*) override;

private:
  const DetectorConstruction* fDet;
};

}  // namespace B4c

#endif