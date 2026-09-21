#include "RunAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"
#include <sstream>

// Initialize static atomic variables
std::atomic<G4double> RunAction::sTotalEdep{0.};
std::atomic<G4int> RunAction::sTotalProduced{0};
std::atomic<G4int> RunAction::sTotalDetected{0};
std::atomic<G4int> RunAction::sTotalEvents{0};

RunAction::RunAction(const G4String& outputPrefix)
    : G4UserRunAction(),
      fOutputPrefix(outputPrefix)
{
}

RunAction::~RunAction()
{
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    // Reset atomic counters
    sTotalEdep = 0.;
    sTotalProduced = 0;
    sTotalDetected = 0;
    sTotalEvents = 0;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    // Only master thread prints summary
    if (!IsMaster()) return;

    G4int nofEvents = run->GetNumberOfEvent();
    if (nofEvents == 0) return;

    sTotalEvents += nofEvents;
  
    G4int totalEvents = sTotalEvents.load();
    G4double totalEdep = sTotalEdep.load();
    G4int totalProduced = sTotalProduced.load();
    G4int totalDetected = sTotalDetected.load();

}

// Thread-safe add functions using atomic operations
void RunAction::AddEdep(G4double edep)
{
    G4double old = sTotalEdep.load();
    while (!sTotalEdep.compare_exchange_weak(old, old + edep));
}

void RunAction::AddProducedPhotons(G4int count)
{
    sTotalProduced += count;  // Atomic add for integers
}

void RunAction::AddDetectedPhotons(G4int count)
{
    sTotalDetected += count;  // Atomic add for integers
}
