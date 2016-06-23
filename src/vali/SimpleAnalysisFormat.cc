//C
#include <cassert>

//nuwro src
#include "pdg.h"
#include "event1.h"
#include "RooTrackerEvent.h"

#include "SimpleAnalysisFormat.h"

//Causes a number of assert statements to be included.
//Should be enabled for validation
//#define SAF_STRICT

namespace {

const int kStdHepIdxPx = 0;
const int kStdHepIdxPy = 1;
const int kStdHepIdxPz = 2;
const int kStdHepIdxE = 3;

}

SimpleAnalysisFormat::~SimpleAnalysisFormat(){}

SimpleAnalysisFormat::SimpleAnalysisFormat(){

  _FourMomentum_Transfer = &FourMomentum_Transfer;
  _ISLepton_4Mom = &ISLepton_4Mom;
  _StruckNucleon_4Mom = &StruckNucleon_4Mom;
  _HMFSLepton_4Mom = &HMFSLepton_4Mom;
  _HMProton_4Mom = &HMProton_4Mom;
  _HMPion_4Mom = &HMPion_4Mom;
  _HMChrgPion_4Mom = &HMChrgPion_4Mom;
  _HMProton_PreFSI_4Mom = &HMProton_PreFSI_4Mom;
  _HMPion_PreFSI_4Mom = &HMPion_PreFSI_4Mom;
  _HMChrgPion_PreFSI_4Mom = &HMChrgPion_PreFSI_4Mom;
  _HMNucleonPion_PreFSI_3Mom = &HMNucleonPion_PreFSI_3Mom;

  Reset();
}

void SimpleAnalysisFormat::HandleProton(TLorentzVector const &StdHepPTLV,
    Double_t &StdHepP3Mod, Int_t StdHepStatus){

  switch(StdHepStatus){
    case 1: { // Nuclear leaving FS
      if(!HMProton.PDG){
        HMProton.PDG = 2212;
        HMProton.Momentum = StdHepP3Mod;
        HMProton.FourMomentum = StdHepPTLV;
      }
      else if(StdHepP3Mod > HMProton.Momentum){
        HMProton.Momentum = StdHepP3Mod;
        HMProton.FourMomentum = StdHepPTLV;
      }
      break;
    }
    case 14: { // Pre FSI FS
      if(!HMProton_PreFSI.PDG){
        HMProton_PreFSI.PDG = 2212;
        HMProton_PreFSI.Momentum = StdHepP3Mod;
        HMProton_PreFSI.FourMomentum = StdHepPTLV;
      }
      else if(StdHepP3Mod > HMProton_PreFSI.Momentum){
        HMProton_PreFSI.Momentum = StdHepP3Mod;
        HMProton_PreFSI.FourMomentum = StdHepPTLV;
      }
      break;
    }
    default: {
      std::cout << "[WARN]: Unexpected StdHepStatus: " << StdHepStatus
        << std::endl;
#ifdef SAF_STRICT
      assert(false);
#endif
    }
  }
}

void SimpleAnalysisFormat::HandlePion(TLorentzVector const &StdHepPTLV,
    Double_t &StdHepP3Mod, Int_t StdHepPdg, Int_t StdHepStatus){
  bool Chrgd = PDG::charge(StdHepPdg);
  switch(StdHepStatus){
    case 1: {// Nuclear leaving FS
      if(!HMPion.PDG){
        HMPion.PDG = StdHepPdg;
        HMPion.Momentum = StdHepP3Mod;
        HMPion.FourMomentum = StdHepPTLV;
      }
      else if(StdHepP3Mod > HMPion.Momentum){
        HMPion.PDG = StdHepPdg;
        HMPion.Momentum = StdHepP3Mod;
        HMPion.FourMomentum = StdHepPTLV;
      }

      if(Chrgd){
        if(!HMChrgPion.PDG){
          HMChrgPion.PDG = StdHepPdg;
          HMChrgPion.Momentum = StdHepP3Mod;
          HMChrgPion.FourMomentum = StdHepPTLV;
        }
        else if(StdHepP3Mod > HMChrgPion.Momentum){
          HMChrgPion.PDG = StdHepPdg;
          HMChrgPion.Momentum = StdHepP3Mod;
          HMChrgPion.FourMomentum = StdHepPTLV;
        }
      }
      break;
    }
    case 14: {// Pre FSI FS
      if(!HMPion_PreFSI.PDG){
        HMPion_PreFSI.PDG = StdHepPdg;
        HMPion_PreFSI.Momentum = StdHepP3Mod;
        HMPion_PreFSI.FourMomentum = StdHepPTLV;
      }
      else if(StdHepP3Mod > HMPion_PreFSI.Momentum){
        HMPion_PreFSI.Momentum = StdHepP3Mod;
        HMPion_PreFSI.FourMomentum = StdHepPTLV;
      }

      if(Chrgd){
        if(!HMChrgPion_PreFSI.PDG){
          HMChrgPion_PreFSI.PDG = StdHepPdg;
          HMChrgPion_PreFSI.Momentum = StdHepP3Mod;
          HMChrgPion_PreFSI.FourMomentum = StdHepPTLV;
        }
        else if(StdHepP3Mod > HMChrgPion_PreFSI.Momentum){
          HMChrgPion_PreFSI.Momentum = StdHepP3Mod;
          HMChrgPion_PreFSI.FourMomentum = StdHepPTLV;
        }
      }
      break;
    }
    default: {
      std::cout << "[WARN]: Unexpected StdHepStatus: " << StdHepStatus
        << std::endl;
#ifdef SAF_STRICT
      assert(false);
#endif
    }
  }
}

