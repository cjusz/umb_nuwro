//C
#include <cstdio>
#include <cstdlib>

//C++
#include <iostream>
#include <vector>
#include <algorithm>

//Linux
#include <sys/stat.h>

//POSIX
#include <unistd.h>

//ROOT
#include "event1.h"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TBits.h"

//Nuwro
#include "pdg.h"
#include "generatormt.h"
#include "RooTrackerEvent.h"

using namespace RooTrackerUtils;

bool FileExists(std::string strFilename) {
  struct stat stFileInfo;
  int intStat;
  // Attempt to get the file attributes
  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    // We were able to get the file attributes
    // so the file obviously exists.
    return true;
  } else {
    // We were not able to get the file attributes.
    // This may mean that we don't have permission to
    // access the folder which contains this file. If you
    // need to do that level of checking, lookup the
    // return values of stat which will give you
    // more details on why stat failed.
    return false;
  }
}

void DisplayInfo(){
  std::cout << std::endl;
  std::cout << "Usage: nuwro2rootracker [-x extended] [-n #evts] [-p POT] nuwrofile.root rootrackerfile_template.root" << std::endl;
  std::cout << "Converts nuwro output file to a series of rootracker tree format files." << std::endl;
  std::cout << "Extended mode splits up struck nucleon and nuclear target entries and also writes out pre-FSI info." << std::endl;
  std::cout << "The @ character in the template will be substituted with file number." << std::endl;
  std::cout << "#evts is the number of events per one output file" << std::endl;
  std::cout << "(must be less than the number of evts in the input file)." << std::endl;
  std::cout << "If #evts not specified, one output file will be created." << std::endl;
  std::cout << "POT is the number of POTs in the input file." << std::endl;
  exit(0);
}

