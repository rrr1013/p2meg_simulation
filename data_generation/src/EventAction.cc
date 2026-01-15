// EventAction.cc
#include "EventAction.hh"
#include "SensitiveDetector.hh"
#include "MyRun.hh"
#include "Constants.hh"

#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

// 追加
#include "G4ios.hh"
#include <cmath>
#include <limits> 

EventAction::EventAction()
: G4UserEventAction(),
  fEthNaI(10.*MeV),
  fEthPS(1.*MeV),
  fCoincDT(10.*ns),
  fPSHCID(-1),
  fNaIHCID(-1)
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
    // --- safe run pointer ---
    auto* baseRun = G4RunManager::GetRunManager()->GetNonConstCurrentRun();
    auto* myRun   = dynamic_cast<MyRun*>(baseRun);
    if (!myRun) {
        G4cout << "[FATAL] CurrentRun is not MyRun. Check RunAction::GenerateRun()." << G4endl;
        return;
    }

    // 1 event = 1 μ とするならここで加算
    myRun->AddMuon();
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    // --- safe run pointer ---
    auto* baseRun = G4RunManager::GetRunManager()->GetNonConstCurrentRun();
    auto* myRun   = dynamic_cast<MyRun*>(baseRun);
    if (!myRun) {
        G4cout << "[FATAL] CurrentRun is not MyRun. Check RunAction::GenerateRun()." << G4endl;
        return;
    }

    auto hce = event->GetHCofThisEvent();
    if (!hce) return;

    // --- HCID を取得（初回のみ）---
    if (fPSHCID < 0 || fNaIHCID < 0) {
        auto sdManager = G4SDManager::GetSDMpointer();

        if (fPSHCID < 0) {
            fPSHCID  = sdManager->GetCollectionID("PSSD/HitsCollection");
            G4cout << "[DEBUG] PSHCID  = " << fPSHCID << G4endl;
        }
        if (fNaIHCID < 0) {
            fNaIHCID = sdManager->GetCollectionID("NaISD/HitsCollection");
            G4cout << "[DEBUG] NaIHCID = " << fNaIHCID << G4endl;
        }
    }
    if (fPSHCID < 0 || fNaIHCID < 0) return;

    auto psHits  = static_cast<SimpleHitsCollection*>(hce->GetHC(fPSHCID));
    auto naiHits = static_cast<SimpleHitsCollection*>(hce->GetHC(fNaIHCID));
    if (!psHits || !naiHits) return;

    // デバッグ（必要なら）
    // G4cout << "[DEBUG] PS hits size  = " << psHits->GetSize()
    //        << "  NaI hits size = " << naiHits->GetSize() << G4endl;

    // --- bin-wise hit information ---
    struct HitInfo {
        bool   hitPS   = false;
        bool   hitNaI  = false;
        double timePS  = std::numeric_limits<double>::max();
        double timeNaI = std::numeric_limits<double>::max();
        double edepPS  = 0.0;
        double edepNaI = 0.0;
    };

    std::vector<HitInfo> hitMap(kNbin);

    // ===============================
    // PS hits
    // ===============================
    for (size_t i = 0; i < psHits->GetSize(); ++i) {
        auto hit = (*psHits)[i];
        int bin  = hit->GetCopyNo();

        if (bin < 0 || bin >= kNbin) continue;   // ★FIX: safety

        auto& info = hitMap[bin];

        info.edepPS += hit->GetEdep();           // ★FIX: 先に全加算
        info.timePS = std::min(info.timePS,
                               hit->GetTime()); // ★FIX: 最小時刻
    }

    // ===============================
    // NaI hits
    // ===============================
    for (size_t i = 0; i < naiHits->GetSize(); ++i) {
        auto hit = (*naiHits)[i];
        int bin  = hit->GetCopyNo();

        if (bin < 0 || bin >= kNbin) continue;   // ★FIX: safety

        auto& info = hitMap[bin];

        info.edepNaI += hit->GetEdep();          // ★FIX: 先に全加算
        info.timeNaI = std::min(info.timeNaI,
                                hit->GetTime()); // ★FIX: 最小時刻
    }

    // ===============================
    // ★FIX: ここで初めてしきい値判定
    // ===============================
    for (int bin = 0; bin < kNbin; ++bin) {
        auto& info = hitMap[bin];

        info.hitPS  = (info.edepPS  >= fEthPS);
        info.hitNaI = (info.edepNaI >= fEthNaI);
    }

    // // --- back-to-back e+ – gamma coincidence ---
    // // 前提：kNbin は偶数（180度反対側が整数ビンになるため）
    // // 必要なら Constants.hh 側で static_assert してもOK

    // for (int bin_e = 0; bin_e < kNbin; ++bin_e) {
    //     const auto& info_e = hitMap[bin_e];

    //     // positron candidate: PS + NaI in same bin
    //     if (!(info_e.hitPS && info_e.hitNaI)) continue;

    //     // PS–NaI time coincidence for e+ candidate
    //     const double dt_e = std::abs(info_e.timePS - info_e.timeNaI);
    //     if (dt_e > fCoincDT) continue;

    //     // opposite bin for gamma
    //     const int bin_g = (bin_e + kNbin/2) % kNbin;

    //     const auto& info_g = hitMap[bin_g];

    //     // gamma candidate: NaI only (no PS)
    //     if (!(info_g.hitNaI && !info_g.hitPS)) continue;

    //     // NaI–NaI time coincidence between e+ side and gamma side
    //     const double dt_eg = std::abs(info_e.timeNaI - info_g.timeNaI);
    //     if (dt_eg > fCoincDT) continue;

    //     // coincidence found (count once per matched bin_e)
    //     myRun->AddCoincidence(bin_e, 1);

    //     // デバッグ用（必要なら）
    //     // G4cout << "[DEBUG] Coincidence: bin_e=" << bin_e
    //     //        << " bin_g=" << bin_g
    //     //        << " dt_e=" << dt_e/ns << " ns"
    //     //        << " dt_eg=" << dt_eg/ns << " ns" << G4endl;
    // }
        
    
    
    // --- angle distribution (first-hit only) ---

    const double dphi = 2.0 * CLHEP::pi / kNbin;

    // ===============================
    // 1) e+ : 最初に入った bin を選ぶ
    // ===============================
    int best_bin_e = -1;
    double best_time_e = std::numeric_limits<double>::max();

    for (int bin = 0; bin < kNbin; ++bin) {
        const auto& info = hitMap[bin];

        if (!(info.hitPS && info.hitNaI)) continue;

        //NaIに最初に入った時刻（実験のe側トリガーがNaI基準ならこれ）
        const double t_e = info.timeNaI;

        if (t_e < best_time_e) {
            best_time_e = t_e;
            best_bin_e  = bin;
        }
    }

    // e+ が見つからなければ終了
    if (best_bin_e < 0) return;

    // ===============================
    // 2) gamma : 最初の NaI hit で |dt| 最小
    // ===============================
    int best_bin_g = -1;
    double best_dt = std::numeric_limits<double>::max();

    for (int bin = 0; bin < kNbin; ++bin) {
        if (bin == best_bin_e) continue;

        const auto& info = hitMap[bin];

        if (!(info.hitNaI && !info.hitPS)) continue;

        const double dt_ge =
            hitMap[best_bin_e].timeNaI - info.timeNaI;

        const double abs_dt = std::abs(dt_ge);
        if (abs_dt > fCoincDT) continue;

        if (abs_dt < best_dt) {
            best_dt    = abs_dt;
            best_bin_g = bin;
        }
    }

    // gamma が見つからなければ終了
    if (best_bin_g < 0) return;

    // ===============================
    // 3) 角度・cos・エネルギー計算
    // ===============================
    const double phi_e = (best_bin_e + 0.5) * dphi;
    const double phi_g = (best_bin_g + 0.5) * dphi;

    double theta = std::abs(phi_g - phi_e);
    if (theta > CLHEP::pi)
        theta = 2.0 * CLHEP::pi - theta;

    // ZX 平面で (0,0,-1) となす角の cos
    double cos_pos = -std::cos(phi_e);
    double cos_gam = -std::cos(phi_g);

    double Epos = hitMap[best_bin_e].edepPS
                + hitMap[best_bin_e].edepNaI;
    double Egam = hitMap[best_bin_g].edepNaI;

    double dt = (hitMap[best_bin_e].timeNaI
            - hitMap[best_bin_g].timeNaI) / ns;

    // ===============================
    // 4) 出力（1イベント1ペア）
    // ===============================
    myRun->AddCoincData(Epos/MeV,
                        Egam/MeV,
                        dt,
                        theta,
                        cos_pos,
                        cos_gam);

}
