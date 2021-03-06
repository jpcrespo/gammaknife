//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************  
//

#include "G4SDManager.hh"
#include "G4RunManager.hh" 
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4Colour.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4NistManager.hh"
#include "G4VSolid.hh"
#include "GammaKnifeDetectorMessenger.hh"
#include "GammaKnifeDetectorConstruction.hh"

#include "G4SystemOfUnits.hh"
#include "G4Ellipsoid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Box.hh"   
#include "G4EllipticalTube.hh"

#include <stdlib.h>
using namespace std;  


GammaKnifeDetectorConstruction::GammaKnifeDetectorConstruction()
  : physicalTreatmentRoom(0),
    patientPhysicalVolume(0),
    patientLogicalVolume(0),
    solidColl_helmet(0),
    helmetSize(4)
{
  // Messenger to change parameters of the geometry
  detectorMessenger = new GammaKnifeDetectorMessenger(this);
}

GammaKnifeDetectorConstruction::~GammaKnifeDetectorConstruction()
{
  delete detectorMessenger;
}

G4VPhysicalVolume* GammaKnifeDetectorConstruction::Construct()
{
  // Define the geometry components
  ConstructBeamLine();

  return physicalTreatmentRoom;
}


void GammaKnifeDetectorConstruction::ConstructBeamLine()
{
    
   // NIST Materials
    G4Material* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
     G4Material* patient_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_BRAIN_ICRP"); //Se agrego material cerebro 
    G4Material* skull_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_B-100_BONE"); // Se agrego material para craneo 
    G4Material* soft_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_TISSUE_SOFT_ICRP"); // Se agrego material para cuello   
    // G4Material* water = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
    G4Material* cobalt = G4NistManager::Instance()->FindOrBuildMaterial("G4_Co");
    G4Material* Pb = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb");
    G4Material* tungsten = G4NistManager::Instance()->FindOrBuildMaterial("G4_W");
    G4Material* Al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
    G4Material* Fe = G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe");

    // Steel as non-NIST material
    G4Element* elFe = G4NistManager::Instance()->FindOrBuildElement("Fe");
    G4Element* elNi = G4NistManager::Instance()->FindOrBuildElement("Ni");
    G4Element* elCr = G4NistManager::Instance()->FindOrBuildElement("Cr");
    G4Material* steel = new G4Material("StainlessSteel", 7.80 * g/cm3, 3 /* components */);  
    steel -> AddElement(elFe, 70 * perCent);
    steel -> AddElement(elCr, 18 * perCent);
    steel -> AddElement(elNi, 12 * perCent); 

 
  // -----------------------------
  // Treatment room - World volume
  // -----------------------------

  // Treatment room sizes
  const G4double worldX = 400.0 *cm;
  const G4double worldY = 400.0 *cm;
  const G4double worldZ = 400.0 *cm;

  G4Box* treatmentRoom = new G4Box("TreatmentRoom",worldX,worldY,worldZ);

  G4LogicalVolume* logicTreatmentRoom = new G4LogicalVolume(treatmentRoom,
                                                            air,
                                                            "logicTreatmentRoom",
							    0,0,0);

  physicalTreatmentRoom = new G4PVPlacement(0,
					    G4ThreeVector(),
					    "physicalTreatmentRoom",
					    logicTreatmentRoom,
					    0,false,0);


  // The treatment room is invisible in the Visualisation
  logicTreatmentRoom -> SetVisAttributes (G4VisAttributes::GetInvisible()); //Hace que se invisible sala de tratamiento


  // Visualisation attributes of all elements colours
   G4VisAttributes * grayFe = new G4VisAttributes(G4Colour(0.5 ,0.5 ,0.5)); // Cubierto de hierro
  grayFe -> SetVisibility(true);
  grayFe -> SetForceSolid(true);

  
  G4VisAttributes * SubVol = new G4VisAttributes(G4Colour(0.0 ,0.0 ,1.0,0.3)); //Atributos del volumen madre de cada colimador   
  SubVol -> SetVisibility(true);
  SubVol -> SetForceSolid(true);

   G4VisAttributes * blueCobalt = new G4VisAttributes(G4Colour(0. ,0. ,0.7)); // Fuente
  blueCobalt -> SetVisibility(true);
  blueCobalt -> SetForceSolid(true);

  G4VisAttributes * graySS = new G4VisAttributes(G4Colour(0.9 ,0.9 ,0.9));  // Acero inoxidable
  graySS -> SetVisibility(true);
  graySS -> SetForceSolid(true);

  G4VisAttributes * grayAl = new G4VisAttributes(G4Colour(0.7 ,0.7 ,0.7));  // Casquetes cubren fuente de cobalto
  grayAl -> SetVisibility(true);
  grayAl -> SetForceSolid(true);

  G4VisAttributes * blackLead = new G4VisAttributes(G4Colour(0.2 ,0.2 ,0.2)); //  Blindaje de la fuente
  blackLead -> SetVisibility(true);
  blackLead -> SetForceSolid(true);


   G4VisAttributes * colorTungsten = new G4VisAttributes(G4Colour(0.3 ,0.3 ,0.3));
  colorTungsten -> SetVisibility(true);
  colorTungsten -> SetForceSolid(true);    // El tungsteno es util para colimar tiene unas aberturas que simulan el casco
                                        // Tambien el colimador estacionario
  // El colimador estacionario esta hecho de Tungsteno y plomo y colimador final que simula el casco que esta hecho de tungsteno
  
   G4VisAttributes * yellowbrain = new G4VisAttributes(G4Colour(0.9 ,0.9 ,0)); //  Atributos y color cerebro 
  yellowbrain -> SetVisibility(true);
  yellowbrain -> SetForceSolid(true);

  
  //-------------------------------------
  // Mother volume of each collimator
  //-------------------------------------

  G4double CorrVM = 16.5*cm+13.35*cm; 
  G4double HeightMC = 13.35*cm;
  G4double InnerRadiusMC = 0.;
  G4double OuterRadiusMC = 50.0*mm;
  G4double StartAngleMC = 0.*deg;
  G4double EndAngleMC = 360.*deg;
  G4double DistCentro = -29.85*cm;
  
  // esta posicion significa que la punta del colimador esta a 16.5 cm del centroorigen de coordenadas
  G4ThreeVector PositionMC = G4ThreeVector(0,0,DistCentro);   
  
  
  G4Tubs* SolidMotherColl = new G4Tubs("SolidMotherColl",    
			       InnerRadiusMC,    
			       OuterRadiusMC,   
			       HeightMC,          
			       StartAngleMC,     
			       EndAngleMC);
  
  G4LogicalVolume* logicMotherColl = new G4LogicalVolume(SolidMotherColl,air,"logicMotherColl",0,0,0);
  
    G4VPhysicalVolume* PV_MC = new G4PVPlacement(0,           
			    PositionMC,
			    logicMotherColl,
			    "PV_MC",
			    logicTreatmentRoom,
			    false,
			    0);
			    //logicMotherColl -> SetVisAttributes(SubVol);             

  //--------------------------------------------
  // Cylinder source "Tube_source"               // COBALTO 60
  //--------------------------------------------
  G4double innerRadiusOfTheTube_source = 0.;    //Dimensiones
  G4double outerRadiusOfTheTube_source = 0.5*mm;
  G4double hightOfTheTube_source = 1*cm;
  G4double startAngleOfTheTube = 0.*deg;
  G4double spanningAngleOfTheTube = 360.*deg; 

  G4ThreeVector positionTube_source = G4ThreeVector(0,0,-40.1*cm+CorrVM); // Posicionado

  solidTube_source = new G4Tubs("solidTube_source",
				innerRadiusOfTheTube_source,  //Radio interno
				outerRadiusOfTheTube_source, // Radio externo
				hightOfTheTube_source,      //Altura
				startAngleOfTheTube,       // Angulo inicio
				spanningAngleOfTheTube);   // Angulo final
  logicTube_source = new G4LogicalVolume(solidTube_source,cobalt,"logicTube_source",0,0,0);
   physiTube_source = new G4PVPlacement(0,
				positionTube_source,
				logicTube_source,
				"Tube_source",
				logicMotherColl,
				false,
				0);

				logicTube_source -> SetVisAttributes(blueCobalt);   // Toma lo atributos del cobalto 60


  //-------------------------------------
  // Cylinder covering source "Tube"           //Blindaje de acero inoxidable que cubre la fuente
  //-------------------------------------
  G4double innerRadiusOfTheTube = 0.5*mm;
  G4double outerRadiusOfTheTube = 4.*mm;
  G4double hightOfTheTube = 1*cm;


  G4ThreeVector positionTube = G4ThreeVector(0,0,-40.1*cm+CorrVM);

  solidTube = new G4Tubs("solidTube",
				 innerRadiusOfTheTube,
				 outerRadiusOfTheTube,
				 hightOfTheTube,
				 startAngleOfTheTube,
				 spanningAngleOfTheTube);
  logicTube = new G4LogicalVolume(solidTube,steel,"logicTube",0,0,0);
   physiTube = new G4PVPlacement(0,
				positionTube,
				logicTube,
				"Tube",
				logicMotherColl,
				false,
				0);

				logicTube -> SetVisAttributes(graySS);    //  Tome los atributos del acero inoxidable

  //---------------------------------------
  // Cylinder covering source "Tube_Al"
  //---------------------------------------
  G4double innerRadiusOfTheTube_Al = 4.*mm;
  G4double outerRadiusOfTheTube_Al = 15.*mm;
  G4double hightOfTheTube_Al = 1*cm;

  G4ThreeVector positionTube_Al = G4ThreeVector(0,0,-40.1*cm+CorrVM);

  solidTube_Al = new G4Tubs("solidTube_Al",
				 innerRadiusOfTheTube_Al,
				 outerRadiusOfTheTube_Al,
				 hightOfTheTube_Al,
				 startAngleOfTheTube,
				 spanningAngleOfTheTube);
  logicTube_Al = new G4LogicalVolume(solidTube_Al,Al,"logicTube_Al",0,0,0);
   physiTube_Al = new G4PVPlacement(0,
				positionTube_Al,
				logicTube_Al,
				"Tube_Al",
				logicMotherColl,
				false,
				0);

				logicTube_Al -> SetVisAttributes(grayAl);    //Tomo los atributos del aluminio

  //----------------------------------------------
  // Cylinder covering external part of the source "Tube_Fe"
  //----------------------------------------------
  G4double innerRadiusOfTheTube_Fe = 15.*mm;
  G4double outerRadiusOfTheTube_Fe = 50.*mm;
  G4double hightOfTheTube_Fe = 1*cm;


  G4ThreeVector positionTube_Fe = G4ThreeVector(0,0,-40.1*cm+CorrVM);

  solidTube_Fe = new G4Tubs("solidTube_Fe",
				 innerRadiusOfTheTube_Fe,
				 outerRadiusOfTheTube_Fe,
				 hightOfTheTube_Fe,
				 startAngleOfTheTube,
				 spanningAngleOfTheTube);
  logicTube_Fe = new G4LogicalVolume(solidTube_Fe,Fe,"logicTube_Fe",0,0,0);
  /* physiTube_Fe = new G4PVPlacement(0,
				positionTube_Fe,
				logicTube_Fe,
				"Tube_Fe",
			        logicMotherColl,
				false,
				0);
				logicTube_Fe -> SetVisAttributes(grayFe); */     // Tome los atributos del hierro




 //------------------------------------------------
 // Cylinder covering posterior part "Tube_post"
 //------------------------------------------------

  G4double innerRadiusOfTheTube_post = 0;
  G4double outerRadiusOfTheTube_post = 50*mm;
  G4double hightOfTheTube_post = 1*cm;

  G4ThreeVector positionTube_post = G4ThreeVector(0,0,-42.2*cm+CorrVM); // Esta alejado a 1cm de la fuente

  solidTube_post = new G4Tubs("solidTube_post",
				 innerRadiusOfTheTube_post,
				 outerRadiusOfTheTube_post,
				 hightOfTheTube_post,
				 startAngleOfTheTube,
				 spanningAngleOfTheTube);
 logicTube_post = new G4LogicalVolume(solidTube_post,Fe,"logicTube_post",0,0,0);
 /* physiTube_post = new G4PVPlacement(0,
				positionTube_post,
				logicTube_post,
				"Tube_post",
			        logicMotherColl,
				false,
				0);

				logicTube_post -> SetVisAttributes(grayFe); */     // Tomo los atributos del hierro


 //------------------------------------------------
 // Fixed cylinder collimator "Tube_coll"             //Colimador estacionario
 //------------------------------------------------

  G4double innerRadiusOfTheTube_coll = 2.5*mm;
  G4double outerRadiusOfTheTube_coll = 15.*mm;
  G4double hightOfTheTube_coll = 3.25*cm;

  G4ThreeVector positionTube_coll = G4ThreeVector(0,0,-35.2*cm+CorrVM); // Aproximadamante en la mitad del tubo

  solidTube_coll = new G4Tubs("solidTube_coll",
				 innerRadiusOfTheTube_coll,
				 outerRadiusOfTheTube_coll,
				 hightOfTheTube_coll,
				 startAngleOfTheTube,
				 spanningAngleOfTheTube);
 logicTube_coll = new G4LogicalVolume(solidTube_coll,tungsten,"logicTube_coll",0,0,0);
 physiTube_coll = new G4PVPlacement(0,
				positionTube_coll,
				logicTube_coll,
				"Tube_coll",
				logicMotherColl,
				false,
				0);

				logicTube_coll -> SetVisAttributes(colorTungsten);


 //------------------------------------------------
 // Cylinder covering fixed collimator "Tube_coll_Fe" // Cobertura de hierro del colimador estacionario
 //------------------------------------------------

  G4double innerRadiusOfTheTube_coll_Fe = 15.*mm;
  G4double outerRadiusOfTheTube_coll_Fe = 50.*mm;
  G4double hightOfTheTube_coll_Fe = 3.25*cm;

  G4ThreeVector positionTube_coll_Fe = G4ThreeVector(0,0,-35.2*cm+CorrVM);

  solidTube_coll_Fe = new G4Tubs("solidTube_coll_Fe",
				 innerRadiusOfTheTube_coll_Fe,
				 outerRadiusOfTheTube_coll_Fe,
				 hightOfTheTube_coll_Fe,
				 startAngleOfTheTube,
				 spanningAngleOfTheTube);
 logicTube_coll_Fe = new G4LogicalVolume(solidTube_coll_Fe,Fe,"logicTube_coll_Fe",0,0,0);
 /* physiTube_coll_Fe = new G4PVPlacement(0,
				positionTube_coll_Fe,
				logicTube_coll_Fe,
				"Tube_coll_Fe",
				logicMotherColl,
				false,
				0);

				logicTube_coll_Fe -> SetVisAttributes(grayFe); */     //Atibutos hierro


 //------------------------------------------------
 // Fixed truncated cone collimator "Coll_fixed"      //Colimador final
 //------------------------------------------------

  G4double Rmin1Coll_fixed = 2.5*mm;
  G4double Rmax1Coll_fixed = 15.*mm;
  G4double Rmin2Coll_fixed = 4.25*mm;
  G4double Rmax2Coll_fixed = 15.*mm;
  G4double hightColl_fixed = 4.625*cm;


  G4ThreeVector positionColl_fixed = G4ThreeVector(0,0,-27.325*cm+CorrVM);

  solidColl_fixed = new G4Cons("solidColl_fixed",
			       Rmin1Coll_fixed,
			       Rmax1Coll_fixed,
			       Rmin2Coll_fixed,
			       Rmax2Coll_fixed,
			       hightColl_fixed,
			       startAngleOfTheTube,
			       spanningAngleOfTheTube);
  logicColl_fixed = new G4LogicalVolume(solidColl_fixed,Pb,"logicColl_fixed",0,0,0);
  physiColl_fixed = new G4PVPlacement(0,
				      positionColl_fixed,
				      logicColl_fixed,
				      "Coll_fixed",
				      logicMotherColl,
				      false,
				      0);

				      logicColl_fixed -> SetVisAttributes(blackLead);      //Atributos del plomo


 //-----------------------------------------------------------
 // Cilinder covering fixed collimator "Coll_fixed_Fe"             // Colimador final
 //-----------------------------------------------------------

  G4double Rmin1Coll_fixed_Fe = 15.*mm;
  G4double Rmax1Coll_fixed_Fe = 50.*mm;
  G4double Rmin2Coll_fixed_Fe = 15.*mm;
  G4double Rmax2Coll_fixed_Fe = 40.*mm;
  G4double hightColl_fixed_Fe = 4.625*cm;


  G4ThreeVector positionColl_fixed_Fe = G4ThreeVector(0,0,-27.325*cm+CorrVM); // Posicion del colimador final

  solidColl_fixed_Fe = new G4Cons("solidColl_fixed_Fe",
			       Rmin1Coll_fixed_Fe,
			       Rmax1Coll_fixed_Fe,
			       Rmin2Coll_fixed_Fe,
			       Rmax2Coll_fixed_Fe,
			       hightColl_fixed_Fe,
			       startAngleOfTheTube,    //
			       spanningAngleOfTheTube);
  logicColl_fixed_Fe = new G4LogicalVolume(solidColl_fixed_Fe,Fe,"logicColl_fixed_Fe",0,0,0);
  /* physiColl_fixed_Fe = new G4PVPlacement(0,
				      positionColl_fixed_Fe,
		 		      logicColl_fixed_Fe,
				      "Coll_fixed_Fe",
				      logicMotherColl,
				      false,
	 			      0);

	 			      logicColl_fixed_Fe -> SetVisAttributes(grayFe); */   // Atributos del hierro


 //------------------------------------------------
 // Mobile truncate cone collimator "Coll_helmet"   // Cono simula el casco
 //------------------------------------------------
  G4double Rmax1Coll_helmet = 15.*mm; //Radio mayor
  G4double Rmax2Coll_helmet = 15.*mm; // Radio menor
  G4double hightColl_helmet = 3.0*cm; // Altura  


  G4ThreeVector positionColl_helmet = G4ThreeVector(0,0,-19.5*cm+CorrVM); // Posicion del casco

  solidColl_helmet = new G4Cons("solidColl_helmet",
                               0.0,  // will be set later
                               Rmax1Coll_helmet,
                               0.0,  // will be set later
			       Rmax2Coll_helmet,
			       hightColl_helmet,
			       startAngleOfTheTube,
                               spanningAngleOfTheTube);
  UpdateHelmet(); // Set the proper inner radii

  logicColl_helmet = new G4LogicalVolume(solidColl_helmet,tungsten,"logicColl_helmet",0,0,0); // Material del casco colimador
   physiColl_helmet = new G4PVPlacement(0,
				      positionColl_helmet,
 				      logicColl_helmet,
				      "Coll_helmet",
				      	logicMotherColl,
				      false,
				      0);

				      logicColl_helmet -> SetVisAttributes(colorTungsten);    // Toma los atributos del Tungsteno

 //--------------------------------------------------------------
 // Truncated cone covering mobile collimator "Coll_helmet_Fe"
 //--------------------------------------------------------------

  G4double Rmin1Coll_helmet_Fe = 15.*mm; //Radio menor parte posterior
  G4double Rmax1Coll_helmet_Fe = 40.*mm;// Radio mayor parte posterior
  G4double Rmin2Coll_helmet_Fe = 15.*mm;//Radio menor anterior
  G4double Rmax2Coll_helmet_Fe = 30.*mm;// Radio mayor parte anterior
  G4double hightColl_helmet_Fe = 3.0*cm;// Altura

  G4ThreeVector positionColl_helmet_Fe = G4ThreeVector(0,0,-19.5*cm+CorrVM);

  solidColl_helmet_Fe = new G4Cons("solidColl_helmet_Fe",
			       Rmin1Coll_helmet_Fe,
			       Rmax1Coll_helmet_Fe,
			       Rmin2Coll_helmet_Fe,
			       Rmax2Coll_helmet_Fe,
			       hightColl_helmet_Fe,
			       startAngleOfTheTube,
			       spanningAngleOfTheTube);
  logicColl_helmet_Fe = new G4LogicalVolume(solidColl_helmet_Fe,Fe,"logicColl_helmet_Fe",0,0,0);
  /* physiColl_helmet_Fe = new G4PVPlacement(0,
				      positionColl_helmet_Fe,
				      logicColl_helmet_Fe,
				      "Coll_helmet_Fe",
				      logicMotherColl,
				      false,
				      0);

 				      logicColl_helmet_Fe -> SetVisAttributes(grayFe); */

				      
  //-------------------------------------------------------------------------------------------------------//
 //-------------------Rotacion de fuentes incidentes en el paciente 201 colimadores------------------------//
 //-------------------------------------------------------------------------------------------------------//
 
  G4double phi = 183.8*deg; 
 G4double Theta =86*deg;  

 /* G4RotationMatrix rotm = G4RotationMatrix();     
   rotm.rotateY(Theta); 
   rotm.rotateZ(phi);
   G4ThreeVector uz = G4ThreeVector(std::cos(phi)*std::sin(Theta),  std::sin(phi)*std::sin(Theta),std::cos(Theta));     
   G4ThreeVector position = (DistCentro)*uz;
   G4Transform3D transform = G4Transform3D(rotm,position);
  
     new G4PVPlacement(transform,
		     logicMotherColl, 
		     "PV_MC",
		     logicTreatmentRoom,
		     false,
		     2,
		     0); */         
 
  for (phi=3.8*deg;phi<356.2*deg;phi+=7.6*deg){ 

   G4RotationMatrix rotm = G4RotationMatrix();
   rotm.rotateY(Theta); 
   rotm.rotateZ(phi);
   G4ThreeVector uz = G4ThreeVector(std::cos(phi)*std::sin(Theta),  std::sin(phi)*std::sin(Theta),std::cos(Theta));     
   G4ThreeVector position = (DistCentro)*uz;
   G4Transform3D transform = G4Transform3D(rotm,position);
  
   new G4PVPlacement(transform,
		     logicMotherColl, 
		     "PV_MC",
		     logicTreatmentRoom,
		     false,
		     2,
		     0);  
 }

   
 Theta =77.5*deg; 
 for(phi=4*deg;phi<356*deg;phi+=8*deg){

   cout<<"valor de phi= "<<phi<<"valor de theta"<< Theta<<endl;
	
   G4RotationMatrix rotm = G4RotationMatrix();
   rotm.rotateY(Theta); 
   rotm.rotateZ(phi);
   G4ThreeVector uz = G4ThreeVector(std::cos(phi)*std::sin(Theta),  std::sin(phi)*std::sin(Theta),std::cos(Theta));     
   G4ThreeVector position = (DistCentro)*uz;
   G4Transform3D transform = G4Transform3D(rotm,position);

   new G4PVPlacement(transform,
		     logicMotherColl, 
		     "PV_MC",
		     logicTreatmentRoom,
		     false,
		     2,
		     0);
 }
   
 Theta =68.5*deg; 
 for(phi=0*deg;phi<351*deg;phi+=9*deg){ 

   cout<<"valor de phi= "<<phi<<"valor de theta"<< Theta<<endl;
	
   G4RotationMatrix rotm = G4RotationMatrix();
   rotm.rotateY(Theta); 
   rotm.rotateZ(phi);
   G4ThreeVector uz = G4ThreeVector(std::cos(phi)*std::sin(Theta),  std::sin(phi)*std::sin(Theta),std::cos(Theta));     
   G4ThreeVector position = (DistCentro)*uz;
   G4Transform3D transform = G4Transform3D(rotm,position);

   new G4PVPlacement(transform,
		     logicMotherColl, 
		     "PV_MC",
		     logicTreatmentRoom,
		     false,
		     2,
		     0);
 }
 
 
   
 Theta =60*deg; 
 for(phi=4.5*deg;phi<355.5*deg;phi+=9*deg){ 

   cout<<"valor de phi= "<<phi<<"valor de theta"<< Theta<<endl;
	
   G4RotationMatrix rotm = G4RotationMatrix();
   rotm.rotateY(Theta); 
   rotm.rotateZ(phi);
   G4ThreeVector uz = G4ThreeVector(std::cos(phi)*std::sin(Theta),  std::sin(phi)*std::sin(Theta),std::cos(Theta));     
   G4ThreeVector position = (DistCentro)*uz;
   G4Transform3D transform = G4Transform3D(rotm,position);

   new G4PVPlacement(transform,
		     logicMotherColl, 
		     "PV_MC",
		     logicTreatmentRoom,
		     false,
		     2,
		     0);
 }

    
 Theta =51*deg; 
 for(phi=0*deg;phi<350*deg;phi+=10*deg){ 

   cout<<"valor de phi= "<<phi<<"valor de theta"<< Theta<<endl;
	
   G4RotationMatrix rotm = G4RotationMatrix();
   rotm.rotateY(Theta); 
   rotm.rotateZ(phi);
   G4ThreeVector uz = G4ThreeVector(std::cos(phi)*std::sin(Theta),  std::sin(phi)*std::sin(Theta),std::cos(Theta));     
   G4ThreeVector position = (DistCentro)*uz;
   G4Transform3D transform = G4Transform3D(rotm,position);

   new G4PVPlacement(transform,
		     logicMotherColl, 
		     "PV_MC",
		     logicTreatmentRoom,
		     false,
		     2,
		     0);
		     }  
  
 
 			 	        
 //-----------------------------------------
  // Patient --> water spherical phantom      //Se cambio por material cerebro BRAIN_ICRP
  //-----------------------------------------
  // Geometria de una elipsoide
 
 
  G4double ax = 6. * cm;
  G4double by= 9. * cm;
  G4double cz = 6.5 * cm; 
 

  G4Ellipsoid* patient = new G4Ellipsoid("patient", ax, by, cz); 

  //G4Orb* patient = new G4Orb("patient",8.*cm);
    patientLogicalVolume = new G4LogicalVolume(patient,
							     patient_mat,
							      "patientLog", 0, 0, 0);
  
   //Rotation patient
    patientPhysicalVolume = new G4PVPlacement( new G4RotationMatrix(),
                                            G4ThreeVector(0., 0., 0.),
					    "patientPhys",
					    patientLogicalVolume,
					    physicalTreatmentRoom,
					    false,0); 

  // Visualisation attributes of the patient
 patientLogicalVolume -> SetVisAttributes(yellowbrain);
    
 /* G4VisAttributes * redWire = new G4VisAttributes(G4Colour(0.8 ,0. ,0.));   //Red de color rojo 
  redWire -> SetVisibility(true);
  redWire -> SetForceWireframe(true);
  redWire -> SetForceAuxEdgeVisible(true);
  patientLogicalVolume -> SetVisAttributes(redWire); */      
 

  //-------------SKULL--------------------------------// Esta seccion se creo para el craneo //Material G4_B-100_BONE
    // Outer cranium
  G4double ax1 = 6.8 * cm;//a out skull
  G4double by1 = 9.8 * cm; // bout
  G4double cz1 = 8.3 * cm; //cout
 
  G4Ellipsoid* craniumOut =  new G4Ellipsoid("CraniumOut", ax1, by1, cz1);

  ax1 = 6. * cm; //a in
  by1 = 9. * cm; //b in 
  cz1= 6.5 * cm; // cin
 
  G4Ellipsoid* craniumIn =  new G4Ellipsoid("CraniumIn", ax1, by1, cz1);
 

  G4SubtractionSolid* cranium =  new G4SubtractionSolid("Cranium",
							craniumOut,
							craniumIn,0,
							G4ThreeVector(0.0, 0.0,0.0 * cm));  
  
     G4LogicalVolume* logicSkull = new G4LogicalVolume(cranium,
							      skull_mat,
							      "skullLog", 0, 0, 0);
   
   //Rotacion del craneo 
   G4VPhysicalVolume* physSkull = new G4PVPlacement( new G4RotationMatrix(),
                                            G4ThreeVector(0., 0., 0.),
					    "physicalSkull",
					    logicSkull,
					    physicalTreatmentRoom,
						     false,0,true);
    // Visualisation attributes of the skull
  G4VisAttributes * orangeWire = new G4VisAttributes(G4Colour(1 ,0.4 ,0.));  //Color naranja
  orangeWire -> SetVisibility(true);
  orangeWire -> SetForceWireframe(true);
  // orangeWire -> SetForceAuxEdgeVisible(true);
  logicSkull -> SetVisAttributes(orangeWire);    
 
    
  
  //--------------------------------------------------------------------------//
  //-----------------------Neck-----------------------------------------------//   
  G4double dx= 2 *cm;   
  G4double dy= 3 * cm; 
  G4double dz= 4.5 * cm;  

  G4EllipticalTube* neck = new G4EllipticalTube("neck",dx, dy,dz);
  
  G4LogicalVolume* logicneck = new G4LogicalVolume(neck,
							      soft_mat,   
							      "logicalneck", 0, 0,0);  

  G4VPhysicalVolume* physneck= new G4PVPlacement( new G4RotationMatrix(),
                                            G4ThreeVector(0., 0.,13*cm),            
					    "Physneck",
					    logicneck,
					    physicalTreatmentRoom,
					    false,0);    
  
  

  // Visualisation attributes of the Neck
  G4VisAttributes * Neck = new G4VisAttributes(G4Colour(0.9 ,0.9 ,0)); //Color Amarillo  
  Neck -> SetVisibility(true);
  Neck -> SetForceSolid(true);
  logicneck -> SetVisAttributes(Neck);  //Atributos del cuello

 
  //--------------------------------------------------------------------------//
  //-----------------------Scoring Mesh---------------------------------------//
  // Box
  G4double Box_hx = 22.5*mm;
  G4double Box_hy = 22.5*mm;
  G4double Box_hz = 22.5*mm;

  G4Box* solidmesh = new G4Box("Box2" ,Box_hx,Box_hy,Box_hz);

   G4LogicalVolume* logicmesh = new G4LogicalVolume(solidmesh,
							      patient_mat,   
							      "ScoringMesh", 0, 0, 0);

   /*G4VPhysicalVolume *physmesh= new G4PVPlacement( new G4RotationMatrix(),
                                            G4ThreeVector(0., 0., 0.),
					    "ScoringPhys",
					    logicmesh,
					    physicalTreatmentRoom,
					    false,0); */

 // Visualisation attributes of the Scoring Mesh
  G4VisAttributes * bluemesh = new G4VisAttributes(G4Colour(0 ,0 ,0.6)); //Color Azul 
  bluemesh -> SetVisibility(true);
  bluemesh -> SetForceSolid(true);
 // blueWire -> SetForceWireframe(true);
 // blueWire -> SetForceAuxEdgeVisible(true);
  logicmesh -> SetVisAttributes(bluemesh);  //Atributos del Scoring Mesh   
  
 
}

