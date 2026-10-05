#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

ActionInitialization::ActionInitialization(const G4String& outputPrefix)
    : fOutputPrefix(outputPrefix)
{
}

ActionInitialization::~ActionInitialization() {}

void ActionInitialization::Build() const
{
    // Create primary generator
    SetUserAction(new PrimaryGeneratorAction(fOutputPrefix));
    
    // Create run action
    auto runAction = new RunAction(fOutputPrefix);
    SetUserAction(runAction);
    
    // Create event action
    auto eventAction = new EventAction(runAction);
    SetUserAction(eventAction);
    
    // Create stepping action
    SetUserAction(new SteppingAction(eventAction, fOutputPrefix));
}

void ActionInitialization::BuildForMaster() const
{
    // Master thread doesn't generate particles
}
