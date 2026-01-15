#include "G4MuToEGammaDecay.hh"

#include "G4DecayProducts.hh"
#include "G4DynamicParticle.hh"

#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"

#include "G4PhysicalConstants.hh"
#include "Randomize.hh"
#include "G4RandomDirection.hh"

// ================================
// コンストラクタ
// ================================
G4MuToEGammaDecay::G4MuToEGammaDecay(
  const G4String& parentName,
  G4double br
)
: G4VDecayChannel(parentName, br)
{
    SetParent(parentName);
    SetBR(br);
    SetNumberOfDaughters(2);

    if (parentName == "mu+") {
      SetDaughter(0, "e+");
      SetDaughter(1, "gamma");
    } else if (parentName == "mu-") {
      SetDaughter(0, "e-");
      SetDaughter(1, "gamma");
    }  else {
      G4Exception("G4MuToEGammaDecay",
                  "InvalidParent",
                  FatalException,
                  "Parent must be mu+ or mu-");
    }
}

// ================================
G4MuToEGammaDecay::~G4MuToEGammaDecay() = default;

// ================================
// μ → e γ 崩壊
// ================================
G4DecayProducts*
G4MuToEGammaDecay::DecayIt(G4double)
{
  // 親粒子
  const G4String& pname = GetParentName();

  // 電荷判定
  G4bool isMuonPlus  = (pname == "mu+");
  G4bool isMuonMinus = (pname == "mu-");

  if (!isMuonPlus && !isMuonMinus) {
    G4Exception("G4MuToEGammaDecay::DecayIt",
                "InvalidParent", FatalException,
                "Parent is not mu+ or mu-");
  }

  // 質量
  G4double mMu = GetParentMass();
  G4double me  = G4Electron::Electron()->GetPDGMass();

  // 2体崩壊運動量
  G4double p = (mMu*mMu - me*me) / (2.0 * mMu);

  // μ 偏極
  G4ThreeVector spin = GetPolarization();
  G4double Pmu = std::min(spin.mag(), 1.0);

  // 基準軸
  G4ThreeVector ez;
  if (spin.mag() > 0.) {
    ez = spin.unit();
  } else {
    ez = G4RandomDirection();
  }

  G4ThreeVector ex = ez.orthogonal().unit();
  G4ThreeVector ey = ez.cross(ex);

  // 角度生成
  G4double cosTheta = GenerateCosTheta(Pmu);
  G4double sinTheta = std::sqrt(1.0 - cosTheta*cosTheta);
  G4double phi = CLHEP::twopi * G4UniformRand();

  // 運動量方向
  G4ThreeVector pe_dir =
      sinTheta * std::cos(phi) * ex +
      sinTheta * std::sin(phi) * ey +
      cosTheta * ez;

  G4ThreeVector pe =  p * pe_dir;
  G4ThreeVector pg = -p * pe_dir;

  // DecayProducts
  G4DecayProducts* products = new G4DecayProducts();

  // e±
  if (isMuonPlus) {
    products->PushProducts(
      new G4DynamicParticle(G4Positron::Positron(), pe)
    );
  } else {
    products->PushProducts(
      new G4DynamicParticle(G4Electron::Electron(), pe)
    );
  }

  // γ
  products->PushProducts(
    new G4DynamicParticle(G4Gamma::Gamma(), pg)
  );

  return products;
}

// ================================
// cosθ 分布生成
// ================================
G4double
G4MuToEGammaDecay::GenerateCosTheta(G4double Pmu) const
{
  while (true) {
    G4double c = 2.0 * G4UniformRand() - 1.0;
    G4double y = G4UniformRand();
    if (y < 0.5 * (1.0 + Pmu * c)) return c;
  }
}
