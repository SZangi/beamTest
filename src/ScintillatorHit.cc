#include "ScintillatorHit.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <iomanip>


// define the necessary functions for a hit

G4ThreadLocal G4Allocator<ScintillatorHit>* ScintillatorHitAllocator = nullptr;

// The operator function def

G4bool ScintillatorHit::operator==(const ScintillatorHit& right) const
{
    return ( this == &right ) ? true : false;
}

// the draw function override (no drawing)

void ScintillatorHit::Draw()
{;}

// the print function override (also no printing)
void ScintillatorHit::Print()
{
    G4cout << "the energy deposited was " << G4BestUnit(EneDep,"Energy") << G4endl;
    }