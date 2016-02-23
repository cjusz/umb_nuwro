#ifndef __TRANSVERSITY_VARIABLE_OBJECTS_SEEN__
#define __TRANSVERSITY_VARIABLE_OBJECTS_SEEN__

//C++
#include <iostream>
#include <stdexcept>

//ROOT
#include "TObject.h"
#include "TLorentzVector.h"
#include "TTree.h"

static const Int_t kNThreshMax = 10;
static const Int_t kMaxFSMomenta = 20;

///\brief Structure used to hold intermediate StdHep information before
///finalising
struct PartStruct {
  PartStruct(){
    Reset();
  }
  PartStruct(Int_t PDG, TLorentzVector FourMomentum){
    this->PDG = PDG;
    this->Momentum = FourMomentum.Vect().Mag();
    this->FourMomentum = FourMomentum;
  }
  Int_t PDG;
  Double_t Momentum;
  TLorentzVector FourMomentum;
#ifdef SAF_STRICT
  Int_t StdHepStatus;
#endif
  void Reset(){
    PDG = 0;
    Momentum = 0;
    FourMomentum.SetXYZT(0,0,0,0);
#ifdef SAF_STRICT
    StdHepStatus = 0;
#endif
  }
};

inline std::ostream& operator<<(std::ostream& os,
  const TLorentzVector& tlv){
  return os << "[" << tlv[0] << ", " << tlv[1] << ", " << tlv[2] << ", "
    << tlv[3]  << ", M2: " << tlv.M2() << "]";
}

inline std::ostream& operator<<(std::ostream &os, PartStruct const & ps){
  return os << "{ PDG: " << ps.PDG << ", FourMomentum: " << ps.FourMomentum
    << "}";
}

struct SimpleAnalysisFormat {

protected:
//******************************************************************************
//                 Properties that shouldn't be saved
//******************************************************************************

  PartStruct HMFSLepton;
  PartStruct ISLepton;
  PartStruct StruckNucleon;
  PartStruct HMProton;
  PartStruct HMPion;
  PartStruct HMChrgPion;
  PartStruct HMProton_PreFSI;
  PartStruct HMPion_PreFSI;
  PartStruct HMChrgPion_PreFSI;

  Double_t PreFSI_ESum;
  TVector3 PreFSI_PSum;
  Double_t FS_ESum;
  TVector3 FS_PSum;
  Double_t ChargedFS_ESum;
  TVector3 ChargedFS_PSum;

  TLorentzVector* _ISLepton_4Mom;
  TLorentzVector* _StruckNucleon_4Mom;
  TLorentzVector* _HMFSLepton_4Mom;
  TLorentzVector* _HMProton_4Mom;
  TLorentzVector* _HMPion_4Mom;
  TLorentzVector* _HMChrgPion_4Mom;
  TLorentzVector* _HMProton_PreFSI_4Mom;
  TLorentzVector* _HMPion_PreFSI_4Mom;
  TLorentzVector* _HMChrgPion_PreFSI_4Mom;
  TVector3* _HMNucleonPion_PreFSI_3Mom;

  TLorentzVector* _FourMomentum_Transfer;

public:

  SimpleAnalysisFormat();
  virtual ~SimpleAnalysisFormat();

//******************************************************************************
//                           Event Properties
//******************************************************************************

//Generator reaction code
  Int_t NeutConventionReactionCode;
  Int_t NuWroDyn;
  bool NuWroCC;
  bool NuWroAnty;

  Double_t EvtWght;

  TLorentzVector FourMomentum_Transfer;

  Double_t PreFSI_HadrMass;
  Double_t FS_HadrMass;
  Double_t FS_ChargedHadrMass;

//******************************************************************************
//                     Pertinent Particle Properties
//******************************************************************************

//Neutrino
  Int_t ISLepton_PDG;
  TLorentzVector ISLepton_4Mom;

//Struck Nucleon
  Int_t StruckNucleonPDG;
  TLorentzVector StruckNucleon_4Mom;

//HMFSLepton
  Int_t HMFSLepton_PDG;
  TLorentzVector HMFSLepton_4Mom;

//Highest Momentum Proton
  Int_t HMProton_PDG;
  TLorentzVector HMProton_4Mom;
  TLorentzVector HMProton_PreFSI_4Mom;

//Highest Momentum Pion
  Int_t HMChrgPion_PDG;
  TLorentzVector HMChrgPion_4Mom;
  Int_t HMChrgPion_PreFSI_PDG;
  TLorentzVector HMChrgPion_PreFSI_4Mom;

  Int_t HMPion_PDG;
  TLorentzVector HMPion_4Mom;
  Int_t HMPion_PreFSI_PDG;
  TLorentzVector HMPion_PreFSI_4Mom;

//Reconstructed Delta 3Mom
  TVector3 HMNucleonPion_PreFSI_3Mom;

//******************************************************************************
//                       Subsequent Species Sums
//******************************************************************************

  Int_t NPreFSIParticles;
  Int_t NFinalStateParticles;

  Int_t NFinalStateLeptons;
  Int_t NProtons;
  Int_t NGammas;
  Int_t NNeutrons;
  Int_t NPiPlus;
  Int_t NPiZero;
  Int_t NPiMinus;
  Int_t NPions;
  Int_t NChargedPions;
  Int_t NOtherParticles;

//******************************************************************************
//                       Tangible Target Traits
//******************************************************************************

  Int_t TargetPDG;
  Int_t TargetZ;

//******************************************************************************
//******************************************************************************

  virtual void HandleProton(TLorentzVector const &StdHepPTLV,
    Double_t &StdHepP3Mod, Int_t StdHepStatus=1);
  virtual void HandlePion(TLorentzVector const &StdHepPTLV,
    Double_t &StdHepP3Mod, Int_t StdHepPdg, Int_t StdHepStatus=1);
  virtual void HandleStruckNucleon(TLorentzVector const &StdHepPTLV,
    Int_t StdHepPdg);
  virtual bool HandleStdHepParticle(
                            Int_t &StdHepPdg,
                            Int_t &StdHepStatus,
                            Double_t const (&StdHepP4)[4]);

  virtual void Finalise();

  virtual void Reset();

  virtual void AddBranches(TTree* tree);

};

#endif
