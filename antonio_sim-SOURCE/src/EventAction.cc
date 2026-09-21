#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh" 

EventAction::EventAction(RunAction* runAction)
    : G4UserEventAction(),
      fRunAction(runAction),
      fDetectedPhotons(0),
      fProducedPhotons(0),
      fEdep(0.)
{
    for (int i = 0; i < 16; i++) fBarEdep[i] = 0.;
}

EventAction::~EventAction()
{
}

void EventAction::BeginOfEventAction(const G4Event*)
{
    fDetectedPhotons = 0;
    fProducedPhotons = 0;
    fEdep = 0.;
    for (int i = 0; i < 16; i++) fBarEdep[i] = 0.;
}

void EventAction::EndOfEventAction(const G4Event*)
{
    // Add event totals to run accumulables
    fRunAction->AddEdep(fEdep);
    fRunAction->AddProducedPhotons(fProducedPhotons);
    fRunAction->AddDetectedPhotons(fDetectedPhotons);
}

void EventAction::AddPhoton()
{
    fDetectedPhotons++;
}

void EventAction::AddProducedPhoton()
{
    fProducedPhotons++;
}

void EventAction::AddEdep(G4int barID, G4double edep)
{
    fEdep += edep;
    if (barID >= 0 && barID < 16)
    {
        fBarEdep[barID] += edep;
    }
}
