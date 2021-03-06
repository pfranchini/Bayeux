// -*- mode: c++; -*-
// g2b.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <list>

#include <datatools/utils.h>
#include <datatools/ioutils.h>

#include <genbb_help/genbb_mgr.h>

#include <datatools/io_factory.h>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>
#include <geomtools/bio_guard.h>
#include <genbb_help/bio_guard.h>

void usage (std::ostream & a_out = std::clog)
{
  using namespace std;
  a_out << "g2b -- convert GENBB files to Boost archive file" << endl;
  a_out << "Usage: g2b [OPTION]... [INFILE1] [INFILE2]... [OUTFILE]" << endl;
  a_out << " Options: " << endl;
  a_out << "   -d|--debug  : print debug info" << endl;
  a_out << "   -h|--help   : print help" << endl;
  return;
}

int main (int a_argc, char ** a_argv)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    list<string> input_files;
    string       output_file;

    int iarg = 1;
    while (iarg < a_argc) {
      string arg = a_argv[iarg];
      if (arg[0] == '-') {
        if (arg == "-d" || arg == "--debug") debug = true;
        if (arg == "-h" || arg == "--help")  {
          usage (cout);
        }
      } else {
        if (iarg < (a_argc - 1)) {
          input_files.push_back (arg);
        } else {
          output_file = arg;
        }
      }
      iarg++;
    }

    if (input_files.size () < 1) {
      throw logic_error ("Missing input GENBB files!");
    }

    if (output_file.empty ()) {
      throw logic_error ("Missing output Boost files!");
    }

    genbb::genbb_mgr mgr;
    // genbb input data files:
    for (list<string>::const_iterator i = input_files.begin ();
         i != input_files.end ();
         i++) {
      string infile = *i;
      datatools::expand_path (infile);
      clog << datatools::io::notice << "Input GENBB file = '" << infile << "'" << endl;
      mgr.set (infile);
    }
    if (debug) mgr.dump ();

    datatools::expand_path (output_file);
    clog << datatools::io::notice << "Output Boost file = '" << output_file << "'" << endl;

    // initialize the manager:
    mgr.initialize_simple ();
    if (debug) mgr.dump ();

    size_t count = 0;
    datatools::data_writer writer;
    writer.init (output_file, datatools::using_multi_archives);

    // main loop on primary events source:
    while (mgr.has_next ()) {
      // working primary event:
      genbb::primary_event pe;
      mgr.load_next (pe);
      writer.store (pe);
      count++;
    }
    mgr.reset ();

    clog << datatools::io::notice << "Number of loaded events: " << count << endl;
    if (debug) mgr.dump ();

  }
  catch (exception & x) {
    cerr << datatools::io::error << "g2b: " << x.what () << endl;
    usage (cerr);
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << datatools::io::error << "g2b: " << "Unexpected error !" << endl;
    usage (cerr);
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of g2b.cxx
