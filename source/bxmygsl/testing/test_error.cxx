// test_error.cxx

// Ourselves:
#include <mygsl/error.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

int main(/*int argc_ , char ** argv_*/)
{
  try
    {
      mygsl::error::off ();
      std::clog << "run test error 1..." << std::endl;
      MYGSL_ERROR("test error 1",1);
      gsl_error ("test error 1",__FILE__, __LINE__,1);
      // try {
      //   mygsl::error::on ();
      //   std::clog << "run test error 2..." << std::endl;
      //   MYGSL_ERROR("test error 2",2);
      // }
      // catch (std::exception & x)
      //   {
      //     std::cerr << "WARNING: as expected : " << x.what () << std::endl;
      //     return (EXIT_FAILURE);
      //   }
    }
  catch (std::exception & x)
    {
      std::cerr << "ERROR: " << x.what () << std::endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}
