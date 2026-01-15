#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
    // ParticleGun（1 粒子）を作成
    fParticleGun = new G4ParticleGun(1);

    // 粒子の種類：mu+
    auto particleDefinition =
        G4ParticleTable::GetParticleTable()->FindParticle("mu+");
    fParticleGun->SetParticleDefinition(particleDefinition);

    // 位置：ターゲット中心を仮に (0, 0, 0) とする
    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.));

    // 運動量方向：適当に z+
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));

    // 運動エネルギー
    // 静止 mu+ を作りたい場合は 0 MeV
    fParticleGun->SetParticleEnergy(0.*MeV);

    // -z方向に100%偏極
    fParticleGun->SetParticlePolarization(G4ThreeVector(0., 0., -1.));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    // ここで粒子が1イベントにつき1発生する
    fParticleGun->GeneratePrimaryVertex(anEvent);
}
