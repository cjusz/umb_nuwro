#ifndef _event1_h_
#define _event1_h_

#include <iostream>
#include <vector>
#include "TObject.h"
#include "params.h"
#include "particle.h"

class flags {
 public:
  /// primary vertex flags
  bool qel;  ///< (quasi) elastic       (qel == dyn/2==0)
  bool res;  ///< resontant             (res == dyn/2==1)
  bool dis;  ///< deep inelastic        (dis == dyn/2==2)
  bool coh;  ///< coherent              (coh == dyn/2==3)
  bool mec;  ///< meson exhange current (mec == dyn/2==4)

  bool nc;  ///< neutral current       (nc == dyn%2)
  bool cc;  ///< charged current       (cc == !nc)

  bool anty;  ///< true if antineutrino (anty==in[0].pdg<0)
};

using namespace std;

class event : public TObject {
 public:
  flags flag;  ///< flags for convenient filtering of the events in root scripts
  params par;  ///< copy of all the input parameters
               ///< NOTE: some parameters (e.g. nucleus_p, nucleus_n) can vary
  ///< from event to event (e.g. in case of the detector simulation)
  vector<particle> in;    ///< vector of incoming particles
  vector<particle> temp;  ///< vector of temporary particles (daughters of
                          /// primary vertex in DIS)
  vector<particle> out;   ///< vector of outgoing particles (before fsi)
  vector<particle> post;  ///< vector of particles leaving the nucleus
  vector<particle>
      all;  ///< vector of all particles (inclluding temporary fsi particles)

  double weight;  ///< cross section of current event in cm^2 (set to total
                  /// cross section on saving to file)
  double norm;  ///< norm of the initial neutrino (for weighted beams; not used)
  vec r;        ///< position of the event inside the detector
  double
      density;  ///< density of the detector matter in point of the interaction
  int dyn;  ///< dynamics channel of the primary vertex. Possible values are:
            ///< 0,1 - qel  cc/nc - (quasi) elastic
            ///< 2,3 - res  cc/nc - resonant (via delta) with some background
            ///< 4,5 - dis  cc/nc - deep inelastric
            ///< 6,7 - coh  cc/nc - coherent
            ///< 8,9 - mec  cc/nc - meson exhchange current

  int nod[14];        ///< number of rescattering interactions of given type:
                      ///< 0 - nucleon elastic,
                      ///< 1 - nucleon ce,
                      ///< 2 - nucleon spp,
                      ///< 3 - nucleon dpp,
                      ///< 4 - pion elastic,
                      ///< 5 - pion ce,
                      ///< 6 - pion spp,
                      ///< 7 - pion dpp,
                      ///< 8 - pion abs,
                      ///< 9 - jailed,
                      ///< 10 - escape
                      ///< 11 - pion tpp
                      ///< 12 - no pion interactions
                      ///< 13 - no nucleon interactions
  int pr;             ///< number of protons  in the residual nucleus
  int nr;             ///< number of neutrons in the residual nucleus
  double r_distance;  //< distance from nucleus center of absorption point (if
                      //happened)