void GammaKnifeDetectorConstruction::UpdateHelmet()  
{
    if (solidColl_helmet)

      {
      switch( helmetSize ) //los cuatro posibles diametros que saldrian a la cabeza del paciente
        {
        case 18:
	  solidColl_helmet->SetInnerRadiusMinusZ( 4.15 * mm );    //Diametro 1
            solidColl_helmet->SetInnerRadiusPlusZ( 5.3  * mm );
            break;

        case 14:
            solidColl_helmet->SetInnerRadiusMinusZ( 3.15 * mm );
            solidColl_helmet->SetInnerRadiusPlusZ( 4.25  * mm );
            break;

        case 8:
            solidColl_helmet->SetInnerRadiusMinusZ( 1.9 * mm );
            solidColl_helmet->SetInnerRadiusPlusZ( 2.5  * mm );
            break;

        case 4:
            solidColl_helmet->SetInnerRadiusMinusZ( 1. * mm );
            solidColl_helmet->SetInnerRadiusPlusZ( 1.25  * mm );
            break;
        }
        // Inform the run manager about change in the geometry
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
    }
} 

void GammaKnifeDetectorConstruction::SetHelmetSize(G4int size)
{
    if (size != helmetSize) // Only if the size changes //Mirar helmetSize en macro
    {
        // Allow only valid numbers
        switch( size )
        {
        case 18:
        case 14: 
        case 8:
        case 4:
            helmetSize = size;
            G4cout << "Helmet size set to " << helmetSize << std::endl;
            UpdateHelmet();
            break;
        default:
      G4Exception("GammaKnifeDetectorConstruction::SetHelmetSize()",
		  "GammaKnife001", FatalException,
		  "Error: Invalid helmet size.");
            return;
        }
    }
}



