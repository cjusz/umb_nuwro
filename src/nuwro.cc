#include <iomanip>
#include <iomanip>
#include <sstream>
#include <vector>
#include "Analyser1.h"
#include "Interaction.h"
#include "TFile.h"
#include "TH1.h"
#include "TROOT.h"
#include "TTree.h"
#include "args.h"
#include "beam.h"
#include "beamHist.h"
#include "beam_uniform.h"
#include "chooser.h"
#include "coh.h"
#include "cohevent2.h"
#include "dis/disevent.h"
#include "dis/resevent2.h"
#include "dis/singlepion.h"
#include "event1.h"
#include "ff.h"
#include "geomy.h"
#include "hist.h"
#include "kaskada7.h"
#include "mecevent.h"
#include "nucleusmaker.h"
#include "pauli.h"
#include "pdg.h"
#include "qelevent.h"
#include "sfevent.h"
#include "stdlib.h"
#include "target_mixer.h"
#include "beam_mixed.h"
#include "beam_singleroothist.h"
#include "beam_mixedroothist.h"

extern double SPP[2][2][2][3][40];
// extern double sppweight;

params *p1 = NULL;
string data_dir;
#include "nuwro.h"

NuWro::~NuWro() {
  delete _mixer;
  delete _detector;
  delete _beam;
  delete _nucleus;
}

NuWro::NuWro() {
  _mixer = NULL;
  _detector = NULL;
  _beam = NULL;
  _nucleus = NULL;
}

void NuWro::set(params &par) {
  p = par;

  frandom_init(par.random_seed);

  dismode = false;

  if (par.target_type == 1) _mixer = new target_mixer(par);
  _detector = make_detector(par);

  _beam = create_beam(par, _detector);

  _nucleus = make_nucleus(par);

  ff_configure(par);
  refresh_dyn(par);
}

void NuWro::refresh_target(params &par) {
  delete _nucleus;
  _nucleus = make_nucleus(par);
}

void NuWro::refresh_dyn(params &par) {
  bool active[] = {par.dyn_qel_cc, par.dyn_qel_nc, par.dyn_res_cc,
                   par.dyn_res_nc, par.dyn_dis_cc, par.dyn_dis_nc,
                   par.dyn_coh_cc, par.dyn_coh_nc, par.dyn_mec_cc,
                   par.dyn_mec_nc};

  _procesy.reset(active);
}

geomy *NuWro::make_detector(params &p) {
  if (p.target_type != 2) return NULL;

  if (p.geo_file.length()) {
    try {
      if (p.geo_d.norm2() == 0)
        return new geomy(p.geo_file, p.geo_name);
      else
        return new geomy(p.geo_file, p.geo_name, p.geo_volume, p.geo_d,
                         p.geo_o);
    } catch (...) {
      cerr << "Failed to make detector." << endl;
      exit(3);
    }
  } else {
    cerr << "Failed to make detector. Parameter geo_file must not be empty if "
            "target_type=2."
         << endl;
    exit(4);
    return NULL;
  }
}

void NuWro::init(int argc, char **argv) {
  //  dismode=false;
  dismode = true;
  set_dirs(argv[0]);
  a.read(argc, argv);
  p.read(a.input);
  p.read(a.params, "command line");

  p.list(cout);
  p.list(string(a.output) + ".par");
  p1 = &p;
  _progress.open(a.progress);
  frandom_init(p.random_seed);
  if (p.beam_test_only == 0 && p.kaskada_redo == 0)
    if (p.dyn_dis_nc or p.dyn_res_nc or p.dyn_dis_cc or p.dyn_res_cc) {
      cout << " Calculating the one pion functions ..." << endl;
      singlepion(p);
      /*  for (int c = 0; c < 40; c++)
              cout << 1210 + c * 20 << "  MeV -> "
               << setw(10)<< SPP[0][0][0][0][c] << "  "
               << setw(10)<< SPP[0][0][0][1][c] << "  "
               << setw(10)<< SPP[0][0][0][2][c] << "  "
               << setw(10)<< SPP[0][0][1][0][c] << "  "
               << setw(10)<< SPP[0][0][1][1][c] << "  "
                       << setw(10)<< SPP[0][0][1][2][c] << endl;
*/
    }
  if (p.kaskada_redo == 0) {
    _nucleus = make_nucleus(p);
    if (p.target_type == 1)
      _mixer = new target_mixer(p);
    else
      _mixer = NULL;
    _detector = make_detector(p);

    cout << "Creating the beam ..." << endl;
    _beam = create_beam(p, _detector);
    if (_beam == NULL) {
      cerr << "No beam defined." << endl;
      exit(5);
    }
  }
  ff_configure(p);
  refresh_dyn(p);
}