  event() : weight(0), norm(1) {}  ///< default constructor
  ~event() {}
  inline void check()
      const;                ///< stop program if event weight or momentum of any
                            /// particle is NaN (not a number)
  inline void clear_fsi();  ///< clear the fsi intermediate particles tracks
  inline particle nu() const;      ///< initial neutrino
  inline particle N0() const;      ///< initial nucleon
  inline vect q() const;           ///< fourmomentum transfer
  inline double q0() const;        ///< energy transfer
  inline double qv() const;        ///< momentum transfer
  inline double q2() const;        ///< momentum transfer squared
  inline double s() const;         ///< s - variable
  inline double costheta() const;  ///< cos theta lab
  inline double E() const;         ///< initial neutrino energy
  inline int charge(int r)
      const;  ///< total charge: 0 - initial, 1 - before fsi, 2 - after fsi
  inline double W() const;  ///< invariant mass (before fsi, all particles
                            /// except the rescattered lepton)
  inline int n()
      const;  ///< number of particles after primery vertex (before fsi)
  inline int f() const;           ///< number of particles leaving nucleous
  inline int nof(int pdg) const;  ///< number of particles after primery vertex
  inline int nof(int pdg1,
                 int pdg2) const;  ///< number of particles after primery vertex
  inline int nof(int pdg1, int pdg2,
                 int pdg3) const;  ///< number of particles after primery vertex
  inline int fof(int pdg) const;   ///< number of particles leaving nucleus
  inline int fof(int pdg1,
                 int pdg2) const;  ///< number of particles leaving nucleus
  inline int fof(int pdg1, int pdg2,
                 int pdg3) const;  ///< number of particles leaving nucleus
  inline double przod() const;
  inline double tyl() const;
  inline int number_of_nucleon_elastic() const;  ///< number of nucleon elastic
                                                 /// interactions during fsi
  inline int number_of_nucleon_ce()
      const;  ///< number of nucleon ce interactions during fsi
  inline int number_of_nucleon_spp()
      const;  ///< number of nucleon spp interactions during fsi
  inline int number_of_nucleon_dpp()
      const;  ///< number of nucleon dpp interactions during fsi
  inline int number_of_pion_elastic()
      const;  ///< number of pion elastic interactions during fsi
  inline int number_of_pion_ce()
      const;  ///< number of pion ce interactions during fsi
  inline int number_of_pion_spp()
      const;  ///< number of pion spp interactions during fsi
  inline int number_of_pion_dpp()
      const;  ///< number of pion dpp interactions during fsi
  inline int number_of_pion_tpp()
      const;  ///< number of pion tpp interactions during fsi
  inline int number_of_pion_abs()
      const;  ///< number of pions absorbed during fsi
  inline int number_of_pion_no_interactions()
      const;  ///< number of pion steps with no interactions
  inline int number_of_nucleon_no_interactions()
      const;  ///< number of nucleon steps with no interactions
  inline double absorption_position()
      const;  ///< positions where absorption occured
  inline int number_of_jailed()
      const;  ///< number of nucleons jailed in nucleous during fsi
  inline int number_of_escape()
      const;  ///< number of particles that escaped from
              /// nucleus during fsi
  inline int number_of_interactions()
      const;  ///< total number of interactions during fsi
  inline int number_of_particles(
      int pdg, bool fsi) const;  ///< number of particles before/after fsi
  inline double nuc_kin_en()
      const;  ///< total kinetic energy of nucleons that left the nucleus
  inline int num_part_thr(
      int pdg, bool fsi,
      double threshold) const;  ///< number of particles with
                                /// momentum above threshold
  /// before/after fsi
  inline double proton_cosine(bool fsi,
                              double thr) const;  ///< cosine of the angle
                                                  /// between two protons
  /// with momenta above thr
  inline double proton_transp_mom() const;
  inline double proton_transp_mom2() const;
  inline int proton_transp() const;
  inline int proton_pair_number1(bool fsi, double thr) const;
  inline int proton_pair_number2(bool fsi, double thr) const;
  inline double part_max_mom(
      int pdg,
      bool fsi) const;  ///< maximal momentum of particle pdg
  inline double part_sec_mom(
      int pdg, bool fsi) const;  ///< second largest momentum of particle pdg
  inline double vert_act(double pion_threshold, bool fsi,
                         double proton_threshold) const;
  inline double Erec(double Bin) const;  ///< reconstructed neutrino energy
  ClassDef(event, 1);
};

/// add particle to a list and set its id to position in the list.
inline void registration(vector<particle> &list, particle &p);

/// I M P L E M E N E T A T I O N

/// clear the fsi intermediate particles tracks
void event::clear_fsi() {
  post = vector<particle>();
  all = vector<particle>();
  // for(int i=0;i<12;i++) nod[i]=0; //it is already done on the begining of the
  // cascade
}

