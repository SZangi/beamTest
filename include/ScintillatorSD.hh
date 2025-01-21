#ifndef ScintillatorSD_hh
#define ScintillatorSD_hh 1

#include "G4VSensitiveDetector.hh"

#include <vector>

#include "ScintillatorHit.hh"

class G4Step;
class G4HCofThisEvent;

class ScintillatorSD : public G4VSensitiveDetector
{
    public:
        ScintillatorSD(const G4String& name,
                        const G4String& hitsCollectionName);
        ~ScintillatorSD() override = default;
    
    void Initialize(G4HCofThisEvent* hitCollection) override;
    G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    void EndOfEvent(G4HCofThisEvent* hitCollection) override;

    private:
        ScintillatorHitsCollection* HitsCollection = nullptr;
};

#endif