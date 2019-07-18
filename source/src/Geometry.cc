//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geometry.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "Geometry.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4VPhysicalVolume.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"
#include "G4SubtractionSolid.hh"
#include "SensitiveVolume.hh"
#include "G4SDManager.hh"


//------------------------------------------------------------------------------
class PixelDet{
  public:
    PixelDet(G4String name, G4double pos_X, G4double pos_Y, G4double pos_Z, G4int copyNum, G4PVPlacement*& physVol_World);
};
PixelDet::PixelDet(G4String name, G4double pos_X, G4double pos_Y, G4double pos_Z, G4int copyNum, G4PVPlacement*& physVol_World)
{
  //define parameters
  G4double leng_X_Pix = 100.0 * mm; // X-full-length of pixel: global envelop
  G4double leng_Y_Pix = 100.0 * mm; // Y-full-length of pixel: global envelop
  G4double leng_Z_Pix = 0.64 * mm;   // Z-full-length of pixel: global envelop
  G4int nDiv_Y = 100;               // Number of divisions in Y-direction
  G4int nDiv_X = 100;               // Number of divisions in Y-direction
  auto materi_Man = G4NistManager::Instance();
  auto materi_Pix = materi_Man->FindOrBuildMaterial("G4_SILICON_DIOXIDE");

  //generate global envelope
  auto solid_PixEnvG = new G4Box("Solid_PixEnvG", leng_X_Pix / 2.0, leng_Y_Pix / 2.0, leng_Z_Pix / 2.0);
  auto logVol_PixEnvG = new G4LogicalVolume(solid_PixEnvG, materi_Pix, "LogVol_PixEnvG");
  auto threeVect_LogV_PixEnvG = G4ThreeVector(pos_X, pos_Y, pos_Z);
  auto rotMtrx_LogV_PixEnvG = G4RotationMatrix();
  auto trans3D_LogV_PixEnvG = G4Transform3D(rotMtrx_LogV_PixEnvG, threeVect_LogV_PixEnvG);
  new G4PVPlacement(trans3D_LogV_PixEnvG, "PhysVol_PixEnvG", logVol_PixEnvG, physVol_World, false, copyNum);

  //generate local envelope
  auto solid_PixEnvL = new G4Box("Solid_PixEnvL", leng_X_Pix / 2.0, leng_Y_Pix / (2.0 * nDiv_Y), leng_Z_Pix / 2.0);
  auto logVol_PixEnvL = new G4LogicalVolume(solid_PixEnvL, materi_Pix, "LogVol_PixEnvL");
  logVol_PixEnvL->SetVisAttributes(G4VisAttributes::Invisible);
  new G4PVReplica("PhysVol_PixEnvL", logVol_PixEnvL, logVol_PixEnvG, kYAxis, nDiv_Y, leng_Y_Pix/nDiv_Y);

  //generate elements
  auto solid_PixElmt = new G4Box("Solid_PixElmt", leng_X_Pix / 2.0 / nDiv_X, leng_Y_Pix / 2.0 / nDiv_Y, leng_Z_Pix / 2.0);
  auto logVol_PixElmt = new G4LogicalVolume(solid_PixElmt, materi_Pix, "LogVol_PixElmt");
  logVol_PixElmt->SetVisAttributes(G4VisAttributes::Invisible);
  new G4PVReplica(name, logVol_PixElmt, logVol_PixEnvL, kXAxis, nDiv_X, leng_X_Pix/nDiv_X);

  //add sensitive detector
  auto SV = new SensitiveVolume("SensitiveVolume");
  logVol_PixElmt->SetSensitiveDetector(SV);         // Add sensitivity to the logical volume
  auto SDman = G4SDManager::GetSDMpointer();
  SDman->AddNewDetector(SV);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
  Geometry::Geometry() {}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
  Geometry::~Geometry() {}
//------------------------------------------------------------------------------

/////// 実装必須のConstruct()
// 最後に物理ワールドへのポインタを返す

//------------------------------------------------------------------------------
  G4VPhysicalVolume* Geometry::Construct()
//------------------------------------------------------------------------------
{
   auto materi_Man = G4NistManager::Instance();//Get pointer "material manager"
   auto solid_World = new G4Box("Solid_World", 2.0*m/2.0, 2.0*m/2.0, 2.0*m/2.0); //define world volume

   // Generate world
   auto materi_World = materi_Man->FindOrBuildMaterial("G4_AIR");
   auto logVol_World = new G4LogicalVolume(solid_World, materi_World, "LogVol_World");
   logVol_World->SetVisAttributes (G4VisAttributes::Invisible);
   G4int copyNum_World = 0;               // Set ID number of world
   auto physVol_World  =  new G4PVPlacement(G4Transform3D(), "PhysVol_World", logVol_World, 0, false, copyNum_World);

  /////// Generate pixel detector
  PixelDet pixelFront1("FrontPixelDetector1", 0.0 * cm, 0.0 * cm, 0.0 * cm, 1,physVol_World);
  PixelDet pixelFront2("FrontPixelDetector2", 0.0 * cm, 0.0 * cm, 3.0 * cm, 2,physVol_World);
  PixelDet pixelBack1("BackPixelDetector1", 0.0 * cm, 0.0 * cm, 40.0 * cm, 3,physVol_World);
  PixelDet pixelBack2("BackPixelDetector2", 0.0 * cm, 0.0 * cm, 43.0 * cm, 4,physVol_World);

/////// Generate Water
   auto leng_X_WaterEnvG = 100*mm;          // X-full-length of Waterel: local envelop 
   auto leng_Y_WaterEnvG = 100*mm;   // Y-full-length of Waterel: local envelop 
   auto leng_Z_WaterEnvG = 40*mm;          // Z-full-length of Waterel: local envelop 
   auto solid_WaterEnvG = new G4Box("Solid_WaterEnvG", 100*mm/2.0, 100*mm/2.0, 40*mm/2.0);

   auto materi_WaterEnvG = materi_Man->FindOrBuildMaterial("G4_WATER");
   auto logVol_WaterEnvG = new G4LogicalVolume(solid_WaterEnvG, materi_WaterEnvG, "LogVol_WaterEnvG");
   auto threeVect_LogV_WaterEnvG = G4ThreeVector(0.0*cm, 0.0*cm, 20.0*cm);
   auto rotMtrx_LogV_WaterEnvG = G4RotationMatrix();
   auto trans3D_LogV_WaterEnvG = G4Transform3D(rotMtrx_LogV_WaterEnvG, threeVect_LogV_WaterEnvG);
   G4int copyNum_LogV_WaterEnvG = 2000;                // Set ID number of LogV_WaterEnvG
   new G4PVPlacement(trans3D_LogV_WaterEnvG, "PhysVol_WaterEnvG", logVol_WaterEnvG, physVol_World, 
                     false, copyNum_LogV_WaterEnvG);
    //Generate Local envelope of Water
   G4int nDiv_Z = 200;                                 // Number of divisions in Y-direction 
   auto leng_X_WaterEnvL = leng_X_WaterEnvG;          // X-full-length of Waterel: local envelop 
   auto leng_Y_WaterEnvL = leng_Y_WaterEnvG;   // Y-full-length of Waterel: local envelop 
   auto leng_Z_WaterEnvL = leng_Z_WaterEnvG/nDiv_Z;          // Z-full-length of Waterel: local envelop 
   auto solid_WaterEnvL = new G4Box("Solid_WaterEnvL", leng_X_WaterEnvL/2.0, leng_Y_WaterEnvL/2.0, leng_Z_WaterEnvL/2.0);

   auto materi_WaterEnvL = materi_Man->FindOrBuildMaterial("G4_WATER");
   auto logVol_WaterEnvL = new G4LogicalVolume(solid_WaterEnvL, materi_WaterEnvL, "LogVol_WaterEnvL");
   logVol_WaterEnvL->SetVisAttributes (G4VisAttributes::Invisible);
   new G4PVReplica("PhysVol_WaterEnvL", logVol_WaterEnvL, logVol_WaterEnvG, kZAxis, nDiv_Z, leng_Z_WaterEnvL); 
  /////// Sensitive Detector
    auto WaterSV = new SensitiveVolume("SensitiveVolume");
    logVol_WaterEnvL->SetSensitiveDetector(WaterSV);         // Add sensitivity to the logical volume
    auto SDmanWater = G4SDManager::GetSDMpointer();
    SDmanWater->AddNewDetector(WaterSV);


/////// Return the physical world
   return physVol_World;
}
