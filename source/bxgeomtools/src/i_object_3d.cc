// -*- mode: c++; -*-
/* i_object_3d.cc
 */

#include <geomtools/box.h>

namespace geomtools {

  using namespace std;

  const uint32_t i_object_3d::ALL_SURFACES = 0xFFFFFFFF;
  const uint32_t i_object_3d::NO_SURFACES  = 0;

  void i_object_3d::compute_bounding_box (box & bb_)
  {
    bb_.reset ();
    return;
  }

  double i_object_3d::get_tolerance () const
  {
    return _tolerance_;
  }

  void i_object_3d::set_tolerance (double tolerance_)
  {
    DT_THROW_IF (tolerance_ <= 0.0, std::logic_error, "Tolerance value '" << tolerance_ << "' must be positive !");
    _tolerance_ = tolerance_;
    return;
  }

  datatools::properties & i_object_3d::properties ()
  {
    return _properties_;
  }

  const datatools::properties & i_object_3d::properties () const
  {
    return _properties_;
  }

  bool i_object_3d::is_composite () const
  {
    return false;
  }

  bool i_object_3d::has_user_draw () const
  {
    return _user_draw_ != 0;
  }

  void * i_object_3d::get_user_draw () const
  {
    return _user_draw_;
  }

  void i_object_3d::set_user_draw (void * user_draw_)
  {
    _user_draw_ = user_draw_;
    return;
  }

  // ctor:
  i_object_3d::i_object_3d ()
  {
    _tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE;
    _user_draw_ = 0;
    return;
  }

  i_object_3d::i_object_3d (double tolerance_)
  {
    _user_draw_ = 0;
    _tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE;
    if (tolerance_ <= 0.0)
      {
        _tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE;
      }
    else
      {
        set_tolerance (tolerance_);
      }
    return;
  }

  // dtor:
  i_object_3d::~i_object_3d ()
  {
    i_object_3d::reset ();
    return;
  }

  void i_object_3d::reset ()
  {
    _user_draw_ = 0;
    _tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE;
    _properties_.clear ();
    return;
  }


  void i_object_3d::tree_dump (ostream & out_,
                               const string & title_,
                               const string & indent_,
                               bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ())
      {
        out_ << indent << title_ << endl;
      }
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Shape name : \"" << get_shape_name () << "\"" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Tolerance  = " << _tolerance_ << endl; /* / CLHEP::mm << " mm" << endl; */

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Properties : ";
      if ( _properties_.size () == 0)
        {
          out_ << "<empty>";
        }
      out_ << endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _properties_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Dimensionality : " << get_dimensional () <<  endl;
    }

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "User draw : " << (has_user_draw () ? "Yes": "No") << endl;

    }
    return;
  }

  /*** Object 3D getter interface ***/

  const i_object_3d *
  i_object_3d::i_getter::get (const string & name_)
  {
    datatools::properties params;
    return get (name_, params);
  }


} // end of namespace geomtools

// end of i_object_3d.cc
