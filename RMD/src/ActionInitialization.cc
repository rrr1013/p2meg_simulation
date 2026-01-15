#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "StackingAction.hh"


ActionInitialization::ActionInitialization()
 : G4VUserActionInitialization()
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::Build() const
{
    // ここでユーザーアクションを登録する
    SetUserAction(new PrimaryGeneratorAction());
    //SetUserAction(new StackingAction);
    SetUserAction(new RunAction());
    SetUserAction(new EventAction());
    //SetUserAction(new SteppingAction()); 
    // などを追加していく
}

void ActionInitialization::BuildForMaster() const
{
    // マスタースレッド用アクションを登録（統計処理用の RunAction など）
    SetUserAction(new RunAction());
}
