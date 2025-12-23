## beamTest ReadMe

### The big picture

The beamTest program is meant to be a reasonably accurate simulation of radiation production from alpha particle transport through the Firefly device beamline. Unlike on the real beamline, in this simulation, the particles are not accelerated, and are created at the source with their full energy. The geometry is a very abstract representation of the beamline, with details included only when they are necessary to capture certain features of radiation production on the real device, such as beam spread due to bending, and beam strike on real components. For more detail see the [Geometry](#geometry) section of this file.

The user may specify from a macro file, or the command line, three main parameters of the simulation:

- The energy of the particles. [Particle Source](#particle-source)
- The magnetic field strengths which steer the beam through the accelerator. [Geometry](#geometry)
- The output file name [Outputs](#outputs)

For more information on each, see the linked section. The default output is a *.csv file, which contains the neutron flux through each cell in a mesh overlaid on the accelerator enclosure. This output may be processed using the "BeamTestAnalysis.ipynb" file to produce some lovely contour plots of radiation dose in the enclosure.

Currently, I believe the source code is written for Geant4 v.11.3.1, however, if it does not properly compile with that version, it is likely written for Geant4 v.11.1.2. To resolve that see [Multithreading](#multithreading). 



### How To Run
1. Download this git repo and unpack to a source directory
2. Make a build directory "mkdir build"
3. From the beamTest directory, run the command "cmake -B /build" (I didn't write a GNUMakeFile, but if you are opposed to using CMake, let me know and I can probably write one)
4. Enter the build directory and execute the "make" command
5. Return to the main directory "cd ../"  
At this point you must decide if you want to run in interactive or batch mode.  
    * To run in interactive mode, if you want to for example, tune the magnetic field strengths for a new particle energy, execute the command "/build/beamTest" and a GUI window should come up. Commands can then be executed from the GUI's command line.
    * To run in batch mode (recommended for full data runs) execute the command "/build/beamTest &lt;your Macro Here&gt;"



### Geometry

A top down view of the model geometry is shown below, with components labeled according to the real life features they are modeling. The blue path is the trajectories of 1000 2.2 MeV alpha particles traveling through the geometry.

![Implanter model geometry. From left to right, the linear accelerator, the FEM, the ERS filter, the steering magnets, the corrector magnets, and the process chamber with dose cup in the back.](./Overhead_view_Labelled.png "The Geometry")

The only components not labeled are the FEM magnets - the curve between the Linac and the ERS. No beam focusing is modeled, and the only fields in the model are in the FEM and Corrector magnets, which may be specified from the command line with the commands "/beamTest/field/field1/value < your value here > kilogauss" and "/beamTest/field/field1/value < your value here > kilogauss" respectively. The linear accelerator (linac) is modeled as a block of solid graphite, with a narrowing square tunnel bored through the center. The linac is not modeled to reflect the true geometry of the RF linear accelerator, but rather the effective narrowing of the beam liners relative to the beam width, as the beam broadens as it is accelerated through the linear accelerator. Narrowing the tunnel, rather than widening the beam proved to be a much simpler way to accomplish the same physical effect. 

Several dosimeters are positioned throughout the geometry as well, and are used to calculate the flux to dose conversion ratios, which are necessary for radiation dose calculations. They have independent outputs which may be turned on and off from the runAction.

A more detailed description of the different geometry components may be found in my thesis. 

### Particle Source

The particle source is located at the far left of the blue beam path in the figure above, and is a 5cmx5cm square source lined up with the long axis of the linear accelerator tunnel. The source, as specified in the PrimaryGeneratorAction, is a general particle source (gps), to allow maximum configuration from the command line. The default configuration of the source, as specified in the current "ParticleGun.mac" file, is to fire alpha particles in the direction of the linear accelerator beam tunnel, with an energy randomly sampled from a Gaussian distribution centered at 3.5 MeV, with a 0.07 MeV standard deviation.

The location, size, energy, direction, and particle type may all be changed from the command line or a macro. It is recommended not to change the location, size, or direction as these are all physical parameters which have been tuned to give results which match experimental measurement. Energy and particle type however, must be changed to simulate different scenarios. If either of those are changed, be certain to re-align the beam path to terminate at the beam stop, by tuning the magnetic field strengths.

### Multithreading

Multithreading, the parallel processing capability provided with the Geant4 toolkit, can be used with this program. Multithreading is controlled by how the G4RunManager is instantiated. If a G4RunManager is initialized directly through   
    `new G4RunManager;`   
multithreading will be disabled. If instead   `G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);`  
is used to instantiate the run manager, multithreading will be enabled, and a run will automatically use the maximum number of allowed threads as determined by the system handler. A different number of threads may be specified with the command `/run/numberOfThreads`

Adding together outputs from different threads during a multithreaded run is handled by the G4Accumulable class. From Geant4 v.11.1 to v.11.3, this class changed how a new variable is registered. If the code is not compiling correctly due to issues with G4Accumulabes, check the change log for Geant4 v.11.3 and update how the accumulables are handled in the runAction, and that should fix any issues. If I have time, I will create two different branches of this code in the repo, one for v.11.1 and one for v.11.3 so others don't have to fix this in the future.

### Outputs

The program will output several things upon finishing. The first are the number of events processed, and the total dose recorded by the dosimeter near the dose cup. These are printed to the terminal from which the program was run. From the runAction.cc file, several other command line outputs can be turned on or off, such as the ratio of dose at several points, and total dose at the corrector magnets or the ERS.

The primary output of the program is the .csv file containing information from the mesh used to measure neutron flux throughout the enclosure. This scoring mesh is created in the ParticleGun.mac macro, and establishes a 200x100 grid of cells which span the length and breadth of the enclosure. The weighted neutron flux through each is recorded during the simulation, and output to the file specified by the `/score/dumpOutputToFile` command, as shown below:

    /score/dumpQuantityToFile bxMesh nFlux MeshDump_Raw_35.csv

This example will write the scores contained in the "bxMesh" scorer (the mesh created earlier), which were sorted using the "nFlux" filter (only neutrons), to the file "MeshDump_Raw_35.csv" which will be written to at the end of the simulation. As this output is an aggregate, not a record of every individual event, if you terminate the program before it finishes using Ctrl+C, nothing will be written to this file. 

The last output of the program is a root file, containing a histogram of neutrons passing through the detectors located throughout the enclosure. The name of the root file may be specified from the macro, as follows:
    
    /analysis/openFile Only_DoseCup_Box1.root
    ...
    /run/beamOn .....
    ...
    /analysis/write
    /analysis/closeFile
In this example, the file is named "Only_DoseCup_Box1.root" and all root histograms generated by the program will be written to this file. The names of each individual histogram are specified in the runAction, I believe.

Currently, each detector is placed next to a dosimeter, and the small green cylinders can be seen next to the large brown cylinders of the dosimeters in the geometry overview. The neutrons are binned based on energy into bins corresponding to the neutron energies listed in ICRP 26 Table A.41, neutron dose coefficients. This allows for easy conversion from flux to dose, using the "DoseCalc.cc" root macro. This system was used to convert neutron flux to dose, as a neutron energy histogram could not be generated from each square of the mesh, so a flux to dose conversion ratio was necessary.


