#ifndef PhotonSD_h
#define PhotonSD_h

#include <G4VSensitiveDetector.hh>
#include <fstream>
#include <sstream>
#include <vector>
#include <G4Threading.hh>

class PhotonSD : public G4VSensitiveDetector
{
public:
    PhotonSD(const G4String& name, const G4String& outputPrefix = "photon_hits");
    virtual ~PhotonSD();

    virtual void Initialize(G4HCofThisEvent* hce);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void EndOfEvent(G4HCofThisEvent* hce);
    
    static void MergeFiles(const G4String& prefix, const G4String& outputFile);   
    
private:
    // Per-thread data
    struct ThreadData {
        std::ostringstream buffer;
        G4int hitCount;
        G4int eventCount;
        
        ThreadData() : hitCount(0), eventCount(0) {}
    };
    
    // Use thread-local storage for zero contention
    static G4ThreadLocal ThreadData* fThreadData;
    
    G4int fCurrentEventID;
    G4int fBufferSize;
    G4String fOutputPrefix;
    
    void FlushThreadBuffer(G4int threadID);
};

#endif
