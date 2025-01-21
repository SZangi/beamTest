/*
********************************************************
beamTest

A simple code to fire a beam of charged particles
down a path with a few curves defined by their magnetic
field strength into a target

********************************************************
*/

// G4 Header Files
#include "G4RunManager.hh" 
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "QGSP_BIC.hh"
#include "QGSP_BIC_AllHP.hh"
#include "QBBC.hh"
#include "G4GenericBiasingPhysics.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4TrajectoryDrawByParticleID.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4ScoringManager.hh"
#include "G4AnalysisManager.hh"
#include "G4TScoreHistFiller.hh"

#include <iostream>

// User Header Files
#include "geometryConstruction.hh"
#include "PGA.hh"
#include "PhysicsList.hh"
#include "runAction.hh"
#include "EventAction.hh"

int main(int argc, char *argv[])
{
  // Create a runManager to handle the flow of operations in the program.  
  G4RunManager* runManager = new G4RunManager;

  // Activate command-based scorer
  G4ScoringManager::GetScoringManager();

  // Add an analysis manager for histograms
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
      
  // Activate command based histogramming
  auto histFiller = new G4TScoreHistFiller<G4AnalysisManager>;

  // Create new "mandatory defined" class objects and tell the
  // runManager to initialize them for use
  runManager -> SetUserInitialization(new geometryConstruction);
  // new physics list with individual lists registered for different processes
  QGSP_BIC_AllHP* physicsList = new QGSP_BIC_AllHP(0); //QBBC is the recommended list for thin target high precision modelling
    physicsList->RegisterPhysics(new G4StepLimiterPhysics());
    G4GenericBiasingPhysics* biasingPhysics = new G4GenericBiasingPhysics();
    biasingPhysics -> Bias("alpha");
    physicsList->RegisterPhysics(biasingPhysics);

  runManager -> SetUserInitialization(physicsList);
  
  runManager -> SetUserAction(new PGA);
  
  // Create new "user defined" class objects and tell the runManager
  // to initialize them for use

  runAction *rnAction = new runAction();
  runManager->SetUserAction(rnAction);

  runManager->SetUserAction(new EventAction(rnAction));

  runManager -> Initialize();
  
  // If the shell variable for visualization use is set, then create a
  // manager to handle the visualization processes  
  G4VisManager* visManager = new G4VisExecutive();
  visManager -> Initialize();
  //G4TrajectoryDrawByParticleID* model = new G4TrajectoryDrawByParticleID;
  //visManager->RegisterModel(model);
  //visManager->SelectTrajectoryModel(model->Name());

  // Get the U(ser)I(interface) pointer to allow...*suspense*
  // ...user interface!
  G4UImanager* UI = G4UImanager::GetUIpointer();
  if (argc == 1)
  {
    // Create a modern UI interface with embedded OpenGL graphics
    G4UIExecutive *UIExecutive = new G4UIExecutive(argc, argv, "Qt");
    UI-> ApplyCommand("/control/execute RMatrixGen.vis"); 
    UI -> ApplyCommand("/run/verbose 1");
    UI -> ApplyCommand("/event/verbose 0");
    UI -> ApplyCommand("/hits/verbose 0");
    UI -> ApplyCommand("/tracking/verbose 0");
    UI -> ApplyCommand("/tracking/storeTrajectory 1");

    UIExecutive -> SessionStart();

    // Make sure to "delete" anything that we have create above using
    // "new".  This is good memory management

    delete UIExecutive;
    delete UI;
  }
  else{
    G4String command = "/control/execute ";
    G4String fileName;
    if (argc ==2){
      fileName = argv[1];
      }
    
    UI->ApplyCommand(command+fileName);
    G4cout<< "Press Enter to continue"<<G4endl;
    G4int go = cin.get();
  }

  delete visManager;
  delete runManager;
  
  return 0;
}