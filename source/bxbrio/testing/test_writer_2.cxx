// -*- mode: c++ ; -*-
// test_writer_2.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/properties.h>
#include <brio_test_data.cc>

// Serialization code :
#include <datatools/properties.ipp>
#include <brio_test_data.ipp>

#include <brio/writer.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'brio::writer'!" << endl;

      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      bool dump = false;
      size_t data_count = 10;
      long seed = 314159;
      bool text = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
              string option = token;
              if ((option == "-d") || (option == "--debug"))
                {
                  logging = datatools::logger::PRIO_DEBUG;
                }
              else if ((option == "-D") || (option == "--dump"))
                {
                  dump = true;
                }
              else if ((option == "-v") || (option == "--verbose"))
                {
                  logging = datatools::logger::PRIO_NOTICE;
                }
              else if ((option == "-m") || (option == "--many"))
                {
                  data_count = 1000;
                }
              else if ((option == "-M") || (option == "--manymany"))
                {
                  data_count = 100000;
                }
              else if ((option == "-t"))
                {
                  text = true;
                }
              else
                {
                  clog << "warning: ignoring option '" << option << "'!" << endl;
                }
            }
          else
            {
              string argument = token;
              {
                clog << "warning: ignoring argument '" << argument << "'!" << endl;
              }
            }
          iarg++;
        }

      srand48 (seed);

      // Setup a brio writer:
      string filename = "test_io_2.brio";
      if (text)
        {
          filename = "test_io_2.trio";
        }
      brio::writer my_writer (filename, logging);

      // Uncomment the following instruction to use a 'data' store:
      //my_writer.add_store ("data");

      // Store `data' randomized objects within an *automatic* store;
      for (int i = 0; i < (int) data_count; i++)
        {
          brio::test::data_t data;
          data.randomize ();
          if (dump)
            {
              data.dump (clog, "Data to be stored in the *automatic* store: ");
            }
          my_writer.store (data);
        }

      {
        my_writer.add_store ("dummy");
        datatools::properties infos;
        infos.store_flag ("test");
        my_writer.store (infos);
      }

      // Print writer's status:
      if (dump) my_writer.tree_dump (clog, "brio::writer: ");

      // Close the file:
      my_writer.close ();

      cerr << "notice: " << "The end." << endl;

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
  return (error_code);
}
