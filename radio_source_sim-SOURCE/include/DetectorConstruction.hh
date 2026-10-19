#ifndef B1DetectorConstruction_h
#define B1DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"   
#include "DetectorMessenger.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction(const G4String& outputPrefix = "photon_hits");
    ~DetectorConstruction();

    G4VPhysicalVolume* Construct() override;

    G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }

    // Setter / getter used by the messenger
    void      SetDistance_SD(G4double d) { fDistance_SD = d; }
    G4double  GetDistance_SD() const     { return fDistance_SD; }

  protected:
    G4String fOutputPrefix;
    G4LogicalVolume* fScoringVolume = nullptr;
    
  private:
    DetectorMessenger* fMessenger = nullptr;
  
    void BuildShield(G4LogicalVolume* motherVolume, G4double detectorHalfZ);

    //Materials
    G4Material* fMatPlexiglass = nullptr;
    G4Material* fMatEpoxy      = nullptr;
    G4Material* fMatCellophane = nullptr;

    // Dimensions
    G4double fDistance_SD      = 0.0 * mm;

    // Active spot (visual marker)
    G4double fActiveRadius     = 1.5 * mm;     // 3 mm diameter
    G4double fActiveThickness  = 10 * um;      // very thin

    // Paper
    G4double fThickPaper       = 160 * um;

    // Overlap check
    G4bool fCheckOverlaps      = true;
    
};

#endif
