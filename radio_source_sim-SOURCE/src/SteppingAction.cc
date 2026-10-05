#include "SteppingAction.hh"
#include "EventAction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4VProcess.hh"       
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include "G4PhysicalConstants.hh" 
#include <iomanip>
#include <sstream>

// Initialize thread-local storage
G4ThreadLocal SteppingAction::ThreadData* SteppingAction::fThreadData = nullptr;

SteppingAction::SteppingAction(EventAction* eventAction, const G4String& outputPrefix)
    : G4UserSteppingAction(),
      fEventAction(eventAction),
      fOutputPrefix(outputPrefix)
{
}

SteppingAction::~SteppingAction()
{
    if (fThreadData)
    {
        delete fThreadData;
        fThreadData = nullptr;
    }
}

void SteppingAction::OpenThreadFile()
{
    if (!fThreadData)
    {
        fThreadData = new ThreadData();
    }
    
    if (!fThreadData->fileOpened)
    {
        G4int threadID = G4Threading::G4GetThreadId();
        if (threadID < 0) threadID = 0;
        
        std::ostringstream filename;
        filename << fOutputPrefix << "_t" << threadID << ".csv";
        
        fThreadData->file.open(filename.str());
        if (fThreadData->file.is_open())
        {
            fThreadData->file << "EventID,BarID,Wavelength_nm,Energy_eV,Time_ns,PosX_cm,PosY_cm,PosZ_cm\n";
            fThreadData->file.flush();
            fThreadData->fileOpened = true;
            G4cout << "Thread " << threadID << ": Opened " << filename.str() << G4endl;
        }
    }
}

void SteppingAction::WritePhotonToCSV(G4int eventID, G4int barID,
                                       G4double energy, G4double wavelength,
                                       G4double time, G4double posX, 
                                       G4double posY, G4double posZ)
{
    if (!fThreadData || !fThreadData->file.is_open())
        return;
    
    fThreadData->file << std::fixed << std::setprecision(6)
                     << eventID << ","
                     << barID << ","
                     << wavelength << ","
                     << energy/eV << ","
                     << time/ns << ","
                     << posX/cm << ","
                     << posY/cm << ","
                     << posZ/cm << "\n";
    
    fThreadData->hitCount++;
    
    // Flush every 100 hits
    if (fThreadData->hitCount % 100 == 0)
    {
        fThreadData->file.flush();
    }
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    auto track = step->GetTrack();
    auto particleType = track->GetDefinition();

    // ===================================================================
    // COUNT PRODUCED SCINTILLATION PHOTONS
    // ===================================================================
    if (particleType == G4OpticalPhoton::OpticalPhotonDefinition())
    {
        if (track->GetCurrentStepNumber() == 1)
        {
            auto creatorProcess = track->GetCreatorProcess();
            if (creatorProcess && creatorProcess->GetProcessName() == "Scintillation")
            {
                if (fEventAction)
                {
                    fEventAction->AddProducedPhoton();
                }
            }
        }
    }

    auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    std::string volumeName = volume->GetName();

    // ===================================================================
    // ENERGY DEPOSITION IN SCINTILLATOR BARS
    // ===================================================================
    if (volumeName.find("Scint") != std::string::npos)
    {
        G4double edepStep = step->GetTotalEnergyDeposit();
        if (fEventAction && edepStep > 0.0)
        {
            G4int barID = GetBarID(step);
            fEventAction->AddEdep(barID, edepStep);
        }
    }

    // ===================================================================
    // PHOTON DETECTION + CSV RECORDING
    // ===================================================================
    if (volumeName == "Detector_Logic")
    {
        if (particleType == G4OpticalPhoton::OpticalPhotonDefinition())
        {
            if (step->IsFirstStepInVolume())
            {
                // Open thread file on first use
                OpenThreadFile();
                
                G4double energy = track->GetKineticEnergy();
                G4double wavelength = ((twopi * hbarc) / energy) / nm;
                G4double time = track->GetGlobalTime();
                G4ThreeVector pos = step->GetPreStepPoint()->GetPosition();
                G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
                G4int barID = GetBarID(step);
                
                // Write to per-thread CSV (your functionality)
                WritePhotonToCSV(eventID, barID, energy, wavelength, 
                                time, pos.x(), pos.y(), pos.z());
                
                // Update event counters
                if (fEventAction)
                {
                    fEventAction->AddPhoton();
                }
                
                // Kill photon after detection
                track->SetTrackStatus(fStopAndKill);
            }
        }
    }
}

G4int SteppingAction::GetBarID(const G4Step* step)
{
    G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();
    
    for (G4int i = 0; i < touchable->GetHistoryDepth(); i++)
    {
        G4String volName = touchable->GetVolume(i)->GetName();
        
        // Match physical volume name "Scint_Phys_N"
        if (volName.find("Bar_Phys_") != std::string::npos)
        {
            return touchable->GetCopyNumber(i);
        }
    }
    
    return -1;  // Not found
}

void SteppingAction::MergePhotonFiles(const G4String& prefix, 
                                       const G4String& outputFile)
{
    G4cout << "\n=== Merging photon CSV files ===" << G4endl;
    
    std::ofstream merged(outputFile);
    if (!merged.is_open())
    {
        G4cerr << "ERROR: Cannot create " << outputFile << G4endl;
        return;
    }
    
    merged << "EventID,BarID,Wavelength_nm,Energy_eV,Time_ns,PosX_cm,PosY_cm,PosZ_cm\n";
    
    int filesMerged = 0;
    int totalLines = 0;
    
    for (int i = 0; i < 100; i++)
    {
        std::ostringstream filename;
        filename << prefix << "_t" << i << ".csv";
        
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
           << " photons) into " << outputFile << G4endl;
}
