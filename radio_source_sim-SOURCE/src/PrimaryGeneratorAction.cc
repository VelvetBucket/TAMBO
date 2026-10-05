#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include <iomanip>
#include <sstream>

// Initialize thread-local storage
G4ThreadLocal PrimaryGeneratorAction::ThreadData* PrimaryGeneratorAction::fThreadData = nullptr;

PrimaryGeneratorAction::PrimaryGeneratorAction(const G4String& outputPrefix)
    : fParticleGun(nullptr),
      fOutputPrefix(outputPrefix)
{
    fParticleGun = new G4GeneralParticleSource();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
    
    // Close thread file
    if (fThreadData)
    {
        delete fThreadData;
        fThreadData = nullptr;
    }
}

void PrimaryGeneratorAction::OpenThreadFile()
{
    if (!fThreadData)
    {
        fThreadData = new ThreadData();
    }
    
    if (!fThreadData->fileOpened)
    {
        G4int threadID = G4Threading::G4GetThreadId();
        std::ostringstream filename;
        filename << fOutputPrefix << "_generated_t" << threadID << ".csv";
        
        fThreadData->threadFilename = filename.str();
        fThreadData->file.open(filename.str());
        
        if (fThreadData->file.is_open())
        {
            fThreadData->file << "EventID,Particle,Energy_GeV,PosX_cm,PosY_cm,PosZ_cm\n";
            fThreadData->file.flush();
            fThreadData->fileOpened = true;
            G4cout << "Thread " << threadID << ": Opened " << filename.str() << G4endl;
        }
    }
}

void PrimaryGeneratorAction::WriteToFile(G4int eventID, const G4String& particleName,
                                          G4double energy, G4ThreeVector position)
{
    if (!fThreadData || !fThreadData->file.is_open())
        return;
    
    fThreadData->file << std::fixed << std::setprecision(6)
                     << eventID << ","
                     << particleName << ","
                     << energy/GeV << ","
                     << position.x()/cm << ","
                     << position.y()/cm << ","
                     << position.z()/cm << "\n";
    
    fThreadData->count++;
    if (fThreadData->count % 100 == 0)
    {
        fThreadData->file.flush();
    }
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    // Open thread file on first call
    OpenThreadFile();
    
    // GPS generates the primary vertex
    fParticleGun->GeneratePrimaryVertex(event);
    
    // Record what was generated
    G4PrimaryVertex* vertex = event->GetPrimaryVertex();
    if (vertex && vertex->GetNumberOfParticle() > 0)
    {
        G4PrimaryParticle* primary = vertex->GetPrimary();
        G4double energy = primary->GetKineticEnergy();
        G4String name = primary->GetParticleDefinition()->GetParticleName();
        G4ThreeVector pos = vertex->GetPosition();
        
        WriteToFile(event->GetEventID(), name, energy, pos);
    }
}

void PrimaryGeneratorAction::MergeGeneratedFiles(const G4String& prefix, const G4String& outputFile)
{
    G4cout << "\n=== Merging generated particle files ===" << G4endl;
    
    std::ofstream merged(outputFile);
    if (!merged.is_open())
    {
        G4cerr << "ERROR: Cannot create " << outputFile << G4endl;
        return;
    }
    
    merged << "EventID,Particle,Energy_GeV,PosX_cm,PosY_cm,PosZ_cm\n";
    
    int filesMerged = 0;
    int totalLines = 0;
    
    for (int i = 0; i < 100; i++)
    {
        std::ostringstream filename;
        filename << prefix << "_generated_t" << i << ".csv";
        
        std::ifstream infile(filename.str());
        if (!infile.is_open())
            continue;
        
        G4cout << "  Merging: " << filename.str() << G4endl;
        
        std::string line;
        std::getline(infile, line);  // Skip header
        
        while (std::getline(infile, line))
        {
            if (!line.empty())
            {
                merged << line << "\n";
                totalLines++;
            }
        }
        
        infile.close();
        std::remove(filename.str().c_str());  // Delete thread file
        filesMerged++;
    }
    
    merged.close();
    G4cout << "Merged " << filesMerged << " files (" << totalLines 
           << " entries) into " << outputFile << G4endl;
}
