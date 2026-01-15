#include "MyPhysicsList.hh"

// 電磁相互作用
#include "G4EmStandardPhysics_option4.hh"
// 崩壊
#include "G4DecayPhysics.hh"

// 粒子定義
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

// プロセス
#include "G4Decay.hh"
#include "G4DecayTable.hh"
#include "G4MuonRadiativeDecayChannelWithSpin.hh"
#include "G4MuToEGammaDecay.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ProcessManager.hh"

MyPhysicsList::MyPhysicsList()
{
    // ===== 物理モジュール登録 =====
    //
    // 1) 電磁相互作用 : option4（精度高めの標準EM）
    RegisterPhysics(new G4EmStandardPhysics_option4());

    // 2) 崩壊物理 : 全ての不安定粒子の decay を登録
    //    → ただし後で μ± だけ RMD に上書きする
    RegisterPhysics(new G4DecayPhysics());
}

MyPhysicsList::~MyPhysicsList() {}

void MyPhysicsList::ConstructParticle()
{
    // 登録済み PhysicsConstructor (Em, Decay など) に粒子定義をやらせる
    G4VModularPhysicsList::ConstructParticle();
}

void MyPhysicsList::ConstructProcess()
{
    // EM, Decay など登録済みモジュールのプロセスをまず構築
    G4VModularPhysicsList::ConstructProcess();

    // そのあとで「μ± の decay table だけ RMD に差し替える」
    auto particleIterator = GetParticleIterator();
    particleIterator->reset();

    while ((*particleIterator)())
    {
        G4ParticleDefinition* particle = particleIterator->value();
        if (particle == G4MuonPlus::MuonPlusDefinition() ||
            particle == G4MuonMinus::MuonMinusDefinition())
        {
            // --- μ± の decay table を RMD のみにする ---

            // 既存の decayTable は Geant4 側が持っているので
            // ユーザ側では単に新しい table を作って差し替えればよい
            auto* table = new G4DecayTable();

            // BranchingRatio = 1.0 で 100% RMD
            //table->Insert(new G4MuonRadiativeDecayChannelWithSpin(
            //     particle->GetParticleName(), 1.0));

            // BranchingRatio = 1.0 で 100% MEG
            table->Insert(new G4MuToEGammaDecay(
                particle->GetParticleName(), 1.0));

            // 既存の decay channel を捨てて RMD のみに
            particle->SetDecayTable(table);
        }
    }
}

void MyPhysicsList::SetCuts()
{
    // デフォルトの EM カット長を 1 mm に設定
    SetDefaultCutValue(0.1*mm);

    // デフォルトセットを実行
    SetCutsWithDefault();
}