void NuWro::makeevent(event *e, params &p) {
  static double max_norm = 0;
  particle nu;
  int dyn = e->dyn;
  if (_detector) {
    material mat;
    do {
      nu = _beam->shoot(dyn > 1 && dyn < 6 && dismode);
      if (nu.travelled > 0 && p.beam_weighted == 0) {
        if (nu.travelled < frandom() * max_norm) continue;
        if (nu.travelled > max_norm) max_norm = nu.travelled;
      }
      nu.travelled = 0;
      nu.r = vec(nu.r) + p.beam_offset;
      if (nu.r.x == 0 && nu.r.y == 0 && nu.r.z == 0)
        mat = _detector->getpoint();
      else
        mat = _detector->getpoint(nu.p(), nu.r);

    } while (not(mat.Z + mat.N > 0 &&
                 mat.w_density >= frandom() * _detector->max_dens()));

    /// change nucleus
    e->r = mat.r;
    p.nucleus_p = mat.Z;
    p.nucleus_n = mat.N;
    p.nucleus_E_b = 0;  // Use library value for E_b
    p.nucleus_kf = 0;   // Use library value for kF
                        //		cout<<mat.Z<<' '<<mat.N<<' '<<endl;
    if (mat.Z == 0 && mat.N == 0) throw "Empty isotope 00";
  } else {
    nu = _beam->shoot(dyn > 1 && dyn < 6 && dismode);
    nu.r = vec(nu.r) + p.beam_offset;
  }
  if (_detector or _mixer)  // _nucleus not reusable
  {
    delete _nucleus;
    _nucleus = make_nucleus(p);
    // cout<<"make_nucleus "<<_nucleus->p<<" "<<_nucleus->n<<endl;
  }
  e->in.push_back(nu);  // insert neutrino
  if (dyn < 6) {
    // insert target nucleon
    e->in.push_back(_nucleus->get_nucleon());
    e->in[0].r = e->in[1].r;
    assert(e->in[1] * e->in[1] > 0);
  }

  e->weight = 0;
  if (nu.travelled > 0) e->norm = nu.travelled;
  // else e->norm remains 1;

  e->flag.cc = dyn % 2 == 0;
  e->flag.nc = dyn % 2 == 1;

  e->flag.qel = dyn / 2 == 0;
  e->flag.res = dyn / 2 == 1;
  e->flag.dis = dyn / 2 == 2;
  e->flag.coh = dyn / 2 == 3;
  e->flag.mec = dyn / 2 == 4;

  e->flag.anty = nu.pdg < 0;

  if (p.beam_test_only) {
    e->weight = 1;
    e->out.push_back(e->in[0]);
    return;
  }
  double factor = 1.0;

  if (p.cc_smoothing and e->flag.cc and dyn == 0)  // only in qel_cc
  {
    if (e->in[0].pdg > 0) {
      factor = _nucleus->frac_neutron();
      e->in[1].set_neutron();
    } else {
      factor = _nucleus->frac_proton();
      e->in[1].set_proton();
    }
  }
  e->par = p;

  switch (dyn) {
    case 0:
      if (p.dyn_qel_cc)  // qel cc
      {
        if (p.sf_method > 0 and has_sf(*_nucleus, p.sf_method))
          sfevent2cc(p, *e, *_nucleus);
        else
          qelevent1(p, *e, *_nucleus, false);
      }
      break;
    case 1:
      if (p.dyn_qel_nc)  // qel nc
      {
        if (p.sf_method > 0 and has_sf(*_nucleus, p.sf_method))
          sfevent2nc(p, *e, *_nucleus);
        else
          qelevent1(p, *e, *_nucleus, true);
      }
      break;
    case 2:
      if (p.dyn_res_cc)  // res cc
      {
        resevent2(p, *e, true);
        if (p.pauli_blocking) mypauli_spp(*e, *_nucleus);
      }
      break;
    case 3:
      if (p.dyn_res_nc)  // res nc
      {
        resevent2(p, *e, false);
        if (p.pauli_blocking) mypauli_spp(*e, *_nucleus);
      }
      break;
    case 4:
      if (p.dyn_dis_cc)  // dis cc
      {
        disevent(p, *e, true);
        if (p.pauli_blocking) mypauli_spp(*e, *_nucleus);
      }
      break;
    case 5:
      if (p.dyn_dis_nc)  // dis nc
      {
        disevent(p, *e, false);
        if (p.pauli_blocking) mypauli_spp(*e, *_nucleus);
      }
      break;
    case 6:
      if (p.dyn_coh_cc)  // coh cc
      {
        if (p.coh_new) switch (p.coh_kind) {
            case 1:
              cohevent_cj(p, *e, *_nucleus, true);
              break;
            case 2:
              cohevent_bs(p, *e, *_nucleus, true);
              break;
            default:
              cohevent_bs(p, *e, *_nucleus, true);
              break;
          }
        else
          cohevent2(p, *e, *_nucleus, true);
      }
      break;
    case 7:
      if (p.dyn_coh_nc)  // coh nc
      {
        if (p.coh_new) switch (p.coh_kind) {
            case 1:
              cohevent_cj(p, *e, *_nucleus, false);
              break;
            case 2:
              cohevent_bs(p, *e, *_nucleus, false);
              break;
            default:
              cohevent_bs(p, *e, *_nucleus, false);
              break;
          }
        else
          cohevent2(p, *e, *_nucleus, false);
      }
      break;
    case 8:
      if (p.dyn_mec_cc)  // mec cc
      // if( nu.pdg>0 || !(p.mec_kind==3) )// al flavor states/antineutrinos
      // available
      {
        if (_nucleus->A() <= 1) break;
        switch (p.mec_kind) {
          case 1:
            mecevent_tem(p, *e, *_nucleus, true);
            break;
          case 2:
            mecevent2(p, *e, *_nucleus, true, false);
            break;
          case 3:
            mecevent_Nieves(p, *e, *_nucleus, true);
            break;
          case 4:
            mecevent2(p, *e, *_nucleus, true, true);
            break;
          default:
            mecevent_tem(p, *e, *_nucleus, true);
            break;
        }
        for (int i = 0; i < e->out.size(); i++) {
          e->out[i].r = e->in[1].r;
          e->out[i].set_momentum(e->out[i].p().fromZto(e->in[0].p()));
        }
      }
      break;
    case 9:
      if (p.dyn_mec_nc)       // mec nc
        if (p.mec_kind == 1)  // only TEM for NC
        {
          if (_nucleus->A() <= 1) break;
          switch (p.mec_kind) {
            case 1:
              mecevent_tem(p, *e, *_nucleus, false);
              break;
            default:
              mecevent_tem(p, *e, *_nucleus, false);
              break;
          }
          for (int i = 0; i < e->out.size(); i++) {
            e->out[i].r = e->in[1].r;
            e->out[i].set_momentum(e->out[i].p().fromZto(e->in[0].p()));
          }
        }
      break;
  }
  e->weight *= factor;

  if (e->weight == 0) {
    e->out.clear();

    e->out.push_back(e->in[0]);
    e->out.push_back(e->in[1]);
  }
  //      e->check();
}  // end of makeevent

