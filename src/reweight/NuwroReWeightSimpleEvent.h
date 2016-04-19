#ifndef NUWROREWEIGHTSIMPLEEVENT_H_SEEN
#define NUWROREWEIGHTSIMPLEEVENT_H_SEEN
#include "event1.h"

//#define DEBUG_SRWEv

namespace SRW {

struct QEEventProperties {
  // Also Need:
  //  NeutrinoEnergy
  //  NeutrinoPdg
  //  IsCC
  double FourMomentumTransfer;
  double FSLeptonMass;
  double NucleonMass;  // This may be the same as RES effective mass, but RES
                       // code boosts the nucleon before calculating the mass.
  char QELKind;        // process type: 0 - cc, 1 - nc proton, 2 - nc neutron
};

inline
void FillQEEventProperties(QEEventProperties &qev, event const &ev) {
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
  char NOutPart;
  int SecondOutPartPDG;
  int ThirdOutPartPDG;
  double EnergyTransfer;
  double NucleonEffectiveMass;
};

inline
void FillRESEventProperties(RESEventProperties &rev, event const &ev) {
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
        NeutrinoEnergy(0),
        NeutrinoPDG(0),
        IsAntiNeutrino(false),
        IsCC(false),
        NuWroDynCode(0) {
    new (&DynProp.QE) QEEventProperties();
  }
  SRWEvent(event const &ev)
      : NominalWeight(ev.weight),
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
  }
  void SetQEEvent(event const &ev) {
    new (&DynProp.QE) QEEventProperties();
    FillQEEventProperties(DynProp.QE, ev);
  }
};
}
#endif
