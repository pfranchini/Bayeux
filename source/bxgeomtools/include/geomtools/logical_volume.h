// -*- mode: c++ ; -*- 
/* logical_volume.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-08
 * Last modified: 2010-02-08
 * 
 * License: 
 * 
 * Description: 
 *   Logical volume.
 * 
 * History: 
 * 
 */

#ifndef __geomtools__logical_volume_h
#define __geomtools__logical_volume_h 1

#include <string>
#include <map>


#include <datatools/utils/properties.h>
#include <datatools/utils/i_tree_dump.h>

#include <geomtools/i_shape_3d.h>
#include <geomtools/material.h>

namespace geomtools {

  using namespace std;
  using namespace datatools::utils;

  class physical_volume;

  class logical_volume
    : public datatools::utils::i_tree_dumpable
  {
  public:
    typedef map<string, const physical_volume *> physicals_col_t;

    static const string HAS_REPLICA_FLAG;

  private:
    
    void _clear_shape_ ();

  public:

    bool is_locked () const;

    void lock ();

    void unlock ();

    const string & get_name () const;

    void set_name (const string &);

    const properties & parameters () const;

    properties & parameters ();

    bool has_shape () const;

    void set_shape (const i_shape_3d &);

    void set_shape (const i_shape_3d *);

    const i_shape_3d & get_shape () const;

    const physicals_col_t & get_physicals () const;

    logical_volume ();

    logical_volume (const string &);

    logical_volume (const string &, const i_shape_3d &);

    logical_volume (const string &, const i_shape_3d *);

    virtual ~logical_volume ();

    virtual void tree_dump (ostream & out_         = clog, 
                            const string & title_  = "", 
                            const string & indent_ = "", 
                            bool inherit_          = false) const;

    bool has_physical (const string & name_) const;

    bool has_material_ref () const;

    string get_material_ref () const;

    void set_material_ref (const string & = "");

    const physical_volume & get_physical (const string & name_) const;
      
    void add_physical (const physical_volume & phys_, const string & name_ = "");

    bool is_replica () const;


  private:

    string     _name_;
    bool       _locked_;
    properties _parameters_;
    bool       _own_shape_;
    const i_shape_3d * _shape_;
    physicals_col_t  _physicals_;

  public:

    typedef map<string, const logical_volume *> dict_t;

  };

} // end of namespace geomtools

#endif // __geomtools__logical_volume_h

// end of logical_volume.h