void SimpleAnalysisFormat::HandleStruckNucleon(TLorentzVector const &StdHepPTLV,
  Int_t StdHepPdg){

  StruckNucleon.Momentum = StdHepPTLV.Vect().Mag();
  StruckNucleon.FourMomentum = StdHepPTLV;
  StruckNucleon.PDG = StdHepPdg;
}

bool SimpleAnalysisFormat::HandleStdHepParticle(
  Int_t &StdHepPdg,
  Int_t &StdHepStatus,
  Double_t const (&StdHepP4)[4]){

  TLorentzVector StdHepPTLV = TLorentzVector(
    StdHepP4[kStdHepIdxPx],
    StdHepP4[kStdHepIdxPy],
    StdHepP4[kStdHepIdxPz],
    StdHepP4[kStdHepIdxE]);
  Double_t StdHepP3Mod = StdHepPTLV.Vect().Mag();

  if((abs(StdHepPdg) > 10) && (abs(StdHepPdg) < 19)){ // Leptons
    switch(StdHepStatus){
      case 0: { // Incoming
        if(ISLepton.PDG){
          std::cout << "[WARN]: Found second incoming lepton, new: "
            << PartStruct(StdHepPdg, StdHepPTLV) << ", old: " << ISLepton
            << std::endl;
#ifdef SAF_STRICT
          assert(false);
#endif
        }
        ISLepton.PDG = StdHepPdg;
        ISLepton.FourMomentum = StdHepPTLV;
        ISLepton.Momentum = StdHepP3Mod;
        break;
      }
      case 1: { // Outgoing

        if(!HMFSLepton.PDG){
          HMFSLepton.PDG = StdHepPdg;
          HMFSLepton.Momentum = StdHepP3Mod;
          HMFSLepton.FourMomentum = StdHepPTLV;
        }
        else if(StdHepP3Mod > HMFSLepton.Momentum){
          HMFSLepton.PDG = StdHepPdg;
          HMFSLepton.Momentum = StdHepP3Mod;
          HMFSLepton.FourMomentum = StdHepPTLV;
        }

        NFinalStateLeptons++;
        NFinalStateParticles++;
        break;
      }
      case 14:{ //Ignore PreFSI lepton for the moment.
        return false;
      }
      default:{
        std::cout << "[WARN]: Found unexpected StdHepStatus (" << StdHepStatus
          << ") Lepton: " << PartStruct(StdHepPdg, StdHepPTLV)
          << std::endl;
#ifdef SAF_STRICT
        assert(false);
#endif
      }
    }
    return true;
  } // End Leptons

  if(StdHepPdg >= 1000000000){//Nuclear PDGs
    if(StdHepStatus == 0){ //Target Nucleus
      TargetPDG = StdHepPdg;
      TargetZ = ((StdHepPdg/10000)%1000);
      if(StdHepPTLV.M2()){
        std::cout << "[WARN]: Found incoming nucleus with momentum: "
          << PartStruct(StdHepPdg, StdHepPTLV) << ", this is likely from "
          "running nuwro2rootracker not in extended mode, so that struck "
          "nucleon and nuclear target information is combined."
          << std::endl;
#ifdef SAF_STRICT
        assert(false);
#endif
      }
      return true;
    }
    return false;
  }

  if(StdHepStatus == 0){ // Any other incoming particles
    if( (StdHepPdg == 2212) || (StdHepPdg == 2112)){ // Struck Nucleon
      this->HandleStruckNucleon(StdHepPTLV, StdHepPdg);
      return true;
    }
    std::cout << "[WARN]: Found unexpected incoming particle: "
      << PartStruct(StdHepPdg, StdHepPTLV)
      << std::endl;
#ifdef SAF_STRICT
      assert(false);
#endif
    return false;
  }

  if( (StdHepStatus != 1) && (StdHepStatus != 14)){ // Any other non-final states.
    std::cout << "[WARN]: Found unexpected particle: "
      << PartStruct(StdHepPdg, StdHepPTLV)
      << ", with status: " << StdHepStatus
      << std::endl;
#ifdef SAF_STRICT
      assert(false);
#endif
    return false;
  }

  bool charged = PDG::charge(StdHepPdg);

  if(StdHepStatus == 1){
    NFinalStateParticles++;

    //Hadronic Mass Sum
    if(PDG::charge(StdHepPdg)){
      ChargedFS_ESum += StdHepPTLV.E();
      ChargedFS_PSum += StdHepPTLV.Vect();
    }
    FS_ESum += StdHepPTLV.E();
    FS_PSum += StdHepPTLV.Vect();

    switch(StdHepPdg){
      case 22:{
        NGammas++;
        break;
      }
      case 2212:{
        this->HandleProton(StdHepPTLV, StdHepP3Mod);
        NProtons++;
        break;
      }
      case 2112:{
        NNeutrons++;
        break;
      }
      case 211:{
        NPiPlus++;
      } // Don't break
      case -211:{
        NPiMinus += (StdHepPdg==-211); // Catch pi+ fall through
        NChargedPions++;
      } // Don't break
      case 111:{
        this->HandlePion(StdHepPTLV, StdHepP3Mod, StdHepPdg);
        NPiZero += (StdHepPdg==111);
        NPions++;
        break;
      }
      //Other particles
      default: {
        NOtherParticles++;
        if(PDG::mass(StdHepPdg) == -1){
          std::cout << "[WARN]: Found Particle with unexpected PDG: "
            << StdHepPdg << std::endl;
#ifdef SAF_STRICT
          assert(false);
#endif
        }
      }
    }
  } else { // StdHepStatus == 14
    NPreFSIParticles++;
    //Hadronic Mass Sum
    PreFSI_ESum += StdHepPTLV.E();
    PreFSI_PSum += StdHepPTLV.Vect();

    switch(StdHepPdg){ // Currently only care specifically about:
      case 2212: { // Protons
        this->HandleProton(StdHepPTLV, StdHepP3Mod, StdHepStatus);
        break;
      }
      case 211:
      case 111:
      case -211: { // Pions
        this->HandlePion(StdHepPTLV,StdHepP3Mod,StdHepPdg);
        break;
      }
      default: { //Other
        if(PDG::mass(StdHepPdg) == -1){
          std::cout << "[WARN]: Found Particle with unexpected PDG: "
            << StdHepPdg << std::endl;
#ifdef SAF_STRICT
          assert(false);
#endif
        }
      }
    }
  }
  return true;
}

