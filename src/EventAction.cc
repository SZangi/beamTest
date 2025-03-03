#include "EventAction.hh"
#include "runAction.hh"
#include "ScintillatorHit.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4AnalysisManager.hh"

#include <iostream>

class RunAction;

G4VHitsCollection* GetHC(const G4Event* event, G4int collId) {
  auto hce = event->GetHCofThisEvent();
  if (!hce) {
      G4ExceptionDescription msg;
      msg << "No hits collection of this event found." << G4endl;
      G4Exception("EventAction::EndOfEventAction()",
                  "Code001", JustWarning, msg);
      return nullptr;
  }

  auto hc = hce->GetHC(collId);
  if ( ! hc) {
    G4ExceptionDescription msg;
    msg << "Hits collection " << collId << " of this event not found." << G4endl;
    G4Exception("EventAction::EndOfEventAction()",
                "Code001", JustWarning, msg);
  }
  return hc;
}

EventAction::EventAction(runAction *currentRun)
    : rnAction(currentRun)
    {for (int i = 0; i < EneDep_vec.size(); ++i) EneDep_vec[i] = -1;}

void EventAction::BeginOfEventAction(const G4Event*)
{
  if (NumHC >0){
    for (int i = 0; i < HC_names.size();i++){
      if (HC_ID_vec[i] == -1){
          auto sdManager = G4SDManager::GetSDMpointer();
          G4String HC_name = HC_names[i] + "/ScintillatorHC"+G4String(std::to_string(i+1));
          HC_ID_vec[i] = sdManager->GetCollectionID(HC_name);
      }
      }

    for (int i = 0; i <HistIDs.size();i++){
      if(HistIDs[i] == -1){
        auto analysisManager = G4AnalysisManager::Instance();
        HistIDs[i] = analysisManager->GetH1Id(Hist_names[i]);
        G4cout<<"HistID Set"<<G4endl;
      }
    }
  }
}

void EventAction::EndOfEventAction(const G4Event* event)
{
  //auto analysisManager = G4AnalysisManager::Instance();
  if (NumHC >0){
  G4int nhit = 0;
  G4float totEneDep = 0;
  G4float Ene = 0;
  ScintillatorHitsCollection* hc = new ScintillatorHitsCollection;
  for (int j = 0; j < HC_ID_vec.size();j++){
    totEneDep = 0;
    hc = dynamic_cast<ScintillatorHitsCollection*>(GetHC(event, HC_ID_vec[j]));
    if ( ! hc ) return;
      nhit = hc->entries();
    if (j < 3){
      for (unsigned long i = 0; i < nhit; ++i) {
        //auto hit = dynamic_cast<ScintillatorHit*>(hc->GetHit(i));
        //EneDep_vec[j] = (*hc)[i]->GetEnergyDep();
        //delete hit;
        EneDep_vec[j] = (*hc)[i]->GetEnergyDep();
        rnAction->AddEDep(EneDep_vec[j],j);
        totEneDep += EneDep_vec[j];
      }
    
      if (totEneDep > 100*keV) rnAction->AddHit(j);
    }
    else{
      if (nhit > 0){
        Ene = 0;
        // only care about first hit per event
        //auto hit = dynamic_cast<ScintillatorHit*>(hc->GetHit(0));
        Ene = (*hc)[0]->GetEnergy();
        //Ene = hit->GetEnergy()*MeV;
        //delete hit;
      }
    }
  }
}
}

