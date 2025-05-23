#ifndef runAction_hh
#define runAction_hh 1

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4Accumulable.hh"

#include <string>

using namespace std;

class G4Run;

class runAction : public G4UserRunAction
{
    public:
        runAction();
        ~runAction();

        void BeginOfRunAction(const G4Run*) override;
        void EndOfRunAction(const G4Run*) override;

        void AddEDep(G4float edep, G4int i) {EnergyDeposited[i] += edep;};
        void AddHit(G4int i) {Hits[i] ++;};

        vector<G4float> EnergyDeposited = {-1,-1,-1};
        vector<G4int> Hits = {-1,-1,-1};

        vector<G4int> HistIds = {-1,-1,-1};

    private:
        const vector<G4double> N_Energies = {0.01,0.02,0.03,0.05,0.07,0.1,0.15,0.2,0.3,0.5,0.7,0.9,1,1.2,2,3,4,5,6,7};
        //vector<G4double> Conv_Fac = {18.3,23.8,29.0,38.5,47.2,59.8,80.2,99.0,133,188,231,267,282,310,383,432,458,474};
        vector<G4double> Conv_Fac = {7.70,10.2,12.7,17.3,21.5,27.2,35.2,42.5,54.7,75.0,92.8,108,116,130,178,220,250,272,282,290};

        vector<G4String> Pos_Names = {"Dose Cup","ERS", "Corrector"};

        vector<G4double> dose = {-1,-1,-1};

        G4Accumulable<G4double> Faraday_dose = 0;
        G4Accumulable<G4double> ERS_dose = 0;
        G4Accumulable<G4double> Corrector_dose = 0;

};

#endif