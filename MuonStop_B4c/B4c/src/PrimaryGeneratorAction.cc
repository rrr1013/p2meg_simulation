#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
namespace B4c {
PrimaryGeneratorAction::PrimaryGeneratorAction(){
  fGun=new G4ParticleGun(1);
  auto p=G4ParticleTable::GetParticleTable()->FindParticle("e+");
  fGun->SetParticleDefinition(p);
  fGun->SetParticleEnergy(53.*MeV);
  fGun->SetParticleMomentumDirection({0.,0.,1.});
  fGun->SetParticlePosition({0.,0.,-49.*cm}); // ワールド端の1cm内側
}
PrimaryGeneratorAction::~PrimaryGeneratorAction(){ delete fGun; }
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* ev){ fGun->GeneratePrimaryVertex(ev); }
}