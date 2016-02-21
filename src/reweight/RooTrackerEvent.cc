#include <stdexcept>

#include "vec.h"
#include "vect.h"

#include "RooTrackerEvent.h"

RooTrackerEvent::RooTrackerEvent(){
  JackedIn = false;
  InpToMev = 1.0;
}

bool RooTrackerEvent::JackIn(TTree* tree){
  JackedIn = true;
  Int_t addCode = 0;
  addCode = tree->SetBranchAddress("EvtCode", &NeutReactionCode);
  if(addCode){
    std::cerr << "Branch EvtCode failed with " << addCode << std::endl;
  }
  JackedIn = JackedIn && (!addCode);
  addCode = tree->SetBranchAddress("StdHepN", &StdHepN);
  if(addCode){
    std::cerr << "Branch StdHepN failed with " << addCode << std::endl;
  }
  JackedIn = JackedIn && (!addCode);
  addCode = tree->SetBranchAddress("StdHepPdg", StdHepPdg);
  if(addCode){
    std::cerr << "Branch StdHepPdg failed with " << addCode << std::endl;
  }
  JackedIn = JackedIn && (!addCode);
  addCode = tree->SetBranchAddress("StdHepP4", StdHepP4);
  if(addCode){
    std::cerr << "Branch StdHepP4 failed with " << addCode << std::endl;
  }
  JackedIn = JackedIn && (!addCode);
  addCode = tree->SetBranchAddress("StdHepStatus", StdHepStatus);
  if(addCode){
    std::cerr << "Branch StdHepStatus failed with " << addCode << std::endl;
  }
  JackedIn = JackedIn && (!addCode);

  return JackedIn;
}

particle RooTrackerEvent::MakeParticle(Int_t const &inpStatus, Int_t const &inpPdg,
  Double_t const (&inpP4)[4]){

  particle part;
  if( (inpPdg == 12) ||
      (inpPdg == 14) ||
      (inpPdg == 16) ||
      (inpPdg == 22) ) {
    part.set_pdg_and_mass(inpPdg,0);
    part.set_momentum(vec(inpP4[0]*InpToMev, inpP4[1]*InpToMev,
      inpP4[2]*InpToMev));
  } else {
    part = particle(vect(inpP4[3]*InpToMev, inpP4[0]*InpToMev,
      inpP4[1]*InpToMev, inpP4[2]*InpToMev));
    part.pdg = inpPdg;
  }

  if(!part.is_valid()){
    std::cerr << "[ERROR]: Bad NuWro particle: { " << part.mass() << ", ["
      << part.x << ", " <<  part.y << ", " << part.z << ", " << part.t
      << "] }" << std::endl;
    std::cerr << "\tGenerated from: { StdHepStatus: " << inpStatus
      << ", StdHepPdg: " << inpPdg << ", StdHepP4: [ " << inpP4[0]
      << ", " << inpP4[1] << ", " << inpP4[2] << ", " << inpP4[3] << "]"
      << std::endl;
    throw std::exception();
  }
  return part;
}

int RooTrackerEvent::GetDynFromNeut(){
  int NeutCode = NeutReactionCode->GetString().Atoi();
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

flags RooTrackerEvent::GetFlags(Int_t nupdg){
  int dyn = GetDynFromNeut();
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
int RooTrackerEvent::GetDynFromFlags(flags const &fl){
  return (fl.qel + 2*fl.res + 4*fl.dis + 6*fl.coh + 8*fl.mec + fl.nc);
}

event RooTrackerEvent::GetNuWroEvent1(){
  event ev;

  for(Int_t i = 0; i < StdHepN; ++i){
    particle part = MakeParticle(StdHepStatus[i], StdHepPdg[i], StdHepP4[i]);
    if(StdHepStatus[i] == 0){
      ev.in.push_back(part);
      //If we have found the incoming neutrino
      if(ev.in.back().lepton() && !(StdHepPdg[i]&1)){
        ev.flag = GetFlags(StdHepPdg[i]);
        ev.dyn = GetDynFromFlags(ev.flag);
      }
    } else if (StdHepStatus[i] == 1){
      ev.out.push_back(part);
    } else {
      std::cerr << "[ERROR]: Found StdHepStatus " << StdHepStatus[i]
        << std::endl;
    }
    ev.all.push_back(part);
  }
  return ev;
}

RooTrackerEvent::~RooTrackerEvent(){}
