#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SubtractionSolid.hh"
#include "G4Trd.hh"

#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4UserLimits.hh"
#include "G4SDManager.hh"

#include <iostream>

#include "geometryConstruction.hh"
#include "MagneticField.hh"
#include "MagneticField2.hh"
#include "ScintillatorSD.hh"
#include "XSBiasing.hh"

G4ThreadLocal MagneticField* geometryConstruction::fMagneticField = nullptr;
G4ThreadLocal G4FieldManager* geometryConstruction::fFieldMgr = nullptr;

G4ThreadLocal MagneticField2* geometryConstruction::fMagneticField2 = nullptr;
G4ThreadLocal G4FieldManager* geometryConstruction::fFieldMgr2 = nullptr;

geometryConstruction::geometryConstruction()
{;}

geometryConstruction::~geometryConstruction()
{;}

G4VPhysicalVolume *geometryConstruction::Construct()
{

  G4NistManager* nist = G4NistManager::Instance();

// Materials //
  G4double a = 1.01*g/mole;
  G4double z = 1.;
  G4double density = universe_mean_density; // A Geant4 constant

  G4Material* air = new G4Material("Galactic", 
				      z=1., 
				      a=1.01*g/mole,
				      density= universe_mean_density,
				      kStateGas, 
				      3.e-18*pascal, 
				      2.73*kelvin);

  a = 12.01 *g/mole;
  z = 6.;
  density = 2.26 *g/cm3;

  G4Element *C = new G4Element("Carbon", "C", z,a);
  G4Material *graphite = new G4Material("Carb", density,1);
  graphite -> AddElement(C, 100. *perCent);

  G4Element *Fe = new G4Element("Iron","Fe", 26.,54.*g/mole);
  G4Element *O = new G4Element("Oxygen","O",8.,16.*g/mole);
  G4Material *iron = new G4Material("Pure_Iron",7.87*g/cm3,1);
  iron -> AddElement(Fe, 100. *perCent);

  G4Material *carbon13 = IsoCarbon13();
  G4Material *scintMat = mixScintillator();

  G4Material* tissue = nist->FindOrBuildMaterial("G4_A-150_TISSUE");

  G4Material* ej309 = EJ309();

  G4Material* poly = nist->FindOrBuildMaterial("G4_POLYETHYLENE");

  G4Material* aluminum = nist->FindOrBuildMaterial("G4_Al");

  G4Material* concrete = nist->FindOrBuildMaterial("G4_CONCRETE");

// Volumes //

// Volume Dimensions //
G4double Hall_width = 6.5*m;

G4double World_Width = 4.5*m;
G4double World_Length = 8.7*m;

G4double Floor_Height = 1*m;

G4double Linac_Width = 29. *cm;
G4double Linac_Length = 3.172*m;
G4double Linac_Height = Linac_Width;

G4double Linac_Cent_Side1 = 1.83*m;
G4double Linac_Cent_Side2 = 1.41*m;

G4double Linac_tunnel_width = 5.*cm;
G4double Linac_tunnel_final = 4.19469*cm;

G4double SM_width = 21.8*cm;
G4double SM_out_rad = 1.16*m + SM_width/2;
G4double SM_in_rad = 1.16*m - SM_width/2;

G4double SM_Cent_Side1 = 44*cm;
G4double SM_Cent_Side2 = 3.722*m;

G4double ERS_Width = 42.1*cm;
G4double ERS_Length = 1.263*m;
G4double ERS_Height = ERS_Width;

G4double ERS_Cent_Side2 = 80.4*cm;
G4double ERS_Cent_Side1 = 3.67*m;

G4double ERS_Faraday_Width = 10*cm;
G4double ERS_Faraday_Length = 10*cm;
G4double ERS_Faraday_Height = 10*cm;

G4double ERS_Faraday_Cent = 18.9*cm;

G4double Target_Cham_Width = 1.26*m;
G4double Target_Cham_Length = 1.28*m;
G4double Target_Cham_Height = 1.2*m;

G4double Target_Cham_Thickness = 1.7*cm;

G4double Target_Cent_Side1 = 2.0*m;
G4double Target_Cent_Side2 = -20*cm;

G4double doseCupWidth = 20*cm;
G4double doseCupLength = 2*cm;
G4double doseCupHeight = 10*cm;

// Volume Definitions //
    G4Box* hallSolid
        = new G4Box("hallBox",World_Length/2,1.75*m,Hall_width/2);
    G4LogicalVolume* hallLogical 
        = new G4LogicalVolume(hallSolid,air,"hallLogical",0,0,0);
    G4VPhysicalVolume* hallPhysical = new G4PVPlacement(0,G4ThreeVector(),
        hallLogical,
        "hallPhysical",
        nullptr,
        false,
        0);

    G4Box* worldSolid
        = new G4Box("worldBox",World_Length/2,1.75*m,World_Width/2);
    G4LogicalVolume* worldLogical
        = new G4LogicalVolume(worldSolid,air,"worldLogical",
        0,
        0,
        0);
    G4VPhysicalVolume* worldPhysical = new G4PVPlacement(
        0, G4ThreeVector(0,0,1*m), 
        worldLogical, 
        "worldPhysical", 
        hallLogical, 
        false, 
        0);

    G4Box* floorSolid = 
        new G4Box("floorBox",World_Length/2, Floor_Height/2, World_Width/2);

    G4LogicalVolume* floorLogical
        = new G4LogicalVolume(floorSolid, concrete,"floorLogical",
            0,
            0,
            0);
    new G4PVPlacement(0,G4ThreeVector(0,-1.25*m,0),
        floorLogical,
        "floorPhysical",
        worldLogical,
        false,
        0);
    
    G4Box* wallSolid = 
        new G4Box("wallBox",World_Length/2,1.75*m,0.75*m);
    G4LogicalVolume* wallLogical =
        new G4LogicalVolume(wallSolid,concrete,"wallLogical",0,0,0);
    new G4PVPlacement(0,G4ThreeVector(0,0,-2.25*m),
        wallLogical,
        "wallPhysical",
        hallLogical,
        false,
        0);

    G4Box* linacShell
        = new G4Box("linacBox",Linac_Length/2,Linac_Height/2,Linac_Width/2);

    G4VSolid* linacLiner
        = new G4Trd("linacLiner", Linac_tunnel_width/2,Linac_tunnel_final/2,Linac_tunnel_width/2,Linac_tunnel_final/2,Linac_Length/2);

    G4RotationMatrix* linerRotate = new G4RotationMatrix();
    linerRotate->rotateY(-90*deg);
    G4VSolid* linacSolid = 
        new G4SubtractionSolid("linacS",linacShell,linacLiner,linerRotate,G4ThreeVector(0,0,0));

    G4LogicalVolume* linacLogical
        = new G4LogicalVolume(linacSolid,carbon13,"linacLogical",
        0,
        0,
        0);
    auto fieldRot = new G4RotationMatrix();
    fieldRot->rotateY(36.152*deg);

    G4VPhysicalVolume* linacPhysical
        = new G4PVPlacement(fieldRot,G4ThreeVector(Linac_Cent_Side1-World_Length/2,0.*cm,World_Width/2-Linac_Cent_Side2),
                            linacLogical,
                            "linacPhysical",
                            worldLogical,
                            false,
                            0);    



    G4Tubs* magSolid
        = new G4Tubs("magSolid",SM_in_rad,SM_out_rad,5*cm,(270-36)*deg,61.3*deg);
    magLogical
        = new G4LogicalVolume(magSolid,air,"magLogical",
        0,
        0,
        0);

    auto userLimits = new G4UserLimits(10 * cm);
    magLogical->SetUserLimits(userLimits);
    
    auto fieldRot1 = new G4RotationMatrix();
    fieldRot1->rotateX(90.*deg);
    G4VPhysicalVolume* magPhysical = new G4PVPlacement(
        fieldRot1,G4ThreeVector(SM_Cent_Side2-(World_Length/2)+12*cm,0*cm,World_Width/2-SM_Cent_Side1-SM_in_rad+12*cm),
        magLogical,
        "magPhysical",
        worldLogical,
        false,
        0);

    auto ersSolid
        = new G4Box("ersBox",ERS_Length/2,ERS_Height/2,ERS_Width/2);
    G4LogicalVolume *ersLogical
        = new G4LogicalVolume(ersSolid,air,"ersLogical");
    auto fieldRot2 = new G4RotationMatrix();
    fieldRot2->rotateY(-23.*deg);
    G4VPhysicalVolume* ersPhysical 
            = new G4PVPlacement(fieldRot2,G4ThreeVector(World_Length/2-ERS_Cent_Side1,0*cm,World_Width/2-ERS_Cent_Side2+21*cm),
                                ersLogical,
                                "ersPhysical",
                                worldLogical,
                                false,
                                0); 

    auto recieveSolid
        = new G4Box("recieveBox",ERS_Faraday_Length/2,ERS_Faraday_Height/2,ERS_Faraday_Width/2);
    G4LogicalVolume *recieveLogical
        = new G4LogicalVolume(recieveSolid,carbon13,"recieveLogical");
    G4VPhysicalVolume* recievePhysical 
            = new G4PVPlacement(0,G4ThreeVector(ERS_Faraday_Cent-ERS_Length/2,0*cm,0.*cm),
                                recieveLogical,
                                "recievePhysical",
                                ersLogical,
                                false,
                                0); 

    // ERS Box shell

    auto recieveInner
        = new G4Box("recieve_inner",ERS_Length/2-0.25*cm,
                    ERS_Height/2-0.25*cm,
                    ERS_Width/2-0.25*cm);
    auto recieveEnter 
        = new G4Box("recieveEnter", 1*cm,5*cm,5*cm);
    auto recieveShell1 = new G4SubtractionSolid("recieveShell1",ersSolid,recieveInner);
    auto recieveShell2 = new G4SubtractionSolid("recieveShell2",recieveShell1,recieveEnter,0,G4ThreeVector(ERS_Length/2,0,0));
    auto recieveShell = new G4SubtractionSolid("recieveShell2",recieveShell2,recieveEnter,0,G4ThreeVector(-ERS_Length/2,0,0));
    G4LogicalVolume *recShellLogical
        = new G4LogicalVolume(recieveShell,graphite,"recShellLogical");
    /*
    G4VPhysicalVolume* recShellPhysical
        = new G4PVPlacement(0,G4ThreeVector(0,0,0),
                            recShellLogical,
                            "recShellPhysical",
                            ersLogical,
                            false,
                             0);
    */
    auto ersFaradaySolid
        = new G4Box("ersFaradayBox",ERS_Faraday_Length/2,ERS_Faraday_Height/4,ERS_Faraday_Width/4);
    G4LogicalVolume *ersFaradayLogical
        = new G4LogicalVolume(ersFaradaySolid,air,"ersFaradayLogical");
    G4VPhysicalVolume* ersFaradayPhysical
        = new G4PVPlacement(0,G4ThreeVector(0,0,0),
                            ersFaradayLogical,
                            "ersFaradayPhysical",
                            recieveLogical,
                            false,
                            0);

    auto SteeringSolid
        = new G4Box("steeringBox", 30/2*cm,5*cm,1*cm);
    G4LogicalVolume *steeringLogical
        = new G4LogicalVolume(SteeringSolid,graphite,"steeringLogical");
    G4VPhysicalVolume *steeringPhysical
        = new G4PVPlacement(0,G4ThreeVector(-15*cm,0,-5*cm),
        steeringLogical,
        "steeringPhysical",
        ersLogical,
        false,
        0);

    auto SteeringSolid1
        = new G4Box("steeringBox1", 30/2*cm,5*cm,1*cm);
    G4LogicalVolume *steeringLogical1
        = new G4LogicalVolume(SteeringSolid1,graphite,"steeringLogical1");
    G4VPhysicalVolume *steeringPhysical1
        = new G4PVPlacement(0,G4ThreeVector(-15*cm,0,5*cm),
        steeringLogical1,
        "steeringPhysical1",
        ersLogical,
        false,
        0);

    auto fieldRot3 = new G4RotationMatrix();
    fieldRot3->rotateY(-7*deg);

    auto scanningSolid1
        = new G4Box("scanningBox1", 57.3/2*cm,5*cm,1*cm);
    G4LogicalVolume *scanningLogical1
        = new G4LogicalVolume(scanningSolid1,graphite,"scanningLogical1");
    G4VPhysicalVolume *scanningPhysical1
        = new G4PVPlacement(fieldRot3,G4ThreeVector(30*cm,0,-10*cm),
        scanningLogical1,
        "scanningPhysical1",
        ersLogical,
        false,
        0);

    auto fieldRot4 = new G4RotationMatrix();
    fieldRot4->rotateY(7*deg);
    auto scanningSolid2
        = new G4Box("scanningBox2", 57.3/2*cm,5*cm,1*cm);
    G4LogicalVolume *scanningLogical2
        = new G4LogicalVolume(scanningSolid2,graphite,"scanningLogical2");
    G4VPhysicalVolume *scanningPhysical2
        = new G4PVPlacement(fieldRot4,G4ThreeVector(30*cm,0,10*cm),
        scanningLogical2,
        "scanningPhysical2",
        ersLogical,
        false,
        0);

    
    // For neutron dose monitor
    G4Tubs* detector2 = new G4Tubs("dosimeterSolid",0*cm,4.5*2.54*cm,6.3*2.54*cm,0,2*pi);

    G4LogicalVolume *dosimeterLogic = 
        new G4LogicalVolume(detector2,air,"dosimeterLogical");

    new G4PVPlacement(fieldRot1,G4ThreeVector(Target_Cent_Side1+50*cm,0*cm,Target_Cent_Side2-130*cm),
                    dosimeterLogic,
                    "dosimeterPhysical",
                    worldLogical,
                    false,
                    0);
    
    G4Tubs* detector1 = new G4Tubs("detectSolid",0*cm,1*2.54*cm,1*2.54*cm,0,2*pi); // For EJ309 detector

    G4LogicalVolume *detectLogic 
        = new G4LogicalVolume(detector1,ej309,"detectorLogical");

    Detectors.push_back(detectLogic);
    
    
    G4VPhysicalVolume *detectPhysical
        = new G4PVPlacement(0,G4ThreeVector(Target_Cent_Side1+30*cm,0*cm,Target_Cent_Side2-130*cm),
            detectLogic,
            "detectPhysical",
            worldLogical,
            false,
            0);

    // ERS EJ309
    G4LogicalVolume *ERSdetectLogic
        = new G4LogicalVolume(detector1,ej309,"ERSdetectLogic");

    Detectors.push_back(ERSdetectLogic);

    G4VPhysicalVolume *ERSdetectPhysical
        = new G4PVPlacement(0,G4ThreeVector(World_Length/2-ERS_Cent_Side1-50*cm,ERS_Height/2+2.54*cm,World_Width/2-65*cm),
            ERSdetectLogic,
            "ERSdetectPhysical",
            worldLogical,
            false,
            0);
    
    //ERS Dose meter
    G4LogicalVolume *ERSdoseLogic
        = new G4LogicalVolume(detector2,air,"ERSdoseLogic");

    new G4PVPlacement(fieldRot1,G4ThreeVector(World_Length/2-ERS_Cent_Side1-10*cm,ERS_Height/2+17.54*cm,World_Width/2-35*cm),
            ERSdoseLogic,
            "ERSdosePhysical",
            worldLogical,
            false,
            0);
    

    G4LogicalVolume *FaradaydetectLogic
        = new G4LogicalVolume(detector1,ej309,"FaradaydetectLogic");

    Detectors.push_back(FaradaydetectLogic);

    G4VPhysicalVolume *FaradaydetectPhysical
    = new G4PVPlacement(0,G4ThreeVector(1.9*m,47*cm,1.3*m),
        FaradaydetectLogic,
        "FaradaydetectPhysical",
        worldLogical,
        false,
        0); 

    // Faraday Dose Meter

    G4LogicalVolume *FaradayDoseLogic
        = new G4LogicalVolume(detector2,air,"FaradaydoseLogic");

    new G4PVPlacement(fieldRot1,G4ThreeVector(2.1*m,60*cm,0.9*m),
        FaradayDoseLogic,
        "FaradaydosePhysical",
        worldLogical,
        false,
        0);     

    G4Tubs* mag2Solid = new G4Tubs("mag2Solid",1.52*m,2.18*m,10*cm,(360-40)*deg,32*deg);
    mag2Logical = new G4LogicalVolume(mag2Solid,air,"mag2Logical",
                    0,
                    0,
                    0);

    mag2Logical -> SetUserLimits(userLimits);

    G4VPhysicalVolume* mag2Physical = new G4PVPlacement(
        fieldRot1, G4ThreeVector(World_Length/2-226.5*cm-2*m,0,World_Width/2-126*cm-75*cm),
        mag2Logical,
        "mag2Physical",
        worldLogical,
        false,
        0);

    G4LogicalVolume* mag2Logical_1 = new G4LogicalVolume(mag2Solid,iron,"mag2Logical_1",
                    0,
                    0,
                    0);
    G4VPhysicalVolume* mag2Physical_1 = new G4PVPlacement(
        fieldRot1, G4ThreeVector(World_Length/2-226.5*cm-2*m,20*cm,World_Width/2-126*cm-75*cm),
        mag2Logical_1,
        "mag2Physical_1",
        worldLogical,
        false,
        0);    
    
    G4Tubs* mag3Solid = new G4Tubs("mag2Solid",1.52*m,2.18*m,7*cm,(360-40)*deg,32*deg);
    G4LogicalVolume* mag2Logical_3 = new G4LogicalVolume(mag3Solid,iron,"mag2Logical_3",0,0,0);
    new G4PVPlacement(fieldRot1, G4ThreeVector(World_Length/2-226.5*cm-2*m,37*cm,World_Width/2-126*cm-75*cm),
                mag2Logical_3,
                "mag2Physical_3",
                worldLogical,
                false,
                0);

    G4LogicalVolume* mag2Logical_2 = new G4LogicalVolume(mag2Solid,iron,"mag2Logical_2",
                    0,
                    0,
                    0);
    G4VPhysicalVolume* mag2Physical_2 = new G4PVPlacement(
        fieldRot1, G4ThreeVector(World_Length/2-226.5*cm-2*m,-20*cm,World_Width/2-126*cm-75*cm),
        mag2Logical_2,
        "mag2Physical_2",
        worldLogical,
        false,
        0);   

    G4Box* magBoxOuter = new G4Box("magBoxOuter",80/2*cm,30/2*cm,40/2*cm);
    G4Box* magBoxInner = new G4Box("magBoxInner",80/2*cm-Target_Cham_Thickness/2,30/2*cm-Target_Cham_Thickness/2,40/2*cm-Target_Cham_Thickness/2);
    G4VSolid* magBox = new G4SubtractionSolid("magBox",magBoxOuter,magBoxInner);;

    G4LogicalVolume* magBoxL = new G4LogicalVolume(magBox, aluminum,"magBoxLog",
                        0, 0, 0);

    auto fieldRot5 = new G4RotationMatrix();
    fieldRot5->rotateY(-60*deg);

    G4VPhysicalVolume* magBoxP = new G4PVPlacement(fieldRot5,G4ThreeVector(1.75*m,60*cm,1.*m),
                                                    magBoxL,
                                                    "magBoxP",
                                                    worldLogical,
                                                    false,
                                                    0);

    G4Box* targetInner
        = new G4Box("targetInner",Target_Cham_Length/2,Target_Cham_Height/2,Target_Cham_Width/2);
    
    G4Box* targetShell 
        = new G4Box("targShell",Target_Cham_Length/2+Target_Cham_Thickness,
        Target_Cham_Height/2+Target_Cham_Thickness,
        Target_Cham_Width/2+Target_Cham_Thickness);

    G4VSolid* targetBox
        = new G4SubtractionSolid("targetBox", targetShell, targetInner, 0 , G4ThreeVector(0,0,0));

    G4Box* targetGap
        = new G4Box("targOpening",Target_Cham_Thickness,Linac_tunnel_width/2,40*cm/2);

    G4Box* doseCupGap 
        = new G4Box("doseCupGap", Target_Cham_Thickness,doseCupHeight/2,doseCupWidth/2);

    G4VSolid* targetMinDose
        = new G4SubtractionSolid("targetMinDose", targetBox,doseCupGap, 0, G4ThreeVector(Target_Cham_Length/2,0,doseCupLength/2+14*cm));

    G4VSolid* targetSolid
        = new G4SubtractionSolid("targetFinal", targetBox, targetGap, 0, G4ThreeVector(-Target_Cham_Length/2,0,15*cm));

    G4LogicalVolume* targetLogical
        = new G4LogicalVolume(targetSolid,iron,"targetLogical",
        0,
        0,
        0);

    //new G4PVPlacement(fieldRot1, G4ThreeVector(0,0,Target_Cham_Width/2-51.1*cm),detectLogic,"DetectorPhysical",targetLogical,false,0);

    auto fieldRot6 = new G4RotationMatrix();
    fieldRot6->rotateY(-82*deg);
    G4VPhysicalVolume* targetPhysical
        = new G4PVPlacement(fieldRot6,G4ThreeVector(Target_Cent_Side1,0.*cm,Target_Cent_Side2),
                            targetLogical,
                            "targetPhysical",
                            worldLogical,
                            false,
                            0);

    G4Box* doseCupSolid 
        = new G4Box("doseCupBox",doseCupLength/2,doseCupHeight/2,doseCupWidth/2);
    G4LogicalVolume* doseCupLogical
        = new G4LogicalVolume(doseCupSolid,carbon13,"doseCupLogical",
            0,
            0,
            0);

    G4VPhysicalVolume* doseCupPhysical 
        = new G4PVPlacement(fieldRot6,
        //G4ThreeVector(0,0,0),
        G4ThreeVector(Target_Cent_Side1+Target_Cham_Width/2-40*cm,0,Target_Cent_Side2-Target_Cham_Length/2+doseCupLength/2+3*cm),
        doseCupLogical,
        "doseCupPhysical",
        worldLogical,
        false,
        0);
    
    // Visualization //
    auto visAttributes = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
    visAttributes->SetVisibility(true);
    worldLogical->SetVisAttributes(visAttributes);

    auto visAttributes_solid = new G4VisAttributes(G4Colour(0.9,0.9,0.9,0.5));
    visAttributes_solid->SetForceSolid(1);
    recieveLogical->SetVisAttributes(visAttributes_solid);
    scanningLogical1->SetVisAttributes(visAttributes_solid);
    scanningLogical2->SetVisAttributes(visAttributes_solid);
    steeringLogical1->SetVisAttributes(visAttributes_solid);
    steeringLogical->SetVisAttributes(visAttributes_solid);
    linacLogical->SetVisAttributes(visAttributes_solid);
    doseCupLogical->SetVisAttributes(visAttributes_solid);
    floorLogical->SetVisAttributes(visAttributes_solid);
    wallLogical->SetVisAttributes(visAttributes_solid);

    auto visAttributes_iron = new G4VisAttributes(G4Colour(0.6,0.4,0,0.5));
    visAttributes_iron -> SetForceSolid(1);
    mag2Logical_1->SetVisAttributes(visAttributes_iron);
    mag2Logical_2->SetVisAttributes(visAttributes_iron);
    recShellLogical->SetVisAttributes(visAttributes_iron);
    
    auto visAttributes_detect = new G4VisAttributes(G4Colour(0,0.7,0.2,1.0));
    visAttributes_detect->SetForceSolid(1);
    detectLogic->SetVisAttributes(visAttributes_detect);
    ERSdetectLogic->SetVisAttributes(visAttributes_detect);
    FaradaydetectLogic->SetVisAttributes(visAttributes_detect);
    
    auto visAttributes_dose = new G4VisAttributes(G4Colour(0.5,0.5,0,0.5));
    visAttributes_dose -> SetForceSolid(1);
    dosimeterLogic->SetVisAttributes(visAttributes_dose);
    ERSdoseLogic->SetVisAttributes(visAttributes_dose);
    FaradayDoseLogic->SetVisAttributes(visAttributes_dose);

    auto visAttributes_wire = new G4VisAttributes(G4Colour(0.,1.,0.));
    visAttributes_wire ->SetForceWireframe(1);
    ersFaradayLogical->SetVisAttributes(visAttributes_wire);
    
    return hallPhysical;
}

