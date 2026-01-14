// ********************************************************************
// * Example B4c (modified)                                           *
// * Main program                                                     *
// ********************************************************************

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"

#include "G4RunManagerFactory.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4SteppingVerbose.hh"

// Physics list factory & constructors
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4DecayPhysics.hh"
#include "FTFP_BERT.hh"  // fallback
#include "G4EmStandardPhysics_option4.hh"

#include <cstdlib>
#include <string>

//------------------------------------------------------------------------------
// helper to print usage
static void PrintUsage() {
  G4cerr << "Usage: exampleB4c [-m macro.mac] [-t nThreads]\n";
}

int main(int argc, char** argv)
{
  // --- parse CLI args (minimal) ---
  G4String macro;
  G4int nThreads = 0;
#ifdef G4MULTITHREADED
  for (int i=1; i<argc; ++i) {
    G4String a = argv[i];
    if      (a == "-m" && i+1 < argc) { macro = argv[++i]; }
    else if (a == "-t" && i+1 < argc) { nThreads = std::atoi(argv[++i]); }
    else { PrintUsage(); return 1; }
  }
#else
  for (int i=1; i<argc; ++i) {
    G4String a = argv[i];
    if      (a == "-m" && i+1 < argc) { macro = argv[++i]; }
    else { PrintUsage(); return 1; }
  }
#endif

  // optional: best units in stepping verbose
  G4SteppingVerbose::UseBestUnit(4);

  // --- UI exec if interactive ---
  G4UIExecutive* ui = nullptr;
  if (macro.empty()) {
    ui = new G4UIExecutive(argc, argv);
  }

  // --- Run manager ---
  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
#ifdef G4MULTITHREADED
  if (nThreads > 0) { runManager->SetNumberOfThreads(nThreads); }
#endif

  // --- User init: detector & physics & actions ---
  runManager->SetUserInitialization(new B4c::DetectorConstruction());

  // --- Physics list: FTFP_BERT_EMZ (i.e., EM option4) ---
  G4PhysListFactory factory;
  G4VModularPhysicsList* physicsList = nullptr;

  if (factory.IsReferencePhysList("FTFP_BERT_EMZ")) {
    physicsList = factory.GetReferencePhysList("FTFP_BERT_EMZ");
  } else {
    // Fallback: start from FTFP_BERT and replace EM with option4
    physicsList = new FTFP_BERT();
    // Replace EM part with option4 if API is available; otherwise register on top
#if G4VERSION_NUMBER >= 1100
    physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
#else
    physicsList->RegisterPhysics(new G4EmStandardPhysics_option4());
#endif
  }
  // Natural decays (mu+ lifetime etc.)
  physicsList->RegisterPhysics(new G4DecayPhysics());
  runManager->SetUserInitialization(physicsList);

  // actions
  runManager->SetUserInitialization(new B4c::ActionInitialization());

  // --- Visualization ---
  auto* visManager = new G4VisExecutive();
  visManager->Initialize();

  auto* UImanager = G4UImanager::GetUIpointer();

  if (!macro.empty()) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command + macro);
  } else {
    // interactive
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;
  return 0;
}
