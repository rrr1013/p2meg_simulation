#include "DetectorConstruction.hh"
#include "TargetConfig.hh"

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4Material.hh"
#include "G4ios.hh"

// for region-specific production cuts
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4ProductionCuts.hh"

namespace B4c {

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}
DetectorConstruction::~DetectorConstruction() = default;

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  auto nist = G4NistManager::Instance();

  // --- Materials ---
  auto worldMat  = nist->FindOrBuildMaterial(TargetConfig::kWorldMaterial);
  auto targetMat = nist->FindOrBuildMaterial(TargetConfig::kTargetMaterial);

  // --- World ---
  auto worldSolid = new G4Box("World", TargetConfig::kWorldHalf, TargetConfig::kWorldHalf, TargetConfig::kWorldHalf);
  auto worldLV    = new G4LogicalVolume(worldSolid, worldMat, "WorldLV");
  auto worldPV    = new G4PVPlacement(nullptr, {}, worldLV, "WorldPV", nullptr, false, 0, true);

  // --- Target slab (thin film) ---
  auto targetSolid = new G4Box("Target",
                               TargetConfig::kTargetHalfX,
                               TargetConfig::kTargetHalfY,
                               TargetConfig::kTargetHalfZ); // half-length along Z
  fTargetLog = new G4LogicalVolume(targetSolid, targetMat, "TargetLV");
  new G4PVPlacement(nullptr,
                    {TargetConfig::kTargetPosX, TargetConfig::kTargetPosY, TargetConfig::kTargetPosZ},
                    fTargetLog, "TargetPV", worldLV, false, 0, true);

  // --- Simple vis attributes ---
  auto wVis = new G4VisAttributes(G4Colour(0.9,0.9,0.9,0.0)); // invisible wireframe
  wVis->SetVisibility(false);
  worldLV->SetVisAttributes(wVis);

  auto tVis = new G4VisAttributes(G4Colour(0.2,0.8,1.0,0.3));
  tVis->SetForceSolid(true);
  fTargetLog->SetVisAttributes(tVis);

  // Startup log
  G4cout << "[Geom] World=" << worldLV->GetMaterial()->GetName()
         << "  Target=" << fTargetLog->GetMaterial()->GetName()
         << "  Size(half)=(" << TargetConfig::kTargetHalfX/mm << " mm, "
                             << TargetConfig::kTargetHalfY/mm << " mm, "
                             << TargetConfig::kTargetHalfZ/mm << " mm)" << G4endl;

  return worldPV;
}

void DetectorConstruction::ConstructSDandField()
{
  // --- Region-specific production cuts for the thin film ---
  if (!fTargetLog) return;

  // create or fetch "TargetRegion"
  G4Region* reg = G4RegionStore::GetInstance()->FindOrCreateRegion("TargetRegion");
  reg->AddRootLogicalVolume(fTargetLog);

  // set small cuts (example: 10 um for e±, 10 um for gamma)
  auto cuts = new G4ProductionCuts();
  cuts->SetProductionCut(10.*um, G4ProductionCuts::GetIndex("e-"));
  cuts->SetProductionCut(10.*um, G4ProductionCuts::GetIndex("e+"));
  cuts->SetProductionCut(10.*um, G4ProductionCuts::GetIndex("gamma"));
  // optional: for hadrons if needed
  // cuts->SetProductionCut(10.*um, G4ProductionCuts::GetIndex("proton"));

  reg->SetProductionCuts(cuts);

  G4cout << "[Cuts] TargetRegion: e-/e+ = 10 um, gamma = 10 um" << G4endl;
}

} // namespace B4c
