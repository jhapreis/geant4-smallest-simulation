/* ========================================================================
        This is a continuation of tutorial002.
    Compile it with
    g++ tutorial003.cpp `geant4-config --libs` -I${G4INCLUDES} -o tutorial003
    Where YOU have to define ${G4INCLUDES} to point to the include files in
    your local installation.
    On linux, you can just source the compile.sh script.
    Gustavo Valdiviesso 2020
   ======================================================================== */

#include <iostream>

// The Manager
#include "G4RunManager.hh"
#include "G4RunManagerFactory.hh"

// This is the User Interface
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"

#define G4VIS_USE_OPENGLX
#define G4VIS_USE_OPENGLQT

// This is the Visualization Engine
#include "G4VisExecutive.hh"
#include "G4VisManager.hh"

// The Detector
#include "G4VUserDetectorConstruction.hh"
#include "G4Box.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

// The Physics List
#include "G4VUserPhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "QBBC.hh"

// The Action Initialization (particle source)
#include "G4VUserActionInitialization.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Geantino.hh"



/**
 * Constructor
*/
class MyDetector : public G4VUserDetectorConstruction {
public:
    virtual G4VPhysicalVolume* Construct() override;
};

G4VPhysicalVolume* MyDetector::Construct(){

    // This is just a Liquid-Argon filled box
    auto lAr = G4NistManager::Instance()->FindOrBuildMaterial("G4_lAr");
    auto worldBox = new G4Box("worldBox", 1000, 1000, 1000);
    auto logicalWorld = new G4LogicalVolume(worldBox, lAr, "Logical World");
    auto physicalWorld = new G4PVPlacement(0, {0,0,0}, logicalWorld, "Physical World", 0, false, 0);

    return physicalWorld;
}



/**
 * Generator
*/
class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction{
private:
    G4ParticleGun *m_ParticleGun;
public:

    MyPrimaryGenerator();
    ~MyPrimaryGenerator();

    virtual void GeneratePrimaries(G4Event* anEvent) override;
};

MyPrimaryGenerator::MyPrimaryGenerator() :
        G4VUserPrimaryGeneratorAction(),
        m_ParticleGun( new G4ParticleGun( G4Geantino::Definition() ) )
{

};

MyPrimaryGenerator::~MyPrimaryGenerator(){

    delete m_ParticleGun;
};

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent){

    m_ParticleGun->GeneratePrimaryVertex(anEvent);

};



/**
 * Action initialization
*/
class MyActionInitialization : public G4VUserActionInitialization{
public:
    virtual void Build() const override;
};

void MyActionInitialization::Build() const {
    SetUserAction( new MyPrimaryGenerator );
};



// This is the Main code.
int main(int argc, char** argv){

    G4UIExecutive* ui = nullptr;
    if ( argc == 1 ) { ui = new G4UIExecutive(argc, argv); }

    // auto *runManager = new G4RunManager();

    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

    // auto *factory = new G4PhysListFactory();

    // Physics list
    G4VModularPhysicsList* physicsList = new QBBC;
    physicsList->SetVerboseLevel(1);
    runManager->SetUserInitialization(physicsList);

    runManager->SetUserInitialization( new MyDetector() );
    runManager->SetUserInitialization( physicsList );
    runManager->SetUserInitialization( new MyActionInitialization() );
    runManager->Initialize();

    // Initialize visualization
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    G4UImanager* UImanager = G4UImanager::GetUIpointer();


    if (argc == 1){
        // interactive mode
        UImanager->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();
        delete ui;
    } else {
        // batch mode
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command+fileName);
    }
    
    delete visManager;
    delete runManager;
};
