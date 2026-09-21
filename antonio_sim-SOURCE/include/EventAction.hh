#ifndef EventAction_h
#define EventAction_h

#include <G4UserEventAction.hh>
#include <G4Types.hh>        
#include <G4String.hh> 

class RunAction;

class EventAction : public G4UserEventAction
{
public:
    EventAction(RunAction* runAction);
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);
    
    void AddPhoton();
    void AddProducedPhoton();
    void AddEdep(G4int barID, G4double edep);

private:
    RunAction* fRunAction;
    G4int fDetectedPhotons;
    G4int fProducedPhotons;
    G4double fEdep;
    G4int fBarEdep[16];  // Energy deposition per bar
};

#endif

