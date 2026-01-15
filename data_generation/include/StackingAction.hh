#ifndef StackingAction_h
#define StackingAction_h 1

#include "G4UserStackingAction.hh"
#include "globals.hh"

// ------------------------------------------------------------
// StackingAction
//  ・一次陽電子のみを見る
//  ・運動量方向の y 成分で判定
//  ・|dir.y()| が大きい（= zx 平面から外れる）場合は kill
// ------------------------------------------------------------
class StackingAction : public G4UserStackingAction
{
public:
    StackingAction();
    virtual ~StackingAction() = default;

    virtual G4ClassificationOfNewTrack
    ClassifyNewTrack(const G4Track* track) override;

private:
    // zx 平面からの許容範囲（方向余弦）
    // |dir.y()| < fYMax のときのみ tracking を許可
    G4double fYMax;
};

#endif