/// initial neutrino
particle event::nu() const { return in[0]; }

/// initial nucleon
particle event::N0() const { return in[1]; }

/// fourmomentum transfer
vect event::q() const {
  vect q = in[0] - out[0];
  return q;
}

/// energy transfer
double event::q0() const { return in[0].t - out[0].t; }

/// momentum transfer
double event::qv() const {
  vect q = in[0] - out[0];
  return vec(q).norm();
}

/// fourmomentum transfer squared
double event::q2() const {
  vect q = in[0] - out[0];
  return q * q;
}

/// s variable
double event::s() const {
  vect q = in[0] + in[1];
  return q * q;
}

/// cos theta lab
double event::costheta() const { return cos(vec(in[0]), vec(out[0])); }

/// neutrino energy
double event::E() const { return in[0].t; }

/// number of particles after primary vertex
int event::n() const { return out.size(); }

/// number of particles leaving nucleus
int event::f() const { return post.size(); }

/// number of particles with given pdg after primary vertex
int event::nof(int pdg) const {
  int c = 0;
  for (size_t i = 0; i < out.size(); i++)
    if (out[i].pdg == pdg) c++;
  return c;
}

/// number of particles with given pdg after primary vertex
int event::nof(int pdg1, int pdg2) const {
  int c = 0;
  for (size_t i = 0; i < out.size(); i++)
    if (out[i].pdg == pdg1 || out[i].pdg == pdg2) c++;
  return c;
}

/// number of particles with given pdg after primary vertex
int event::nof(int pdg1, int pdg2, int pdg3) const {
  int c = 0;
  for (size_t i = 0; i < out.size(); i++)
    if (out[i].pdg == pdg1 || out[i].pdg == pdg2 || out[i].pdg == pdg3) c++;
  return c;
}

/// number of particles with given pdg leaving nucleus
int event::fof(int pdg) const {
  int c = 0;
  for (size_t i = 0; i < post.size(); i++)
    if (post[i].pdg == pdg) c++;
  return c;
}

/// number of particles with given pdg leaving nucleus
int event::fof(int pdg1, int pdg2) const {
  int c = 0;
  for (size_t i = 0; i < post.size(); i++)
    if (post[i].pdg == pdg1 || post[i].pdg == pdg2) c++;
  return c;
}

/// number of particles with given pdg leaving nucleus
int event::fof(int pdg1, int pdg2, int pdg3) const {
  int c = 0;
  for (size_t i = 0; i < post.size(); i++)
    if (post[i].pdg == pdg1 || post[i].pdg == pdg2 || post[i].pdg == pdg3) c++;
  return c;
}

/// total charge: 0 - initial, 1 - before fsi, 2 - after fsi
int event::charge(int r) const {
  vector<particle> const *set = &post;
  int c = 0;
  switch (r) {
    case 0:
      return par.nucleus_p + in[0].charge();
      break;
    case 1:
      set = &out;
      c = par.nucleus_p - (dyn < 6) * in[1].charge();
      break;
    case 2:
      set = &post;
      c = pr;
      break;
  }
  for (size_t i = 0; i < set->size(); i++) c += (*set)[i].charge();
  return c;
}

/// invaraint mass
double event::W() const {
  vect h = out[1];
  for (size_t a = 2; a < out.size(); a++) h = h + out[a];
  return sqrt(h * h);
}

///
double event::przod() const {
  int licz = 0;
  vect h = out[1];
  for (size_t a = 2; a < out.size(); a++) h = h + out[a];

  vect tran = in[0] - out[0];
  vec ptr = vec(tran.x, tran.y, tran.z);

  for (size_t b = 1; b < out.size(); b++) {
    if ((out[b].pdg != 111) && (out[b].pdg != 22) && (out[b].pdg != 130) &&
        (out[b].pdg != 2112) && (out[b].pdg != -2112))

    {
      vect hh = out[b];
      hh.boost(-h.v());

      vec phh = vec(hh.x, hh.y, hh.z);

      if (ptr * phh > 0) licz = licz + 1;
    }
  }
  return licz;
}