void NuWro::finishevent(event *e, params &p) {
  for (int i = 0; i < 1 /* e->in.size()*/; i++) {
    e->in[i].endproc = e->dyn;
    registration(e->all, e->in[i]);
  }
  for (int i = 0; i < e->out.size(); i++) {
    e->out[i].mother = 0;
    registration(e->all, e->out[i]);
  }
  if (p.beam_test_only) return;

  for (int j = 0; j < e->in.size(); j++) {
    particle p = e->in[j];
    p.endproc = e->dyn;
    registration(e->all, p);
  }

  // e->pr=_nucleus->Zr(); 	// 1. po co to?
  // e->nr=_nucleus->Nr(); 	// 2. powoduje break, segmentation fault

  // copy particle from out to post if coherent interaction

  if (!e->flag.coh) {
    kaskada k(p, *e);
    k.kaskadaevent();  // runs only if p.kaskada_on is true
  } else
  //	if(e->post.size()==0)   // copy out to post if no fsi
  {
    for (int j = 0; j < e->out.size(); j++) {
      particle p = e->out[j];
      p.endproc = e->dyn;
      registration(e->all, p);
      e->post.push_back(p);
    }
  }
}  // end of finishevent

void NuWro::raport(double i, double n, const char *text, int precision, int k,
                   bool toFile) {
  static int prev = -1;
  int proc = precision * i / n;
  if (proc != prev) {
    prev = proc;
    cerr.precision(3);
    if (toFile) {
      _progress.seekp(ios_base::beg);
      _progress << proc * 100.0 / precision << " " << text << '\r' << flush;
      _progress.flush();
    } else {
      if (k >= 0) cerr << "Dyn[" << k << "] ";
      cerr << showpoint << proc * 100.0 / precision << text << '\r' << flush;
    }
    cerr.precision();

    //	   printf("%f3.1 %s\r", proc/10.0,text);
  }
}  // end of report
void NuWro::pot_report(ostream &o) {
  double tot = 0;
  if (_detector) {
    double pd = _detector->nucleons_per_cm2();
    for (int i = 0; i < _procesy.size(); i++)
      if (_procesy.avg(i) > 0) {
        tot += _procesy.avg(i);
        double epp = _procesy.avg(i) * pd * _beam->nu_per_POT();
        o << "dyn[" << i << "] events per POT = " << epp << endl;
        o << "       POT per event = " << 1 / epp << endl;
      }
    double epp = tot * pd * _beam->nu_per_POT();

    o << "Total: events per POT= " << epp << endl
      << "       POT per event = " << 1 / epp << endl
      << endl;
    o << " BOX nuclons per cm2 = " << pd << endl;
    o << "Total cross section  = " << tot << " cm2" << endl;
    o << "Reaction probability = " << tot * pd << endl;
    o << "Average BOX density  = " << _detector->density() / g * cm3 << " g/cm3"
      << endl;
    o << "Estimated BOX mass   = " << _detector->vol_mass() / kg << " kg"
      << endl;
    o << "Fraction of protons  = " << _detector->frac_proton() << endl;
  }
}

