#include "dirs.h"
#include <cstdlib>
#include <cstring>
#include <fstream>

static string data_dir;
static string bin_dir;

string get_data_dir() { return data_dir; }

string get_bin_dir() { return bin_dir; }

void set_dirs(char *exename) {
  int n = strlen(exename);
  do {
    --n;
  } while (n >= 0 && exename[n] != '/');
  if (exename[n] == '/'){  // path was given
    exename[n] = 0;
    bin_dir = string(exename) + "/";
    exename[n] = '/';
  } else {  // no path given - exemine PATH try to find which exename was used
    char *p = getenv("PATH");
    if (p) {
      while (*p) {
        char *q = p;
        while (*q && *q != ':') q++;
        if (*q) *q++ = 0;
        if (ifstream((string(p) + "/" + exename).c_str())) {
          bin_dir = string(p) + "/";
        }
        p = q;
      }
    }
  }
  // try to use the environment variable "NUWRO"
  char *nuwro_dir = getenv("NUWRO");
  if (nuwro_dir) {
  	std::string ndir = nuwro_dir;
    data_dir = ndir + (ndir[(ndir.length()-1)]=='/'?"":"/") + "data/";
  }
}

bool open_data_file(ifstream &incfile, string name) {
  incfile.open(name.c_str());
  if (incfile.is_open()) return true;
  incfile.open((data_dir + name).c_str());
  if (incfile.is_open())
    return true;
  else
    return false;
}