///
double event::tyl() const {
  int licz = 0;
  vect h = out[1];
  for (size_t a = 2; a < out.size(); a++) h = h + out[a];

  vect tran = in[0] - out[0];
  vec ptr = vec(tran.x, tran.y, tran.z);

  for (size_t b = 1; b < out.size(); b++) {
    if ((out[b].pdg != 111) && (out[b].pdg != 22) && (out[b].pdg != 130) &&
        (out[b].pdg != 2112) && (out[b].pdg != -2112))

    {
      vect hh = out[b];
      hh.boost(-(h.v()));

      vec phh = vec(hh.x, hh.y, hh.z);

      if (ptr * phh < 0) licz = licz + 1;
    }
  }
  return licz;
}

int event::number_of_nucleon_elastic() const { return nod[0]; }

int event::number_of_nucleon_ce() const { return nod[1]; }

int event::number_of_nucleon_spp() const { return nod[2]; }

int event::number_of_nucleon_dpp() const { return nod[3]; }

int event::number_of_pion_elastic() const { return nod[4]; }

int event::number_of_pion_ce() const { return nod[5]; }

int event::number_of_pion_spp() const { return nod[6]; }

int event::number_of_pion_dpp() const { return nod[7]; }

int event::number_of_pion_tpp() const { return nod[11]; }

int event::number_of_pion_abs() const { return nod[8]; }

int event::number_of_jailed() const { return nod[9]; }

int event::number_of_escape() const { return nod[10]; }

int event::number_of_interactions() const {
  int noi = 0;
  for (size_t i = 0; i < 11; i++) {
    noi = noi + nod[i];
  }

  return noi;
}

int event::number_of_pion_no_interactions() const { return nod[12]; }

int event::number_of_nucleon_no_interactions() const { return nod[13]; }

double event::absorption_position() const { return r_distance; }

/// number of particles of given pdg code ( 0 - before FSI, 1 - after FSI )
int event::number_of_particles(int pdg, bool fsi) const {
  int number = 0;

  if (fsi) {
    for (size_t k = 0; k < post.size(); k++) {
      if (post[k].pdg == pdg) number++;
    }
  } else if (!fsi) {
    for (size_t k = 0; k < out.size(); k++) {
      if (out[k].pdg == pdg) number++;
    }
  }

  return number;
}

/// total kinetic energy of nucleons that left the nucleus
double event::nuc_kin_en() const {
  double sum = 0;
  for (size_t k = 0; k < post.size(); k++) {
    if (post[k].pdg == 2212 || post[k].pdg == 2112) sum += post[k].Ek();
  }
  return sum;
}

/// number of particles of given pdg with momentum above threshold before/after
/// fsi
int event::num_part_thr(int pdg, bool fsi, double threshold) const {
  int number = 0;

  if (fsi) {
    for (size_t k = 0; k < post.size(); k++) {
      if ((post[k].pdg == pdg) && (post[k].momentum() > threshold)) number++;
    }
  } else if (!fsi) {
    for (size_t k = 0; k < out.size(); k++) {
      if ((out[k].pdg == pdg) && (out[k].momentum() > threshold)) number++;
    }
  }

  return number;
}

