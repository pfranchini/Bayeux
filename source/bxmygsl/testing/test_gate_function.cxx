// test_gate_function.cxx

// Ourselves:
#include <mygsl/gate_function.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

int main(/*int argc_ , char ** argv_*/)
{
  try {

    mygsl::gate_function gf(1.2, 0.8, 1.0);
    gf.initialize_simple();
    gf.tree_dump(std::clog, "Gate function: ");

    {
      double dx = 0.005;
      std::cout << "# gate function:" << std::endl;
      for (double x = 0.0; x <= 5.0 + 0.001 * dx; x += dx) {
        std::cout << x << ' ' << gf(x) << std::endl;
      }
      std::cout << std::endl << std::endl;
    }

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}
