#include "SteppingAction.hh"
#include "EventAction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4VProcess.hh"       
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include "G4PhysicalConstants.hh" 
#include "Randomize.hh"
#include <iomanip>
#include <sstream>
#include <cmath>

// Define PDE for 2.5V and 5.0V overvoltage
const int n_2_5 = 46;
const std::array <double, n_2_5> wavelength_2_5 = {300.,310.49859551, 315.51966292, 320.99719101, 322.82303371, 326.4747191, 328.75702247, 331.95224719, 336.51685393, 341.08146067, 350.21067416, 360.25280899, 369.38202247, 376.68539326, 383.0758427, 388.55337079, 396.76966292, 405.8988764,  415.94101124, 427.80898876, 427.80898876, 442.41573034, 450.63202247, 459.76123596, 469.80337079, 483.49719101, 499.92977528, 516.36235955, 531.88202247, 547.40168539, 560.18258427, 576.61516854, 600.3511236, 630.47752809, 650.56179775, 678.86235955, 698.94662921, 725.42134831, 749.61376404, 773.80617978, 800.28089888, 826.75561798, 850.03511236, 874.22752809, 899.78932584, 950.}; //nm

const std::array <double, n_2_5> efficiency_2_5 = {3.24324324, 3.24324324, 5.06756757, 7.7027027, 10.03378378, 12.97297297, 15.50675676, 17.73648649, 20.16891892, 21.48648649, 22.65202703, 23.31081081, 24.22297297, 26.0472973, 27.36486486, 28.37837838, 29.39189189, 30.15202703, 30.50675676, 30.10135135, 30.05067568, 28.58108108, 27.77027027, 26.55405405, 25.03378378, 23.10810811, 20.67567568, 18.5472973, 16.57094595, 14.84797297, 13.53040541, 12.06081081, 10.38851351, 8.61486486, 7.60135135, 6.33445946, 5.52364865, 4.45945946, 3.69932432, 2.98986486, 2.38175676, 1.92567568, 1.62162162, 1.31756757, 1.06418919, 0.65878378}; //%

const int n_5_0 = 39;
const std::array <double, n_5_0> wavelength_5_0 = {300.91292135, 310.95505618, 316.43258427, 321.91011236, 323.73595506, 326.4747191, 328.3005618, 331.03932584, 333.77808989, 337.42977528, 340.16853933, 351.12359551, 361.16573034, 370.29494382, 377.59831461, 385.81460674, 395.85674157, 407.7247191, 421.41853933, 439.67696629, 450.63202247, 462.5, 475.28089888, 483.04073034, 499.92977528, 516.36235955, 531.88202247, 549.22752809, 567.48595506, 599.43820225, 614.04494382, 650.56179775, 677.9494382, 700.77247191 ,750.98314607, 800.28089888, 850.49157303, 900.70224719, 949.08707865}; //nm

const std::array <double, n_5_0> efficiency_5_0 = {4.15540541, 4.20608108, 7.19594595, 10.03378378, 12.46621622, 15.60810811, 20.16891892, 22.09459459, 24.22297297, 26.14864865, 27.77027027, 29.39189189, 30., 31.57094595, 33.85135135, 35.77702703, 38.00675676, 39.83108108, 40.84459459, 40.23648649, 39.22297297, 37.5, 35.57432432, 34.15540541, 30.70945946, 27.87162162, 25.03378378, 22.39864865, 20.06756757, 16.16554054, 15.10135135, 12.06081081, 10.03378378, 8.61486486, 5.77702703, 3.85135135, 2.48310811, 1.62162162, 1.06418919}; //%

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
            fThreadData->file << "EventID,BarID,Wavelength_nm,Energy_eV,wasDetected_2_5_V,wasDetected_5_0_V,Time_ns,PosX_cm,PosY_cm,PosZ_cm\n";
            fThreadData->file.flush();
            fThreadData->fileOpened = true;
            G4cout << "Thread " << threadID << ": Opened " << filename.str() << G4endl;
        }
    }
}

void SteppingAction::WritePhotonToCSV(G4int eventID, G4int barID,
                                       G4double energy, G4double wavelength, 
                                       G4String wasDetected_2_5,
                                       G4String wasDetected_5_0,
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
                     << wasDetected_2_5 << ","
                     << wasDetected_5_0 << ","
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
                G4String wasDetected_2_5 = "-";
                G4String wasDetected_5_0 = "-";
                
                // See which photons are "detected"
                G4double probability = G4UniformRand() * 100.0;
                auto it25 = std::lower_bound(wavelength_2_5.begin(), wavelength_2_5.end(), 
                wavelength);
                auto it50 = std::lower_bound(wavelength_5_0.begin(), wavelength_5_0.end(), 
                wavelength);
                
                if (it25 == wavelength_2_5.begin() || it25 == wavelength_2_5.end()) {
                  wasDetected_2_5 = "F";
                } else {
                  double lower_wl = *(it25 - 1);
                  double upper_wl = *it25;
                  
                  int ix = it25 - wavelength_2_5.begin(); 
                  double lower_eff = efficiency_2_5[ix - 1];
                  double upper_eff = efficiency_2_5[ix];
                  
                  double interp_eff = std::lerp(lower_eff, upper_eff, (wavelength -
                  lower_wl) / (upper_wl - lower_wl));
                  
                  if (probability < interp_eff){
                    wasDetected_2_5 = "T";
                  } else {
                    wasDetected_2_5 = "F";
                  };
                };
                
                if (it50 == wavelength_5_0.begin() || it50 == wavelength_5_0.end()) {
                  wasDetected_5_0 = "F";
                } else {
                  double lower_wl = *(it50 - 1);
                  double upper_wl = *it50;
                  
                  int ix = it50 - wavelength_5_0.begin(); 
                  double lower_eff = efficiency_5_0[ix - 1];
                  double upper_eff = efficiency_5_0[ix];
                  
                  double interp_eff = std::lerp(lower_eff, upper_eff, (wavelength -
                  lower_wl) / (upper_wl - lower_wl));
                  
                  if (probability < interp_eff){
                    wasDetected_5_0 = "T";
                  } else {
                    wasDetected_5_0 = "F";
                  };
                  //wasDetected_5_0 = std::to_string(interp_eff);
                };
                
                // Write to per-thread CSV (your functionality)
                WritePhotonToCSV(eventID, barID, energy, wavelength, 
                                wasDetected_2_5, wasDetected_5_0,
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
    
    merged << "EventID,BarID,Wavelength_nm,Energy_eV,WasDetected_2_5_V,WasDetected_5_0_V,Time_ns,PosX_cm,PosY_cm,PosZ_cm\n";
    
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