void SimpleAnalysisFormat::Finalise(){
//******************************************************************************
//                     Pertinent Particle Properties
//******************************************************************************

//Incoming Lepton
  ISLepton_PDG = ISLepton.PDG;
  ISLepton_4Mom = ISLepton.FourMomentum;

//Struck Nucleon
  StruckNucleonPDG = StruckNucleon.PDG;
  StruckNucleon_4Mom = StruckNucleon.FourMomentum;

//HMFSLepton
  HMFSLepton_PDG = HMFSLepton.PDG;
  HMFSLepton_4Mom = HMFSLepton.FourMomentum;

//Highest Momentum Proton
  HMProton_PDG = HMProton.PDG;
  HMProton_4Mom = HMProton.FourMomentum;
  HMProton_PreFSI_4Mom = HMProton_PreFSI.FourMomentum;

//Highest Momentum Pion
  HMChrgPion_PDG = HMChrgPion.PDG;
  HMChrgPion_4Mom = HMChrgPion.FourMomentum;
  HMChrgPion_PreFSI_PDG = HMChrgPion_PreFSI.PDG;
  HMChrgPion_PreFSI_4Mom = HMChrgPion_PreFSI.FourMomentum;

  HMPion_PDG = HMPion.PDG;
  HMPion_4Mom = HMPion.FourMomentum;
  HMPion_PreFSI_PDG = HMPion_PreFSI.PDG;
  HMPion_PreFSI_4Mom = HMPion_PreFSI.FourMomentum;

//Reconstructed Delta 3Mom
  HMNucleonPion_PreFSI_3Mom =
    (HMProton_PreFSI.FourMomentum + HMPion_PreFSI.FourMomentum).Vect();

//******************************************************************************
//                           Event Properties
//******************************************************************************

//Generator reaction code
  flags nwfl = RooTrackerUtils::GetFlagsFromNeutCode(NeutConventionReactionCode,
    ISLepton.PDG);
  NuWroDyn = RooTrackerUtils::GetDynFromFlags(nwfl);
  NuWroCC = nwfl.cc;
  NuWroAnty = nwfl.anty;

#ifdef DEBUG_SAF
  std::cout << "[INFO]: NeutCode: " << NeutConventionReactionCode
    << ", NuWroDyn: " << NuWroDyn << ", NuWroCC: " << NuWroCC
    << ", NuWroAnty: " << NuWroAnty << std::endl;
#endif

  FourMomentum_Transfer = (ISLepton_4Mom - HMFSLepton_4Mom);

  PreFSI_HadrMass = sqrt(PreFSI_ESum*PreFSI_ESum - PreFSI_PSum.Mag2());
  FS_HadrMass = sqrt(FS_ESum*FS_ESum - FS_PSum.Mag2());
  FS_ChargedHadrMass = sqrt(ChargedFS_ESum*ChargedFS_ESum - ChargedFS_PSum.Mag2());


//******************************************************************************
//******************************************************************************
}

