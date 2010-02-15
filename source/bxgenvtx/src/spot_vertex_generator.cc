// -*- mode: c++ ; -*- 
/* spot_vertex_generator.cc
 */

#include <genvtx/spot_vertex_generator.h>

namespace genvtx {

  const geomtools::vector_3d &  spot_vertex_generator::get_spot () const
  {
    return __spot;
  }
  
  void spot_vertex_generator::set_spot (const geomtools::vector_3d & new_value_)
  {
    __spot = new_value_;
  }
  
  void spot_vertex_generator::set_spot (double x_, double y_, double z_)
  {
    __spot.set (x_, y_, z_);
  }
  
  // ctor:
  spot_vertex_generator::spot_vertex_generator ()
  {
  }

  spot_vertex_generator::spot_vertex_generator (double x_, double y_, double z_)
  {
    set_spot (x_, y_, z_);
  }

  spot_vertex_generator::spot_vertex_generator (const geomtools::vector_3d & spot_)
  {
    __spot = spot_;
  }
  
  // dtor:
  spot_vertex_generator::~spot_vertex_generator ()
  {
  }
  
  void spot_vertex_generator::_shoot_vertex (mygsl::rng & random_, 
			       geomtools::vector_3d & vertex_)
  {
    vertex_ = __spot;
  }

  /**********************************************************************/

  // static method used within a vertex generator factory:
  i_vertex_generator * 
  spot_vertex_generator::create (const properties & configuration_)
  {
    cerr << "DEVEL: spot_vertex_generator::create: Entering..." << endl;
    configuration_.tree_dump (cerr, "spot_vertex_generator::create: configuration:", "DEVEL: ");
    using namespace std;
    bool devel = false;
    //devel = true;

    // parameters of the spot vertex generator:
    double x, y, z;
    x = y = z = 0.0;

    if (configuration_.has_key ("x"))
      {
	x = configuration_.fetch_real ("x");
      }

    if (configuration_.has_key ("y"))
      {
	y = configuration_.fetch_real ("y");
      }

    if (configuration_.has_key ("z"))
      {
	z = configuration_.fetch_real ("z");
      }

    // create a new parameterized 'spot_vertex_generator' instance:
    spot_vertex_generator * ptr = new spot_vertex_generator;
    ptr->set_spot (x, y, z);
    return ptr;	
  }

  string spot_vertex_generator::vg_id () const
  {
    return "genvtx::spot_vertex_generator";
  }

  vg_creator_t spot_vertex_generator::vg_creator () const
  {
    return spot_vertex_generator::create;
  }

  // register this creator:   
  i_vertex_generator::creator_registration<spot_vertex_generator> spot_vertex_generator::__CR;

} // end of namespace genvtx

// end of spot_vertex_generator.cc
