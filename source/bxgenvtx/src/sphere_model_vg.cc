// -*- mode: c++ ; -*-
/* sphere_model_vg.cc
 */

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <limits>

#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <mygsl/rng.h>

#include <materials/manager.h>

#include <geomtools/geom_info.h>
#include <geomtools/logical_volume.h>
#include <geomtools/i_shape_3d.h>
#include <geomtools/sphere.h>
#include <geomtools/manager.h>
#include <geomtools/mapping.h>
#include <geomtools/mapping_plugin.h>
#include <geomtools/materials_plugin.h>

#include <genvtx/utils.h>
#include <genvtx/sphere_model_vg.h>
#include <genvtx/detail/geom_manager_utils.h>

namespace genvtx {

  GENVTX_VG_REGISTRATION_IMPLEMENT(sphere_model_vg,"genvtx::sphere_model_vg");

  bool sphere_model_vg::is_mode_valid () const
  {
    return is_mode_bulk () || is_mode_surface ();
  }

  bool sphere_model_vg::is_mode_bulk () const
  {
    return get_mode () == utils::MODE_BULK;
  }

  bool sphere_model_vg::is_mode_surface () const
  {
    return get_mode () == utils::MODE_SURFACE;
  }

  void sphere_model_vg::set_skin_skip (double skin_skip_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_skip_ = skin_skip_;
    return;
  }

  void sphere_model_vg::set_skin_thickness (double skin_thickness_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");
    _skin_thickness_ = skin_thickness_;
    return;
  }

  const std::string & sphere_model_vg::get_origin_rules () const
  {
    return _origin_rules_;
  }

  void sphere_model_vg::set_origin_rules (const std::string & origin_rules_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _origin_rules_ = origin_rules_;
    return;
  }

  bool sphere_model_vg::has_mapping_plugin_name() const
  {
    return ! _mapping_plugin_name_.empty();
  }

  const std::string & sphere_model_vg::get_mapping_plugin_name() const
  {
    return _mapping_plugin_name_;
  }

  void sphere_model_vg::set_mapping_plugin_name(const std::string & mpn_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _mapping_plugin_name_ = mpn_;
    return;
  }

  bool sphere_model_vg::has_materials_plugin_name() const
  {
    return ! _materials_plugin_name_.empty();
  }

  const std::string & sphere_model_vg::get_materials_plugin_name() const
  {
    return _materials_plugin_name_;
  }

  void sphere_model_vg::set_materials_plugin_name(const std::string & mpn_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    _materials_plugin_name_ = mpn_;
    return;
  }

  int sphere_model_vg::get_mode () const
  {
    return _mode_;
  }

