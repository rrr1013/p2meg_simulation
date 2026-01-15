#include "MyRun.hh"
#include "Constants.hh"

#include "G4Run.hh"

// --------------------------------------------------
// コンストラクタ
// --------------------------------------------------
MyRun::MyRun()
: G4Run(),
  fNmu(0)
  //fNcoinc(kNbin, 0)
{}


// --------------------------------------------------
// μ カウント（EventAction から呼ばれる）
// --------------------------------------------------
void MyRun::AddMuon()
{
    ++fNmu;
}

// --------------------------------------------------
// coincidence カウント（EventAction / SD から呼ばれる）
// --------------------------------------------------
// void MyRun::AddCoincidence(G4int bin, G4int n)
// {
//     if (bin >= 0 && bin < (G4int)fNcoinc.size()) {
//         fNcoinc[bin] += n;
//     }
// }
void MyRun::AddCoincData(double Epos, double Egam,
                         double dt, double theta,
                        double cos_pos, double cos_gam)
{
    CoincData data;
    data.Epos    = Epos;
    data.Egam    = Egam;
    data.dt      = dt;
    data.theta   = theta;
    data.cos_pos = cos_pos;
    data.cos_gam = cos_gam;

    fCoincData.push_back(data);
}

const std::vector<MyRun::CoincData>&
MyRun::GetCoincData() const
{
    return fCoincData;
}

// --------------------------------------------------
// getter（RunAction から参照）
// --------------------------------------------------
G4int MyRun::GetNmu() const
{
    return fNmu;
}

// const std::vector<G4int>& MyRun::GetNcoinc() const
// {
//     return fNcoinc;
// }

// --------------------------------------------------
// MT 用 Merge（Worker → Master）
// --------------------------------------------------
void MyRun::Merge(const G4Run* run)
{
    const auto* local = static_cast<const MyRun*>(run);

    // μ 数を合算
    fNmu += local->fNmu;

    // // bin ごとの coincidence を合算
    // for (size_t i = 0; i < fNcoinc.size(); ++i) {
    //     fNcoinc[i] += local->fNcoinc[i];
    // }
    
    // coincidence データをマージ
    fCoincData.insert(
        fCoincData.end(),
        local->fCoincData.begin(),
        local->fCoincData.end()
    );

    // Geant4 内部カウンタもマージ
    G4Run::Merge(run);
}