void geometryConstruction::ConstructSDandField()
{
    /*
    MagneticField* magField = new MagneticField();
    G4FieldManager* fieldMgr = new G4FieldManager();
    fieldMgr->SetDetectorField(magField);
    fieldMgr->CreateChordFinder(magField);
    magLogical->SetFieldManager(fieldMgr,false);
    */

// Sensitive Detector defintion
auto sdManager = G4SDManager::GetSDMpointer();
G4String SDname="/scintillator1";

G4String ERS_SDname = "/scintillator2";

G4String Faraday_SDname = "/scintillator3";

G4String Dosimeter_SDname = "/dosimeter1";

auto scintillator = new ScintillatorSD(SDname,"ScintillatorHC");
sdManager->AddNewDetector(scintillator);
SetSensitiveDetector("detectorLogical",scintillator, false);

auto ERSscintillator = new ScintillatorSD(ERS_SDname,"ScintillatorHC");
sdManager->AddNewDetector(ERSscintillator);
SetSensitiveDetector("ERSdetectLogic",ERSscintillator, false);

auto Faradayscintillator = new ScintillatorSD(Faraday_SDname,"ScintillatorHC");
sdManager->AddNewDetector(Faradayscintillator);
SetSensitiveDetector("FaradaydetectLogic",Faradayscintillator, false);

auto dosimeter = new ScintillatorSD(Dosimeter_SDname,"ScintillatorHC");
sdManager->AddNewDetector(dosimeter);
SetSensitiveDetector("dosimeterLogical",dosimeter, false);


// magnetic field for first cylinder --------------------------------------------
  fMagneticField = new MagneticField();
  fFieldMgr = new G4FieldManager();
  fFieldMgr->SetDetectorField(fMagneticField);
  fFieldMgr->CreateChordFinder(fMagneticField);
  G4bool forceToAllDaughters = false;
  magLogical->SetFieldManager(fFieldMgr, forceToAllDaughters);

// magnetic field for second cylinder ------------------------------------------

    fMagneticField2 = new MagneticField2();
    fFieldMgr2 = new G4FieldManager();
    fFieldMgr2->SetDetectorField(fMagneticField2);
    fFieldMgr2->CreateChordFinder(fMagneticField2);
    mag2Logical->SetFieldManager(fFieldMgr2, forceToAllDaughters);

// cross-section biasing for graphite components -------------------------------

    XSBiasing* alpha_rxs = 
        new XSBiasing("alpha");  // select the particle we want to bias

    // get appropriate volume for biasing
    G4LogicalVolume* logic_dosecup = G4LogicalVolumeStore::GetInstance()->GetVolume("doseCupLogical");
    G4LogicalVolume* logic_ers = G4LogicalVolumeStore::GetInstance()->GetVolume("recieveLogical");
    G4LogicalVolume* logic_linac = G4LogicalVolumeStore::GetInstance()->GetVolume("linacLogical");

    alpha_rxs->AttachTo(logic_dosecup); // apply biasing
    alpha_rxs->AttachTo(logic_ers);
    alpha_rxs->AttachTo(logic_linac);

    G4cout << " Attaching biasing operator " << alpha_rxs->GetName()
         << " to logical volumes " << logic_dosecup->GetName() << G4endl <<
         logic_ers->GetName() << G4endl << logic_linac->GetName()
         << G4endl; // prints out what biasing we've done
}

