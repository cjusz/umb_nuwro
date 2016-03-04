#include "params.h"
#include "args.h"
#include "dirs.h"

void usage(char const *argv0){
  std::cout << "[USAGE]: " << argv0
    << " [-i input_paramaters_file] [-p \"param1="
    "value1\"] [-p \"param2=value2\"]...\n";
}

int main(int argc, char** argv){
	set_dirs(argv[0]);
	params p;

  if( (argc%2) == 0){
    usage(argv[0]);
    return 22;
  }

	args a;
  a.read (argc, argv);
	p.read (a.input);
	p.read (a.params, "command line");
	p.list ();
	return 0;
}
