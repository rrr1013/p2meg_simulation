#ifndef B4c_ActionInitialization_h
#define B4c_ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

namespace B4c {

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization();            // 宣言のみ（=defaultは付けない）
    ~ActionInitialization() override;  // 宣言のみ

    void BuildForMaster() const override;
    void Build() const override;
};

} // namespace B4c

#endif