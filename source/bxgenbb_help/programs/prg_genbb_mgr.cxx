// -*- mode: c++; -*- 
// prg_genbb_mgr.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <genbb_help/genbb_mgr.h>

/* Serialization dedicated code */
#include <genbb_help/the_serializable.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {  
      bool debug = false;
      string genbb_file = "";

      int iarg = 1;
      while (iarg < argc_)
	{
	  string arg = argv_[iarg];
	  if (arg[0] == '-')
	    {
	      if (arg == "-d" || arg == "--debug") debug = true;
	    }
	  else
	    {
	      if (genbb_file.empty ())
		{
		  genbb_file = arg;
		}
	    }
	  iarg++;
	}
    
      if (genbb_file.empty ())
	{
	  throw runtime_error ("Missing GENBB generated events file!");
	}

      genbb::genbb_mgr mgr;

      // genbb input data files:
      mgr.set (genbb_file);
      if (debug) mgr.dump ();

      // initialize the manager:
      mgr.init ();
      if (debug) mgr.dump ();

      // working primary event:
      genbb::primary_event pe;

      size_t count = 0;
      // main loop on primary events source:
      while (mgr.has_next ())
	{
	  mgr.load_next (pe);
	  pe.dump (cout);
	  cout << endl;
	  count++;
	}
      mgr.reset ();

      clog << "Number of loaded events: " << count << endl; 
      if (debug) mgr.dump ();
   
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error !" << endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of prg_genbb_mgr.cxx
