////////////////This function calculates RES event
////////////////RES region is defined in the hadronic mass from 1080 to the parameter res_dis_cut introduced in params.txt
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////The following model of the #Sect is postulated
///////// XSect (nu + N -> mu + N' + pi)/dW d omega  = [XSect (nu + N -> mu + N' + pi)/dW d omega ]_Delta * alfadelta (W) +
//////////                                             [XSect (nu + N -> mu + N' + pi)/dW d omega ]_DIS_SPP* SPP(W) * alfados(W) +
//////////                                             [XSect (nu + N -> mu + N' + pi)/dW d omega ]_DIS_nonSPP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// The main idea behind is that DIS contribution simulates non-resonant part
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////// The functions alfa and beta are a priori arbitrary but they have to satisfy the conditions
/////////// alfadis(res_dis_cut)=1, ....alfadelta(res_dis_cut=0,............alfadis (1080)=0
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////alfadis and alfadelta are defined in alfa.cc
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////In principle for each channel the functions should be different. They should be fitted to experimental data
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////Channels are labelled by 4 integers corresponding to: nu/anu   cc/nc   proton/neutron    pi+/pi0/pi-
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "params.h"
#include "pdg_name.h"
#include "LeptonMass.h"
#include "parameters.h"
#include "jednostki.h"
#include "masses.h"
#include "grv94_bodek.h"
#include "dis_cr_sec.h"
#include "delta.h"
#include "fragmentation.h"
#include "vect.h"
#include "charge.h"
#include "event1.h"
#include "alfa.h"
#include "charge.h"
#include "dis2res.h"
#include<TMCParticle.h>
#include<TPythia6.h>
#include "pauli.h"
#include "singlepion.h"

TPythia6 *pythia71 = new TPythia6 ();
extern "C" int pycomp_ (const int *);
extern double SPP[2][2][2][3][40];

double pdd_red (double en)
{
  if (en>=1000)
    return 0.85;
  else if (en>750)
    return 0.65 + en*0.05/250.0;
  else//if (en<=750)
    return 0.2 + en*0.2/250.0;
}

