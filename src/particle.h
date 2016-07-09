#ifndef _particle_h_
#define _particle_h_
#include <cassert>
#include <cmath>
#include <iostream>
#include "generatormt.h"
#include "jednostki.h"
#include "pdg.h"
#include "vect.h"

using namespace PDG;

/// elementary particle on a mass shell is represented as a lorentz fourvector
/// and a pdg number. the position r-is included for tracting it in the nucleous
/// ks and orgi are introduced by Jarek for compatibility with the HEP-event
/// structure
class particle : public vect {
  double _mass;  ///< on shell mass
 public:
  vect r;   ///< position and time relative to the centre of the Nucleus event
            ///start time
  int pdg;  ///< pdg code of the particle
  char ks;  ///< from HEP event
  char orgin;  ///< from HEP event
  double
      travelled;  ///< distance from creation // (or norm of initial neutrino)
  int id;         ///< index in the vector 'all'
  int mother;     ///< index of mother in the vector 'all'
  int endproc;    ///< id of process that destroyed the particle
  double his_fermi;
  bool primary;

 public:
  inline particle() {
    travelled = x = y = z = t = _mass = pdg = mother = 0;
    id = -1;
    his_fermi = 0;
  }
  inline particle(int code, double mass);  ///< create particle at rest
  inline particle(double mass);            ///< create particle at rest
  inline particle(vect fourmomentum);      ///< create moving particle
  inline bool operator==(particle& p2);    ///< check if self->pdg==pdg2
  inline bool operator!=(particle& p2);    ///< check if self->pdg!=pdg2
  inline double E() const;                 ///< particle energy
  inline double energy() const;                  ///< particle energy
  inline double Ek() const;                      ///< particle kinetic energy
  inline double mass() const;              ///< particle mass
  inline double m() const;                       ///< particle mass
  inline int charge() const;                     ///< particle charge
  inline double mass2() const;                   ///< mass squared
  inline void set_mass(double mass);  ///< set particle mass and adjust energy
  inline void set_pdg_and_mass(int pdg,
                               double mass);  ///< set particle pdg and mass
  inline void set_proton() {
    set_pdg_and_mass(pdg_proton, mass_proton);
  }  ///< set particle pdg and mass
  inline void set_neutron() {
    set_pdg_and_mass(pdg_neutron, mass_neutron);
  }  ///< set particle pdg and mass
  inline void set_pi() {
    set_pdg_and_mass(pdg_pi, mass_pi);
  }  ///< set particle pdg and mass
  inline void set_piP() {
    set_pdg_and_mass(pdg_piP, mass_piP);
  }  ///< set particle pdg and mass
  inline void set_piM() {
    set_pdg_and_mass(-pdg_piP, mass_piP);
  }  ///< set particle pdg and mass
  inline void set_fermi(double x) { his_fermi = x; }  ///< set fermi energy

  inline void set_momentum(vec p);  ///< set particle momentum and adjust energy
  inline void set_energy(
      double E);       ///< set particle energy and adjust momentum
  inline vec p();      ///< the momentum as a 3-vector
  inline vect& p4();   ///< the four-momentum
  inline vect const & p4() const;   ///< the four-momentum
  inline vec v();      ///< the velocity as a 3-vector
  inline double v2();  ///< the velocity squared
  inline void krok_czasowy(double dt);  ///< move the particle by v*dt
  inline void krok(double dl);          ///< move the particle by dl
  inline void krok(vec dl);             ///< move the particle by dl
  inline double momentum() const;       ///< value of the momentum
  inline double momentum2() const;      ///< momenutm squared
  inline bool is_valid();  ///< checkes for nan in energy an momentum
  inline bool decay(particle& p1,
                    particle& p2);  ///< cause particle to decay into p1 and p2
                                    ///< (preserves the masses of p1 and p2)

  inline bool lepton();   /// true if lepton
  inline bool pion();     /// true if pion
  inline bool nucleon();  /// true if nucleon

  inline particle&
  mom()  /// mother particle !!! ONLY for elements of the vector all !!!
  {
    return this[mother - id];
  }

  inline int proc()  /// how was the particle created !!! ONLY for elements of
                     /// the vector all !!!
  {
    return this[mother - id].endproc;
  }

  inline int
  gen()  /// which generation!!! ONLY for elements of the vector all !!!
  {
    int ile = 0;
    particle* p = this;
    while (p->id) {
      p = &this[p->mother - id];
      ile++;
    }
    return ile;
  }

  inline double mcos() { return p().dir() * mom().p().dir(); }
  inline double Ek_in_frame(vec v);  ///<

  friend ostream& operator<<(ostream& out, particle p) {
    return out << "[" << p.pdg << "/" << p._mass << "]" << vect(p) << "@"
               << p.r / fermi;
  }
};  // class particle

bool decay(vect v, particle& p1,
           particle& p2);  /// the total momentum is enough
bool decay2(vect k, vect p4, particle& p1, particle& p2, double& coef);