//////////////////////////////////////////////////////////////
//              Test events
//////////////////////////////////////////////////////////////
void NuWro::test_events(params &p) {
  if (p.number_of_test_events > 0 && p.beam_test_only == 0) {
    hist hq2((char *)"q2", 0, 2 * GeV2, 100);
    hist hq0((char *)"q0", 0,
             (p.beam_type == 0 ? atof(p.beam_energy.c_str()) * MeV : 2 * GeV),
             100);
    hist hqv((char *)"qv", 0,
             (p.beam_type == 0 ? atof(p.beam_energy.c_str()) * MeV : 2 * GeV),
             100);
    hist hT((char *)"T", 0, 2 * GeV, 100);
    TFile *te = NULL;
    TTree *t1 = NULL;
    event *e = NULL;
    if (p.save_test_events) {
      dismode = false;
      te = new TFile((string("weighted.") + a.output).c_str(), "recreate");
      t1 = new TTree("treeout", "Tree of events");
      e = new event();
      t1->Branch("e", "event", &e);
      delete e;
    }

    refresh_dyn(p);

    int saved = 0;
    for (int i = 0; i < p.number_of_test_events; i++) {
      e = new event();

      e->dyn = _procesy.choose();  // choose dynamics
      if (_mixer) _mixer->prepare(p);
      makeevent(e, p);
      double bias = 1;
      if (dismode && e->dyn > 1 && e->dyn < 6) bias = e->in[0].t;
      _procesy.add(e->dyn, e->weight, bias);
      e->weight /=
          _procesy.ratio(e->dyn);  // make avg(weight)= total cross section
      //~ if(_detector and _beam->nu_per_POT() != 0)
      //~ e->POT=e->weight * _detector->nucleons_per_cm2() /
      //_beam->nu_per_POT();

      if (e->weight > 0) {
        hq2.insert_value(-e->q2(), e->weight * cm2);
        hq0.insert_value(e->q0(), e->weight * cm2);
        hqv.insert_value(e->qv(), e->weight * cm2);
        hT.insert_value(e->in[0].E(), e->weight * cm2);
      } else {
        hq2.insert_value(0, 0);
        hq0.insert_value(0, 0);
        hqv.insert_value(0, 0);
        hT.insert_value(0, 0);
      }
      switch (p.save_test_events) {
        case 0:
          break;
        case 1:
          finishevent(e, p);
          t1->Fill();
          break;
        case 2:
          if (e->weight > 0) {
            saved++;
            e->weight = e->weight * saved / (i + 1);
            finishevent(e, p);
            t1->Fill();
          }
          break;
        default:
          cerr << "Parameter save_test_events=" << p.save_test_events;
          cerr << " out of range. Should be ,1, or 2)" << endl;
          exit(12);
      }
      delete e;
      raport(i + 1, p.number_of_test_events, " % of test events ready...", 1000,
             -1, bool(a.progress));
    }  // end of nuwro loop
    if (p.save_test_events) {
      te->Write();
      te->Close();
    }

    cout << endl;
    _procesy.report();
    _procesy.set_weights_to_avg();
    string prefix;
    //		if(strlen(a.output)>5 &&
    // string(".root")==a.output[strlen(a.output)-5])
    prefix = "";
    //		else
    //			prefix=a.output;
    hq2.plot(prefix + "q2.txt", GeV2, 1e-38 * cm2 / GeV2);
    hq0.plot(prefix + "q0.txt", GeV, 1e-38 * cm2 / GeV);
    hqv.plot(prefix + "qv.txt", GeV, 1e-38 * cm2 / GeV);
    hT.plot(prefix + "T.txt", GeV, 1e-38 * cm2 / GeV);

    ofstream totals((prefix + "totals.txt").c_str(), ios::app);
    totals << p.beam_energy;
    double tot = 0;
    for (int i = 0; i < _procesy.size(); i++) {
      totals << ' ' << _procesy.avg(i);
      tot += _procesy.avg(i);
    }
    totals << endl;
    pot_report(cout);
    if (_detector) {
      ofstream potinfo("POTinfo.txt");
      pot_report(potinfo);
    }
  }
}

