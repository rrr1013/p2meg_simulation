#ifndef ActionInitialization_h
#define ActionInitialization_h

#include "G4VUserActionInitialization.hh"

class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization();
    virtual ~ActionInitialization();

    // マルチスレッド実行時：ワーカースレッド用のアクションを登録
    virtual void Build() const;

    // マスタースレッド用（統計処理などがあればここで登録）
    virtual void BuildForMaster() const;
};

#endif
