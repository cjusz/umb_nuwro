/////////////////////////////////////////////////////////////////////////////////////
//     Tworzenie histogram�w i zapisywanie ich w plikach dla xmgrace
/////////////////////////////////////////////////////////////////////////////////////

#ifndef _hist_h_
#define _hist_h_
#include <iostream>
#include <fstream>

/////////////////////////////////////////////////////////////////////////////////////
///   KLASA HIST - r�niczkowe przekroje czynne i ich wykresy
/////////////////////////////////////////////////////////////////////////////////////
class hist
{
private:
  char * name;                  // nazwa parametru
  double min, max, width;       // minimum , maximim i szeroko�� kube�ka
  int nkub;                     // ilo�� kube�k�w 
  double *sum;			// tablica kube�k�w
  int count;			// ilo�� wstawie�
  double _total;                // suma wstawie�
  inline int ind (double arg);   // nr kube�ka je�li parametr wynosi 'arg'
  inline double srodek (int i, double pol=0.5);  // �rodek i-tego kube�ka

public:

   inline hist (char *n = NULL, double minv = 0, double maxv = 1,  int ile = 1);
    
   inline ~hist () { delete [] sum;}
   
   inline  void insert_value (double val, double weight);
   // Wstawienie warto�ci do histogramu 
   
   inline  void plot (ostream & out, double xunit, double yunit, double pol=0.5, char separator='\t');
   // zrzuca do strumienia out dane potrzebne do zrobienia 
   // wykresu zale�no�ci warto�ci przekroju czynnego od parametru

   
   inline  void plot (string filename, double xunit, double yunit, double pol=0.5, char separator='\t');
   // filename=nazwa pliku kt�ry zostanie utworzony.
   // parametr=1, 2 lub 3 numer parametru na osi x   

   
   inline  double total (){    return _total;  }   
   //ca�kowity przekr�j czynny ? NIE 
   // Suma wrzuconych liczb

};

/////////////////////////////////////////////////////////////////////////////////////
///                    I M P L E M E N T A C J A                                  ///
/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////
double hist::srodek (int i, double pol)   // �rodek itego kube�ka
////////////////////////s/////////////////////////////////////////////////////////////
  {
    return (i + pol) * width + min;
  }

/////////////////////////////////////////////////////////////////////////////////////
int hist::ind (double arg)   // nr kube�ka je�li parametr wynosi 'arg'
/////////////////////////////////////////////////////////////////////////////////////
  {
    if (arg < min || arg>max)
      return -1;
    int i = int ((arg - min) / width);
    if (i >= nkub)
      return -1;
    return i;
  }



/////////////////////////////////////////////////////////////////////////////////////
hist::hist (char *n, double minv, double maxv, int ile)
/////////////////////////////////////////////////////////////////////////////////////
           :name(n), 
	    min(minv), 
	    max(maxv), 
	    width((maxv-minv)/ile),
	    nkub(ile),
	    count(0),
	    _total(0)
  { 
    sum = new double[nkub];
    for(int i=0;i< nkub;i++)
         sum[i] = 0;
  }

/////////////////////////////////////////////////////////////////////////////////////
  void hist::insert_value (double val, double weight)
/////////////////////////////////////////////////////////////////////////////////////
  {
    int i = ind (val);
    count++;
    if (i >= 0 && weight!=0)
      {sum[i] += weight;
       _total += weight;
      } 
  }

/////////////////////////////////////////////////////////////////////////////////////
  void hist::plot (ostream & out, double xunit, double yunit, double pol, char separator)
/////////////////////////////////////////////////////////////////////////////////////
{
    out << "#  " << name << " depandence "<<endl;
    out << "#  mean = " << _total/count/(yunit*xunit)<<endl;
    
    for (int i = 0; i < nkub; i++)
      {
		out << srodek (i, pol) / xunit << separator;
		if (count > 0)
		  out << (sum[i] / count) /width / yunit << endl;
		else
		  out << 0 << endl;
      }
  }
  
/////////////////////////////////////////////////////////////////////////////////////
  void hist::plot (string filename, double xunit, double yunit, double pol, char separator)
/////////////////////////////////////////////////////////////////////////////////////
  {
    std::ofstream wyk (filename.c_str());
    plot (wyk, xunit, yunit, pol, separator);
  }

/////////////////////////////////////////////////////////////////////////////////////
///        K O N I E C   I M P L E M E N T A C J I                                ///
/////////////////////////////////////////////////////////////////////////////////////
#endif
