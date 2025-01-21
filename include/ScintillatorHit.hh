#ifndef ScintillatorHit_hh
#define ScintillatorHit_hh 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

class ScintillatorHit : public G4VHit
{
    public:
        ScintillatorHit() = default;
        ScintillatorHit(const ScintillatorHit&) = default;
        ~ScintillatorHit() override = default;

        ScintillatorHit& operator =(const ScintillatorHit&) = default;
        G4bool operator==(const ScintillatorHit&) const;

        inline void* operator new(size_t);
        inline void operator delete(void*);

        void Draw() override;
        void Print() override;

        void SetParticleType (G4String type)    { PartType = type;};
        void SetEnergyDep    (G4double edep)    { EneDep = edep;};
        void SetEnergy       (G4double ene)     { Energy = ene;};

        G4String GetParticleType() const    {return PartType; };
        G4double GetEnergyDep() const       {return EneDep; };
        G4double GetEnergy() const          {return Energy; };

    private:
        G4String PartType;
        G4double EneDep = -1;
        G4double Energy = -1;
};

// below we define new allocators for the scintillator hit class -> these are ripped straight from the B2 example

using ScintillatorHitsCollection = G4THitsCollection<ScintillatorHit>;

extern G4ThreadLocal G4Allocator<ScintillatorHit>* ScintillatorHitAllocator;

inline void* ScintillatorHit::operator new(size_t)
{
    if(!ScintillatorHitAllocator)
        ScintillatorHitAllocator = new G4Allocator<ScintillatorHit>;
    return (void *) ScintillatorHitAllocator->MallocSingle();
}

inline void ScintillatorHit::operator delete(void *hit)
{
  ScintillatorHitAllocator->FreeSingle((ScintillatorHit*) hit);
}

#endif