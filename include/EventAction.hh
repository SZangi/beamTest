#ifndef EventAction_hh
#define EventAction_hh 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "runAction.hh"

#include <vector>
#include <array>

class runAction;

class EventAction : public G4UserEventAction
{
    public:
        EventAction(runAction*);
        ~EventAction() override = default;

        void BeginOfEventAction(const G4Event*) override;
        void EndOfEventAction(const G4Event*) override;

    private:

        vector<G4int> HC_ID_vec = {-1,-1,-1,-1};
        vector<G4double> EneDep_vec = {0,0,0};
        vector<G4double> E_vec = {0};
        vector<G4String> HC_names = {"scintillator1","scintillator2","scintillator3","dosimeter1"};
        runAction *rnAction;

        vector<G4int> HistIDs = {-1};
        vector<G4String> Hist_names = {"DosimeternFlux"};
};

#endif