/// cosine if the angle between the two outgoing protons
double event::proton_cosine(bool fsi, double thr) const {
  int numer[2];
  int ile = 0;
  if (fsi) {
    for (size_t k = 0; k < post.size(); k++) {
      if (post[k].pdg == 2212 && post[k].momentum() > thr) {
        numer[ile] = k;
        ile++;
      }
    }
    return cos(post[numer[0]], post[numer[1]]);
    // return ( post[numer[0]].x*post[numer[1]].x +
    // post[numer[0]].y*post[numer[1]].y + post[numer[0]].z*post[numer[1]].z
    // )/post[numer[0]].momentum()/post[numer[1]].momentum();

  } else {
    for (size_t k = 0; k < out.size(); k++) {
      if (out[k].pdg == 2212 && out[k].momentum() > thr) {
        numer[ile] = k;
        ile++;
      }
    }
    return cos(out[numer[0]], out[numer[1]]);
    // return ( out[numer[0]].x*out[numer[1]].x +
    // out[numer[0]].y*out[numer[1]].y + out[numer[0]].z*out[numer[1]].z
    // )/out[numer[0]].momentum()/out[numer[1]].momentum();
  }
}

/// calculates momentum of proton which did not suffer from fsi (the first one,
/// sometimes there are two,see below)
double event::proton_transp_mom() const {
  for (size_t k = 0; k < out.size(); k++)
    if (out[k].pdg == 2212) {
      for (size_t l = 0; l < post.size(); l++) {
        if (post[l].pdg == 2212) {
          //        double kos = ( out[k].x*post[l].x +
          // out[k].y*post[l].y + out[k].z*post[l].z
          //)/out[k].momentum()/post[l].momentum();
          double kos = cos(out[k], post[l]);
          if (kos > 0.999) return out[k].momentum();
        }
      }
    }
  return 0;
}

/// calculates momentum of proton which did not suffer from fsi (the second one
/// if it happens -- very rarely, but stil...)
double event::proton_transp_mom2() const {
  int counter = 0;
  for (size_t k = 0; k < out.size(); k++)
    if (out[k].pdg == 2212) {
      for (size_t l = 0; l < post.size(); l++) {
        if (post[l].pdg == 2212) {
          double kos = (out[k].x * post[l].x + out[k].y * post[l].y +
                        out[k].z * post[l].z) /
                       out[k].momentum() / post[l].momentum();
          if (kos > 0.999 && counter == 1) return out[k].momentum();

          if (kos > 0.999 && counter == 0) counter++;
        }
      }
    }
  return 0;
}

/// calculates number of protons which did not suffer from fsi
int event::proton_transp() const {
  int ile = 0;
  for (size_t k = 0; k < out.size(); k++)
    if (out[k].pdg == 2212) {
      for (size_t l = 0; l < post.size(); l++) {
        if (post[l].pdg == 2212) {
          //          double kos = ( out[k].x*post[l].x
          //+
          // out[k].y*post[l].y + out[k].z*post[l].z
          //)/out[k].momentum()/post[l].momentum();
          double kos = cos(out[k], post[l]);
          if (kos > 0.999) ile++;
        }
      }
    }
  return ile;
}

int event::proton_pair_number1(bool fsi, double thr) const {
  int numer[2];
  int ile = 0;
  if (fsi) {
    for (size_t k = 0; k < post.size(); k++) {
      if (post[k].pdg == 2212 && post[k].momentum() > thr) {
        numer[ile] = k;
        ile++;
      }
    }
    return numer[0];
  } else {
    for (size_t k = 0; k < out.size(); k++) {
      if (out[k].pdg == 2212 && out[k].momentum() > thr) {
        numer[ile] = k;
        ile++;
      }
    }
    return numer[0];
  }
}

int event::proton_pair_number2(bool fsi, double thr) const {
  int numer[2];
  int ile = 0;
  if (fsi) {
    for (size_t k = 0; k < post.size(); k++) {
      if (post[k].pdg == 2212 && post[k].momentum() > thr) {
        numer[ile] = k;
        ile++;
      }
    }
    return numer[1];
  } else {
    for (size_t k = 0; k < out.size(); k++) {
      if (out[k].pdg == 2212 && out[k].momentum() > thr) {
        numer[ile] = k;
        ile++;
      }
    }
    return numer[1];
  }
}

