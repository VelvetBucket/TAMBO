#include <vector>

// Select a proper single-/multithreaded run manager
#include <G4MTRunManager.hh>
#include <G4RunManager.hh>

#ifdef G4VIS_USE
    #include <G4VisExecutive.hh>
#endif

#ifdef G4UI_USE
    #include <G4UIExecutive.hh>
#endif

#include <G4String.hh>
#include <G4UImanager.hh>
#include <G4ScoringManager.hh>

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "PhysicsList.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include "G4SystemOfUnits.hh"

using namespace std;

/* Main function that enables to:
 * - run any number of macros (put them as command-line arguments)
 * - start interactive UI mode (no arguments or "-i")
 */
int main(int argc, char** argv)
{
    
    vector<G4String> macros;    
    bool interactive = false;
    G4String outputPrefix = "photon_hits";  // Default
    G4double distanceSD = 0.0;         // default
    G4bool   distanceSet = false;
    
    // Parse command line arguments
    if  (argc == 1)
    {
        interactive = true;
        G4cout << "No arguments -> interactive mode" << G4endl;
    }   
    else
    {
        for (int i = 1; i < argc; i++)
        {
            G4String arg = argv[i];
            if (arg == "-i" || arg == "--interactive")
            {
                interactive = true;
                G4cout << "Found -i flag -> interactive mode" << G4endl;
                continue;
            }
            else if (arg == "--distance" && i + 1 < argc)
            {
                // Expect: --distance <value> <unit>  (e.g. 1 cm)
                G4double value = std::atof(argv[++i]);
                G4String unit  = "mm";
                if (i + 1 < argc) {
                    G4String next = argv[i + 1];
                    if (next == "mm" || next == "cm" || next == "m" ||
                        next == "um") {
                        unit = next;
                        ++i;
                    }
                }

                if      (unit == "mm") distanceSD = value * mm;
                else if (unit == "cm") distanceSD = value * cm;
                else if (unit == "m")  distanceSD = value * m;
                else if (unit == "um") distanceSD = value * um;

                distanceSet = true;
                G4cout << "Command line: distanceSD = " << distanceSD/mm 
                       << " mm" << G4endl;
            }
            else
            {
                macros.push_back(arg);
            }                      
        }
    }
    
    #ifdef G4MULTITHREADED
      G4cout << "=== G4MULTITHREADED is defined ===" << G4endl;
      auto runManager = new G4MTRunManager();
      G4cout << "Using G4MTRunManager" << G4endl;
    #else
      G4cout << "=== G4MULTITHREADED is NOT defined ===" << G4endl;
      auto runManager = new G4RunManager();
      G4cout << "Using G4RunManager (sequential)" << G4endl;
    #endif

    // Set number of threads BEFORE Initialize
    #ifdef G4MULTITHREADED
      G4int nThreads = G4Threading::G4GetNumberOfCores() - 1;
      if (nThreads <= 0) {
        nThreads = G4Threading::G4GetNumberOfCores();
      }
      runManager->SetNumberOfThreads(nThreads);
      G4cout << "Number of threads set to: " << runManager->GetNumberOfThreads() << G4endl;
    #endif
    
    #ifdef G4VIS_USE
        G4VisManager* visManager = new G4VisExecutive("quiet");
        visManager->SetVerboseLevel(0);
        visManager->Initialize();
    #endif  
  
    // Extract prefix from first macro filename
    if (!macros.empty())
    {
        outputPrefix = macros[0];
        // Remove path
        size_t lastSlash = outputPrefix.find_last_of("/\\");
        if (lastSlash != G4String::npos)
        {
            outputPrefix = outputPrefix.substr(lastSlash + 1);
        }
        // Remove extension(s) (e.g., .mac, .in, etc.)
        size_t lastDot = outputPrefix.find_last_of(".");
        if (lastDot != G4String::npos)
        {
            outputPrefix = outputPrefix.substr(0, lastDot);
        }
        // Add _output suffix
        outputPrefix += "_output";
        
        G4cout << "Output prefix set to: " << outputPrefix << G4endl;
    }
    
     // --- Detector construction ---
    auto detector = new DetectorConstruction(outputPrefix);

    // *** SET THE PARAMETER BEFORE Initialize() ***
    if (distanceSet)
        detector->SetDistance_SD(distanceSD);

    runManager->SetUserInitialization(detector);
    
    runManager->SetUserInitialization(new PhysicsList());
    runManager->SetUserInitialization(detector);
    runManager->SetUserInitialization(new ActionInitialization(outputPrefix));

    G4ScoringManager::GetScoringManager();
    
    // Initialize the run manager
    runManager->Initialize();

    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // Execute macros if any
    for (auto macro : macros)
    {
        G4String command = "/control/execute ";
        UImanager->ApplyCommand(command + macro);
    }    

    // Handle interactive mode
    if (interactive)
    {
        #ifdef G4UI_USE
            G4cout << "Starting interactive UI session..." << G4endl;
            G4UIExecutive* ui = new G4UIExecutive(argc, argv);
            UImanager->ApplyCommand("/control/execute macro/init_vis.mac");
            ui->SessionStart();
            delete ui;
        #else
            // If UI is not available but interactive was requested
            G4cerr << "ERROR: Interactive mode requested but Geant4 UI support is not compiled!" << G4endl;
            G4cerr << "Rebuild with G4UI_USE defined or use batch mode with macros." << G4endl;
            delete runManager;
            return 1;
        #endif
    }

    #ifdef G4VIS_USE
        delete visManager;
    #endif
    delete runManager;
    
    // Now print the final summary (after all threads have finished)
    G4cout << "\n";
    G4cout << "===========================================================" << G4endl;
    G4cout << "  FINAL Run Summary" << G4endl;
    G4cout << "===========================================================" << G4endl;
    G4cout << "  Events processed:        " << RunAction::GetTotalEvents() << G4endl;
    G4cout << "  Total energy deposited:  " << RunAction::GetTotalEdep() / MeV << " MeV" << G4endl;
    G4cout << "  Scintillation photons:   " << RunAction::GetTotalProduced() << G4endl;
    G4cout << "  Detected photons:        " << RunAction::GetTotalDetected() << G4endl;
    G4cout << "===========================================================" << G4endl;
    G4cout << G4endl;
    
    // Merge photon CSV files
    SteppingAction::MergePhotonFiles(outputPrefix, outputPrefix + "_merged.csv");

    // Also merge generated particles if you still have that
    PrimaryGeneratorAction::MergeGeneratedFiles(outputPrefix, outputPrefix + "_generated_merged.csv");
    
    G4cout << "Program finished normally." << G4endl;
    return 0;
}
