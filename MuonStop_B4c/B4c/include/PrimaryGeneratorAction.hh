#ifndef B4c_PrimaryGeneratorAction_h
#define B4c_PrimaryGeneratorAction_h 1
#include "G4VUserPrimaryGeneratorAction.hh"
class G4ParticleGun; class G4Event;
namespace B4c {
class PrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction{
 public:
  PrimaryGeneratorAction(); ~PrimaryGeneratorAction() override;
  void GeneratePrimaries(G4Event*) override;
 private: G4ParticleGun* fGun=nullptr;
};
}
#endif