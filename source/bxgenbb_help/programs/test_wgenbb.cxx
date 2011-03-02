// -*- mode: c++; -*- 
// test_wgenbb.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genbb_help/wgenbb.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {  
      bool debug = false;
      bool test  = false;
      bool dump  = false;
      size_t max_count = 10;

      int iarg = 1;
      while (iarg < argc_)
	{
	  string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug = true;
	  if (arg == "-t" || arg == "--test") test = true;
	  if (arg == "-D" || arg == "--dump") dump = true;
	  if (arg == "-1000") max_count = 1000;
	  iarg++;
	}
    
      datatools::utils::properties config;
      if (debug) config.store_flag ("debug");
      config.store_flag ("local_prng");
      config.store ("seed", 314159);
      config.store ("decay_type", "DBD");
      config.store ("decay_isotope", "Se82");
      config.store ("decay_dbd_level", 0);
      config.store ("decay_dbd_mode", 1);
      config.store ("energy_min", 0.0);
      config.store ("energy_max", 5.0);
      config.store ("energy_unit", "MeV");
      if (debug) config.tree_dump (clog, "Configuration: ", "debug: ");
 
      genbb::wgenbb WGBB;
      WGBB.set_debug (debug);
      WGBB.initialize (config);

      // working primary event:
      genbb::primary_event pe;

      for (int i = 0; i < max_count; i++)
	{
	  if (((i % 100) == 0) || (i + 1) == max_count) 
	    {
	      clog << "Count : " << i << endl;
	    }
	  WGBB.load_next (pe);
	  if (dump) pe.dump ();
	}

       if (debug) clog << "debug: " << "The end." << endl;
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_wgenbb.cxx