G4Material *geometryConstruction::IsoCarbon13() {
  // Carbon13
  G4double a;
  G4double z;
  G4double n;
  G4double density = 1.604*g/cm3;
  G4int nComp;

  G4Isotope *C13 = new G4Isotope("Carbon-13", z= 6, n= 13, a= 13.0035 *g/mole);
  G4Element *PureC13 = new G4Element("Pure Carbon13", "C", nComp = 1);
  PureC13 -> AddIsotope(C13, 100. *perCent);
  G4Material *Carbon13 = new G4Material("Carbon13", density, nComp);
  Carbon13 -> AddElement(PureC13, 100. *perCent);

  return Carbon13;
}

G4Material *geometryConstruction::mixScintillator()
{
  // Saint Gobain BC404 specifications obtained from:
  // http://www.detectors.saint-gobain.com/ -> search for "BC404"
  G4double a;
  G4double z;
  G4double density;
  G4int nComp;
  G4int Catoms;
  G4int Hatoms;
  
  a = 12.01*g/mole;
  z = 6.;
  G4Element *C = new G4Element("carbon", "C", z,a);

  a = 1.01*g/mole;
  z = 1.;
  G4Element *H = new G4Element("hydrogen", "H", z,a);
  
  // Another way to create materials...specifying the number of atoms
  // of each element...
  density = 1.032*g/cm3;
  nComp = 2;
  Catoms = 474;
  Hatoms = 521;
  G4Material *scintillator = new G4Material("scintillator", density, nComp);
  scintillator -> AddElement(C, Catoms);
  scintillator -> AddElement(H, Hatoms);

  return scintillator;
}

G4Material *geometryConstruction::EJ309()
{
    G4double density = 0.959*g/cm3;
    G4int C_atoms = 435;
    G4int H_atoms = 543;

    G4Material *EJ309 = new G4Material("EJ309",density,2);
    EJ309 -> AddElement(G4NistManager::Instance()->FindOrBuildElement(6), C_atoms);
    EJ309 -> AddElement(G4NistManager::Instance()->FindOrBuildElement(1), H_atoms);
    
    return EJ309;
}
