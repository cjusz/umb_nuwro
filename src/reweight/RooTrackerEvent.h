#ifndef ROOTRACKEREVENT_HXX_SEEN
#define ROOTRACKEREVENT_HXX_SEEN

#include <stdexcept>

#include "TTree.h"

#include "vec.h"
#include "vect.h"

#include "event1.h"
#include "particle.h"

//As in nuwro2rootracker
static const int kNPmax = 4000;

struct RooTrackerEvent {
  TObjString* NeutReactionCode;
  Int_t StdHepN;
  Int_t StdHepStatus[kNPmax];
  Int_t StdHepPdg[kNPmax];
  Double_t StdHepP4[kNPmax][4];

  Double_t InpToMev;

  bool JackedIn;

  RooTrackerEvent();

  bool JackIn(TTree* tree);

  particle MakeParticle(Int_t const &inpStatus, Int_t const &inpPdg,
    Double_t const (&inpP4)[4]);

  int GetDynFromNeut();

  flags GetFlags(Int_t nupdg);
  int GetDynFromFlags(flags const &fl);

  event GetNuWroEvent1();

  ~RooTrackerEvent();
};

#endif