int main (int argc, char *argv[]){

  Long64_t nEntriesToCopy = -1;
  bool onefile = true;
  float pot = 1;
  bool usepot = false;
  bool extended_mode = false;

  int c;
  while( (c = getopt(argc, argv, "xn:p:h?") ) != -1){
    switch(c){
      case 'n': {
        if(optarg && sscanf(optarg, "%lli", &nEntriesToCopy) >0 ){
          onefile=false;
          std::cout << "[INFO]: Each file will have " << nEntriesToCopy
            << " evts." << std::endl;
        }else{
          std::cerr << "[ERROR]: Number of events missing or unreadable!"
            << std::endl;
          return 1;
        }
        break;
      }
      case 'p': {
        if (optarg && sscanf(optarg,"%e",&pot) > 0 ){
          usepot = true;
          std::cout << "[INFO]: POT information will be added to output "
            "(total POT " << pot << ")." << std::endl;
        }else{
          std::cerr << "[ERROR]: Number of POTs missing or unreadeable!"
            << std::endl;
          return 2;
        }
        break;
      }
      case 'x': {
        std::cout << "[INFO]: Running in extended output mode. "
          "Will output event::out vector to StdHep arrays" << std::endl;
        extended_mode = true;
        break;
      }
      case 'h':
      case '?': {
        DisplayInfo();
        return 0;
      }
    }
  }

  if( (argc - optind) < 2){
    std::cerr << "[ERROR]: Insufficient number of parameters!" << std::endl;
    return 4;
  }
  if(!FileExists(argv[optind])){
    std::cerr << "[ERROR]: File \"" << argv[optind]
      << "\" does not exist or is inaccessible." << std::endl;
    return 8;
  }
  TFile *fin = TFile::Open(argv[optind]);
  if(!fin || !fin->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open file \"" << argv[optind]
      << "\" for reading." << std::endl;
    return 16;
  }

  TTree *nwTree = static_cast<TTree*>(fin->Get("treeout"));
  event *nwEv = 0; // ROOT is in control of object life cycle
  nwTree->SetBranchAddress("e", &nwEv);

  Long64_t nEntries = nwTree->GetEntries();
  if(nEntriesToCopy == -1){ nEntriesToCopy = nEntries; }

  std::cout << "[INFO]: " << nEntries << " entries in the input file."
    << std::endl;

  double coef = 1e38/nEntries;
  int i0 = 0;

  std::stringstream ss("");
  Long64_t NCopiedEvts = 0;
  int i = 0, fnum = 0;
  std::string outtemplate = argv[(optind+1)];
  int index = outtemplate.find("@");
  if (!onefile && (index == std::string::npos)){
    std::cerr << "[ERROR]: No @ character in the output template." << std::endl;
    return 32;
  }

  //Don't need to increment, inner loop does that
  for(Long64_t ent = 0; ent < nEntries;){
    std::string fname = outtemplate;
    char buf[5];
    sprintf(buf,"%d",fnum);
    if(!onefile){ fname.replace(index,1,buf); }

    TFile *fout = TFile::Open(fname.c_str(),"RECREATE");
    fnum++;

    TTree *fOutputTree = new TTree("nRooTracker","RooTracker");

    //Add relevant branches to TTree for writing out.
    RooTrackerEvent rtEv;
    rtEv.JackIn(fOutputTree,false);

    Long64_t NEvsInFile = 0;
    //Increment until we have filled up another file or until we hit the end.
    for(; ent < std::min(NCopiedEvts+nEntriesToCopy,nEntries); ++ent){

      nwTree->GetEntry(ent);

      if(!(nwEv->weight > 0)){ continue; }

      NEvsInFile++;

      rtEv.fEvtFlags->Set(8,"00000000");
      ss << GetNeutReactionCodeFromNuwroEvent1(*nwEv);
      rtEv.fEvtCode->SetString(ss.str().c_str());
      ss.str("");
      rtEv.fEvtNum = ent;
      rtEv.fEvtXSec = nwEv->weight*coef;
      rtEv.fEvtDXSec = 0;
      rtEv.fEvtWght = nwEv->weight*1e38;
      rtEv.fEvtProb = 0;
      rtEv.fEvtVtx[0] = nwEv->r.x/1000.0;
      rtEv.fEvtVtx[1] = nwEv->r.y/1000.0;
      rtEv.fEvtVtx[2] = nwEv->r.z/1000.0;
      rtEv.fEvtVtx[3] = 0;

      rtEv.fStdHepN = 0;

      //incoming particles
      int NucleusPdg = 1e9 + (nwEv->par.nucleus_p*1e4)
        + (nwEv->par.nucleus_p+nwEv->par.nucleus_n)*10;

      for(size_t nin = 0; nin < nwEv->in.size(); ++nin){
        particle &in_part = nwEv->in[nin];

        //struck nucleon
        if(in_part.pdg == 2112 || in_part.pdg == 2212){ //neutron or proton -> nucleus
          if (nwEv->par.nucleus_p == 1 && nwEv->par.nucleus_n == 0){ //hydrogen
            rtEv.fStdHepPdg[rtEv.fStdHepN] = in_part.pdg; //proton code
          }else{
            if(extended_mode){
              rtEv.fStdHepPdg[rtEv.fStdHepN] = NucleusPdg;
              rtEv.fStdHepStatus[rtEv.fStdHepN] = 0;
              rtEv.fStdHepP4[rtEv.fStdHepN][3] = 0;
              rtEv.fStdHepP4[rtEv.fStdHepN][0] = 0;
              rtEv.fStdHepP4[rtEv.fStdHepN][1] = 0;
              rtEv.fStdHepP4[rtEv.fStdHepN][2] = 0;
              rtEv.fStdHepN++; // fill out the rest of this entry as usual

              rtEv.fStdHepPdg[rtEv.fStdHepN] = in_part.pdg;
            } else { // Merge the struck nucleon and nuclear target entries
              rtEv.fStdHepPdg[rtEv.fStdHepN] = NucleusPdg; //nucleus code
            }
          }
        //else assume neutrino (gief ewro)
        }else{
          rtEv.fStdHepPdg[rtEv.fStdHepN] = in_part.pdg; //neutrino code
        }

        rtEv.fStdHepStatus[rtEv.fStdHepN] = 0; //incoming status
        for(int k = 0; k < 4; k++){
          rtEv.fStdHepX4[rtEv.fStdHepN][k] = 0;
        }
        for(int k = 0; k < 3; k++){
          rtEv.fStdHepPolz[rtEv.fStdHepN][k] = 0;
        }

        rtEv.fStdHepP4[rtEv.fStdHepN][3] = in_part.E()/1000.0;
        rtEv.fStdHepP4[rtEv.fStdHepN][0] = in_part.x/1000.0;
        rtEv.fStdHepP4[rtEv.fStdHepN][1] = in_part.y/1000.0;
        rtEv.fStdHepP4[rtEv.fStdHepN][2] = in_part.z/1000.0;
        rtEv.fStdHepFd[rtEv.fStdHepN] = 0;
        rtEv.fStdHepLd[rtEv.fStdHepN] = 0;
        rtEv.fStdHepFm[rtEv.fStdHepN] = 0;
        rtEv.fStdHepLm[rtEv.fStdHepN] = 0;
        rtEv.fStdHepN++;
      } // end incoming particles

      //Coherent channel - Add target which is not present in the nuwro file
      if(nwEv->in.size() < 2){
        if( (nwEv->par.nucleus_p==1) && (nwEv->par.nucleus_n==0) ){ //hydrogen
          rtEv.fStdHepPdg[rtEv.fStdHepN] = 2212; //proton code
        } else {
          rtEv.fStdHepPdg[rtEv.fStdHepN] = NucleusPdg; //nucleus code
        }
        rtEv.fStdHepStatus[rtEv.fStdHepN] = 0;//incoming
        for(int k = 0; k < 4; k++){
          rtEv.fStdHepX4[rtEv.fStdHepN][k] = 0;
        }
        for(int k = 0; k < 3; k++){
          rtEv.fStdHepPolz[rtEv.fStdHepN][k] = 0;
        }
        rtEv.fStdHepP4[rtEv.fStdHepN][3] = 0;
        rtEv.fStdHepP4[rtEv.fStdHepN][0] = 0;
        rtEv.fStdHepP4[rtEv.fStdHepN][1] = 0;
        rtEv.fStdHepP4[rtEv.fStdHepN][2] = 0;
        rtEv.fStdHepFd[rtEv.fStdHepN] = 0;
        rtEv.fStdHepLd[rtEv.fStdHepN] = 0;
        rtEv.fStdHepFm[rtEv.fStdHepN] = 0;
        rtEv.fStdHepLm[rtEv.fStdHepN] = 0;
        rtEv.fStdHepN++;
      }

      if(extended_mode){
        //Out particles (particles leaving the vertex, preFSI)
        for (int nout = 0; nout < nwEv->out.size(); nout++){
          particle &out_part = nwEv->out[nout];
          rtEv.fStdHepPdg[rtEv.fStdHepN] = out_part.pdg;
          rtEv.fStdHepStatus[rtEv.fStdHepN] = 14; //Hadron in the nucleus
          for(int k = 0; k < 4; k++){
            rtEv.fStdHepX4[rtEv.fStdHepN][k] = 0;
          }
          for(int k = 0; k < 3; k++){
            rtEv.fStdHepPolz[rtEv.fStdHepN][k] = 0;
          }
          rtEv.fStdHepP4[rtEv.fStdHepN][3] = out_part.E()/1000.0;
          rtEv.fStdHepP4[rtEv.fStdHepN][0] = out_part.x/1000.0;
          rtEv.fStdHepP4[rtEv.fStdHepN][1] = out_part.y/1000.0;
          rtEv.fStdHepP4[rtEv.fStdHepN][2] = out_part.z/1000.0;
          rtEv.fStdHepFd[rtEv.fStdHepN] = 0;
          rtEv.fStdHepLd[rtEv.fStdHepN] = 0;
          rtEv.fStdHepFm[rtEv.fStdHepN] = 0;
          rtEv.fStdHepLm[rtEv.fStdHepN] = 0;
          rtEv.fStdHepN++;
        }
      }

      //Nucleus leaving particles
      for (int npost = 0; npost < nwEv->post.size();npost++){
        particle &post_part = nwEv->post[npost];
        rtEv.fStdHepPdg[rtEv.fStdHepN] = post_part.pdg;
        rtEv.fStdHepStatus[rtEv.fStdHepN] = 1; //Final stat
        for(int k = 0; k < 4; k++){
          rtEv.fStdHepX4[rtEv.fStdHepN][k] = 0;
        }
        for(int k = 0; k < 3; k++){
          rtEv.fStdHepPolz[rtEv.fStdHepN][k] = 0;
        }
        rtEv.fStdHepP4[rtEv.fStdHepN][3] = post_part.E()/1000.0;
        rtEv.fStdHepP4[rtEv.fStdHepN][0] = post_part.x/1000.0;
        rtEv.fStdHepP4[rtEv.fStdHepN][1] = post_part.y/1000.0;
        rtEv.fStdHepP4[rtEv.fStdHepN][2] = post_part.z/1000.0;
        rtEv.fStdHepFd[rtEv.fStdHepN] = 0;
        rtEv.fStdHepLd[rtEv.fStdHepN] = 0;
        rtEv.fStdHepFm[rtEv.fStdHepN] = 0;
        rtEv.fStdHepLm[rtEv.fStdHepN] = 0;
        rtEv.fStdHepN++;
      }

      rtEv.fNuParentPdg = 0;
      rtEv.fNuParentDecMode = 11;

      //Where do these come from
      rtEv.fNuParentDecP4[0] = 0.;
      rtEv.fNuParentDecP4[1] = -0.410;
      rtEv.fNuParentDecP4[2] = 6.630;
      rtEv.fNuParentDecP4[3] = 6.644;

      rtEv.fNuParentProP4[0] = 0.202;
      rtEv.fNuParentProP4[1] = -0.227;
      rtEv.fNuParentProP4[2] = 6.738;
      rtEv.fNuParentProP4[3] = 6.747;

      rtEv.fNuParentDecX4[0] = 3.361;
      rtEv.fNuParentDecX4[1] = -212.127;
      rtEv.fNuParentDecX4[2] = 3519.273;
      rtEv.fNuParentDecX4[3] = 0.;

      rtEv.fNuParentProX4[0] = 0.935;
      rtEv.fNuParentProX4[1] = 31.355;
      rtEv.fNuParentProX4[2] = -459.387;
      rtEv.fNuParentProX4[3] = 0;

      rtEv.fNuParentProNVtx= 1 ;

      fOutputTree->Fill();

      std::cout << "\r" << int((ent+1)*100/nEntries) << "\% done."
        << std::flush;
    }

    NCopiedEvts += NEvsInFile;

    double filepot = pot * NEvsInFile/double(nEntries);
    if (usepot){
      fOutputTree->SetWeight(filepot);
    }

    fout->Write();
    fout->Close();

    std::cout << std::endl << "[INFO]: Wrote Output file: "
      << fname.c_str() << " with " << NCopiedEvts << " events, currently at "
      << ent << "th event in the input file." << std::endl;
    if (usepot){
      printf("POT for this file: %e\n",filepot);
    }
  }

  fin->Close();
  delete fin;
  std::cout << "[INFO]: " << NCopiedEvts << " events copied. " << std::endl;
}