void SimpleAnalysisFormat::Reset(){
//******************************************************************************
//                     Event Properties
//******************************************************************************

//Generator reaction code
  NeutConventionReactionCode = 0;
  NuWroDyn = 0;
  NuWroCC = false;
  NuWroAnty = false;

  EvtWght = 0;
  RunAvgEvtWght = 0;

  FourMomentum_Transfer.SetXYZT(0,0,0,0);

  PreFSI_HadrMass = 0;
  FS_HadrMass = 0;
  FS_ChargedHadrMass = 0;

//******************************************************************************
//                     Pertinent Particle Properties
//******************************************************************************

//Incoming Lepton
  ISLepton_PDG = 0;
  ISLepton_4Mom.SetXYZT(0,0,0,0);

//Struck Nucleon
  StruckNucleonPDG = 0;
  StruckNucleon_4Mom.SetXYZT(0,0,0,0);

//HMFSLepton
  HMFSLepton_PDG = 0;
  HMFSLepton_4Mom.SetXYZT(0,0,0,0);

//Highest Momentum Proton
  HMProton_PDG = 0;
  HMProton_4Mom.SetXYZT(0,0,0,0);
  HMProton_PreFSI_4Mom.SetXYZT(0,0,0,0);

//Highest Momentum Pion
  HMChrgPion_PDG = 0;
  HMChrgPion_4Mom.SetXYZT(0,0,0,0);
  HMChrgPion_PreFSI_PDG = 0;
  HMChrgPion_PreFSI_4Mom.SetXYZT(0,0,0,0);

  HMPion_PDG = 0;
  HMPion_4Mom.SetXYZT(0,0,0,0);
  HMPion_PreFSI_PDG = 0;
  HMPion_PreFSI_4Mom.SetXYZT(0,0,0,0);

//Reconstructed Delta 3Mom
  HMNucleonPion_PreFSI_3Mom.SetXYZ(0,0,0);

//******************************************************************************
//                       Subsequent Species Sums
//******************************************************************************

  NPreFSIParticles = 0;
  NFinalStateParticles = 0;

  NFinalStateLeptons = 0;
  NProtons = 0;
  NGammas = 0;
  NNeutrons = 0;
  NPiPlus = 0;
  NPiZero = 0;
  NPiMinus = 0;
  NPions = 0;
  NChargedPions = 0;
  NOtherParticles = 0;

//******************************************************************************
//                       Tangible Target Traits
//******************************************************************************

  TargetPDG = 0;
  TargetZ = 0;

//******************************************************************************
//                       Transients
//******************************************************************************

  //Transients
  HMFSLepton.Reset();
  ISLepton.Reset();
  StruckNucleon.Reset();
  HMProton.Reset();
  HMPion.Reset();
  HMChrgPion.Reset();
  HMProton_PreFSI.Reset();
  HMPion_PreFSI.Reset();
  HMChrgPion_PreFSI.Reset();

  PreFSI_ESum = 0;
  PreFSI_PSum.SetXYZ(0,0,0);
  FS_ESum = 0;
  FS_PSum.SetXYZ(0,0,0);
  ChargedFS_ESum = 0;
  ChargedFS_PSum.SetXYZ(0,0,0);
//******************************************************************************
//******************************************************************************

}

