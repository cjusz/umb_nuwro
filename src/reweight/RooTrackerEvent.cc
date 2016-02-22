#include <stdexcept>

#include "vec.h"
#include "vect.h"

#include "RooTrackerEvent.h"

namespace RooTrackerUtils {

int GetDynFromNeutCode(int NeutCode){
  switch(NeutCode){
    case 1: { return 1;} // QE
    case 2: { return 8;} // MEC
    case 11:
    case 12:
    case 13: { return 2; } //RES
    case 16: { return 4; } //COH
    case 27: { return 3; } //DIS
    default: { return -1; }
  }
}

int GetDynFromFlags(flags const &fl){
  return (fl.qel + 2*fl.res + 4*fl.dis + 6*fl.coh + 8*fl.mec + fl.nc);
}

flags GetFlagsFromNeutCode(int NeutCode, int nupdg){
  int dyn = GetDynFromNeutCode(NeutCode);
  flags fl;
  fl.qel = (dyn/2==0);
  fl.res = (dyn/2==1);
  fl.dis = (dyn/2==2);
  fl.coh = (dyn/2==3);
  fl.mec = (dyn/2==4);
  fl.nc = (!(dyn&1));
  fl.cc = (!fl.nc);
  fl.anty = (nupdg > 0);
  return fl;
}

int GetNeutReactionCodeFromNuwroEvent1(event const &nwEv){
  //simplified neut reaction codes
  int NeutReactCode = 100;
  if (nwEv.flag.qel){
    NeutReactCode = (nwEv.flag.cc ? 1 : 51);
  }
  else if (nwEv.flag.res){//this should be improved
    if (nwEv.flag.cc){
      NeutReactCode = 11;
    }else{
      NeutReactCode = 31;
    }
  }
  else if (nwEv.flag.dis){
    NeutReactCode = (nwEv.flag.cc ? 26 : 46);
  }
  else if (nwEv.flag.coh){
    NeutReactCode = (nwEv.flag.cc ? 16 : 36);
  }
  else if (nwEv.flag.mec){
    NeutReactCode = 2;
  }
  return (nwEv.flag.anty ? -1 : 1)*NeutReactCode;
}

particle GetNuWroParticle(
  Int_t const &inpStatus,
  Int_t const &inpPdg,
  Double_t const (&inpP4)[4],
  Double_t EScale){

  particle part;
  //Known massless
  if( (inpPdg == 12) ||
      (inpPdg == 14) ||
      (inpPdg == 16) ||
      (inpPdg == 22) ) {
    part.set_pdg_and_mass(inpPdg,0);
    part.set_momentum(vec(inpP4[0]*EScale, inpP4[1]*EScale,
      inpP4[2]*EScale));
  } else { //expect mass
    part = particle(vect(inpP4[3]*EScale, inpP4[0]*EScale,
      inpP4[1]*EScale, inpP4[2]*EScale));
    part.pdg = inpPdg;
  }

  //Checks that none of Mass or 4 mom components are nan
  //usually fails due to slightly negative Mass2
  if(!part.is_valid()){

    Double_t M2 = (inpP4[3]*inpP4[3] -
      (inpP4[0]*inpP4[0]+inpP4[1]*inpP4[1]+inpP4[2]*inpP4[2]));

    //Mass is 0 within float precision
    if( fabs(M2) < 1E-8){
      std::cout << "[WARN]: Found unexpected particle with 0 (to within "
        << "precision) mass, { StdHepStatus: " << inpStatus
        << ", StdHepPdg: " << inpPdg << ", StdHepP4: [ " << inpP4[0]
        << ", " << inpP4[1] << ", " << inpP4[2] << ", " << inpP4[3] << "], M2 = "
        << M2 << " }" << std::endl;
      part.set_pdg_and_mass(inpPdg,0);
      std::cout << "\tForcing mass to be 0 -> {Mass : " << part.mass()
        << ", E: " << part.E() << "}" << std::endl;
      return part;
    }

    //Mass is significantly negative.
    std::cerr << "[ERROR]: Bad NuWro particle: { " << part.mass() << ", ["
      << part.x << ", " <<  part.y << ", " << part.z << ", " << part.t
      << "] }" << std::endl;
    std::cerr << "\tGenerated from: { StdHepStatus: " << inpStatus
      << ", StdHepPdg: " << inpPdg << ", StdHepP4: [ " << inpP4[0]
      << ", " << inpP4[1] << ", " << inpP4[2] << ", " << inpP4[3] << "], M2 = "
      << (inpP4[3]*inpP4[3] -
        (inpP4[0]*inpP4[0]+inpP4[1]*inpP4[1]+inpP4[2]*inpP4[2]))
      << " }" << std::endl;
    throw std::exception();
  }
  return part;
}


event GetNuWroEvent1(RooTrackerEvent const & rt, Double_t EScale){
  event ev;

  for(Int_t i = 0; i < rt.fStdHepN; ++i){
    particle part = GetNuWroParticle(rt.fStdHepStatus[i], rt.fStdHepPdg[i],
      rt.fStdHepP4[i], EScale);
    switch(rt.fStdHepStatus[i]){
      case 0: {
        ev.in.push_back(part);
        //If we have found the incoming neutrino
        if(ev.in.back().lepton() && !(rt.fStdHepPdg[i]&1)){
          int NeutCode = rt.fEvtCode->GetString().Atoi();
          ev.flag = GetFlagsFromNeutCode(NeutCode, rt.fStdHepPdg[i]);
          ev.dyn = GetDynFromFlags(ev.flag);
        }
        break;
      }
      case 1: {
        ev.out.push_back(part);
        break;
      }
      case 14: {
        ev.post.push_back(part);
        break;
      }
      default: {
        std::cerr << "[ERROR]: Found StdHepStatus " << rt.fStdHepStatus[i]
          << std::endl;
        break;
      }
    }
    ev.all.push_back(part);
  }
  return ev;
}

RooTrackerEvent::RooTrackerEvent() : fEvtFlags(NULL), fEvtCode(NULL),
  Jacked(0), fEvtNum(0), fEvtXSec(0), fEvtDXSec(0), fEvtWght(0),
  fEvtProb(0), fStdHepN(0), fNuParentPdg(0), fNuParentDecMode(0){
  ClearArray(fEvtVtx);
  ClearArray(fStdHepPdg);
  ClearArray(fStdHepStatus);
  ClearArray2D(fStdHepX4);
  ClearArray2D(fStdHepP4);
  ClearArray2D(fStdHepPolz);
  ClearArray(fStdHepFd);
  ClearArray(fStdHepLd);
  ClearArray(fStdHepFm);
  ClearArray(fStdHepLm);
  ClearArray(fNuParentDecP4);
  ClearArray(fNuParentDecX4);
  ClearArray(fNuParentProP4);
  ClearArray(fNuParentProX4);
}

bool RooTrackerEvent::JackIn(TTree* tree, bool ReadFromTree){
  //TODO make it so that you can use the same tree for reading and writing.
  if(Jacked != 0){
    std::cerr << "[ERROR]: This RooTrackerEvent instance is already set up to"
      << ((Jacked==1)?" read from":" write to") << " a TTree." << std::endl;
    return false;
  }
  if(ReadFromTree){
    Jacked = SetBranchAddresses(tree) ? 1 : -1;
  } else {
    Jacked = AddBranches(tree) ? 2 : -1;
  }
  return (Jacked > 0);
}

bool RooTrackerEvent::SetBranchAddresses(TTree* tree){
  bool Added = true;
  Int_t addCode = 0;
  addCode = tree->SetBranchAddress("EvtCode", &fEvtCode);
  if(addCode){
    std::cerr << "Branch EvtCode failed with " << addCode << std::endl;
  }
  Added = Added && (!addCode);
  addCode = tree->SetBranchAddress("StdHepN", &fStdHepN);
  if(addCode){
    std::cerr << "Branch StdHepN failed with " << addCode << std::endl;
  }
  Added = Added && (!addCode);
  addCode = tree->SetBranchAddress("StdHepPdg", fStdHepPdg);
  if(addCode){
    std::cerr << "Branch StdHepPdg failed with " << addCode << std::endl;
  }
  Added = Added && (!addCode);
  addCode = tree->SetBranchAddress("StdHepP4", fStdHepP4);
  if(addCode){
    std::cerr << "Branch StdHepP4 failed with " << addCode << std::endl;
  }
  Added = Added && (!addCode);
  addCode = tree->SetBranchAddress("StdHepStatus", fStdHepStatus);
  if(addCode){
    std::cerr << "Branch StdHepStatus failed with " << addCode << std::endl;
  }
  Added = Added && (!addCode);

  return Added;
}

bool RooTrackerEvent::AddBranches(TTree* tree){
  bool Created = true;
  Created = Created && tree->Branch("EvtFlags", "TBits",
                                    &fEvtFlags, 32000, 1);
  Created = Created && tree->Branch("EvtCode", "TObjString",
                                    &fEvtCode, 32000, 1);
  Created = Created && tree->Branch("EvtNum",
                                    &fEvtNum, "EvtNum/I");
  Created = Created && tree->Branch("EvtXSec",
                                    &fEvtXSec, "EvtXSec/D");
  Created = Created && tree->Branch("EvtDXSec",
                                    &fEvtDXSec, "EvtDXSec/D");
  Created = Created && tree->Branch("EvtWght",
                                    &fEvtWght, "EvtWght/D");
  Created = Created && tree->Branch("EvtProb",
                                    &fEvtProb, "EvtProb/D");
  // vertex in det coord. [m],[s]
  Created = Created && tree->Branch("EvtVtx",
                                    fEvtVtx, "EvtVtx[4]/D");
  Created = Created && tree->Branch("StdHepN",
                                    &fStdHepN, "StdHepN/I");
  Created = Created && tree->Branch("StdHepPdg",
                                    fStdHepPdg, "StdHepPdg[StdHepN]/I");
  Created = Created && tree->Branch("StdHepStatus",
                                    fStdHepStatus, "StdHepStatus[StdHepN]/I");
  Created = Created && tree->Branch("StdHepX4",
                                    fStdHepX4, "StdHepX4[StdHepN][4]/D");
  // px,py,pz,E in LAB, GeV
  Created = Created && tree->Branch("StdHepP4",
                                    fStdHepP4, "StdHepP4[StdHepN][4]/D");
  Created = Created && tree->Branch("StdHepPolz",
                                    fStdHepPolz, "StdHepPolz[StdHepN][3]/D");
  Created = Created && tree->Branch("StdHepFd",
                                    fStdHepFd, "StdHepFd[StdHepN]/I");
  Created = Created && tree->Branch("StdHepLd",
                                    fStdHepLd, "StdHepLd[StdHepN]/I");
  Created = Created && tree->Branch("StdHepFm",
                                    fStdHepFm, "StdHepFm[StdHepN]/I");
  Created = Created && tree->Branch("StdHepLm",
                                    fStdHepLm, "StdHepLm[StdHepN]/I");

  Created = Created && tree->Branch("NuParentPdg",
                                    &fNuParentPdg, "NuParentPdg/I");
  Created = Created && tree->Branch("NuParentDecMode",
                                    &fNuParentDecMode, "NuParentDecMode/I");
  Created = Created && tree->Branch("NuParentDecP4",
                                    fNuParentDecP4, "NuParentDecP4[4]/D");
  Created = Created && tree->Branch("NuParentDecX4",
                                    fNuParentDecX4, "NuParentDecX4[4]/D");
  Created = Created && tree->Branch("NuParentProP4",
                                    fNuParentProP4, "NuParentProP4[4]/D");
  Created = Created && tree->Branch("NuParentProX4",
                                    fNuParentProX4, "NuParentProX4[4]/D");
  return Created;
}

RooTrackerEvent::~RooTrackerEvent(){}

}
