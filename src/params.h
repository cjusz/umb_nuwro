#ifndef _params_h_
#define _params_h_
#include <string>
#include <string.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>

#include "vec.h"
#include "dirs.h"

//#include "params_all_orig.h" // original params (hand written)
#include "params_all.h" // auto_generated (from params.xml)

using namespace std;

struct line: public std::string {
  line (){}
  line (const std::string & s) : std::string (s){}
  line & operator= (const std::string & s){
    *(std::string *) this = s;
    return *this;
  }
};

template < class T >
inline bool read (T & x, istream & s, char z){
  s >> x;
  return (z == '=') && s; // '+=' allowed only for lines
}


template < class T >
inline void write (T x, std::ostream & s, char const * name=""){
  s << ' ' << x; // space looks nice ofter in "a = 1"
}


inline
bool read (vec & a, istream & s, char z){
  s >> a.x >> a.y >> a.z;
  return (z == '=') && s; // '+=' allowed only for lines
}


inline void
write (vec a, std::ostream & s, char const * name=""){
  s << ' ' << a.x << ' ' << a.y << ' ' << a.z;
}

inline
bool read (std::string & a, istream & s, char z){
  s >> a;
  if (!s)
    a = "";
  return true;
}

inline
bool read (line & x, istream & s, char z){
  switch (z){
    case '=': {
      getline (s, x);
      return true;
    }
    case '+': {
      std::string y;
      getline (s, y);
      x += "\n" + y;
      return true;
    }
  }
  return false;
}

inline
void write (line a, std::ostream & s, char const * name=""){
  std::string printstr = a;
  size_t nlineloc = printstr.find_first_of('\n');
  while(nlineloc!=std::string::npos){ // Add back in `VarName +=` when writing
                                      // multiline variables
    printstr = printstr.replace(nlineloc,nlineloc+1,
      std::string("\n")+name+" += ");
    nlineloc = printstr.find_first_of('\n',nlineloc+1);
  }
  s << printstr;
}


/// The params class is used to input the run time parameters
/// to the generator from a file and to provide them to the
/// interested objects in the generator
/// it also helps to store them in the output file for the record
/// the type of the paremeter must be std::string, vector or
/// any type with friend operator>>(istream&,type&) operator defined
/// all the parameters (type, name)  are defined here
class params {
public:

  /// parameter definitions
#define PARAM(type, name, default_value) type name;
  PARAMS_ALL ()
#undef PARAM
public:
  std::string path_to_data;

  /// constructor
  params () : path_to_data ("")
#define PARAM(type, name, default_value) ,name(default_value)
  PARAMS_ALL ()
#undef PARAM
  {}

  /// read all parameter values from config file filename
  inline
  bool read (const char *filename){
    ifstream dane;
    open_data_file (dane, filename);
    if (!dane)
      cerr << "Could not open: '" << filename << "' using defaults " << endl;
    return read (dane, filename);
  }

  inline
  bool read (istream & dane, const char *filename){
    int line_number = 0;
    double value;
    while (dane.good ()){ // while not end of file
      line_number++;      // increase line number
      //read next line to buffer "line"
      std::string line;
      getline (dane, line);
      // create istream for reading from "line"
      std::stringstream ins (line);
      // skip to next line if this line is commented
      if (line[0] == '#'){
        continue;
      }
      if (line[0] == '@'){
        char malpa;
        std::string incfname;
        ins >> malpa >> incfname;
        ifstream incfile;
        if (open_data_file (incfile, incfname)){
          read (incfile, incfname.c_str());
        } else {
          std::cerr << filename << ':' << line_number
            << ". Could not open file '" << incfname << "' for reading."
            << std::endl;
          exit (11);
        }
        continue;
      }
      ins >> ws;
      // comments must start at the beginning of the line
      if(ins.peek () == '#'){
        std::cerr << filename << ':' << line_number
          << ": '#' in the middle of the line " << endl;
        continue;
      }
      if(ins.eof ()){ // the line was empty
        continue;
      }

      std::string varname = "";
      while(!ins.eof()){
        char b = ins.peek();
        if (b == '=' || b == '+' || b == ' ' || b == '\t'){
          break;
        } else {
          varname += ins.get ();
        }
      }
      char eq = ' ';
      ins >> eq;    // read the '=' sign (or '+' sign)
      switch (eq){
        case '=':{
          break;
        }
        case '+':{
          if(ins.peek () == '='){
            ins.get ();
            break;
          }
        }
        default:{
          std::cerr << filename << ':' << line_number
            << ": '=' expected after  \"" << varname << "\"" << std::endl;
          exit (13);
        }
      }
      //Check if the varname is in the fieldlist and call appiopriate read
      //function
      int kod = 0;
    #define PARAM(type,name,default_value) \
      if(kod==0){ \
        if(varname == #name) {\
          kod = 1 + ::read(name,ins,eq); \
        } \
      }
      PARAMS_ALL ()
    #undef PARAM
      switch (kod) {
        case 0:{
          std::cerr << filename << ':' << line_number
            << ": unknown parameter \"" << varname << "\"" << std::endl;
          exit (14);
        }
        case 1:{
          std::cerr << filename << ':' << line_number
            << ": bad value for parameter \"" << varname << "\"" << std::endl;
          exit (15);
        }
      }
    }
    return true;
  }

  inline
  void list(std::ostream & out = std::cout) {
#define PARAM(type,name,default_value) \
        out << #name" ="; \
        write(name, out, #name); \
        out << std::endl;
    PARAMS_ALL ();
#undef PARAM
  }

  inline
  void list (std::string filename){
    std::ofstream out(filename.c_str ());
    list (out);
  }
};
#endif
