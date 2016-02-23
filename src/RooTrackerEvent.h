#ifndef ROOTRACKEREVENT_HXX_SEEN
#define ROOTRACKEREVENT_HXX_SEEN

#include <stdexcept>

#include "TTree.h"
#include "TBits.h"

#include "vec.h"
#include "vect.h"

#include "event1.h"
#include "particle.h"

namespace RooTrackerUtils {

struct RooTrackerEvent;

int GetDynFromNeutCode(int NeutCode);

int GetDynFromFlags(flags const &fl);

flags GetFlagsFromNeutCode(int NeutCode, int nupdg);

int GetNeutReactionCodeFromNuwroEvent1(event const &nwEv);

particle GetNuWroParticle(Int_t const &inpStatus, Int_t const &inpPdg,
    Double_t const (&inpP4)[4], Double_t EScale=1.0);

event GetNuWroEvent1(RooTrackerEvent const &rt, Double_t EScale=1.0);

template<typename T, size_t N>
void ClearArray(T (&arr)[N]){
  for(size_t i = 0; i < N; ++i){
    arr[i] = 0;
  }
}
template<typename T, size_t N, size_t M>
void ClearArray2D(T (&arr)[N][M]){
  for(size_t i = 0; i < N; ++i){
    for(size_t j = 0; j < M; ++j){
      arr[i][j] = 0;
    }
  }
}

///Objectified RooTrackerEvent for reading and writing TTrees
struct RooTrackerEvent {

  /// The generator-specific event flags.
  TBits* fEvtFlags;

  /// The generator-specific string with the 'event code'
  TObjString* fEvtCode;

  /// The sequence number of the event (the event number).
  int fEvtNum;

  /// The cross section for the event (1E-38 cm2)
  double fEvtXSec;

  /// The differential cross section for the event kinematics
  /// (1E-38 cm2/{K^n})
  double fEvtDXSec;

  /// The weight for the event
  double fEvtWght;

  /// The probability for the event (given the cross section, path lengths,
  double fEvtProb;

  /// The event vertex position in detector coord syst (in meters and seconds).
  double fEvtVtx[4];

  /// The number of particles in the particle arrays to track
  int fStdHepN;

  /// The maximum number of particles that can be in the particle arrays.
  static const int kNPmax = 4000;

  /// The PDG codes for the particles to track. This may include generator
  /// specific codes for pseudo particles.
  int fStdHepPdg[kNPmax]; //[fStdHepN]

  /// The a generator specific status for each particle. Particles with a
  /// status equal to 1 will be tracked.
  int fStdHepStatus[kNPmax]; //[fStdHepN]

  /// The position (x, y, z, t) (fm, second) of the particle in the nuclear frame
  double fStdHepX4[kNPmax][4]; //[fStdHepN]

  /// The 4-momentum (px, py, pz, E) of the particle in the LAB frame (GeV)
  double fStdHepP4[kNPmax][4]; //[fStdHepN]

  /// The particle polarization vector.
  double fStdHepPolz[kNPmax][3]; //[fStdHepN]

  /// The index of the first daughter of the particle in the arrays.
  int fStdHepFd[kNPmax]; //[fStdHepN]

  /// The index last daughter of the particle in the arrays.
  int fStdHepLd[kNPmax]; //[fStdHepN]

  /// The index of the first mother of the particle in there arrays.
  int fStdHepFm[kNPmax]; //[fStdHepN]

  /// The index of the last mother of the particle in the arrays.
  int fStdHepLm[kNPmax]; //[fStdHepN]

  /// The PDG code of the particle which created the parent neutrino.
  int fNuParentPdg;

  /// The interaction mode at the vertex which created the parent neutrino.
  /// This is normally the decay mode of the parent particle.
  int fNuParentDecMode;

  /// The 4 momentum of the particle at the vertex which created the parent
  /// neutrino. This is normally the momentum of the parent particle at the
  /// decay point.
  double fNuParentDecP4[4];

  /// The position (meters, seconds) of the vertex at which the neutrino was
  /// created. This uses the target as the origin.
  double fNuParentDecX4[4];

  /// The momentum (GeV) of the parent particle at it's production point.
  double fNuParentProP4[4];

  /// The position (meters, seconds) of the parent particle at it's
  /// production point. This uses the target as the origin.
  double fNuParentProX4[4];

  /// The vertex ID of the parent particle vertex.
  int fNuParentProNVtx;

  //Whether set up for reading and/or writing successfully
  int Jacked;

  RooTrackerEvent();

  bool SetBranchAddresses(TTree* tree);
  bool AddBranches(TTree* tree);

  ///Use TTree::SetBranchAddress/TTree::Branch to read/write from TTree
  bool JackIn(TTree* tree, bool ReadFromTree = true);

  ~RooTrackerEvent();
};

}
#endif
