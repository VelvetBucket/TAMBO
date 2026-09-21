#ifndef ActionInitialization_h
#define ActionInitialization_h

#include <G4VUserActionInitialization.hh>
#include <G4String.hh>

class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization(const G4String& outputPrefix = "generated");
    virtual ~ActionInitialization();

    virtual void Build() const;
    virtual void BuildForMaster() const;

private:
    G4String fOutputPrefix;
};

#endif
