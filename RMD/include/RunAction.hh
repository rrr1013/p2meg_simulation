#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4Timer.hh"   // ★ 追加

class RunAction : public G4UserRunAction {
public:
    RunAction() = default;
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

    // ★ 重要：独自Runを使う
    G4Run* GenerateRun() override;
    
    private:
    G4Timer fTimer;     // ★ 追加（計算時間測定用）
};

#endif
