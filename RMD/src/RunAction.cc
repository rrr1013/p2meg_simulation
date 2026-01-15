// RunAction.cc  (FIXED)
#include "RunAction.hh"
#include "MyRun.hh"
#include "CalorimeterDigitizer.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Threading.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4DigiManager.hh"

#include "TFile.h"
#include "TTree.h"

//#include <fstream>
#include <cmath>

// 修正点: 独自Runを生成
G4Run* RunAction::GenerateRun()
{
    return new MyRun();
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
    fTimer.Start();   //タイマースタート

    // どちらでも表示してよいが、分かりやすくスレッドIDを付ける
    auto type = G4RunManager::GetRunManager()->GetRunManagerType();
    G4int runID = run->GetRunID();

    if (type == G4RunManager::masterRM) {
        G4cout << "### [MASTER] Run " << runID << " start." << G4endl;
    } else {
        G4cout << "### [WORKER " << G4Threading::G4GetThreadId()
               << "] Run " << runID << " start." << G4endl;
    }
    
    // ================================
    // ★ Digitizer 登録（ここだけ追加）
    // ================================
    auto digiMan = G4DigiManager::GetDMpointer();

    // ---- NaI ----
    if (!digiMan->FindDigitizerModule("NaIDigi")) {
        auto naiDigi = new CalorimeterDigitizer("NaIDigi", "NaISD/HitsCollection");
        digiMan->AddNewModule(naiDigi);
        G4cout << "[RunAction] NaIDigi registered" << G4endl;
    }

    // ---- PS ---- ★追加
    if (!digiMan->FindDigitizerModule("PSDigi")) {
        auto psDigi = new CalorimeterDigitizer("PSDigi", "PSSD/HitsCollection");
        digiMan->AddNewModule(psDigi);
        G4cout << "[RunAction] PSDigi registered" << G4endl;
    }

    // 修正点: ここで fNmu/fNcoinc を RunAction メンバで初期化しない
    // （集計は MyRun が保持するので不要）
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    fTimer.Stop();   // タイマーストップ
    
    auto type = G4RunManager::GetRunManager()->GetRunManagerType();
    G4int runID = run->GetRunID();

    if (type == G4RunManager::masterRM) {
        G4cout << "### [MASTER] Run " << runID << " end." << G4endl;

        G4cout << "==================================" << G4endl;
        G4cout << " Run CPU time : "
               << fTimer.GetUserElapsed()
               << " s" << G4endl;
        G4cout << " Run wall time : "
               << fTimer.GetRealElapsed()
               << " s" << G4endl;
        G4cout << "==================================" << G4endl;

        // 修正点: Master だけがファイル出力（workerが同名ファイルを書かない）
        const auto* myRun = static_cast<const MyRun*>(run);

        const int Nmu = myRun->GetNmu();
        //const auto& Ncoinc = myRun->GetNcoinc();
        //const int    Ntheta = Ncoinc.size(); //追加
        //const double dtheta = CLHEP::pi / Ntheta; //追加

        // ----------------------------
        // ROOT file & tree
        // ----------------------------
        TFile* file = new TFile("MEG_simulation_dataset.root", "RECREATE");
        TTree* tree = new TTree("coinc", "Coincidence data");

        Double_t Epos, Egam, dt, theta, cos_pos, cos_gam;

        tree->Branch("Epos",  &Epos,  "Epos/D");
        tree->Branch("Egam",  &Egam,  "Egam/D");
        tree->Branch("dt",    &dt,    "dt/D");
        tree->Branch("theta", &theta, "theta/D");
        tree->Branch("cos_pos",  &cos_pos,  "cos_pos/D");
    tree->Branch("cos_gam",  &cos_gam,  "cos_gam/D");

        // Fill tree
        for (const auto& d : myRun->GetCoincData()) {
            Epos  = d.Epos;
            Egam  = d.Egam;
            dt    = d.dt;
            theta = d.theta;
            cos_pos  = d.cos_pos;
            cos_gam  = d.cos_gam;
            tree->Fill();
        }

        tree->Write();
        file->Close();
        delete file;

        const auto& coincData = myRun->GetCoincData();
        const int Ntrig = coincData.size();

        G4cout << "ROOT file written: MEG_simulation_dataset.root" << G4endl;

        G4cout << "==================================" << G4endl;
        G4cout << "Run finished summary" << G4endl;
        G4cout << "  Nmu   (generated muons) = " << Nmu   << G4endl;
        G4cout << "  Ntrig (saved events)    = " << Ntrig << G4endl;
        G4cout << "==================================" << G4endl;

    } else {
        // worker側は通常、ファイル出力しない（ログだけならOK）
        G4cout << "### [WORKER " << G4Threading::G4GetThreadId()
               << "] Run " << runID << " end." << G4endl;
    }
}
