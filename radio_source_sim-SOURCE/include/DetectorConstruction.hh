#ifndef B1DetectorConstruction_h
#define B1DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"   

class G4VPhysicalVolume;
class G4LogicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction(const G4String& outputPrefix = "photon_hits");
    ~DetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;

    G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }

  protected:
    G4String fOutputPrefix;
    G4LogicalVolume* fScoringVolume = nullptr;
    
  private:
    void BuildShield(G4LogicalVolume* motherVolume, G4double detectorHalfZ, G4double distanceSD);

    // Shield parameters (so you can tune them)
    G4double fRing_R1       = 3175 * um;
    G4double fRing_R2       = 12700 * um;
    G4double fThickShieldUp = 2794 * um;
    G4double fThickShieldDn = 381 * um;
    G4double fThickPaper    = 160 * um;

    // Materials
    G4Material* fMatPlexiglass = nullptr;
    G4Material* fMatEpoxy      = nullptr;
    G4Material* fMatCellophane = nullptr;
    
};

#endif
