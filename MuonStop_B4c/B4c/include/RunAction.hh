#ifndef B4c_RunAction_h
#define B4c_RunAction_h 1

#include "G4UserRunAction.hh"          // ★ これが必須
#include "G4Accumulable.hh"
#include "G4AccumulableManager.hh"

class G4Run;                           // ★ 前方宣言（ヘッダでは十分）

namespace B4c {

class RunAction : public G4UserRunAction {
public:
  RunAction();
  ~RunAction() override;

  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run*) override;

  // SteppingAction から呼ぶカウンタ API
  void CountPrimaryMu()  { fNMu += 1; }
  void CountStoppedMu()  { fNMuStopped += 1; }

private:
  // スレッド安全カウンタ
  G4Accumulable<G4int> fNMu{0};
  G4Accumulable<G4int> fNMuStopped{0};
};

} // namespace B4c
#endif