/// maximal momentum of particle with given pdg before/after FSI (when fsi=0/1
/// resp.)
double event::part_max_mom(int pdg, bool fsi) const {
  double mom = 0.0;

  if (fsi) {
    for (size_t k = 0; k < post.size(); k++) {
      if ((post[k].pdg == pdg) && (post[k].momentum() > mom))
        mom = post[k].momentum();
    }
  } else if (!fsi) {
    for (size_t k = 0; k < out.size(); k++) {
      if ((out[k].pdg == pdg) && (out[k].momentum() > mom))
        mom = out[k].momentum();
      // cout<<mom<<endl;
    }
  }

  return mom;
}

/// second largest momentum of particle with given pdg before/after FSI (when
/// fsi=0/1 resp.)
double event::part_sec_mom(int pdg, bool fsi) const {
  double mom[2] = {0.0, 0.0};
  double memory;

  if (fsi) {
    for (size_t k = 0; k < post.size(); k++) {
      if (post[k].pdg == pdg) {
        double ped = post[k].momentum();
        if (ped > mom[1]) {
          mom[1] = ped;
        }
        if (ped > mom[0]) {
          memory = mom[0];
          mom[0] = ped;
          mom[1] = memory;
        }
      }
    }
  } else if (!fsi) {
    for (size_t k = 0; k < out.size(); k++) {
      if (out[k].pdg == pdg) {
        double ped = out[k].momentum();
        // cout<<ped<<endl;
        if (ped > mom[1]) {
          mom[1] = ped;
        }
        if (ped > mom[0]) {
          memory = mom[0];
          mom[0] = ped;
          mom[1] = memory;
        }
      }
    }
  }

  return mom[1];
}

double event::vert_act(double pion_threshold, bool fsi,
                       double proton_threshold) const {
  double veract = 0.0;

  if (fsi) {
    for (size_t k = 0; k < post.size(); k++) {
      if ((post[k].pdg == 211) && (post[k].momentum() < pion_threshold))
        veract += post[k].t - post[k].mass();

      if ((post[k].pdg == -211) && (post[k].momentum() < pion_threshold))
        veract += post[k].t - post[k].mass();

      if ((post[k].pdg == 2212) && (post[k].momentum() < proton_threshold))
        veract += post[k].t - post[k].mass();

      // positive kaon; assume never reconstructed
      if (post[k].pdg == 321) veract += post[k].t - post[k].mass();
    }
    return veract;
  } else {
    for (size_t k = 0; k < out.size(); k++) {
      if ((out[k].pdg == 211) && (out[k].momentum() < pion_threshold))
        veract += out[k].t - out[k].mass();

      if ((out[k].pdg == -211) && (out[k].momentum() < pion_threshold))
        veract += out[k].t - out[k].mass();

      if ((out[k].pdg == 2212) && (out[k].momentum() < proton_threshold))
        veract += out[k].t - out[k].mass();

      // positive kaon; assume never reconstructed
      if (out[k].pdg == 321) veract += out[k].t - out[k].mass();
    }
    return veract;
  }
}

/// Reconstructed neutrino energy
double event::Erec(double Bin) const {
  double massprim = in[1].mass() - Bin;
  return (out[0].t * massprim +
          0.5 * (out[1].mass() * out[1].mass() - out[0].mass() * out[0].mass() -
                 massprim * massprim)) /
         (massprim - out[0].t + out[0].z);
}

/// stop program if event weight or momentum of any particle is NaN (not a
/// number)
void event::check() const {
  for (size_t i = 0; i < out.size(); i++) {
    particle p = out[i];
    if (!(p.x == p.x && p.y == p.y && p.z == p.z and p.t == p.t)) {
      cerr << p << endl;
      exit(20);
    }
  }
  if (!(weight == weight)) {
    cerr << "dyn=" << dyn << endl;
    exit(21);
  }
}

/// add particle to a list and set its id to position in the list.
void registration(vector<particle> &list, particle &p) {
  p.id = list.size();
  list.push_back(p);
}

#endif
