#ifndef RunAction_h
#define RunAction_h

#include <G4UserRunAction.hh>
#include <G4String.hh>
#include <G4Types.hh>
#include <atomic>

class G4Run;

class RunAction : public G4UserRunAction
{
public:
    RunAction(const G4String& outputPrefix = "TAMBO");
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);
    
    void AddEdep(G4double edep);
    void AddProducedPhotons(G4int count);
    void AddDetectedPhotons(G4int count);
    
    static G4double GetTotalEdep() { return sTotalEdep.load(); }
    static G4int GetTotalProduced() { return sTotalProduced.load(); }
    static G4int GetTotalDetected() { return sTotalDetected.load(); }
    static G4int GetTotalEvents() { return sTotalEvents.load(); }

private:
    static std::atomic<G4double> sTotalEdep;
    static std::atomic<G4int> sTotalProduced;
    static std::atomic<G4int> sTotalDetected;
    static std::atomic<G4int> sTotalEvents;
    
    G4String fOutputPrefix;
};

#endif