bool particle::decay(particle& p1, particle& p2) {
  return ::decay(p4(), p1, p2);
}

double relative_speed(particle& a, particle& b);  ///<

double Ek_in_frame(particle& a, vec v);  ///<

/// get cos takes value x with probablity
/// proportional to (A*x+B)*x*x*x+1
double get_cos(double A, double B, double C, double D, double E, double F,
               double G, double H);
/////////////////////////////////////////////////////////////////////////////////////
//                I M P L E M E N T A C J A
/////////////////////////////////////////////////////////////////////////////////////

particle::particle(int code, double mass)
    : vect(mass, 0, 0, 0), _mass(mass), pdg(code) {
  id = -1;
  mother = 0;
  endproc = -1;
  travelled = 0;
  his_fermi = 0;
  primary = false;
}

particle::particle(double mass) : vect(mass, 0, 0, 0), _mass(mass) {
  id = -1;
  mother = 0;
  endproc = -1;
  travelled = 0;
  his_fermi = 0;
  primary = false;
}
particle::particle(vect fourmomentum) : vect(fourmomentum) {
  _mass = sqrt(fourmomentum * fourmomentum);
  id = -1;
  mother = 0;
  endproc = -1;
  travelled = 0;
  his_fermi = 0;
  primary = false;
}
inline bool particle::operator==(particle& p2) { return pdg == p2.pdg; }
inline bool particle::operator!=(particle& p2) { return pdg != p2.pdg; }

//    particle(int code):_code(code){}

double particle::energy() const { return t; }

double particle::E() const { return t; }

double particle::Ek() const { return t - _mass; }

double particle::mass() const { return _mass; }

double particle::m() const { return _mass; }

int particle::charge() const { return PDG::charge(pdg); }

double particle::mass2() const { return _mass * _mass; }

void particle::set_mass(double mass) {
  _mass = mass;
  t = sqrt(x * x + y * y + z * z + _mass * _mass);
}

void particle::set_pdg_and_mass(int pdg, double mass) {
  this->pdg = pdg;
  _mass = mass;
  t = sqrt(x * x + y * y + z * z + _mass * _mass);
}

void particle::set_momentum(vec p) {
  x = p.x;
  y = p.y;
  z = p.z;
  t = sqrt(p * p + _mass * _mass);
}

// adjust momentum so that to obtain the given energy
// on the mass shell
void particle::set_energy(double E) {
  if (E < _mass) {
    cerr << "E=" << E << " mass=" << _mass << endl;
    //    exit(31);
  }
  assert(E >= _mass);

  t = E;
  double k = sqrt((E * E - _mass * _mass) / (x * x + y * y + z * z));
  x *= k;
  y *= k;
  z *= k;
  while (v2() > 1) {
    x *= 0.9999999;
    y *= 0.9999999;
    z *= 0.9999999;
  }
}

vec particle::p() { return vec(x, y, z); }

vect& particle::p4() { return *this; }
vect const & particle::p4() const { return static_cast<vect const &>(*this); }

vec particle::v() { return vec(x / t, y / t, z / t); }
double particle::v2() { return (x * x + y * y + z * z) / t / t; }

double particle::momentum() const { return vec(x, y, z).length(); }

double particle::momentum2() const { return x * x + y * y + z * z; }

bool particle::is_valid()  ///< no nan errors in mass and 4-momentum
{
  return _mass == _mass && x == x && y == y && z == z && t == t;
}

void particle::krok(double dl) {
  double V = v().length();
  r += vect(dl * v() / V, dl / V);
  travelled += dl;
}

void particle::krok(vec dl) { r += vect(dl, 0); }

void particle::krok_czasowy(double dt) {
  r += vect(dt * v(), dt);
  travelled += v().length() * dt;
}

bool particle::lepton() {
  int x = pdg > 0 ? pdg : -pdg;
  return x >= 11 && x <= 16;
}

bool particle::pion() { return pdg == 111 || pdg == 211 || pdg == -211; }

bool particle::nucleon() { return pdg == pdg_proton || pdg == pdg_neutron; }

double particle::Ek_in_frame(vec v) {
  vect plab = p4();
  return plab.boost(-v).t - mass();
}

////////////////////////////////////////////////////////////
///  M2 = squared mass of decaying particle
///  ma2, mb2 = squared masses of decay products
/// result = squared momentum of decay products (in cms) if decay possible
///          negative number if deacy is impossible
inline double cms_momentum2(double M2, double ma2, double mb2) {
  if (M2 < ma2 + mb2) return -1;
  double X = M2 + ma2 - mb2;
  return X * X / 4 / M2 - ma2;
}

inline double cms_momentum(double M, double ma,
                           double mb) {  // if(M<ma+mb) return -1;
  double X = (M + ma + mb) * (M - ma - mb) * (M + ma - mb) * (M - ma + mb);

  return sqrt(X) / (2 * M);
}

#endif
