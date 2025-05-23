#include "G4RunManager.hh"
#include "iostream"
#include "G4Accumulable.hh"
#include "G4AccumulableManager.hh"

//#include "G4AccValue.hh"

#include "runAction.hh"
#include "geometryConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4AnalysisManager.hh"


runAction::runAction()
{
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetDefaultFileType("root");
    
    analysisManager->CreateH1("DosimeternFlux", "Neutron Flux in Dose Meter", N_Energies);
    analysisManager->CreateH1("DosimeternFlux2", "Neutron Flux in Dose Meter", N_Energies);
    analysisManager->CreateH1("DosimeternFlux3", "Neutron Flux in Dose Meter", N_Energies);

    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->RegisterAccumulable(Faraday_dose);
    accumulableManager->RegisterAccumulable(ERS_dose);
    accumulableManager->RegisterAccumulable(Corrector_dose);
    
    }

runAction::~runAction()
{;}

void runAction::BeginOfRunAction(const G4Run*)
{for(G4int i = 0; i<Hits.size();i++){  EnergyDeposited[i] = 0.; Hits[i]=0;}
 G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
 accumulableManager->Reset();
}

void runAction::EndOfRunAction(const G4Run* run)
{
    
    if(IsMaster()){
        G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
        accumulableManager->Merge();
        
        G4cout<< "************END OF GLOBAL RUN***********"<<G4endl;
        G4cout<<"Run of "<< run->GetNumberOfEvent() <<" events complete." << G4endl;
        G4double Fin_Faraday_dose = Faraday_dose.GetValue();
        G4double Fin_ERS_dose = ERS_dose.GetValue();
        G4double Fin_Corrector_dose = Corrector_dose.GetValue();
        G4cout<<"Total Dose at " << Pos_Names[0]<< " is "<< Fin_Faraday_dose/10<< " nRem" << G4endl;
        //G4cout<<"Total Dose at " << Pos_Names[1]<< " is "<< Fin_ERS_dose/(2*4.5*2.54*6.3*2.54*2)/10<< " nRem" << G4endl;
        //G4cout<<"Total Dose at " << Pos_Names[2]<< " is "<< Fin_Corrector_dose/(2*4.5*2.54*6.3*2.54*2)/10<< " nRem" << G4endl;
        //G4cout << "Ratio of Dose Cup to ERS is: " << Fin_ERS_dose/Fin_Faraday_dose << G4endl
        //<< "Ratio of Dose Cup to Correctors is: "<< Fin_Corrector_dose/Fin_Faraday_dose<< G4endl;
    }
    else{
    /*
    const auto geometry = static_cast<const geometryConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    for(G4int i = 0; i<Hits.size();i++){
        G4double mass = geometry->GetDetector(i)->GetMass();
        G4double dose = EnergyDeposited[i]/mass;

        G4cout << "Total Hits Are: " << Hits[i] <<G4endl;
        }
    */
    auto analysisManager = G4AnalysisManager::Instance();

    HistIds[0] = analysisManager->GetH1Id("DosimeternFlux");
    HistIds[1] = analysisManager->GetH1Id("DosimeternFlux2");
    HistIds[2] = analysisManager->GetH1Id("DosimeternFlux3");
    for (G4int j=0; j<HistIds.size(); j++){ 
        auto Histogram1D = analysisManager->GetH1(HistIds[j]);
        G4double sum = 0;
        G4bool success = false;
        G4int num_bins = Histogram1D->get_bins();
        for (G4int i=1;i<num_bins; i++){
            sum += Histogram1D->bin_entries(i)*Conv_Fac[i-1];
            }
         //G4cout<<"Dose to hits ratio is " <<<< " is "<< sum/(2*4.5*2.54*6.3*2.54*2)/10<< " nRem" << G4endl;
        dose[j] = sum;
        }
    Faraday_dose = dose[0];
    ERS_dose = dose[1];
    Corrector_dose = dose[2];
    //G4cout << "Ratio of Dose Cup to ERS is: " << dose[1]/dose[0] << G4endl
    //        << "Ratio of Dose Cup to Correctors is: "<< dose[2]/dose[0]<< G4endl;
    
    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->Merge();
    }
}