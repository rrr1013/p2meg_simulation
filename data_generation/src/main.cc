#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "ActionInitialization.hh"
#include "MyPhysicsList.hh"

int main(int argc, char** argv)
{
    // RunManager (MT)
    auto runManager = new G4MTRunManager();
    auto* det = new DetectorConstruction();

    runManager->SetNumberOfThreads(7); //軽くするなら5がいい

    runManager->SetUserInitialization(det);
    runManager->SetUserInitialization(new MyPhysicsList());
    runManager->SetUserInitialization(new ActionInitialization());

    auto* detMessenger = new DetectorMessenger(det);


    // ===============================
    // Qt UI は /vis/open より前に作る
    // ===============================
    G4UIExecutive* ui = nullptr;
    if (argc == 1) {
        ui = new G4UIExecutive(argc, argv, "Qt");
    }

    // Visualization Manager
    auto visManager = new G4VisExecutive();
    visManager->Initialize();

    runManager->Initialize();

    auto UImanager = G4UImanager::GetUIpointer();

    if (ui) {
        // UI を作った後に vis.mac を実行！
        UImanager->ApplyCommand("/vis/initialize");
        UImanager->ApplyCommand("/vis/open");   // ← ここで OGLSQt を開く
        UImanager->ApplyCommand("/vis/viewer/set/style surface");
        UImanager->ApplyCommand("/vis/drawVolume");
        UImanager->ApplyCommand("/vis/scene/add/trajectories smooth");

        UImanager->ApplyCommand("/tracking/verbose 1");
        UImanager->ApplyCommand("/process/verbose 2");
        ui->SessionStart();
        delete ui;
    } else {
        // バッチモード
        UImanager->ApplyCommand("/control/execute " + G4String(argv[1]));
    }

    delete runManager;
    delete visManager;
    return 0;
}
