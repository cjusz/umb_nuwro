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

#include "RooTrackerEvent.h"

//#define DEBUG_SAF

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

///Simple generator analysis event format
struct PODSimpleAnalysisFormat {

//******************************************************************************
//                           Event Properties
//******************************************************************************

//Generator reaction code
  ///NEUT interaction code
  Int_t NeutConventionReactionCode;
  ///NuWro interaction code
  /// 0/1: QEL
  /// 2/3: RES
  /// 4/5: DIS
  /// 6/7: Coherent
  /// 8/9: MEC
  ///Odd values correspond to NC
  Int_t NuWroDyn;
  ///Is CC or NC
  bool NuWroCC;
  ///Is nu or nubar
  bool NuWroAnty;

  ///Generator event weight.
  ///Weighting a histogram of final state properties with this weight will
  ///correctly scale it to a cross section / nucleon / cm^2
  Double_t EvtWght;

  ///Four momentum transfer
  TLorentzVector FourMomentum_Transfer;

  ///Invariant hadronic mass of final state particles before FSI
  Double_t PreFSI_HadrMass;
  ///Invariant hadronic mass of final state particles after FSI
  Double_t FS_HadrMass;
  ///Invariant hadronic mass of charged final state particles after FSI
  Double_t FS_ChargedHadrMass;

//******************************************************************************
//                     Pertinent Particle Properties
//******************************************************************************

//Neutrino
  ///PDG of initial state lepton
  Int_t ISLepton_PDG;
  ///Four momentum of initial state lepton
  TLorentzVector ISLepton_4Mom;

//Struck Nucleon
  ///PDG of initial state lepton
  Int_t StruckNucleonPDG;
  ///Four momentum of initial state lepton
  TLorentzVector StruckNucleon_4Mom;

//HMFSLepton
  ///PDG of initial state lepton
  Int_t HMFSLepton_PDG;
  ///Four momentum of initial state lepton
  TLorentzVector HMFSLepton_4Mom;

//Highest Momentum Proton
  ///PDG of initial state lepton
  Int_t HMProton_PDG;
  ///Four momentum of initial state lepton
  TLorentzVector HMProton_4Mom;
  TLorentzVector HMProton_PreFSI_4Mom;

//Highest Momentum Pion
  ///PDG of initial state lepton
  Int_t HMChrgPion_PDG;
  ///Four momentum of initial state lepton
  TLorentzVector HMChrgPion_4Mom;
  ///PDG of initial state lepton
  Int_t HMChrgPion_PreFSI_PDG;
  ///Four momentum of initial state lepton
  TLorentzVector HMChrgPion_PreFSI_4Mom;

  ///PDG of initial state lepton
  Int_t HMPion_PDG;
  ///Four momentum of initial state lepton
  TLorentzVector HMPion_4Mom;
  ///PDG of initial state lepton
  Int_t HMPion_PreFSI_PDG;
  ///Four momentum of initial state lepton
  TLorentzVector HMPion_PreFSI_4Mom;

//Reconstructed Delta 3Mom
  ///Reconstructed resonance three momentum
  ///HMNucleonPion_PreFSI_3Mom =
  ///(HMProton_PreFSI_4Mom + HMPion_PreFSI_4Mom).Vect()
  TVector3 HMNucleonPion_PreFSI_3Mom;

//******************************************************************************
//                       Subsequent Species Sums
//******************************************************************************

  Int_t NPreFSIParticles;
  Int_t NFinalStateParticles;

  ///The number of final state lepton
  ///\note Pythia6 will sometimes produce electron-positron pairs, do not be too
  ///surprised if this is >1/
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

  ///Nuclear target Pdg code
  Int_t TargetPDG;
  ///Nuclear target proton number
  Int_t TargetZ;

//******************************************************************************
//******************************************************************************

};

