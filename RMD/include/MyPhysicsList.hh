#ifndef MY_PHYSICS_LIST_HH
#define MY_PHYSICS_LIST_HH

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class MyPhysicsList : public G4VModularPhysicsList {
public:
    MyPhysicsList();
    ~MyPhysicsList() override;

protected:
    void ConstructParticle() override;
    void ConstructProcess() override;
    void SetCuts() override;
};

#endif
