#ifndef __geometryConstruction_hh__
#define __geometryConstruction_hh__ 1

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "MagneticField.hh"


class MagneticField;
class MagneticField2;

using namespace std;

class geometryConstruction : public G4VUserDetectorConstruction
{
public:
  geometryConstruction();
  ~geometryConstruction();
  
  // Main function
  G4VPhysicalVolume *Construct() override;
  // Fields and Detectors function
  void ConstructSDandField() override;

  G4Material *IsoCarbon13();
  G4Material *mixScintillator();
  G4Material *EJ309();
  G4Material *dopeSilicon();

  G4LogicalVolume* GetDetector(G4int i) const {return Detectors[i];};

private:
    static G4ThreadLocal MagneticField* fMagneticField;
    static G4ThreadLocal G4FieldManager* fFieldMgr;

    static G4ThreadLocal MagneticField2* fMagneticField2;
    static G4ThreadLocal G4FieldManager* fFieldMgr2;

    G4LogicalVolume* magLogical = nullptr;
    G4LogicalVolume* mag2Logical = nullptr;

protected:
    //G4LogicalVolume* Detector = nullptr;
    //G4LogicalVolume* ERSDetector = nullptr;

    vector<G4LogicalVolume*> Detectors;
};

#endif
