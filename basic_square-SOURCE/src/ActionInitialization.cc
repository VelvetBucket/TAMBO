#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"

ActionInitialization::ActionInitialization(const G4String& outputPrefix)
    : fOutputPrefix(outputPrefix)
{
}

ActionInitialization::~ActionInitialization() {}

void ActionInitialization::Build() const
{
    SetUserAction(new PrimaryGeneratorAction(fOutputPrefix));
}

void ActionInitialization::BuildForMaster() const
{
    // Master thread doesn't generate particles
}
