#include "ScintillatorSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

ScintillatorSD::ScintillatorSD(const G4String& name,
                                const G4String& hitsCollectionName)
    : G4VSensitiveDetector(name)
{
    collectionName.insert(hitsCollectionName);
}

void ScintillatorSD::Initialize(G4HCofThisEvent* hce)
{
    HitsCollection
        = new ScintillatorHitsCollection(SensitiveDetectorName, collectionName[0]);

    G4int hcID
        = G4SDManager::GetSDMpointer()->GetCollectionID(HitsCollection);
    hce->AddHitsCollection(hcID, HitsCollection);
}

G4bool ScintillatorSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    G4double edep = aStep->GetTotalEnergyDeposit();

    //if (edep == 0) return false;
    auto newHit = new ScintillatorHit();

    newHit-> SetParticleType (aStep->GetTrack()->GetDefinition()->GetParticleName());
    newHit-> SetEnergyDep (edep);
    newHit-> SetEnergy (aStep->GetTrack()->GetKineticEnergy());

    HitsCollection->insert( newHit);

    return true;
}

void ScintillatorSD::EndOfEvent(G4HCofThisEvent*)
{
    if ( verboseLevel>1){
        G4int nofHits = HitsCollection->entries();
        G4cout << G4endl
                << "There are "<<nofHits<<" hits in this event" << G4endl;

    }
}