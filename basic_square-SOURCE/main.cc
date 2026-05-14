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
#include "PhysicsList.hh"
#include "PhotonSD.hh" 

using namespace std;

/* Main function that enables to:
 * - run any number of macros (put them as command-line arguments)
 * - start interactive UI mode (no arguments or "-i")
 */
int main(int argc, char** argv)
{
    
    vector<G4String> macros;    
    bool interactive = false;

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

    runManager->SetUserInitialization(new PhysicsList());
    runManager->SetUserInitialization(new DetectorConstruction());
    runManager->SetUserInitialization(new ActionInitialization());

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
    
    PhotonSD::MergeFiles("my_experiment_photons", "my_experiment_photons_merged.csv");
    
    G4cout << "Program finished normally." << G4endl;
    return 0;
}
