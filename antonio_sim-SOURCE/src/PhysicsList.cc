#include "PhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4IonElasticPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4HadronicParameters.hh"
#include "G4UImanager.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList() {
  // EM
  RegisterPhysics(new G4EmStandardPhysics());
  // Decay
  RegisterPhysics(new G4DecayPhysics());
  RegisterPhysics(new G4RadioactiveDecayPhysics());
  // Hadronic HP
  RegisterPhysics(new G4HadronElasticPhysicsHP());
  RegisterPhysics(new G4HadronPhysicsFTFP_BERT_HP());
  RegisterPhysics(new G4IonElasticPhysics());
  RegisterPhysics(new G4IonPhysics());
  // Optical
  RegisterPhysics(new G4OpticalPhysics());
  RegisterPhysics(new G4EmExtraPhysics());
  G4UImanager* UI = G4UImanager::GetUIpointer();
  UI->ApplyCommand("/process/em/gammaNuclear on");
  UI->ApplyCommand("/process/em/muonNuclear on");
}
    
PhysicsList::~PhysicsList() {}
    
void PhysicsList::SetCuts() { G4VModularPhysicsList::SetCuts(); }