inline SimpleAnalysisFormat const MakeSimpleAnalysisFormat(event const &ev){
  //Shamelessly nicked from nuwro2rootracker.cc

  SimpleAnalysisFormat SAFEv;
  SAFEv.NeutConventionReactionCode =
    RooTrackerUtils::GetNeutReactionCodeFromNuwroEvent1(ev);
  SAFEv.EvtWght = ev.weight;

  int NucleusPdg = 1e9 + (ev.par.nucleus_p*1e4)
        + (ev.par.nucleus_p+ev.par.nucleus_n)*10;

  Int_t StdHepPDG, StdHepStatus;
  Double_t StdHepP4[4];

  //Fake up a StdHepArray
  for(size_t nin = 0; nin < ev.in.size(); ++nin){
    particle const &in_part = ev.in[nin];

    //struck nucleon
    if(in_part.pdg == 2112 || in_part.pdg == 2212){ //neutron or proton -> nucleus
      if (ev.par.nucleus_p == 1 && ev.par.nucleus_n == 0){ //hydrogen
        StdHepPDG = in_part.pdg; //proton code
      }else{

        //Handle nuclear target
        StdHepPDG = NucleusPdg;
        StdHepP4[0] = 0; StdHepP4[1] = 0; StdHepP4[2] = 0; StdHepP4[3] = 0;
        StdHepStatus = 0;
        SAFEv.HandleStdHepParticle(StdHepPDG,StdHepStatus,StdHepP4);

        //Handle struck nucleon alone.
        StdHepPDG = in_part.pdg;
        StdHepP4[3] = in_part.E()/1000.0;
        StdHepP4[0] = in_part.x/1000.0;
        StdHepP4[1] = in_part.y/1000.0;
        StdHepP4[2] = in_part.z/1000.0;
        StdHepStatus = 0;
        SAFEv.HandleStdHepParticle(StdHepPDG,StdHepStatus,StdHepP4);
      }
    //else assume neutrino (gief ewro)
    }else{
      StdHepPDG = in_part.pdg;
      StdHepP4[3] = in_part.E()/1000.0;
      StdHepP4[0] = in_part.x/1000.0;
      StdHepP4[1] = in_part.y/1000.0;
      StdHepP4[2] = in_part.z/1000.0;
      StdHepStatus = 0;
      SAFEv.HandleStdHepParticle(StdHepPDG,StdHepStatus,StdHepP4);
    }
  }

  for (int nout = 0; nout < ev.out.size(); nout++){
    particle const &out_part = ev.out[nout];
    StdHepPDG = out_part.pdg;
    StdHepP4[3] = out_part.E()/1000.0;
    StdHepP4[0] = out_part.x/1000.0;
    StdHepP4[1] = out_part.y/1000.0;
    StdHepP4[2] = out_part.z/1000.0;
    StdHepStatus = 14;
    SAFEv.HandleStdHepParticle(StdHepPDG,StdHepStatus,StdHepP4);
  }
  for (int npost = 0; npost < ev.post.size();npost++){
    particle const &post_part = ev.post[npost];
    StdHepPDG = post_part.pdg;
    StdHepP4[3] = post_part.E()/1000.0;
    StdHepP4[0] = post_part.x/1000.0;
    StdHepP4[1] = post_part.y/1000.0;
    StdHepP4[2] = post_part.z/1000.0;
    StdHepStatus = 1;
    SAFEv.HandleStdHepParticle(StdHepPDG,StdHepStatus,StdHepP4);
  }
  SAFEv.Finalise();
  return SAFEv;
}

