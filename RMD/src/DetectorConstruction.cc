// DetectorConstruction.cc
#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"
#include "Constants.hh"  // kNbin を使う
#include "G4SDManager.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"
#include "G4ProductionCuts.hh"
#include "G4Region.hh"

DetectorConstruction::DetectorConstruction()
 : fTargetX(10*mm),  // 元のサイズ
   fTargetY(10*mm),
   fTargetZ(10*um)
{}
DetectorConstruction::~DetectorConstruction() {}

void DetectorConstruction::SetTargetSize(G4double x, G4double y, G4double z)
{
    fTargetX=x; fTargetY=y; fTargetZ=z;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // 材料マネージャ
    auto nist = G4NistManager::Instance();

    auto worldMat = nist->FindOrBuildMaterial("G4_Galactic");
    auto targetMat    = nist->FindOrBuildMaterial("G4_Al");
    auto psMat    = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    auto naiMat   = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");

    //---------------------------
    //  World
    //---------------------------
    G4double worldSize = 2.5*m;
    G4double worldSizeY = 2.5*m;

    auto worldSolid =
        new G4Box("World", worldSize/2, worldSizeY/2, worldSize/2);

    auto worldLV =
        new G4LogicalVolume(worldSolid, worldMat, "WorldLV");

    auto worldPV =
        new G4PVPlacement(0,                     // 回転なし
                          G4ThreeVector(),       // (0,0,0)
                          worldLV,
                          "WorldPV",
                          0,
                          false,
                          0);

    //Worldをワイヤーフレームにする
    auto worldVis = new G4VisAttributes();
    worldVis->SetForceWireframe(true);
    worldLV->SetVisAttributes(worldVis);

    //---------------------------
    //  Target（Al）
    //---------------------------

    auto targetSolid = new G4Box("Target", fTargetX/2, fTargetY/2, fTargetZ/2);

    auto targetLV =
        new G4LogicalVolume(targetSolid, targetMat, "TargetLV");

    new G4PVPlacement(
        0,                      // 回転なし
        G4ThreeVector(0,0,0),   // World の中心に置く
        targetLV,
        "TargetPV",
        worldLV,
        false,
        0);

    //---------------------------
    //  Detector
    //---------------------------
    // --- 1 モジュールの形状（ロジックだけ先に作る）
    G4double psX = 10*cm, psY = 10*cm, psZ = 2*cm; //PSのサイズ。
    G4double naiZ = 30*cm;

    auto psSolid =
        new G4Box("PS", psX/2, psY/2, psZ/2);
    auto psLV =
        new G4LogicalVolume(psSolid, psMat, "PSLV");

    auto naiSolid =
        new G4Box("NaI", psX/2, psY/2, naiZ/2); //NaI
    auto naiLV =
    new G4LogicalVolume(naiSolid, naiMat, "NaILV");


    //角度ビンごとに配置
    int    Nbin   = kNbin;          // 10° ごとに 36 ビン
    double phiMin = 0.0*deg;
    double phiMax = 360.0*deg;
    double dphi   = (phiMax - phiMin)/Nbin;

    double Rps  = 80*cm + psZ/2;  // ターゲット中心から PS中心までの距離
    double Rnai = Rps + psZ/2 + naiZ/2;  // PSのすぐ後ろにNaI

    for(int i = 0; i < Nbin; ++i) {
        double phi = phiMin + (i + 0.5)*dphi;

        // --- PS の中心位置
        G4ThreeVector psPos( Rps * std::sin(phi),
                            0.0,
                            Rps * std::cos(phi) );

        // --- NaI の中心位置
        G4ThreeVector naiPos( Rnai * std::sin(phi),
                            0.0,
                            Rnai * std::cos(phi) );

        // --- 回転行列（y 軸まわりに -phi 回転してターゲット向きに）
        auto rot = new G4RotationMatrix();
        rot->rotateY(-phi);  // あるいは rotateZ(phi) など、座標系に応じて調整

        // --- PS 配置 (copyNo = i)
        new G4PVPlacement(rot, psPos,
                        psLV, "PSPV",
                        worldLV,
                        false, i, true);

        // --- NaI 配置 (copyNo = i)
        new G4PVPlacement(rot, naiPos,
                        naiLV, "NaIPV",
                        worldLV,
                        false, i, true);
    }

    // PS / NaI LogicalVolume をメンバに保存
    fPSLV  = psLV;
    fNaILV = naiLV;

    // ===== ここから Region 設定 =====
    // --- NaI Region ---
    auto naiRegion = new G4Region("NaIRegion");
    naiLV->SetRegion(naiRegion);
    naiRegion->AddRootLogicalVolume(naiLV);

    auto naiCuts = new G4ProductionCuts();
    naiCuts->SetProductionCut(0.01*mm);
    naiRegion->SetProductionCuts(naiCuts);

    // --- PS Region ---
    auto psRegion = new G4Region("PSRegion");
    psLV->SetRegion(psRegion);
    psRegion->AddRootLogicalVolume(psLV);

    auto psCuts = new G4ProductionCuts();
    psCuts->SetProductionCut(0.01*mm);
    psRegion->SetProductionCuts(psCuts);
    // ===== ここまで =====

    return worldPV;
}

void DetectorConstruction::ConstructSDandField()
{
    if (!fNaILV || !fPSLV) {
        G4Exception("DetectorConstruction::ConstructSDandField",
                    "DC001", FatalException,
                    "LogicalVolume pointers are null. Construct() may not have set fNaILV/fPSLV.");
        return;
    }

    auto sdManager = G4SDManager::GetSDMpointer();

    // --- NaI SD ---
    auto naiSD = new SensitiveDetector("NaISD");
    sdManager->AddNewDetector(naiSD);
    fNaILV->SetSensitiveDetector(naiSD);

    // --- PS SD ---
    auto psSD = new SensitiveDetector("PSSD");
    sdManager->AddNewDetector(psSD);
    fPSLV->SetSensitiveDetector(psSD);
}