void NuWro::user_events(params &p) {
  if (p.number_of_test_events < 1 or p.user_events == 0) return;
  params p1 = p;
  Analyser *A = make_analyser(p);
  if (!A) return;
  for (A->start(); !A->end(); A->step()) {
    refresh_dyn(p);
    for (int i = 0; i < p.number_of_test_events; i++) {
      event *e = new event();

      e->dyn = _procesy.choose();  ///< choose dynamics

      A->prepare_event(*e);
      if (_mixer) _mixer->prepare(p);

      makeevent(e, p);

      A->process_event(*e);

      double bias = 1;
      if (dismode && e->dyn > 1 && e->dyn < 6) bias = e->in[0].t;

      _procesy.add(e->dyn, e->weight, bias);

      delete e;

      raport(i + 1, p.number_of_test_events, " % of analyser events ready...",
             1000, -1, bool(a.progress));
      p = p1;
    }  // end of nuwro loop

    A->partial_report();
    _procesy.report();

  }  // end of analyser loop
  A->final_report();
  delete A;
}

void NuWro::real_events(params &p) {
  dismode = true;
  if (p.number_of_events < 1) return;

  /// calculate desired number of events for each dynamics
  _procesy.calculate_counts(p.number_of_events);
  {  /// Write cross sections and counts to screen and file
    ofstream f((string(a.output) + ".txt").c_str());
    _procesy.short_report(cout);
    _procesy.short_report(f);
  }

  event *e = new event;

  string output = a.output;
  int l = output.length();
  if (l < 5 || string(".root") != output.c_str() + l - 5)
    output = output + ".root";
  TFile *ff = new TFile(output.c_str(), "recreate");
  TTree *tf = new TTree("treeout", "Tree of events");
  tf->Branch("e", "event", &e);
  delete e;
  TH1 *xsections = new TH1D("xsections", "xsections", 8, 0, 7);
  for (int i = 0; i < _procesy.size(); i++) {
    xsections->SetBinContent(i + 1, _procesy.avg(i));
    xsections->SetBinError(i + 1, _procesy.sigma(i));
  }

  /////////////////////////////////////////////////////////////
  // The main loop in NPROC -- generate file with unweighted events
  //////////////////////////////////////////////////////////////

  char filename[230];
  if (p.kaskada_redo == 0)
    for (int k = 0; k < _procesy.size(); k++)
      if (_procesy.desired(k) > 0) {
        sprintf(filename, "%s.%d.part", a.output, k);
        TFile *f1 = new TFile(filename, "recreate");
        TTree *t1 = new TTree("treeout", "Tree of events");

        e = new event();
        t1->Branch("e", "event", &e);
        delete e;

        while (_procesy.ready(k) < _procesy.desired(k)) {
          e = new event();
          e->dyn = k;

          if (_mixer) _mixer->prepare(p);
          makeevent(e, p);
          double bias = 1;
          if (!p.beam_test_only && dismode && k > 1 && k < 6) bias = e->in[0].t;
          if (_procesy.accept(e->dyn, e->weight, bias)) {
            finishevent(e, p);
            e->weight = _procesy.total();
            //~ if(_detector and _beam->nu_per_POT() != 0)
            //~ e->POT=e->weight * _detector->nucleons_per_cm2() /
            //_beam->nu_per_POT();
            t1->Fill();
          }
          delete e;

          raport(_procesy.ready(k), _procesy.desired(k),
                 " % of events ready...", 1000, k, bool(a.progress));
        }
        f1->Write();

        // elimination of spurious events for dynamics k
        // by copying only last desired[k] events to outfile
        if (p.mixed_order == 0) {
          cout << endl;
          int nn = t1->GetEntries();
          int start = nn - _procesy.desired(k);
          for (int jj = start; jj < nn; jj++) {
            e = new event();
            t1->GetEntry(jj);
            tf->Fill();
            delete e;
            raport(jj - start + 1, nn - start, " % events copied...", 100, k,
                   bool(a.progress));
          }
          cout << endl;
        } else
          cout << endl;

        f1->Close();
        delete f1;
        if (p.mixed_order == 0) unlink(filename);
      };
  //////////////////////////////////////////////////////////////////////////////////////
  //                    end of the main loop in NPROC
  //////////////////////////////////////////////////////////////////////////////////////
  if (p.mixed_order) {
    TFile *f[_procesy.size()];
    TTree *t[_procesy.size()];
    int n[_procesy.size()], u[_procesy.size()];
    int ile = 0;
    e = new event();
    for (int k = 0; k < _procesy.size(); k++)
      if ((u[k] = _procesy.desired(k)) > 0) {
        sprintf(filename, "%s.%d.part", a.output, k);
        f[k] = new TFile(filename);
        t[k] = (TTree *)f[k]->Get("treeout");
        if (t[k] == NULL) {
          cerr << "tree \"treeout\" not found in file \"" << filename << "\""
               << endl;
          exit(6);
        }

        t[k]->SetBranchAddress("e", &e);
        n[k] = t[k]->GetEntries();
        ile += u[k];
      }
    int nn = ile;
    while (ile > 0) {
      int i = 0;
      int x = frandom() * ile;
      while (x >= u[i]) x -= u[i++];
      t[i]->GetEntry(n[i] - u[i]);
      tf->Fill();
      ile--;
      u[i]--;
      raport(nn - ile, nn, " % events copied...", 100, i, bool(a.progress));
    }
    delete e;
    for (int k = 0; k < _procesy.size(); k++)
      if (_procesy.desired(k)) {
        f[k]->Close();
        delete f[k];
        sprintf(filename, "%s.%d.part", a.output, k);
        unlink(filename);
      }
  }

  // Save Flux and XSec Histograms
  if (p.OutputEvtHistogram || p.beam_type == 4 || p.beam_type == 5){
    cout << "Saving histogram" << endl;
    ff->cd();
    
    int nFlux = 0;
    vector<TH1D> FluxHist;
    vector<int> PDGList;

    // Get Flux Histograms depending on Beam Type

    // Uniform Flux ________________
    if (p.beam_type == 0){
      
      if (dynamic_cast<beam_uniform const *>(_beam)){
	beam_uniform const &bh = dynamic_cast<beam_uniform const &>(*_beam);
	
	TH1D temp_flux = bh.EProf().GetHist();
	temp_flux.SetName( Form("FluxHist_PDG_%i",bh.GetPDG()) );
	temp_flux.SetDirectory(ff);
	
	// Save Flux
	FluxHist.push_back((TH1D)temp_flux);
	PDGList.push_back( bh.GetPDG() );
	
      } else {
	cout << "[ERROR ] : Uniform flux cast failed!" << endl;
	throw;
      }
    }
    // MIXED Flux ______________________
    else if (p.beam_type == 1){
      
      // Now have a list of histograms to save
      if (dynamic_cast<beam_mixed const *>(_beam)){
	beam_mixed const &bh = dynamic_cast<beam_mixed const &>(*_beam);
	
	if (!bh.GetN()){
	  cout << " ERROR No Beams in Mixed Hist" << endl;
	  exit(30);
	}
	
	for (int i = 0; i < bh.GetN(); i++){
	  TH1D temp_flux = bh.GetBeam(i)->EProf().GetHist();
	  temp_flux.SetName( Form("FluxHist_PDG_%i",bh.GetBeam(i)->GetPDG()) );
	  
	  // Save Flux
	  FluxHist.push_back((TH1D)temp_flux);
	  PDGList.push_back( bh.GetBeam(i)->GetPDG() );
	  
	}
      } else {
	cout << "[ERROR ] : Mixed flux cast failed!" << endl;
	throw;
      }
    // Single ROOT HIST ____________________
    } else if (p.beam_type == 5){

      if (dynamic_cast<beam_singleroothist const *>(_beam)){
	beam_singleroothist const &bh = dynamic_cast<beam_singleroothist const &>(*_beam);

	TH1D temp_flux = bh.EProf().GetHist();
	temp_flux.SetName( Form("FluxHist_PDG_%i",bh.GetPDG()) );
	temp_flux.SetDirectory(ff);

	// Save Flux
	FluxHist.push_back((TH1D)temp_flux);
	PDGList.push_back( bh.GetPDG() );

      } else {
	cout << "[ERROR ] : Singleroothist flux cast failed!" << endl;
	throw;
      }
    // MIXED ROOT HIST __________
    } else if (p.beam_type == 6){

      if (!dynamic_cast<beam_mixedroothist const *>(_beam)){
	cout << "[ERROR ] : Mixed root hist flux cast failed!" << endl;
	throw;
      }
      beam_mixedroothist const &bh = dynamic_cast<beam_mixedroothist const &>(*_beam);

      if (!bh.GetN()){
	cout << " ERROR No Beams in Mixed Hist" << endl;
	exit(30);
      }

      for (int i = 0; i < bh.GetN(); i++){

	TH1D temp_flux = bh.GetBeam(i)->EProf().GetHist();
	temp_flux.SetName( Form("FluxHist_PDG_%i",bh.GetBeam(i)->GetPDG()) );

	// Save Flux
	FluxHist.push_back((TH1D)temp_flux);
	PDGList.push_back( bh.GetBeam(i)->GetPDG() );

      }
    } else {
      cout << "Not saving flux histograms" << endl;
    }

    // Get Total Flux Integral
    nFlux = FluxHist.size();
    double TotalFluxIntegral = 0.0;
    for (int i = 0; i < nFlux; i++){
      TotalFluxIntegral += FluxHist[i].Integral("width");
    }

    // Setup Other Hists
    vector<TH1D> XSecHist;
    vector<TH1D> EvtHist;

    for (int i = 0; i < nFlux; i++){

      // Make Flux Spectrum
      FluxHist[i].Scale(1.0 / TotalFluxIntegral );
      FluxHist[i].SetName(Form("FluxHist_PDG_%i",PDGList[i]));

      // Make XSec Container
      XSecHist.push_back( (TH1D) FluxHist[i] );
      XSecHist[i].SetNameTitle(Form("XSecHist_PDG_%i",PDGList[i]),
			       "XSecHist;E_{#nu} (GeV);#frac{d#sigma}{dE_{#nu}}"
			       "(cm^{2} GeV^{-1} nucleon^{-1})");
      XSecHist[i].Reset();
      XSecHist[i].SetDirectory(ff);

      // Make Event Container
      EvtHist.push_back( (TH1D) FluxHist[i] );
      EvtHist[i].SetNameTitle(Form("EvtHist_PDG_%i",PDGList[i]),
			      "EvtHist;E_{#nu} (GeV);Number of Events");
      EvtHist[i].Reset();
      EvtHist[i].SetDirectory(ff);

    }

    // Event Loop
    e = new event();
    size_t nentrs = tf->GetEntries();
    for (size_t evt_it = 0; evt_it < nentrs; ++evt_it) {
      tf->GetEntry(evt_it);
      
      // Determine PDG
      for (int i = 0; i < nFlux; i++){
	if (e->nu().pdg == PDGList[i]){

	  XSecHist[i].Fill(e->nu().t / 1.E3, e->weight);
	  EvtHist[i].Fill(e->nu().t / 1.E3);
	  break;
	}
      }
    }

    // Apply Scaling to XSec/Event rates
    for (int i = 0; i < FluxHist.size(); i++){
      double AvgXSec = (XSecHist[i].Integral() * 1E38 / EvtHist[i].Integral());
      EvtHist[i].Scale(1.0 / EvtHist[i].Integral());
      EvtHist[i].Scale( FluxHist[i].Integral("width") * AvgXSec, "width" );

      XSecHist[i].Reset();
      XSecHist[i].Add(&EvtHist[i]);
      XSecHist[i].Divide(&FluxHist[i]);
    }

    // Make Totals
    TH1D FluxTotal = FluxHist[0];
    TH1D EvtTotal  = EvtHist[0];
    TH1D XSecTotal = EvtHist[0];
    XSecTotal.GetYaxis()->SetTitle("XSecTotal;E_{#nu} (GeV);#frac{d#sigma}{dE_{#nu}}"
				   "(cm^{2} GeV^{-1} nucleon^{-1})");
    
    // Save
    for (int i = 0 ; i < FluxHist.size(); i++){
      ff->cd();
      FluxHist[i].Write();
      EvtHist[i].Write();
      XSecHist[i].Write();

      if (i > 0){
	FluxTotal.Add(&FluxHist[i]);
	EvtTotal.Add(&EvtHist[i]);
	XSecTotal.Add(&EvtHist[i]); // Add Up Events
      }
    }

    // Make XSec Total
    XSecTotal.Divide(&FluxTotal);
    
    FluxTotal.SetName("FluxHist");
    EvtTotal.SetName("EvtHist");
    XSecTotal.SetName("XSecHist");

    ff->cd();
    FluxTotal.Write();
    EvtTotal.Write();
    XSecTotal.Write();

    // Clean Up
    XSecHist.clear();
    EvtHist.clear();
    PDGList.clear();
    FluxHist.clear();

    cout << " Done Handling Flux " << endl;
  } else {
    cout << "Not saving flux histograms." << endl;
  }
  
      
  ff->Write();
  ff->Close();
  _progress.close();
  delete ff;
  _procesy.report();
  pot_report(cout);
  if (_detector) {
    ofstream potinfo("POTinfo.txt");
    pot_report(potinfo);
  }

  cout << "Output file: \"" << output << "\"" << endl;
}

