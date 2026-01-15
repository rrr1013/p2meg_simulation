// EventAction.cc
#include "EventAction.hh"
#include "SensitiveDetector.hh"
#include "MyRun.hh"
#include "Constants.hh"
#include "CalorimeterDigit.hh"

#include "G4DCofThisEvent.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4DigiManager.hh"

// 追加
#include "G4ios.hh"
#include <cmath>
#include <limits> 
#include <vector>
#include <algorithm>

EventAction::EventAction()
: G4UserEventAction(),
  fEthNaI(10.*MeV),
  fEthPS(1.*MeV),
  fCoincDT(10.*ns),
  fPSHCID(-1),
  fNaIHCID(-1),
  fPSDCID(-1),
  fNaIDCID(-1)
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
        
    // ===============================
    // 1) Digitize (NaI)
    // ===============================
    // ★ここで Digitize を実行（必須）
    auto* digiMan = G4DigiManager::GetDMpointer();
    digiMan->Digitize("NaIDigi");
    digiMan->Digitize("PSDigi");

    auto* dce = event->GetDCofThisEvent();
    if (!dce) return;

    // ===============================
    // 2) Get NaI DigitsCollection
    // ===============================
    if (fNaIDCID < 0) {
        fNaIDCID = digiMan->GetDigiCollectionID("NaIDigi/DigitsCollection");
        G4cout << "[DEBUG] NaIDCID = " << fNaIDCID << G4endl;
    }
    if (fPSDCID < 0) {
        fPSDCID = digiMan->GetDigiCollectionID("PSDigi/DigitsCollection");
        G4cout << "[DEBUG] PSDCID  = " << fPSDCID << G4endl;
    }
    if (fNaIDCID < 0 || fPSDCID < 0) return;

    auto* naiDC = static_cast<CalorimeterDigitsCollection*>(dce->GetDC(fNaIDCID));
    auto* psDC  = static_cast<CalorimeterDigitsCollection*>(dce->GetDC(fPSDCID));

    static int nprint = 0;
    if (nprint < 50) {
    G4cout << "[DEBUG] naiDC=" << (naiDC? "OK":"NULL")
            << " psDC=" << (psDC? "OK":"NULL");
    if (naiDC) G4cout << " naiN=" << naiDC->entries();
    if (psDC)  G4cout << " psN="  << psDC->entries();
    G4cout << G4endl;
    nprint++;
    }

    if (!naiDC || !psDC) return;
    if (naiDC->entries() == 0) return;  // PS が無くても NaI は必要

    // ===============================
    // 3) Build per-bin info from Digits
    // ===============================
    struct Info {
        bool hitNaI = false;
        bool hitPS  = false;
        double ENaI = 0.0;
        double EPS  = 0.0;
        double tNaI = std::numeric_limits<double>::max();
        double tPS  = std::numeric_limits<double>::max();
    };
    std::vector<Info> m(kNbin);

    // ---- NaI digits ----
    for (size_t i = 0; i < naiDC->entries(); ++i) {
        auto* d = (*naiDC)[i];
        int bin = d->GetChannelID();
        if (bin < 0 || bin >= kNbin) continue;
        m[bin].hitNaI = true;
        m[bin].ENaI  += d->GetEnergy();
        m[bin].tNaI   = std::min(m[bin].tNaI, d->GetTime());
    }

    // ---- PS digits ----
    for (size_t i = 0; i < psDC->entries(); ++i) {
        auto* d = (*psDC)[i];
        int bin = d->GetChannelID();
        if (bin < 0 || bin >= kNbin) continue;
        m[bin].hitPS = true;
        m[bin].EPS  += d->GetEnergy();
        m[bin].tPS   = std::min(m[bin].tPS, d->GetTime());
    }

    // threshold
    for (int bin = 0; bin < kNbin; ++bin) {
        if (m[bin].hitNaI && m[bin].ENaI < fEthNaI) m[bin].hitNaI = false;
        if (m[bin].hitPS  && m[bin].EPS  < fEthPS)  m[bin].hitPS  = false;
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

    // ===============================
    // 4) Select e-side and gamma-side (NaI-only first step)
    //    - e-side: earliest NaI digit (仮)
    //    - gamma : another NaI digit with |dt| < fCoincDT minimizing |dt|
    // ===============================
    int best_bin_e = -1;
    double best_time_e = std::numeric_limits<double>::max();

    for (int bin = 0; bin < kNbin; ++bin) {
        if (!(m[bin].hitPS && m[bin].hitNaI)) continue;
        if (m[bin].tNaI < best_time_e) {
            best_time_e = m[bin].tNaI;
            best_bin_e = bin;
        }
    }

    // e+ が見つからなければ終了
    if (best_bin_e < 0) return;

    // ===============================
    // 2) gamma : 最初の NaI hit で |dt| 最小
    // ===============================
    int best_bin_g = -1;
    double best_abs_dt = std::numeric_limits<double>::max();

    for (int bin = 0; bin < kNbin; ++bin) {
        if (bin == best_bin_e) continue;
        if (!(m[bin].hitNaI && !m[bin].hitPS)) continue;

        double dt = m[best_bin_e].tNaI - m[bin].tNaI;
        double absdt = std::abs(dt);
        if (absdt > fCoincDT) continue;

        if (absdt < best_abs_dt) {
            best_abs_dt = absdt;
            best_bin_g = bin;
        }
    }

    // gamma が見つからなければ終了
    if (best_bin_g < 0) return;

    // ===============================
    // 5) Reconstruct (Epos, Egam, dt, theta, cos)
    // ===============================
    const double dphi = 2.0 * CLHEP::pi / kNbin;

    const double phi_e = (best_bin_e + 0.5) * dphi;
    const double phi_g = (best_bin_g + 0.5) * dphi;

    double theta = std::abs(phi_g - phi_e);
    if (theta > CLHEP::pi)
        theta = 2.0 * CLHEP::pi - theta;

    // ZX 平面で (0,0,-1) となす角の cos
    double cos_pos = -std::cos(phi_e);
    double cos_gam = -std::cos(phi_g);

    double Epos = m[best_bin_e].ENaI; // ★まずはNaIのみ推奨
    double Egam = m[best_bin_g].ENaI;
    double dt_ns = (m[best_bin_e].tNaI - m[best_bin_g].tNaI) / ns;

    // ===============================
    // 6) Store (one pair per event)
    // ===============================
    myRun->AddCoincData(Epos/MeV,
                        Egam/MeV,
                        dt_ns,
                        theta,
                        cos_pos,
                        cos_gam);

}
