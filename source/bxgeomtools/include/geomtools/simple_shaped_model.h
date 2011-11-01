// -*- mode: c++ ; -*- 
/* simple_shaped_model.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-13
 * Last modified: 2010-10-13
 * 
 * License: 
 * 
 * Description: 
 *   Model implementing a logical volume with a simple shape
 *   box, cylinder, sphere, tube, polycone, polyhedra
 * 
 * History: 
 * 
 */

#ifndef __geomtools__simple_shaped_model_h
#define __geomtools__simple_shaped_model_h 1

#include <string> 

#include <geomtools/i_model.h>
#include <geomtools/placement.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>
#include <geomtools/model_with_internal_items_tools.h>

namespace geomtools {

  using namespace std;

  class box;
  class cylinder;
  class tube;
  class sphere;
  class polycone;
  class polyhedra;
  class i_shape_3d;
  class logical_volume;

  // define a geometry model made of a unique simple 3D shape: 
  class simple_shaped_model : public i_model
  {

  public: 

    MWIM & get_internals ();
    const MWIM & get_internals () const;

    const string & get_material_name () const;
    const string & get_filled_material_name () const;
    const string & get_shape_name () const;

    const geomtools::box & get_box () const;
    const geomtools::cylinder & get_cylinder () const;
    const geomtools::tube & get_tube () const;
    const geomtools::sphere & get_sphere () const;
    const geomtools::polycone & get_polycone () const;
    const geomtools::polyhedra & get_polyhedra () const;

    const geomtools::i_shape_3d & get_solid () const;

  public:
 
    bool is_filled () const;

    bool is_filled_by_envelope () const;

    bool is_filled_by_extrusion () const;

    simple_shaped_model ();
  
    virtual ~simple_shaped_model ();

  public:

    virtual string get_model_id () const;

  protected:

    virtual void _post_construct (datatools::utils::properties & setup_);
  
    virtual void _at_construct (const string & name_,
                                const datatools::utils::properties & config_,
                                models_col_t * models_ = 0);

  protected:

    virtual void _construct_box (const string & name_,
                                 const datatools::utils::properties & config_,
                                 models_col_t * models_);

    virtual void _construct_cylinder (const string & name_,
                                      const datatools::utils::properties & config_,
                                      models_col_t * models_);
      
    virtual void _construct_sphere (const string & name_,
                                    const datatools::utils::properties & config_,
                                    models_col_t * models_);
      
    virtual void _construct_tube (const string & name_,
                                  const datatools::utils::properties & config_,
                                  models_col_t * models_);
      
    virtual void _construct_polycone (const string & name_,
                                      const datatools::utils::properties & config_,
                                      models_col_t * models_);

    virtual void _construct_polyhedra (const string & name_,
                                       const datatools::utils::properties & config_,
                                       models_col_t * models_);
      
  private:

    static creator_registration<simple_shaped_model> _g_cr_;
      
  public: 

    virtual void tree_dump (ostream & out_         = clog, 
                            const string & title_  = "", 
                            const string & indent_ = "", 
                            bool inherit_          = false) const;
  private:

    string                 _shape_name_;
    int                    _filled_mode_;
    string                 _material_name_;
    string                 _filled_material_name_;

    geomtools::box *       _box_;
    geomtools::cylinder *  _cylinder_;
    geomtools::tube *      _tube_;
    geomtools::sphere *    _sphere_;
    geomtools::polycone *  _polycone_;
    geomtools::polyhedra * _polyhedra_;

    geomtools::i_shape_3d * _solid_;
    geomtools::i_shape_3d * _inner_shape_; //!> for filled tube or polycone or polyhedra
    geomtools::i_shape_3d * _outer_shape_; //!> for mother polycone or polyhedra

    placement               _inner_placement_;
    logical_volume          _inner_logical_;
    physical_volume         _inner_phys_;

    logical_volume        * _daughter_owner_logical_;
    logical_volume        * _visibility_logical_;

    // internal items:
    MWIM                    _internals_;
  
  };

} // end of namespace geomtools

#endif // __geomtools__simple_shaped_model_h

// end of simple_shaped_model.h
