#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH

#include <G4VUserDetectorConstruction.hh>

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    public:
    DetectorConstruction(const G4String& outputPrefix = "photon_hits");
    // ...
    
    G4String GetOutputPrefix() const { return fOutputPrefix; }
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();
private:
    G4String fOutputPrefix;
};

#endif
