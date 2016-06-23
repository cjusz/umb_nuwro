#ifndef NUWROREWEIGHTSIMPLEEVENT_H_SEEN
#define NUWROREWEIGHTSIMPLEEVENT_H_SEEN
#include "event1.h"

//#define DEBUG_SRWEv

namespace SRW {

struct SRWEvent;
inline
std::ostream &operator<<(std::ostream &os, SRWEvent const &ev);

struct QEEventProperties {
  // Also Need:
  //  NeutrinoEnergy
  //  NeutrinoPdg
  //  IsCC
  double FourMomentumTransfer;
  double FSLeptonMass;
  double NucleonMass;  // This may be the same as RES effective mass, but RES
                       // code boosts the nucleon before calculating the mass.
  int QELKind;        // process type: 0 - cc, 1 - nc proton, 2 - nc neutron
};

inline void FillQEEventProperties(QEEventProperties &qev, event const &ev) {
  qev.FourMomentumTransfer = ev.q2();
  qev.FSLeptonMass = ev.out[0].mass();
  qev.NucleonMass = ev.N0().mass();
  qev.QELKind = ev.flag.cc ? 0 : (ev.in[1].pdg == 2212 ? 1 : 2);
}

struct RESEventProperties {
  // Also Need:
  //  NeutrinoEnergy
  //  NeutrinoPdg
  //  IsCC
  double HadronicMass;
  int StruckNucleonPdg;
  int NOutPart;
  int SecondOutPartPDG;
  int ThirdOutPartPDG;
  double EnergyTransfer;
  double NucleonEffectiveMass;
};

inline void FillRESEventProperties(RESEventProperties &rev, event const &ev) {
  rev.HadronicMass = ev.W();
  rev.StruckNucleonPdg = ev.in[1].pdg;
  rev.NOutPart = ev.out.size();
  rev.SecondOutPartPDG = ev.out[1].pdg;
  rev.ThirdOutPartPDG = ev.out[2].pdg;
  vect IncomingNucleon4Mom = ev.in[1];
  vect IncomingNeutrino4Mom = ev.in[0];
  IncomingNeutrino4Mom.boost(-IncomingNucleon4Mom.v());

  vect fsLep = ev.out[0];
  fsLep.boost(-IncomingNucleon4Mom.v());
  rev.EnergyTransfer = (IncomingNeutrino4Mom.t - fsLep.t);

  rev.NucleonEffectiveMass = sqrt(IncomingNucleon4Mom * IncomingNucleon4Mom);
};

union DynProperties {
  QEEventProperties QE;
  RESEventProperties RES;
};

struct SRWEvent {
  double NominalWeight;
  double mutable CacheWeight;

  double NeutrinoEnergy;
  double NeutrinoPDG;

  bool IsAntiNeutrino;
  bool IsCC;
  int NuWroDynCode;

#ifdef DEBUG_SRWEv
  int Mode;
#endif

  DynProperties DynProp;
  SRWEvent()
      : NominalWeight(0),
        CacheWeight(0xdeadbeef),
        NeutrinoEnergy(0),
        NeutrinoPDG(0),
        IsAntiNeutrino(false),
        IsCC(false),
        NuWroDynCode(0) {
    new (&DynProp.QE) QEEventProperties();
  }
  SRWEvent(event const &ev)
      : NominalWeight(ev.weight),
        CacheWeight(0xdeadbeef),
        NeutrinoEnergy(ev.E()),
        NeutrinoPDG(ev.nu().pdg),
        IsAntiNeutrino(ev.flag.anty),
        IsCC(ev.flag.cc),
        NuWroDynCode(ev.dyn) {
    // Set the union correctly
    switch (ev.dyn) {
      case 0:
      case 1: {
        SetQEEvent(ev);
#ifdef DEBUG_SRWEv
        Mode = 1;
#endif
        break;
      }
      case 2:
      case 3: {
        SetRESEvent(ev);
#ifdef DEBUG_SRWEv
        Mode = 2;
#endif
        break;
      }
      default: {
#ifdef DEBUG_SRWEv
        Mode = -1;
#endif
      }
    }
  }

  void SetRESEvent(event const &ev) {
    new (&DynProp.RES) RESEventProperties();
    FillRESEventProperties(DynProp.RES, ev);
#ifdef DEBUG_SRWEv
    std::cout << "[EVENTS] Loaded " << (*this) << std::endl;
#endif
  }
  void SetQEEvent(event const &ev) {
    new (&DynProp.QE) QEEventProperties();
    FillQEEventProperties(DynProp.QE, ev);
#ifdef DEBUG_SRWEv
    std::cout << "[EVENTS] Loaded " << (*this) << std::endl;
#endif
  }
};

inline
std::ostream &operator<<(std::ostream &os, SRWEvent const &ev) {
  switch (ev.NuWroDynCode) {
    case 0:
    case 1: {
      os << "NuWro SRW QE Ev : {";
      break;
    }
    case 2:
    case 3: {
      os << "NuWro SRW QE Ev : {";
      break;
    }
    default: { os << "NuWro SRW Ev : {"; }
  }

  os << " NominalWeight: " << ev.NominalWeight
     << ", CacheWeight: " << ev.CacheWeight
     << ", NeutrinoEnergy: " << ev.NeutrinoEnergy
     << ", NeutrinoPDG: " << ev.NeutrinoPDG
     << ", IsAntiNeutrino: " << ev.IsAntiNeutrino << ", IsCC: " << ev.IsCC
     << ", NuWroDynCode: " << ev.NuWroDynCode;

  switch (ev.NuWroDynCode) {
    case 0:
    case 1: {
      os << ", FourMomentumTransfer: " << ev.DynProp.QE.FourMomentumTransfer
         << ", FSLeptonMass: " << ev.DynProp.QE.FSLeptonMass
         << ", NucleonMass: " << ev.DynProp.QE.NucleonMass
         << ", QELKind: " << ev.DynProp.QE.QELKind;
      break;
    }
    case 2:
    case 3: {
      os << ", HadronicMass: " << ev.DynProp.RES.HadronicMass
         << ", StruckNucleonPdg: " << ev.DynProp.RES.StruckNucleonPdg
         << ", NOutPart: " << ev.DynProp.RES.NOutPart
         << ", SecondOutPartPDG: " << ev.DynProp.RES.SecondOutPartPDG
         << ", ThirdOutPartPDG: " << ev.DynProp.RES.ThirdOutPartPDG
         << ", EnergyTransfer: " << ev.DynProp.RES.EnergyTransfer
         << ", NucleonEffectiveMass: " << ev.DynProp.RES.NucleonEffectiveMass;
      break;
    }
    default: {}
  }
  return os << " }";
}
}
#endif