///////////////////////////////////////////////////////////////////
void
resevent2 (params & p, event & e, bool cc)
///////////////////////////////////////////////////////////////////
{
  double M2 = M12 * M12;
  bool current = cc;		//cc==true for charge current 
  int nukleon = e.in[1].pdg;
  int lepton = e.in[0].pdg;

  double m = lepton_mass (abs (lepton), current);	//mass of the produced lepton (see pgd header file)
  double m2 = m * m;

  vect nu0 = e.in[0];
  
  e.weight=0;  // in case of error it is not changed




double _E_bind=0;	//binding energy

double pped=e.in[1].length();
vec ped=e.in[1].p();

int wybor=p.nucleus_target;
int numpro=p.nucleus_p;
int numneu=p.nucleus_n;

	switch(wybor)
	{
	case 0: _E_bind=0;//free
	break;
	case 1: _E_bind= p.nucleus_E_b;//FG
	break;
	case 2: _E_bind= 0;//local FG
	break;
	case 3: _E_bind=0;//Bodek
	break;
	case 4: _E_bind = binen (ped, numpro, numneu);//effective SF
	break;
	case 5: _E_bind= deuter_binen (ped);//deuterium 
	break;
	case 6: _E_bind= p.nucleus_E_b; //deuterium like Fermi gas
	break;
	
//in the future it is possible to add SF for other nuclei as well
	default: _E_bind=0;
	}


//_E_bind=N0.bin();


//if (p.target_FG)
//_E_bind= p.target_E_b;
vect nuc0=e.in[1];

nuc0.t-=_E_bind;
//subtract bing energy from nucleon energy insize nucleus
nu0.boost (-nuc0.v ());
//boost to the bound nucleon rest frame
  vec numom = vec (nu0.x, nu0.y, nu0.z);

  double cut = p.res_dis_cut;

int FFset = p.delta_FF_set;
double delta_axial_mass = p.pion_axial_mass;
double delta_C5A = p.pion_C5A;

 // target jadro (p);

  double E = nu0.t;
//neutrino energy on the new frame


double Mefff = sqrt(nuc0 * nuc0);

double Meff = min(Mefff, M12);

//effective mass works well
//cout<<"Meff=  "<<Meff<<"   ";

double Meff2=Meff*Meff;
  if (E < ((1080 + m) * (1080 + m) - Meff2) / 2 / Meff)
    {
      e.weight = 0;
      return;
    }
//threshold energy for pion production
  else
    {
/////////////////////////////////////////////////////////////
//      Selection of points in W, nu plane
/////////////////////////////////////////////////////////////



      double Wmax = min (cut, sqrt (Meff2 + 2 * Meff * E) - m);

      double W = 1080 + (Wmax - 1080) * frandom ();

      double W2 = W * W;
      double E2 = E * E;

      double wminus =
	((Meff + E) * (W2 - Meff2 - m2) + 2 * Meff * E2 -
	 E * sqrt (kwad (W2 - Meff2 - m2 - 2 * Meff * E) -
		   4 * m2 * Meff * (Meff + 2 * E))) / 2 / Meff / (Meff + 2 * E);

      double wplus =
	((Meff + E) * (W2 - Meff2 - m2) + 2 * Meff * E2 +
	 E * sqrt (kwad (W2 - Meff2 - m2 - 2 * Meff * E) -
		   4 * m2 * Meff * (Meff + 2 * E))) / 2 / Meff / (Meff + 2 * E);

      double numin = max (wminus, m);
      double numax = min (wplus, E - m);
      double z=frandom ();
      double nu = numin + (numax - numin) * z*z ;	// enhance low energy transfers are preferred

      double przedzial = (numax - numin) * (Wmax - 1080) *2*z; // but compesated by this jakobian

//cout<<W<<"   "<<nu<<endl;

///////////////////////////////////////////////////////////////
//      End of selection of points in W, nu plane
/////////////////////////////////////////////////////////////

      double fromdis = cr_sec_dis (E, W, nu, lepton, nukleon, current);
//cout<<"fromdis"<<fromdis<<endl;
      if (fromdis < 0)
	fromdis = 0;
//cout<<"fromdis=  "<<fromdis<<endl;
      double q = sqrt (kwad (Meff + nu) - W2);//momentum transfer
      double kprim = sqrt (kwad (E - nu) - m2);//final lepton
      double cth = (E2 + kprim * kprim - q * q) / 2 / E / kprim;//final lepton

      vec kkprim;		//the unit vector in the direction of scattered lepton
      if(abs(cth)>1) 
		return; // e.weight=0 already
      kinfinder (numom, kkprim, cth);	//produces kkprim 

      kkprim = kprim * kkprim;	//multiplied by its length

      vect lepton_out = vect (E - nu, kkprim.x, kkprim.y, kkprim.z);

      vec momtran = numom - kkprim;

      vec hadrspeed = momtran / sqrt (W2 + q * q);//parameter of boost to hadronic rest frame

      vect par[100];
      double ks[100];		//int czy double ???

      par[0] = lepton_out;
//powrot do ukladu spoczywajacej tarczy
      par[0] = par[0].boost (nuc0.v ());	//ok

      particle lept (par[0]);

      if (current == true && lepton > 0)
	{
	  lept.pdg = lepton - 1;
	}
      if (current == true && lepton < 0)
	{
	  lept.pdg = lepton + 1;
	}
      if (current == false)
	{
	  lept.pdg = lepton;
	}

      e.out.push_back (lept);	//final lepton; ok

      int j, k, l, t;

      if (lepton > 0)		//the first part of the translation of the event into "spp language"
	j = 0;
      else
	{
	  j = 1;
	}

      if (current)
	k = 0;
      else
	{
	  k = 1;
	}

      if (nukleon == 2212)
	l = 0;
      else
	{
	  l = 1;
	}

      int pion;

      int finalcharge = charge (nukleon) + (1 - k) * (1 - 2 * j);	//total electric charge of the pion-nucleon system

      if (W < 1210 || fromdis==0)		//PYTHIA does not work in this region and special treatment is required
	{
	  double spp0 = SPP[j][k][l][0][0];
	  double spp1 = SPP[j][k][l][1][0];
	  double spp2 = SPP[j][k][l][2][0];

	  double dis0 = fromdis * spp0 * alfadis (j, k, l, 0, W);
	  double dis1 = fromdis * spp1 * alfadis (j, k, l, 1, W);
	  double dis2 = fromdis * spp2 * alfadis (j, k, l, 2, W);	//can be made simpler !!!

	  double delta0=0, delta1=0, delta2=0;

	  double adel0 = alfadelta (j, k, l, 0, W);
	  double adel1 = alfadelta (j, k, l, 1, W);
	  double adel2 = alfadelta (j, k, l, 2, W);

	  if (finalcharge == 2)
	    {
	      delta0 =
		cr_sec_delta (FFset, delta_axial_mass, delta_C5A, E, W, nu, lepton, nukleon, 2212, 211,
			      current) * adel0;
	      delta1 = delta2 = 0;
	    }

	  if (finalcharge == 1)
	    {
	      delta0 =
		cr_sec_delta (FFset, delta_axial_mass, delta_C5A, E, W, nu, lepton, nukleon, 2112, 211,
			      current) * adel0;
	      delta1 =
		cr_sec_delta (FFset, delta_axial_mass, delta_C5A, E, W, nu, lepton, nukleon, 2212, 111,
			      current) * adel1;
	      delta2 = 0;
	    }

	  if (finalcharge == 0)
	    {
	      delta1 =
		cr_sec_delta (FFset, delta_axial_mass, delta_C5A, E, W, nu, lepton, nukleon, 2112, 111,
			      current) * adel1;
	      delta2 =
		cr_sec_delta (FFset, delta_axial_mass, delta_C5A, E, W, nu, lepton, nukleon, 2212, -211,
			      current) * adel2;
	      delta0 = 0;
	    }

	  if (finalcharge == -1)
	    {
	      delta2 =
		cr_sec_delta (FFset, delta_axial_mass, delta_C5A, E, W, nu, lepton, nukleon, 2112, -211,
			      current) * adel2;
	      delta0 = delta1 = 0;
	    }

//we arrived at the overall strength !!!
	  double wsumie = dis0 + dis1 + dis2 + delta0 + delta1 + delta2;

	  double reldis0 = dis0 / wsumie;
	  double reldis1 = dis1 / wsumie;
	  double reldis2 = dis2 / wsumie;
	  double reldelta0 = delta0 / wsumie;
	  double reldelta1 = delta1 / wsumie;
	  double reldelta2 = delta2 / wsumie;

//cout<<" "<<W<<" "<<nu<<endl;
	  e.weight = wsumie * 1e-38 * przedzial;

	  int channel;
	  double los = frandom ();
	  if ((reldelta0 + reldis0) > los)
	    channel = 0;
	  else
	    {
	      if ((reldelta0 + reldelta1 + reldis0 + reldis1) > los)
		channel = 1;
	      else
		channel = 2;
	    }

	  if (channel == 0)
	    pion = 211;
	  if (channel == 1)
	    pion = 111;
	  if (channel == 2)
	    pion = -211;

	  int nukleon2 = nukleon_out_ (W, lepton, nukleon, pion, current);	//which nucleon in the final state

	  vect finnuk, finpion;

	  kin2part (W, nukleon2, pion, finnuk, finpion);	//produces 4-momenta of final pair: nucleon + pion
	  
	  finnuk = finnuk.boost (hadrspeed);
	  finnuk = finnuk.boost (nuc0.v ());

	  finpion = finpion.boost (hadrspeed);
	  finpion = finpion.boost (nuc0.v ());

	  particle ppion (finpion);
	  particle nnukleon (finnuk);

	  ppion.pdg = pion;
	  nnukleon.pdg = nukleon2;

	  e.out.push_back (ppion);
	  e.out.push_back (nnukleon);
	}
//end of W<1210 ||fromdis==0

      else			//the algorithm starts from the production of PYTHIA event
	{


////////////////////////////////////////////////////
//      Setting Pythia parameters
//      Done by Jaroslaw Nowak
//////////////////////////////////////////////

//stabilne pi0
	  pythia71->SetMDCY (pycomp_ (&pizero), 1, 0);

	  pythia71->SetMSTU (20, 1);	//advirsory warning for unphysical flavour switch off
	  pythia71->SetMSTU (23, 1);	//It sets counter of errors at 0
	  pythia71->SetMSTU (26, 0);	//no warnings printed 

	  // PARJ(32)(D=1GeV) is, with quark masses added, used to define the minimum allowable enrgy of a colour singlet parton system
	  pythia71->SetPARJ (33, 0.1);

	  // PARJ(33)-PARJ(34)(D=0.8GeV, 1.5GeV) are, with quark masses added, used to define the remaining energy below which 
	  //the fragmentation of a parton system is stopped and two final hadrons formed.
	  pythia71->SetPARJ (34, 0.5);
	  pythia71->SetPARJ (35, 1.0);

	  //PARJ(36) (D=2.0GeV) represents the dependence of the mass of final quark pair for defining the stopping point of the 
	  //fragmentation. Strongly corlated with PARJ(33-35)

	  pythia71->SetPARJ (37, 1.);	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

	  //MSTJ(17) (D=2) number of attemps made to find two hadrons that have a combined mass below the cluster mass and thus allow 
	  // a cluster to decay rather than collaps
	  pythia71->SetMSTJ (18, 3);	//do not change

/////////////////////////////////////////////////
//              End of setting Pythia parameters
////////////////////////////////////////////////

	  int nParticle = 0;
	  int nCharged = 0;
	  int NPar = 0;
	  Pyjets_t *pythiaParticle;	//deklaracja event recordu
	  double W1 = W / GeV;	//W1 w GeV-ach potrzebne do Pythii

	  while (NPar < 5)
	    {
	      hadronization (E, W, nu, m, lepton, nukleon, current);
	      pythiaParticle = pythia71->GetPyjets ();
	      NPar = pythia71->GetN ();
	    }
//cout<<NPar<<endl;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////There are three different possible outcomes:
///////////     a) spp event NPar=5
///////////     b) more inelastic event NPar>5, typically 7
///////////     c) single kaon production; this causes technical complications because NPar=5 also in this case
//////////////////////////////////////////////////////////////////////////////////////////////////////////

	  if (NPar == 5 && (pythiaParticle->K[1][3] == 211 || pythiaParticle->K[1][4] == 211 || pythiaParticle->K[1][3] == 111 || pythiaParticle->K[1][4] == 111 || pythiaParticle->K[1][3] == -211 || pythiaParticle->K[1][4] == -211))	//spp condition
	    {

	      if (pythiaParticle->K[1][3] == 211 || pythiaParticle->K[1][4] == 211)	//the second part
		{
		  t = 0;
		  pion = 211;
		}

	      if (pythiaParticle->K[1][3] == 111
		  || pythiaParticle->K[1][4] == 111)
		{
		  t = 1;
		  pion = 111;
		}

	      if (pythiaParticle->K[1][3] == -211
		  || pythiaParticle->K[1][4] == -211)
		{
		  t = 2;
		  pion = -211;
		}
//cout<<pythiaParticle->K[1][3]<<" "<< pythiaParticle->K[1][4]<<endl;
	      double dis_spp = fromdis * alfadis (j, k, l, t, W);	//dis contribution

	      int nukleoncharge = finalcharge + t - 1;	//the charge of final nucleon

	      int nukleon2;

	      if (nukleoncharge == 1)
		{
		  nukleon2 = 2212;
		}
	      else
		{
		  nukleon2 = 2112;
		}

//delta contribution
//if (SPPF (j,k,l,t,W)<0.01)
//cout<<SPPF (j,k,l,t,W)<<" "<<j<<" "<<k<<" "<<l<<" "<<t<<" "<<W<<" "<<endl;

	      double delta_spp =
		cr_sec_delta (FFset, delta_axial_mass, delta_C5A, E, W, nu, lepton, nukleon, nukleon2,
			      pion, current) / SPPF (j, k, l, t,
						     W) * alfadelta (j, k, l,
								     t, W);
//cout<<delta_spp<<endl;

//approximate implementation of pionless delta decays
	if ( (p.nucleus_p + p.nucleus_n)>7 )
	{//cout<<delta_spp<<"  ";
	  //double ennergy = e.in[0].t;
	  //double rescale = pdd_red (ennergy);
	  delta_spp*=pdd_red(e.in[0].t );
	  //cout<<ennergy<<"  "<<rescale<<"  "<<delta_spp<<endl;
	}
//approximate implementation of pionless delta decays	
	  
	      double spp_strength = dis_spp + delta_spp;
//cout<<"spp "<<W<<" "<<nu<<" "<<spp_strength<<endl;
	      e.weight = spp_strength * 1e-38 * przedzial;

	      double reldis = dis_spp / spp_strength;
	      double reldelta = delta_spp / spp_strength;

	      double los = frandom ();

	      if (reldis > los)	//disevent
		{
		  for (int i = 0; i < NPar; i++)
		    {
		      par[i].t = pythiaParticle->P[3][i] * GeV;
		      par[i].x = pythiaParticle->P[0][i] * GeV;
		      par[i].y = pythiaParticle->P[1][i] * GeV;
		      par[i].z = pythiaParticle->P[2][i] * GeV;
		      rotation (par[i], momtran);
		      ks[i] = pythiaParticle->K[0][i];

		      par[i] = par[i].boost (hadrspeed);	//correct direction ???
		      par[i] = par[i].boost (nuc0.v ());
		      particle part (par[i]);

		      part.ks = pythiaParticle->K[0][i];
		      part.pdg = pythiaParticle->K[1][i];
		      part.orgin = pythiaParticle->K[2][i];

		      e.temp.push_back (part);
		      if (ks[i] == 1)	//condition for a real particle in the final state
			{
			  e.out.push_back (part);
			}
		    }
		}
	      else		//deltaevent
		{
		  vect finnuk, finpion;

		  
		  nu0.boost (-hadrspeed);//a boost from nu-N CMS to the hadronic CMS
		  lepton_out.boost (-hadrspeed);//a boost from nu-N CMS to the hadronic CMS
		  kin4part (nu0, lepton_out, W, nukleon2, pion, finnuk, finpion,p.delta_angular); //produces 4-momenta of final pair: nucleon + pion with density matrix information
		  e.weight*= angrew;//reweight according to angular correlation
		 
		  //kin2part (W, nukleon2, pion, finnuk, finpion);	//produces 4-momenta of the final pair: nucleon + pion
		  
			nu0.boost (hadrspeed);//a boost back to the nu-N CMS frame
			nu0.boost (nuc0.v ());//a boost back to tha LAB frame
			
		  lepton_out.boost (hadrspeed);//a boost back to the nu-N CMS frame
		  lepton_out.boost (nuc0.v ());//a boost back to tha LAB frame

		  finnuk = finnuk.boost (hadrspeed);
		  finnuk = finnuk.boost (nuc0.v ());

		  finpion = finpion.boost (hadrspeed);
		  finpion = finpion.boost (nuc0.v ());

		  particle ppion (finpion);
		  particle nnukleon (finnuk);

		  ppion.pdg = pion;
		  nnukleon.pdg = nukleon2;

		  e.out.push_back (ppion);
		  e.out.push_back (nnukleon);

		}


	    }
//end of spp case

	  else			//more inelastic final state or single kaon production

	    {			//cout<<"inel "<<W<<" "<<nu<<endl;
	      e.weight = fromdis * 1e-38 * przedzial;

	      for (int i = 0; i < NPar; i++)
		{
		  par[i].t = pythiaParticle->P[3][i] * GeV;
		  par[i].x = pythiaParticle->P[0][i] * GeV;
		  par[i].y = pythiaParticle->P[1][i] * GeV;
		  par[i].z = pythiaParticle->P[2][i] * GeV;
		  rotation (par[i], momtran);
		  ks[i] = pythiaParticle->K[0][i];

		  par[i] = par[i].boost (hadrspeed);	//correct direction ???
		  par[i] = par[i].boost (nuc0.v());
		  particle part (par[i]);

		  part.ks = pythiaParticle->K[0][i];
		  part.pdg = pythiaParticle->K[1][i];
		  part.orgin = pythiaParticle->K[2][i];

		  e.temp.push_back (part);
		  if (ks[i] == 1)	//condition for a real particle in the final state
		    {
		      e.out.push_back (part);
		    }
		}
	    }
//end of more inelastic part or single kaon production

	
}//end of W>1210 &&  !fromdis==0

 }
// E above threshold
	for(int j=0;j<e.out.size();j++)
		e.out[j].r=e.in[1].r;

}

//the end of main
