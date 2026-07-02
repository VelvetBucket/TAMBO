#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4GeneralParticleSource.hh>
#include <fstream>
#include <mutex>
#include <G4String.hh>
#include <G4Threading.hh>

class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction(const G4String& outputPrefix = "generated_particles");
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event* event);
    
    static void MergeGeneratedFiles(const G4String& prefix, const G4String& outputFile);

private:
    G4GeneralParticleSource* fParticleGun;
    G4String fOutputPrefix;
    
    // Per-thread data
    struct ThreadData
    {
        std::ofstream file;
        G4String threadFilename;
        G4int count;
        bool fileOpened;
        
        ThreadData() : count(0), fileOpened(false) {}
        ~ThreadData() { if (file.is_open()) file.close(); }
    };
    
    G4ThreadLocal static ThreadData* fThreadData;
    
    void OpenThreadFile();
    void WriteToFile(G4int eventID, const G4String& particleName, 
                     G4double energy, G4ThreeVector position);
};

#endif
