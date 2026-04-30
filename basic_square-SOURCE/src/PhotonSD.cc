#include "PhotonSD.hh"
#include "G4Step.hh"
#include "G4OpticalPhoton.hh"
#include "G4Gamma.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include <iomanip>
#include <chrono>
#include <ctime>

// Initialize thread-local storage
G4ThreadLocal PhotonSD::ThreadData* PhotonSD::fThreadData = nullptr;

PhotonSD::PhotonSD(const G4String& name, const G4String& outputPrefix)
    : G4VSensitiveDetector(name),
      fCurrentEventID(0),
      fBufferSize(10000),  // Flush every 10k hits
      fOutputPrefix(outputPrefix)
{
    G4cout << "PhotonSD initialized (per-thread buffered CSV)" << G4endl;
    G4cout << "  Threads: " << G4Threading::G4GetNumberOfCores() << G4endl;
    G4cout << "  Buffer size: " << fBufferSize << " hits" << G4endl;
}

PhotonSD::~PhotonSD()
{
    if (fThreadData)
    {
        FlushThreadBuffer(G4Threading::G4GetThreadId());
        delete fThreadData;
        fThreadData = nullptr;
    }
}

void PhotonSD::Initialize(G4HCofThisEvent* hce)
{
    // Initialize thread-local data on first call per thread
    if (!fThreadData)
    {
        fThreadData = new ThreadData();
        G4cout << "Thread " << G4Threading::G4GetThreadId() 
               << ": CSV buffer initialized" << G4endl;
    }
    
    fCurrentEventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
}

G4bool PhotonSD::ProcessHits(G4Step* step, G4TouchableHistory* history)
{
    G4Track* track = step->GetTrack();
    G4ParticleDefinition* particle = track->GetDefinition();
    
    // Filter for photons
    if (particle != G4OpticalPhoton::OpticalPhotonDefinition() && 
        particle != G4Gamma::GammaDefinition())
        return false;
    
    G4StepPoint* preStep = step->GetPreStepPoint();
    G4ThreeVector position = preStep->GetPosition();
    G4double time = preStep->GetGlobalTime();
    G4double energy = track->GetKineticEnergy();
    
    G4double wavelength = 0.0;
    G4String particleType;
    
    if (particle == G4OpticalPhoton::OpticalPhotonDefinition())
    {
        wavelength = (energy > 0) ? 1239.84 * nm / (energy / eV) : 0.0;
        particleType = "optical";
    }
    else
    {
        wavelength = -1.0;
        particleType = "gamma";
    }
    
    // Write to thread-local buffer (NO LOCK!)
    fThreadData->buffer << std::fixed << std::setprecision(6)
                       << fCurrentEventID << ","
                       << position.x()/cm << ","
                       << position.y()/cm << ","
                       << position.z()/cm << ","
                       << time/ns << ","
                       << wavelength << ","
                       << energy/eV << ","
                       << particleType << "\n";
    
    fThreadData->hitCount++;
    
    // Auto-flush when buffer gets large
    if (fThreadData->hitCount % fBufferSize == 0)
    {
        FlushThreadBuffer(G4Threading::G4GetThreadId());
    }
    
    return true;
}

void PhotonSD::EndOfEvent(G4HCofThisEvent* hce)
{
    fThreadData->eventCount++;
    
    // Flush every 100 events to prevent data loss
    if (fThreadData->eventCount % 100 == 0)
    {
        FlushThreadBuffer(G4Threading::G4GetThreadId());
    }
}

void PhotonSD::FlushThreadBuffer(G4int threadID)
{
    if (!fThreadData || fThreadData->buffer.tellp() == 0)
        return;
    
    // Build filename with thread ID and timestamp
    std::ostringstream filename;
    filename << fOutputPrefix << "_t" << threadID << ".csv";
    
    // Open file in append mode
    std::ofstream file(filename.str(), std::ios::app);
    if (file.is_open())
    {
        file << fThreadData->buffer.str();
        file.close();
        
        G4cout << "Thread " << threadID << ": Flushed " 
               << fThreadData->hitCount << " hits" << G4endl;
    }
    
    // Clear buffer
    fThreadData->buffer.str("");
    fThreadData->buffer.clear();
}

void PhotonSD::MergeFiles(const G4String& prefix, const G4String& outputFile)
{
    G4cout << "Merging files with prefix: " << prefix << G4endl;
    
    std::ofstream mergedFile(outputFile);
    if (!mergedFile.is_open()) {
        G4cerr << "ERROR: Cannot create merged file!" << G4endl;
        return;
    }
    
    // Write header
    mergedFile << "EventID,X_cm,Y_cm,Z_cm,Time_ns,Wavelength_nm,Energy_eV,Particle\n";
    
    int filesMerged = 0;
    
    for (int i = 0; i < 100; i++) {
        std::ostringstream filename;
        filename << prefix << i << ".csv";
        
        std::ifstream threadFile(filename.str());
        if (!threadFile.is_open()) {
            break;  // No more files
        }
        
        // Skip header line of each file
        std::string line;
        std::getline(threadFile, line);  // Skip first line (header)
        
        // Copy rest of file
        while (std::getline(threadFile, line)) {
            mergedFile << line << "\n";
        }
        
        threadFile.close();
        filesMerged++;
    }
    
    mergedFile.close();
    G4cout << "Merged " << filesMerged << " files into: " << outputFile << G4endl;
}
