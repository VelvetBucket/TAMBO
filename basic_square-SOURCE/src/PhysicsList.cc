#include "PhysicsList.hh"

// Select some of the many physics constructors in Geant4
// These are just examples:
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"

#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"

#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronInelasticQBBC.hh"
#include "G4HadronPhysicsINCLXX.hh"
#include "G4IonElasticPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4IonINCLXXPhysics.hh"
#include "G4EmStandardPhysics.hh"  

// particles

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

// You will probably want this:
#include <G4StepLimiterPhysics.hh>

PhysicsList::PhysicsList():G4VModularPhysicsList()
{
  G4int verb = 0;
  SetVerboseLevel(verb);
  
  // EM physics
  RegisterPhysics(new G4EmStandardPhysics());
  
  // Decay
  RegisterPhysics(new G4DecayPhysics());
            
  // Hadron Elastic scattering
  RegisterPhysics( new G4HadronElasticPhysicsHP(verb) );
  
  // Hadron Inelastic physics
  RegisterPhysics( new G4HadronPhysicsFTFP_BERT_HP(verb));
  
  // Ion Elastic scattering
  RegisterPhysics( new G4IonElasticPhysics(verb));
      
  // Ion Inelastic physics
  RegisterPhysics( new G4IonPhysics(verb));

  // Optical Physics
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  G4OpticalParameters* opticalParams = G4OpticalParameters::Instance();
  
  RegisterPhysics( opticalPhysics );
  
  opticalParams->SetWLSTimeProfile("delta");

  //opticalParams->SetScintillationYieldFactor(1.0);
  //opticalParams->SetScintillationExcitationRatio(0.0);

  opticalParams->SetCerenkovMaxPhotonsPerStep(100);
  opticalParams->SetCerenkovMaxBetaChange(10.0);

  opticalParams->SetCerenkovTrackSecondariesFirst(true);
  opticalParams->SetScintTrackSecondariesFirst(true);
  
  // DEBUG: Print all registered physics
  G4cout << "=== Registered Physics ===" << G4endl;
  G4cout << "Number of physics: " << 7 << G4endl;
  for (G4int i = 0; i < 7; i++)
  {
      G4cout << "  " << i << ": " << GetPhysics(i)->GetPhysicsName() << G4endl;
  }
  G4cout << "=========================" << G4endl;

}

PhysicsList::~PhysicsList()
{ }

void PhysicsList::ConstructParticle()
{

  G4BosonConstructor  pBosonConstructor;
  pBosonConstructor.ConstructParticle();

  G4LeptonConstructor pLeptonConstructor;
  pLeptonConstructor.ConstructParticle();

  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4IonConstructor pIonConstructor;
  pIonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();  
}
