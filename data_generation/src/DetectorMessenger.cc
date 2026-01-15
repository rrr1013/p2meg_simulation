#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4RunManager.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction* det)
 : G4UImessenger(),
   fDetector(det)
{
    // Target のサイズ変更
    fTargetSizeCmd =
        new G4UIcmdWith3VectorAndUnit("/det/setTargetSize", this);

    fTargetSizeCmd->SetGuidance("Set target size (half length)");
    fTargetSizeCmd->SetParameterName("x","y","z", false);
    fTargetSizeCmd->SetDefaultUnit("mm");
}

DetectorMessenger::~DetectorMessenger()
{
    delete fTargetSizeCmd;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if(command == fTargetSizeCmd){

        auto v = fTargetSizeCmd->GetNew3VectorValue(newValue);

        fDetector->SetTargetSize(v.x(), v.y(), v.z());
    }
}
