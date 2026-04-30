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

G4VPhysicalVolume* DetectorConstruction::Construct() 
{
	// Materials
	auto nist = G4NistManager::Instance();

	G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
	G4Material* scintillator = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
        
        // World construction
	G4double worldSizeX = 1 * m;
	G4double worldSizeY = 1 * m;
	G4double worldSizeZ = 1 * m;

	auto worldBox = new G4Box("world", worldSizeX / 2, worldSizeY / 2, worldSizeZ / 2);
	auto worldLog = new G4LogicalVolume(worldBox, air, "world");
	auto worldPhys = new G4PVPlacement(nullptr, {}, worldLog, "world", nullptr, false, 0);
	
	//Adding a square
	G4double detectorX = 5 * cm;
	G4double detectorY = 5 * cm;
	G4double detectorZ = 1 * cm;
	
        G4Box* detectorBox = new G4Box("detectorBox",detectorX/2,detectorY/2,detectorZ/2);
        auto detectorLog = new G4LogicalVolume(detectorBox, scintillator, "detectorLog");
        auto detectorPhys = new G4PVPlacement(0, G4ThreeVector(), detectorLog, "detector", worldLog, false, 0);
	// Return the world volume
	return worldPhys;
}

// Implement the following only if you have fields / sensitive detector
/*
void DetectorConstruction::ConstructSDandField()
{

}
*/
