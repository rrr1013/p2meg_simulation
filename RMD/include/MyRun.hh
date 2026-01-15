#ifndef MyRun_h
#define MyRun_h 1

#include "G4Run.hh"
#include "globals.hh"
#include <vector>

class MyRun : public G4Run {
public:
    MyRun();

    struct CoincData {
        double Epos;   // MeV
        double Egam;   // MeV
        double dt;     // ns (e+ - gamma)
        double theta;  // rad (0 - pi)
        double cos_pos;  // cos(angle between e+ and (0,0,-1) in ZX plane)
        double cos_gam;  // cos(angle between gamma and (0,0,-1) in ZX plane)
    };

    void AddMuon();
    //void AddCoincidence(G4int bin, G4int n = 1);
    void AddCoincData(double Epos, double Egam,
                      double dt, double theta,
                      double cos_pos, double cos_gam);
    const std::vector<CoincData>& GetCoincData() const;

    void Merge(const G4Run* run) override;

    G4int GetNmu() const;
    //const std::vector<G4int>& GetNcoinc() const;

private:
    G4int fNmu;
    //std::vector<G4int> fNcoinc;
    std::vector<CoincData> fCoincData;
};

#endif