void NuWro::kaskada_redo(string input, string output) {
  event *e = new event;

  TFile *fi = new TFile(input.c_str());
  TTree *ti = (TTree *)fi->Get("treeout");
  if (ti == NULL) {
    cerr << "tree \"treeout\" not found in file \"" << input << "\"" << endl;
    exit(7);
  }
  ti->SetBranchAddress("e", &e);

  TFile *ff = new TFile(output.c_str(), "recreate");
  TTree *tf = new TTree("treeout", "Tree of events");
  tf->Branch("e", "event", &e);

  int nn = ti->GetEntries();
  for (int i = 0; i < nn; i++) {
    //		e = new event();
    ti->GetEntry(i);
    e->clear_fsi();
    finishevent(e, p);
    tf->Fill();
    //		delete e;
    // if(i%1000==0)
    // cout<<i/1000<<"/"<<nn/1000<<"\r"<<endl;
    raport(i + 1, nn, " % events processed...", 100, e->dyn, bool(a.progress));
  }
  cout << endl;
  fi->Close();
  delete fi;

  ff->Write();
  ff->Close();
  delete e;
  delete ff;
  cout << "Output: \"" << output << "\"" << endl;
}

#include "UserAction.h"

void NuWro::main(int argc, char **argv) {
  try {
    init(argc, argv);
    if (p.kaskada_redo == 1)
      kaskada_redo(a.output, string(a.output) + ".fsi.root");
    else {
      if (not p.beam_test_only) {
        if (p.user_events > 0)
          user_events(p);
        else {
          test_events(p);
          real_events(p);
        }
      }
    }
    genrand_write_state();
  } catch (string s) {
    cout << s << endl;
  } catch (char const *s) {
    cout << s << endl;
  } catch (...) {
    cout << "Nuwro failed" << endl;
  }
}
