#ifndef SteppingAction_h
#define SteppingAction_h

#include <G4UserSteppingAction.hh>
#include <fstream>
#include <mutex>
#include <G4Threading.hh>

class EventAction;

class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction(EventAction* eventAction, 
                   const G4String& outputPrefix = "tambo_photons");
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step* step);
    
    // Merge per-thread CSV files
    static void MergePhotonFiles(const G4String& prefix, 
                                  const G4String& outputFile);

private:
    EventAction* fEventAction;
    G4String fOutputPrefix;
    G4int GetBarID(const G4Step* step);
    // Per-thread data for CSV writing
    struct ThreadData
    {
        std::ofstream file;
        G4int hitCount;
        bool fileOpened;
        
        ThreadData() : hitCount(0), fileOpened(false) {}
        ~ThreadData() { if (file.is_open()) file.close(); }
    };
    
    G4ThreadLocal static ThreadData* fThreadData;
    
    void OpenThreadFile();
    void WritePhotonToCSV(G4int eventID, G4int barID, 
                           G4double energy, G4double wavelength,
                           G4double time, G4double posX, G4double posY, G4double posZ);
};

#endif
