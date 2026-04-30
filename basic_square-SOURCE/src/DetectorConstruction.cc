#include "DetectorConstruction.hh"

#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4NistManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4VisAttributes.hh>

#include <G4Box.hh>
// #include <G4Tubs.hh>
// #include <G4Sphere.hh>
// #include <G4Orb.hh>

#include "PhotonSD.hh" 
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"

G4VPhysicalVolume* DetectorConstruction::Construct() 
{
  // Materials
  auto nist = G4NistManager::Instance();

  G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* scintillator = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  G4Material* silicon = nist->FindOrBuildMaterial("G4_Si");

  // World construction
  G4double worldSizeX = 1 * m;
  G4double worldSizeY = 1 * m;
  G4double worldSizeZ = 1 * m;

  G4Box* worldBox = new G4Box("world", worldSizeX / 2., worldSizeY / 2., worldSizeZ / 2.);
  G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, air, "world");
  G4VPhysicalVolume* worldPhys = new G4PVPlacement(nullptr, {}, worldLog, "world", nullptr, false, 0);

  //Adding a scintillator detector
  G4double detectorX = 5 * cm;
  G4double detectorY = 5 * cm;
  G4double detectorZ = 1 * cm;

  G4Box* detectorBox = new G4Box("detectorBox",detectorX/2.,detectorY/2.,detectorZ/2.);
  G4LogicalVolume* detectorLog = new G4LogicalVolume(detectorBox, scintillator, "detectorLog");
  G4VPhysicalVolume* detectorPhys = new G4PVPlacement(0, G4ThreeVector(), detectorLog, "detector", worldLog, false, 0);

  //Adding SiPM square
  G4double sipmX = 6 * mm;
  G4double sipmY = 6 * mm;
  G4double sipmZ = 0.25 * mm;
  
  //G4cout << "Construct() START: detectorLog = " << sipmLog << G4endl;
  
  G4Box* sipmBox = new G4Box("sipmBox",sipmX/2.,sipmY/2.,sipmZ/2.);
  G4LogicalVolume* sipmLog = new G4LogicalVolume(sipmBox, silicon, "sipmLog");
  G4VPhysicalVolume* sipmPhys = new G4PVPlacement(0, G4ThreeVector(0.,0., -1.*(detectorZ + sipmZ)/2.), sipmLog, "sipm", worldLog, false, 0); 
  
  
  G4cout << "After creation: detectorLog = " << sipmLog << G4endl;
  
  // Return the world volume
  return worldPhys;
}

// Implement the following only if you have fields / sensitive detector
void DetectorConstruction::ConstructSDandField()
{

  G4cout << "ConstructSDandField called" << G4endl;
  //G4cout << "detectorLog pointer: " << detectorLog << G4endl;
  
  // Get the real volume from the store (bypass member variable)
  G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
  G4LogicalVolume* detectorLog = store->GetVolume("sipmLog");
  
  // Create the sensitive detector
  PhotonSD* photonSD = new PhotonSD("PhotonSD", "my_experiment_photons");
  
  // Register it with the SD manager (optional if done in PhotonSD constructor)
  G4SDManager::GetSDMpointer()->AddNewDetector(photonSD);
  
  // Attach to logical volume
  if (detectorLog)
  {
      detectorLog->SetSensitiveDetector(photonSD);
      G4cout << "PhotonSD attached to Detector volume" << G4endl;
  }
  else
  {
      G4cerr << "ERROR: detectorLog is null!" << G4endl;
  }
}

