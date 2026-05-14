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
  G4NistManager*  nist = G4NistManager::Instance();

  G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* silicon = nist->FindOrBuildMaterial("G4_Si");
  
  G4double a, z, density;
  G4int nelements;
  G4Element* H  = new G4Element("Hydrogen","H",  z= 1, a=   1.01*g/mole);
  G4Element* N = new G4Element("Nitrogen", "N", z=7 , a=14.01*g/mole);
  G4Element* O = new G4Element("Oxygen"  , "O", z=8 , a=16.00*g/mole);
  G4Element* C = new G4Element("Carbon"  , "C", z=6 , a=12.01*g/mole);
  

  G4int ncomponents;
  G4double fractionmass;

  // The plastic scintillator:
  //

  G4Material* scintillator = new G4Material("PLASTIC_SC", density = 1.043*g/cm3, ncomponents=2);
  scintillator->AddElement(H, fractionmass=0.077418);
  scintillator->AddElement(C, fractionmass=0.922582);

  const G4int NUMENTRIES = 12;
	  
  G4double PhotonEnergy[NUMENTRIES] = 
	  {	3.44*eV, 3.26*eV, 3.1*eV, 3.02*eV, 2.95*eV,
		  2.92*eV, 2.82*eV, 2.76*eV, 2.7*eV, 2.58*eV,
		  2.38*eV, 2.08*eV };

  G4double RINDEX_scintillator[NUMENTRIES] =
	  { 1.58, 1.58, 1.58, 1.58, 1.58,
		  1.58, 1.58, 1.58, 1.58, 1.58,
		  1.58, 1.58 };

  G4double ABSORPTION_scintillator[NUMENTRIES] =
	  { 210*cm, 210*cm, 210*cm, 210*cm, 210*cm,
		  210*cm, 210*cm, 210*cm, 210*cm, 210*cm,
		  210*cm, 210*cm }; 

  G4double SCINTILLATION_scintillator[NUMENTRIES] =
	  { 0.04, 0.07, 0.20, 0.49, 0.84,
		  1.00, 0.83, 0.55, 0.40, 0.17,
		  0.03, 0. };

  std::reverse(PhotonEnergy, PhotonEnergy + NUMENTRIES);
  std::reverse(RINDEX_scintillator, RINDEX_scintillator + NUMENTRIES);
  std::reverse(ABSORPTION_scintillator, ABSORPTION_scintillator + NUMENTRIES);
  std::reverse(SCINTILLATION_scintillator, SCINTILLATION_scintillator + NUMENTRIES);

  G4MaterialPropertiesTable *scintillator_mt = new G4MaterialPropertiesTable();
  scintillator_mt->AddProperty("RINDEX", PhotonEnergy, RINDEX_scintillator, NUMENTRIES);
  scintillator_mt->AddProperty("ABSLENGTH", PhotonEnergy, ABSORPTION_scintillator, NUMENTRIES);
  scintillator_mt->AddProperty("SCINTILLATIONCOMPONENT1", PhotonEnergy, SCINTILLATION_scintillator, NUMENTRIES);
  scintillator_mt->AddConstProperty("SCINTILLATIONYIELD",500./MeV);
  scintillator_mt->AddConstProperty("RESOLUTIONSCALE",1.0);
  scintillator_mt->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 1.*ns);
  scintillator_mt->AddConstProperty("SCINTILLATIONYIELD1",1.);

  const G4int NUMENTRIES2 = 42;
  G4double PhotonEnergy_WLS_ABS_core[NUMENTRIES2] =
	  { 3.539*eV, 3.477*eV, 3.340*eV, 3.321*eV, 3.291*eV,
		  3.214*eV, 3.162*eV, 3.129*eV, 3.091*eV, 3.086*eV,
		  3.049*eV, 3.008*eV, 2.982*eV, 2.958*eV, 2.928*eV,
		  2.905*eV, 2.895*eV, 2.890*eV, 2.858*eV, 2.813*eV,
		  2.774*eV, 2.765*eV, 2.752*eV, 2.748*eV, 2.739*eV,
		  2.735*eV, 2.731*eV, 2.723*eV, 2.719*eV, 2.698*eV,
		  2.674*eV, 2.626*eV, 2.610*eV, 2.583*eV, 2.556*eV,
		  2.530*eV, 2.505*eV, 2.480*eV, 2.455*eV, 2.431*eV,
		  2.407*eV, 2.384*eV };

  G4double WLS_ABSLENGTH_core[NUMENTRIES2] =
	  { 0.28*cm, 0.28*cm, 0.26*cm, 0.25*cm, 0.24*cm,
		  0.21*cm, 0.19*cm, 0.16*cm, 0.13*cm, 0.13*cm,
		  0.14*cm, 0.11*cm, 0.08*cm, 0.05*cm, 0.02*cm,
		  0.05*cm, 0.08*cm, 0.10*cm, 0.13*cm, 0.10*cm,
		  0.08*cm, 0.07*cm, 0.08*cm, 0.11*cm, 0.13*cm,
		  0.16*cm, 0.19*cm, 0.21*cm, 0.24*cm, 0.27*cm,
		  0.30*cm, 2.69*cm, 3.49*cm, 3.99*cm, 5.00*cm,
		  11.6*cm, 21.6*cm, 33.1*cm, 175*cm, 393*cm,
		  617*cm, 794*cm };

  std::reverse(PhotonEnergy_WLS_ABS_core, PhotonEnergy_WLS_ABS_core + NUMENTRIES2);
  std::reverse(WLS_ABSLENGTH_core, WLS_ABSLENGTH_core + NUMENTRIES2);

  const G4int NUMENTRIES3 = 24;
  G4double PhotonEnergy_WLS_EM_core[NUMENTRIES3] =
	  { 2.69*eV, 2.67*eV, 2.66*eV, 2.64*eV, 2.63*eV,
		  2.61*eV, 2.58*eV, 2.56*eV, 2.55*eV, 2.53*eV,
		  2.50*eV, 2.48*eV, 2.46*eV, 2.45*eV, 2.44*eV,
		  2.43*eV, 2.41*eV, 2.37*eV, 2.33*eV, 2.25*eV,
		  2.24*eV, 2.19*eV, 2.15*eV, 2.08*eV };

  G4double WLS_EMISSION_core[NUMENTRIES3] =
	  {	0., 0.02, 0.09, 0.20, 0.29,
		  0.40, 0.59, 0.70, 0.80, 0.89,
		  1.00, 0.96, 0.88, 0.79, 0.69,
		  0.59, 0.50, 0.40, 0.31, 0.22,
		  0.19, 0.10, 0.06, 0. };

  std::reverse(PhotonEnergy_WLS_EM_core, PhotonEnergy_WLS_EM_core + NUMENTRIES3);
  std::reverse(WLS_EMISSION_core, WLS_EMISSION_core + NUMENTRIES3);

  scintillator_mt->AddProperty("WLSABSLENGTH", PhotonEnergy_WLS_ABS_core, WLS_ABSLENGTH_core, NUMENTRIES2);
  scintillator_mt->AddProperty("WLSCOMPONENT", PhotonEnergy_WLS_EM_core, WLS_EMISSION_core, NUMENTRIES3);
  scintillator_mt->AddConstProperty("WLSTIMECONSTANT", 0.5*ns);
  scintillator->SetMaterialPropertiesTable(scintillator_mt);

  // World construction
  G4double worldSizeX = 1 * m;
  G4double worldSizeY = 1 * m;
  G4double worldSizeZ = 1 * m;

  G4Box* worldBox = new G4Box("world", worldSizeX / 2., worldSizeY / 2., worldSizeZ / 2.);
  G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, air, "world");
  G4VPhysicalVolume* worldPhys = new G4PVPlacement(nullptr, {}, worldLog, "world", nullptr, false, 0);

  //Adding a scintillator plastic
  G4double plasticX = 5 * cm;
  G4double plasticY = 5 * cm;
  G4double plasticZ = 1 * cm;

  G4Box* plasticBox = new G4Box("plasticBox",plasticX/2.,plasticY/2.,plasticZ/2.);
  G4LogicalVolume* plasticLog = new G4LogicalVolume(plasticBox, scintillator, "plasticLog");
  G4VPhysicalVolume* plasticPhys = new G4PVPlacement(0, G4ThreeVector(), plasticLog, "plastic", worldLog, false, 0);

  //Adding SiPM square
  G4double detectorX = 6 * mm;
  G4double detectorY = 6 * mm;
  G4double detectorZ = 0.25 * mm;
  
  G4Box* detectorBox = new G4Box("detectorBox",detectorX/2.,detectorY/2.,detectorZ/2.);
  G4LogicalVolume* detectorLog = new G4LogicalVolume(detectorBox, silicon, "detectorLog");
  G4VPhysicalVolume* detectorPhys = new G4PVPlacement(0, G4ThreeVector(0.,0., -1.*(plasticZ + detectorZ)/2.), detectorLog, "detector", worldLog, false, 0); 
  
  // Return the world volume
  return worldPhys;
}

// Implement the following only if you have fields / sensitive detector
void DetectorConstruction::ConstructSDandField()
{

  G4cout << "ConstructSDandField called" << G4endl;
  
  // Get the real volume from the store (bypass member variable)
  G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
  G4LogicalVolume* detectorLog = store->GetVolume("detectorLog");
  
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