  void sphere_model_vg::set_mode (int mode_)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");
    if ((mode_ == utils::MODE_BULK) || (mode_ == utils::MODE_SURFACE)) {
      _mode_ = mode_;
    } else {
      _mode_ = utils::MODE_INVALID;
    }
    return;
  }

  // Constructor:
  GENVTX_VG_CONSTRUCTOR_IMPLEMENT_HEAD(sphere_model_vg)
  {
    _initialized_ = false;
    _set_defaults_ ();
    return;
  }

  GENVTX_VG_IS_INITIALIZED_IMPLEMENT_HEAD(sphere_model_vg)
  {
    return _initialized_;
  }

  // Destructor :
  GENVTX_VG_DEFAULT_DESTRUCTOR_IMPLEMENT(sphere_model_vg)

  void sphere_model_vg::_set_defaults_ ()
  {
    // Internal reset:
    utils::origin_invalidate (_origin_rules_);
    _mode_           = utils::MODE_INVALID;
    _skin_skip_ = 0.0;
    _skin_thickness_ = 0.0;
    if (_sphere_vg_.is_initialized()) _sphere_vg_.reset ();
    _origin_rules_.clear();
    _mapping_plugin_name_.clear();
    _materials_plugin_name_.clear();
    _entries_.clear ();
    _src_selector_.reset ();
    this->i_vertex_generator::_reset ();
    return;
  }

  void sphere_model_vg::_reset_ ()
  {
    _set_defaults_ ();
    return;
  }

  GENVTX_VG_RESET_IMPLEMENT_HEAD(sphere_model_vg)
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Vertex generator '" << get_name() << "' is not initialized !");
    _reset_ ();
    _initialized_ = false;
    return;
  }

  GENVTX_VG_SHOOT_VERTEX_IMPLEMENT_HEAD(sphere_model_vg,random_,vertex_)
  {
    DT_THROW_IF (! is_initialized (), std::logic_error, "Vertex generator '" << get_name() << "' is not initialized !");
    geomtools::invalidate (vertex_);
    this->_shoot_vertex_spheres (random_, vertex_);
    return;
  }

  void sphere_model_vg::_shoot_vertex_spheres (mygsl::rng & random_,
                                          geomtools::vector_3d & vertex_)
  {
    double ran_w = random_.uniform ();
    int index = -1;
    for (size_t i = 0; i < _entries_.size (); i++) {
      if (ran_w <= _entries_[i].cumulated_weight) {
        index = i;
        break;
      }
    }
    DT_THROW_IF (index < 0, std::logic_error,
                 "Cannot determine the vertex location index in vertex generator '" << get_name() << "' !");
    geomtools::vector_3d src_vtx;
    _sphere_vg_.shoot_vertex (random_, src_vtx);

    const geomtools::placement & world_plct
      = _entries_[index].ginfo->get_world_placement ();
    // Special treatment for geomtools::rotated_sphereed_model :
    if (_entries_[index].ginfo->get_logical().has_effective_relative_placement()) {
      const geomtools::placement & eff_ref_placement = _entries_[index].ginfo->get_logical().get_effective_relative_placement();
      geomtools::vector_3d rel_vtx;
      eff_ref_placement.child_to_mother(src_vtx, rel_vtx);
      src_vtx = rel_vtx;
    }
    world_plct.child_to_mother (src_vtx, vertex_);
    return;
  }

  void sphere_model_vg::_init_ ()
  {
    DT_THROW_IF (! is_mode_valid (), std::logic_error, "Invalid mode in vertex generator '" << get_name() << "' !");
    DT_THROW_IF (! has_geom_manager (), std::logic_error, "Missing geometry manager in vertex generator '" << get_name() << "' !");

    const geomtools::mapping * mapping_ptr
      = detail::access_geometry_mapping(get_geom_manager (), _mapping_plugin_name_);
    DT_THROW_IF (mapping_ptr == 0, std::logic_error,
                 "No available geometry mapping was found in vertex generator '" << get_name() << "' !");

    //DT_LOG_FATAL(get_logging_priority(), "*** Origin rules : " << _origin_rules_);
    _src_selector_.set_id_mgr (get_geom_manager ().get_id_mgr ());
    _src_selector_.initialize (_origin_rules_);
    //_src_selector_.dump (clog, "genvtx::sphere_model_vg::initialize: ID selector:");

    const geomtools::mapping & the_mapping = *mapping_ptr;
    const geomtools::geom_info_dict_type & geom_infos
      = the_mapping.get_geom_infos ();
    std::list<weight_entry_type> entries;
    for (geomtools::geom_info_dict_type::const_iterator i
           = geom_infos.begin ();
         i != geom_infos.end ();
         i++) {
      const geomtools::geom_id & gid = i->first;
      if (_src_selector_.match (gid)) {
        const geomtools::geom_info * ginfo = &(i->second);
        weight_entry_type e;
        e.weight = 0.0;
        e.cumulated_weight = 0.0;
        e.ginfo = ginfo;
        entries.push_back (e);
      }
    }
    DT_THROW_IF (entries.size () == 0, std::logic_error,
                 "Cannot compute any source of vertex in vertex generator '" << get_name() << "' !");

    weight_entry_type dummy;
    _entries_.assign (entries.size (), dummy);
    std::copy (entries.begin (), entries.end (), _entries_.begin ());

    const geomtools::logical_volume * src_log = 0;
    for (size_t i = 0; i < _entries_.size (); i++) {
      DT_LOG_DEBUG(get_logging_priority(),
                   "ID #" << i << " = " << _entries_[i].ginfo->get_id ());
      if (src_log == 0) {
        src_log = &_entries_[i].ginfo->get_logical ();
      } else {
        //DT_THROW_IF (src_log != &_entries_[i].ginfo->get_logical (),
        DT_THROW_IF (! geomtools::logical_volume::same(*src_log, _entries_[i].ginfo->get_logical()),
                     std::logic_error,
                     "Vertex location with different logical geometry volumes ('" << src_log->get_name()
                     << "' vs. '" << _entries_[i].ginfo->get_logical ().get_name()
                     << "') are not supported  (different shapes or materials) in vertex generator '"
                     << get_name() << "' !");
      }
    }

    // Attempt to extract material info :
    double density = -1.0;
    if (is_mode_bulk ()) {
      std::string material_name;
      if (src_log->has_material_ref ()) {
        material_name = src_log->get_material_ref ();
      }
      if (src_log->has_effective_material_ref ()) {
        material_name = src_log->get_effective_material_ref ();
      }
      if (! material_name.empty()) {
        const materials::manager * mat_mgr_ptr
          = detail::access_materials_manager(get_geom_manager (),
                                             _materials_plugin_name_);
        DT_THROW_IF (mat_mgr_ptr == 0, std::logic_error,
                     "No geometry materials plugin named '" << _materials_plugin_name_
                     << "' available from the geometry manager in vertex generator '" << get_name() << "' !");
        const materials::manager & mat_mgr = *mat_mgr_ptr;
        materials::material_dict_type::const_iterator found =
          mat_mgr.get_materials ().find (material_name);
        if (found != mat_mgr.get_materials ().end ()) {
          if (found->second.has_ref ()) {
            const materials::material & the_material = found->second.get_ref ();
            density = the_material.get_density ();
            DT_LOG_TRACE(get_logging_priority(),
                         "Density = " << density / (CLHEP::g/CLHEP::cm3) << " g/cm3");
          }
        }
      }
    }
    int surface_mask = 0;
    if (is_mode_surface ()) {
      _sphere_vg_.set_mode (utils::MODE_SURFACE);
    } else {
      _sphere_vg_.set_mode (utils::MODE_BULK);
    }
    const geomtools::i_shape_3d * src_shape_ptr = 0;
    {
      DT_THROW_IF (! src_log->has_shape (), std::logic_error,
                   "Logical '" << src_log->get_name () << "' has " << "no shape !");
      src_shape_ptr = &src_log->get_shape ();
      if (src_log->has_effective_shape ()) {
        src_shape_ptr = &src_log->get_effective_shape ();
      }
    }
    const geomtools::i_shape_3d & src_shape = *src_shape_ptr;
    DT_THROW_IF (src_shape.get_shape_name () != "sphere", std::logic_error,
                 "Shape is '" << src_shape.get_shape_name () << "' but "
                 << "only sphere shape is supported in vertex generator '" << get_name() << "' !");
    const geomtools::sphere & sphere_shape
      = dynamic_cast<const geomtools::sphere &> (src_shape);
    _sphere_vg_.set_sphere_ref (sphere_shape);
    _sphere_vg_.set_skin_skip(_skin_skip_);
    _sphere_vg_.set_skin_thickness(_skin_thickness_);
    _sphere_vg_.initialize_simple ();
    //_sphere_vg_.tree_dump(std::cerr, "Sphere VG: ", "***** DEVEL ***** ");
    double weight = 0.0;
    if (is_mode_surface ()) {
      weight = sphere_shape.get_surface();
    } else {
      weight = sphere_shape.get_volume();
    }

    // Compute weight:
    _entries_[0].cumulated_weight = _entries_[0].weight;
    for (size_t i = 0; i < _entries_.size (); i++) {
      _entries_[i].weight = weight;
      double cumul = 0.0;
      if (i > 0) cumul = _entries_[i - 1].cumulated_weight;
      _entries_[i].cumulated_weight = cumul + _entries_[i].weight;
    }

    // Store the total weight before normalization for alternative use :
    double the_weight_value = _entries_.back ().cumulated_weight;
    int the_weight_type = weight_info::WEIGHTING_VOLUME;
    if (is_mode_surface ())  {
      the_weight_type = weight_info::WEIGHTING_SURFACE;
    }
    weight_info the_weight_info;
    the_weight_info.set_type(the_weight_type);
    the_weight_info.set_value(the_weight_value);
    if (the_weight_type == weight_info::WEIGHTING_VOLUME && density > 0) {
      // Total mass computed for homogeneous density:
      the_weight_info.set_mass(the_weight_value * density);
    }
    _set_total_weight (the_weight_info);

    // Normalize weight:
    for (size_t i = 0; i < _entries_.size (); i++) {
      _entries_[i].cumulated_weight /= _entries_.back ().cumulated_weight;
    }
    return;
  }

  GENVTX_VG_INITIALIZE_IMPLEMENT_HEAD(sphere_model_vg,setup_,service_manager_,vgens_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error, "Vertex generator '" << get_name() << "' is already initialized !");

    GENVTX_VG_INITIALIZE_BASICS_INVOKE(setup_,service_manager_);
    GENVTX_VG_INITIALIZE_GEO_MANAGER_INVOKE(setup_,service_manager_);

    int mode = utils::MODE_INVALID;
    std::string origin_rules;
    utils::origin_invalidate (origin_rules);
    std::string mode_str;
    double lunit = CLHEP::mm;
    double skin_skip = 0.0 * CLHEP::mm;
    double skin_thickness = 0.0 * CLHEP::mm;

    if (setup_.has_key ("length_unit")) {
      std::string lunit_str = setup_.fetch_string ("length_unit");
      lunit = datatools::units::get_length_unit_from (lunit_str);
    }

    DT_THROW_IF (!setup_.has_key ("origin"),
                 std::logic_error,
                 "Missing 'origin_rules' property in vertex generator '" << get_name() << "' !");
    origin_rules = setup_.fetch_string ("origin");

    if (setup_.has_key ("materials.plugin_name")) {
      std::string mpn;
      mpn = setup_.fetch_string ("materials.plugin_name");
      set_materials_plugin_name(mpn);
    }

    DT_THROW_IF (! setup_.has_key ("mode"),
                 std::logic_error,
                 "Missing 'mode' property in vertex generator '" << get_name() << "' !");
    mode_str = setup_.fetch_string ("mode");

    if (mode_str == "bulk") mode = utils::MODE_BULK;
    if (mode_str == "surface") mode = utils::MODE_SURFACE;

    if (setup_.has_key ("skin_skip")) {
      skin_skip = setup_.fetch_real ("skin_skip");
      if (! setup_.has_explicit_unit ("skin_skip")) skin_skip *= lunit;
    }

    if (setup_.has_key ("skin_thickness")) {
      skin_thickness = setup_.fetch_real ("skin_thickness");
      if (! setup_.has_explicit_unit ("skin_thickness")) skin_thickness *= lunit;
    }

    set_skin_skip(skin_skip);
    set_skin_thickness(skin_thickness);
    set_origin_rules (origin_rules);
    set_mode (mode);

    _init_ ();
    _initialized_ = true;
    return;
  }

  void sphere_model_vg::tree_dump (std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
  {
    namespace du = datatools;
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    this->i_vertex_generator::tree_dump (out_, title_, indent_, true);
    out_ << indent << du::i_tree_dumpable::tag
         << "Mode           : '" << _mode_ << "'" << std::endl;
    out_ << indent << du::i_tree_dumpable::tag
         << "Skin skip      : " << _skin_skip_ / CLHEP::mm << std::endl;
    out_ << indent << du::i_tree_dumpable::tag
         << "Skin thickness : " << _skin_thickness_ / CLHEP::mm << std::endl;
    out_ << indent << du::i_tree_dumpable::tag
         << "Origin rules   : '" << _origin_rules_ << "'" << std::endl;
    out_ << indent << du::i_tree_dumpable::tag
         << "ID selector " << std::endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
         << "Entries        : " << _entries_.size () << std::endl;
    return;
  }

} // end of namespace genvtx

// end of sphere_model_vg.cc
