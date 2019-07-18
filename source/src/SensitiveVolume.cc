//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SensitiveVolume.cc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "SensitiveVolume.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4HCofThisEvent.hh"
#include "G4RunManager.hh"
#include "Analysis.hh"
#include "json.hpp"
#include <iostream>
#include <fstream>
using json = nlohmann::json;
//------------------------------------------------------------------------------
  SensitiveVolume::SensitiveVolume(G4String name)
  : G4VSensitiveDetector(name)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
  SensitiveVolume::~SensitiveVolume()
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
  void SensitiveVolume::Initialize(G4HCofThisEvent*)
//------------------------------------------------------------------------------
{
  //  sum_eDep = 0.;
  // no_Step = 0;
  //  pre_copyNo =0;
  //  run_number = 0;
}

//------------------------------------------------------------------------------
  void SensitiveVolume::EndOfEvent(G4HCofThisEvent*)
//------------------------------------------------------------------------------
{
//  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
//  analysisManager->FillH1(2, sum_eDep);
run_number++;
//G4cout << run_number << "\n";
//G4cout << tracking;
  std::ofstream o("tracking.json");
  o << std::setw(4)  << tracking << std::endl;

}

//------------------------------------------------------------------------------
  G4bool SensitiveVolume::ProcessHits(G4Step* aStep, G4TouchableHistory*)
//------------------------------------------------------------------------------
{

//   G4cout << "=== SenstiveVolume-ProcessHits: Output of Information ===" << G4endl;

   G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    
// energy deposit in this step and its accumulation over steps
    G4double edep = aStep->GetTotalEnergyDeposit();
    G4double stepLength = aStep->GetStepLength();

  
    
  
// Retrieve information from the track object
   G4int nTrack = aStep->GetTrack()->GetParentID();
   G4int nStep  = aStep->GetTrack()->GetCurrentStepNumber();
   G4String particleName = aStep->GetTrack()->GetDefinition()->GetParticleName();

if(aStep->GetTrack()->GetTrackStatus()!=fAlive) return false;
// Primary track is picked up
 //   if (nTrack != 1) return false;
      no_Step = no_Step + 1;    
    /*
   G4cout << "  Track Number: " << nTrack << G4endl;
   G4cout << "  Step  Number: " << nStep << G4endl;
*/
 //  G4cout << " multiplicity = " << no_Step << "  Particle: " << particleName << G4endl;


// Get PreStepPoint and TouchableHandle objects
   G4StepPoint* preStepPoint=aStep->GetPreStepPoint();
   G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();

  // Touchable infomation: Start position of the current step
  G4ThreeVector position_World = preStepPoint->GetPosition();
  G4ThreeVector position_Local = theTouchable->GetHistory()
                                  -> GetTopTransform().TransformPoint(position_World);



// Touchable information: Volume name and copy# at the current step
  G4String volumeName = theTouchable->GetVolume()->GetName();
  G4int copyNo = theTouchable->GetCopyNumber();

  G4String motherVolumeName;
  G4int motherCopyNo;
  if (volumeName != "LogVol_PixElmt" ) {
     motherVolumeName = theTouchable->GetVolume(1)->GetName();
     motherCopyNo = theTouchable->GetCopyNumber(1);
  } 
  else {
     motherVolumeName = "None";
     motherCopyNo = 0;
  }
  if(water_flag){
    if (volumeName == "PhysVol_WaterEnvL" && copyNo == pre_copyNo)
    {
      sum_eDep = sum_eDep + edep;
    }
    else{
    pre_copyNo = copyNo;
    sum_eDep = 0;
        water_flag = 0;
    }
  }
  else if (volumeName == "PhysVol_WaterEnvL" ) {
    water_flag = 1;
          sum_eDep = sum_eDep + edep;
  } 
  


  //  tracking[run_number][volumeName][copyNo]["name"] = volumeName;
  //  tracking[run_number][volumeName][copyNo]["copyNo"] = copyNo;
  //  tracking[run_number][volumeName][copyNo]["x"] = position_World.x();
  //  tracking[run_number][volumeName][copyNo]["y"] = position_World.y();
  //  tracking[run_number][volumeName][copyNo]["z"] = position_World.z();
  //  tracking[run_number][volumeName][copyNo]["sum_eDep"] = sum_eDep;
  //  tracking[run_number][volumeName][copyNo]["nStep"] = no_Step;
  //  tracking[run_number][volumeName][copyNo]["nTrack"] = nTrack;
  //  tracking[run_number][volumeName][copyNo]["run"] = run_number;
 
    //G4cout <<volumeName  <<"\t" <<copyNo  <<"\t" <<sum_eDep <<"\t"  << no_Step  <<"\t" << nTrack <<"\t" << run_number << "\n";

        std::ofstream o2("tracking.dat",std::ios::app);
  o2 << std::setw(4) << run_number << "\t" << volumeName <<"\t" << motherCopyNo <<"\t"  << copyNo <<"\t" << volumeName <<"\t" << position_World.x() <<"\t" << position_World.y() <<"\t" << position_World.z() <<"\t" << sum_eDep <<"\t"<< no_Step <<"\t"<< nTrack <<"\t" << std::endl;
  

  // G4cout << " ******  Pixel ID and energy loss accumulated ******************"<< G4endl;
  //G4cout << "  Volume = " << volumeName
  //       << " , CopyNo = "   << copyNo
  //       << " :: Mother Volume = " << motherVolumeName
  //       << " ,  CopyNo= " << motherCopyNo << " :: sum_eDep =  " << sum_eDep <<G4endl;
  //G4cout << " ******  Entry point  coordinates**************"<< G4endl;
  //G4cout << "  World(x,y,z): "    //      << "::  Local(x,y,z): "
  //     << position_Local.x() << ", " << position_Local.y() << ", "
  //     << position_Local.z() << G4endl;
  // G4cout << " ******  end of this pixel *******************************************"<< G4endl;

  // a primary track with non-zero energy deposition is registerd
  //   if ( edep==0. ) return false;

  // accumulation of energy loss pixel by pixel
  //   G4cout  << "  in pixel ID = " <<motherCopyNo << " , "<< copyNo<< "  sum_eDep  " << sum_eDep <<G4endl;

  return true;
}






