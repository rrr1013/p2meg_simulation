#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIcmdWith3VectorAndUnit;

class DetectorMessenger : public G4UImessenger
{
public:
    DetectorMessenger(DetectorConstruction* det);
    virtual ~DetectorMessenger();

    virtual void SetNewValue(G4UIcommand* command, G4String newValue);

private:
    DetectorConstruction* fDetector;

    G4UIcmdWith3VectorAndUnit* fTargetSizeCmd;
};

#endif
