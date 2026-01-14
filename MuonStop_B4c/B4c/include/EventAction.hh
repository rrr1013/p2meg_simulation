#ifndef B4c_EventAction_h
#define B4c_EventAction_h 1
#include "G4UserEventAction.hh"
class G4Event;
namespace B4c {
class EventAction: public G4UserEventAction{
 public:
  EventAction()=default; ~EventAction() override=default;
  void BeginOfEventAction(const G4Event*) override;
  void EndOfEventAction(const G4Event*) override;
};
}
#endif