void SimpleAnalysisFormat::AddBranches(TTree* tree){

//******************************************************************************
//                     Event Properties
//******************************************************************************

//Generator reaction code
  tree->Branch("NeutConventionReactionCode",&NeutConventionReactionCode,
    "NeutConventionReactionCode/I");
  tree->Branch("NuWroDyn",&NuWroDyn,
    "NuWroDyn/I");
  tree->Branch("NuWroCC",&NuWroCC,
    "NuWroCC/O");
  tree->Branch("NuWroAnty",&NuWroAnty,
    "NuWroAnty/O");

  tree->Branch("EvtWght",&EvtWght,
    "EvtWght/D");
  tree->Branch("RunAvgEvtWght",&RunAvgEvtWght,
    "RunAvgEvtWght/D");

  tree->Branch("FourMomentum_Transfer",&_FourMomentum_Transfer);

  tree->Branch("PreFSI_HadrMass",&PreFSI_HadrMass,
    "PreFSI_HadrMass/D");
  tree->Branch("FS_HadrMass",&FS_HadrMass,
    "FS_HadrMass/D");
  tree->Branch("FS_ChargedHadrMass",&FS_ChargedHadrMass,
    "FS_ChargedHadrMass/D");

//******************************************************************************
//                     Pertinent Particle Properties
//******************************************************************************

//Incoming Lepton
  tree->Branch("ISLepton_PDG",&ISLepton_PDG, "ISLepton_PDG/I");
  tree->Branch("ISLepton_4Mom", &_ISLepton_4Mom);

//Struck Nucleon
  tree->Branch("StruckNucleonPDG",&StruckNucleonPDG, "StruckNucleonPDG/I");
  tree->Branch("StruckNucleon_4Mom", &_StruckNucleon_4Mom);

//HMFSLepton
  tree->Branch("HMFSLepton_PDG",&HMFSLepton_PDG, "HMFSLepton_PDG/I");
  tree->Branch("HMFSLepton_4Mom", &_HMFSLepton_4Mom);

//Highest Momentum Proton
  tree->Branch("HMProton_PDG",&HMProton_PDG, "HMProton_PDG/I");
  tree->Branch("HMProton_4Mom", &_HMProton_4Mom);
  tree->Branch("HMProton_PreFSI_4Mom", &_HMProton_PreFSI_4Mom);

//Highest Momentum Pion
  tree->Branch("HMChrgPion_PDG",&HMChrgPion_PDG, "HMChrgPion_PDG/I");
  tree->Branch("HMChrgPion_4Mom", &_HMChrgPion_4Mom);
  tree->Branch("HMChrgPion_PreFSI_4Mom", &_HMChrgPion_PreFSI_4Mom);

  tree->Branch("HMPion_PDG",&HMPion_PDG, "HMPion_PDG/I");
  tree->Branch("HMPion_4Mom", &_HMPion_4Mom);
  tree->Branch("HMPion_PreFSI_4Mom", &_HMPion_PreFSI_4Mom);

//Reconstructed Delta 3Mom
  tree->Branch("HMPion_4Mom", &_HMNucleonPion_PreFSI_3Mom);

//******************************************************************************
//                       Subsequent Species Sums
//******************************************************************************

  tree->Branch("NPreFSIParticles", &NPreFSIParticles,
    "NPreFSIParticles/I");
  tree->Branch("NFinalStateParticles", &NFinalStateParticles,
    "NFinalStateParticles/I");

  tree->Branch("NFinalStateLeptons", &NFinalStateLeptons,
    "NFinalStateLeptons/I");
  tree->Branch("NProtons", &NProtons, "NProtons/I");
  tree->Branch("NGammas", &NGammas, "NGammas/I");
  tree->Branch("NNeutrons", &NNeutrons, "NNeutrons/I");
  tree->Branch("NPiPlus", &NPiPlus, "NPiPlus/I");
  tree->Branch("NPiZero", &NPiZero, "NPiZero/I");
  tree->Branch("NPiMinus", &NPiMinus, "NPiMinus/I");
  tree->Branch("NPions", &NPions, "NPions/I");
  tree->Branch("NChargedPions", &NChargedPions, "NChargedPions/I");
  tree->Branch("NOtherParticles", &NOtherParticles, "NOtherParticles/I");

//******************************************************************************
//                       Tangible Target Traits
//******************************************************************************

  tree->Branch("TargetPDG", &TargetPDG, "TargetPDG/I");
  tree->Branch("TargetZ", &TargetZ, "TargetZ/I");

//******************************************************************************
//******************************************************************************

}