inline PODSimpleAnalysisFormat MakePODSimpleAnalysisFormat(event const & ev){
  //Proxy object for doing the calculation;

  SimpleAnalysisFormat const &SAFProxy = MakeSimpleAnalysisFormat(ev);

  PODSimpleAnalysisFormat rtnPODSAF;

  //Copy the calculated properties over
  rtnPODSAF.NeutConventionReactionCode = SAFProxy.NeutConventionReactionCode;
  rtnPODSAF.NuWroDyn = SAFProxy.NuWroDyn;
  rtnPODSAF.NuWroCC = SAFProxy.NuWroCC;
  rtnPODSAF.NuWroAnty = SAFProxy.NuWroAnty;
  rtnPODSAF.EvtWght = SAFProxy.EvtWght;
  rtnPODSAF.FourMomentum_Transfer = SAFProxy.FourMomentum_Transfer;
  rtnPODSAF.PreFSI_HadrMass = SAFProxy.PreFSI_HadrMass;
  rtnPODSAF.FS_HadrMass = SAFProxy.FS_HadrMass;
  rtnPODSAF.FS_ChargedHadrMass = SAFProxy.FS_ChargedHadrMass;
  rtnPODSAF.ISLepton_PDG = SAFProxy.ISLepton_PDG;
  rtnPODSAF.ISLepton_4Mom = SAFProxy.ISLepton_4Mom;
  rtnPODSAF.HMFSLepton_PDG = SAFProxy.HMFSLepton_PDG;
  rtnPODSAF.HMFSLepton_4Mom = SAFProxy.HMFSLepton_4Mom;
  rtnPODSAF.StruckNucleonPDG = SAFProxy.StruckNucleonPDG;
  rtnPODSAF.StruckNucleon_4Mom = SAFProxy.StruckNucleon_4Mom;
  rtnPODSAF.HMProton_PDG = SAFProxy.HMProton_PDG;
  rtnPODSAF.HMProton_4Mom = SAFProxy.HMProton_4Mom;
  rtnPODSAF.HMProton_PreFSI_4Mom = SAFProxy.HMProton_PreFSI_4Mom;
  rtnPODSAF.HMChrgPion_PDG = SAFProxy.HMChrgPion_PDG;
  rtnPODSAF.HMChrgPion_4Mom = SAFProxy.HMChrgPion_4Mom;
  rtnPODSAF.HMChrgPion_PreFSI_PDG = SAFProxy.HMChrgPion_PreFSI_PDG;
  rtnPODSAF.HMChrgPion_PreFSI_4Mom = SAFProxy.HMChrgPion_PreFSI_4Mom;
  rtnPODSAF.HMPion_PDG = SAFProxy.HMPion_PDG;
  rtnPODSAF.HMPion_4Mom = SAFProxy.HMPion_4Mom;
  rtnPODSAF.HMPion_PreFSI_PDG = SAFProxy.HMPion_PreFSI_PDG;
  rtnPODSAF.HMPion_PreFSI_4Mom = SAFProxy.HMPion_PreFSI_4Mom;
  rtnPODSAF.HMNucleonPion_PreFSI_3Mom = SAFProxy.HMNucleonPion_PreFSI_3Mom;
  rtnPODSAF.NPreFSIParticles = SAFProxy.NPreFSIParticles;
  rtnPODSAF.NFinalStateParticles = SAFProxy.NFinalStateParticles;
  rtnPODSAF.NFinalStateLeptons = SAFProxy.NFinalStateLeptons;
  rtnPODSAF.NProtons = SAFProxy.NProtons;
  rtnPODSAF.NGammas = SAFProxy.NGammas;
  rtnPODSAF.NNeutrons = SAFProxy.NNeutrons;
  rtnPODSAF.NPiPlus = SAFProxy.NPiPlus;
  rtnPODSAF.NPiZero = SAFProxy.NPiZero;
  rtnPODSAF.NPiMinus = SAFProxy.NPiMinus;
  rtnPODSAF.NPions = SAFProxy.NPions;
  rtnPODSAF.NChargedPions = SAFProxy.NChargedPions;
  rtnPODSAF.NOtherParticles = SAFProxy.NOtherParticles;
  rtnPODSAF.TargetPDG = SAFProxy.TargetPDG;
  rtnPODSAF.TargetZ = SAFProxy.TargetZ;

  return rtnPODSAF;
}

#endif
