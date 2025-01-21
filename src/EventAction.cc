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
  for (int i = 0; i < HC_names.size();i++){
    if (HC_ID_vec[i] == -1){
        auto sdManager = G4SDManager::GetSDMpointer();
        G4String HC_name = HC_names[i] + "/ScintillatorHC";
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

void EventAction::EndOfEventAction(const G4Event* event)
{
  auto analysisManager = G4AnalysisManager::Instance();

  for (int j = 0; j < HC_ID_vec.size();j++){
    G4double totEneDep = 0;
      auto hc = GetHC(event, HC_ID_vec[j]);
      if ( ! hc ) return;
      auto nhit = hc->GetSize();
      if (j < 3){
        for (unsigned long i = 0; i < nhit; ++i) {
          auto hit = static_cast<ScintillatorHit*>(hc->GetHit(i));
          EneDep_vec[j] = hit->GetEnergyDep();
          //G4cout <<"Energy deposited "<<EneDep/keV<<G4endl;
          rnAction->AddEDep(EneDep_vec[j],j);
          totEneDep += EneDep_vec[j];
          //G4cout <<"Total Energy deposited "<<totEneDep/keV<<G4endl;
        }
      
        if (totEneDep > 100*keV) rnAction->AddHit(j);
        }
      else{
        if (nhit > 0){
          G4double Ene = 0;
          // only care about first hit per event
          auto hit = static_cast<ScintillatorHit*>(hc->GetHit(0));
          Ene = hit->GetEnergy()*MeV;
          //G4cout <<"Hit Energy: "<<Ene<<G4endl;
          //if(Ene> 0 *keV) analysisManager->FillH1(HistIDs[j-3],Ene);
        }

      }
      }
}

