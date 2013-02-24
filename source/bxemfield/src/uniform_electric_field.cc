// -*- mode: c++ ; -*- 
/* uniform_electric_field.cc
 */ 

#include <emfield/uniform_electric_field.h>
#include <datatools/properties.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/service_manager.h>

namespace emfield {

  // Registration instantiation macro :
  EMFIELD_REGISTRATION_IMPLEMENT(uniform_electric_field, "emfield::uniform_electric_field");
  
  // Constructor :
  EMFIELD_CONSTRUCTOR_IMPLEMENT_HEAD(uniform_electric_field,flags_)     
  {
    _set_electric_field (true);
    _set_electric_field_can_be_combined (false);
    _set_magnetic_field (false);
    _set_magnetic_field_can_be_combined (false);
    geomtools::invalidate(_uniform_electric_field_);
    return;
  }  
  
  // Destructor :
  EMFIELD_DEFAULT_DESTRUCTOR_IMPLEMENT(uniform_electric_field);
  
  // Getter :
  const geomtools::vector_3d & uniform_electric_field::get_uniform_electric_field () const
  {
    return _uniform_electric_field_;
  }
  
  // Setter :
  void uniform_electric_field::set_uniform_electric_field (const geomtools::vector_3d & e_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::uniform_electric_field::set_uniform_electric_field: "
                << "Cannot change the magnetic field value !";
        throw std::logic_error (message.str ());
      }
    _uniform_electric_field_ = e_;
    return;
  }
  
  EMFIELD_COMPUTE_EFIELD_IMPLEMENT_HEAD(uniform_electric_field,
                                        position_, 
                                        time_, 
                                        electric_field_)
  {
    electric_field_ = _uniform_electric_field_;
    return STATUS_SUCCESS;
  } 

  EMFIELD_COMPUTE_BFIELD_IMPLEMENT_HEAD(uniform_electric_field,
                                        position_, 
                                        time_, 
                                        magnetic_field_)
  {
    geomtools::invalidate (magnetic_field_);
    return STATUS_ERROR;
  } 

  EMFIELD_RESET_IMPLEMENT_HEAD(uniform_electric_field)
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::uniform_electric_field::reset: "
                << "Cannot reset the magnetic field !";
        throw std::logic_error (message.str ());
      }

     geomtools::invalidate(_uniform_electric_field_);

    _set_initialized (false);
    return;
  }

  EMFIELD_INITIALIZE_IMPLEMENT_HEAD(uniform_electric_field,setup_,service_manager_,fields_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "emfield::uniform_electric_field::initialized: "
                << "Field is already initialized !";
        throw std::logic_error (message.str ());
      }

    double e_unit = CLHEP::volt / CLHEP::meter;

    base_electromagnetic_field::_parse_basic_parameters (setup_, service_manager_, fields_);

    if (setup_.has_key ("electric_field.unit"))
      {
        std::string e_unit_str = setup_.fetch_string ("electric_field.unit");
        e_unit = datatools::units::get_electric_field_unit_from (e_unit_str);
      }

    if (! geomtools::is_valid (_uniform_electric_field_))
      {
        if (setup_.has_key ("electric_field.coordinates"))
          {
            std::vector<double> elecfield_coord;
            setup_.fetch ("electric_field.coordinates", elecfield_coord);
            if (elecfield_coord.size () != 3)
              {
                std::ostringstream message;
                message << "emfield::uniform_electric_field::initialized: "
                        << "Invalid electric field vector 'electric_field' !";
                throw std::logic_error (message.str ());
              }
            _uniform_electric_field_.set (elecfield_coord[0], 
                                          elecfield_coord[1], 
                                          elecfield_coord[2]);
          }
      }

    if (! geomtools::is_valid (_uniform_electric_field_))
      {
        double elecfield_amp;
        datatools::invalidate (elecfield_amp);
        int elecfield_axis = geomtools::ROTATION_AXIS_INVALID;

        if (setup_.has_key ("electric_field.magnitude"))
          {
            elecfield_amp = setup_.fetch_real ("electric_field.magnitude"); 
          }

        if (setup_.has_key ("electric_field.axis"))
          {
            std::string axis_str = setup_.fetch_string ("electric_field.axis");
            elecfield_axis = geomtools::get_rotation_axis_from_label (axis_str);
            if (elecfield_axis == geomtools::ROTATION_AXIS_INVALID)
              {
                std::ostringstream message;
                message << "emfield::uniform_electric_field::initialized: "
                        << "Invalid 'electric_field.axis' !";
                throw std::logic_error (message.str ());
              }
          }
        
        geomtools::vector_3d elecfield_direction (0., 0., 0.);
        if (elecfield_axis == geomtools::ROTATION_AXIS_X)
          {
            elecfield_direction.setX (1.0);
          }
        else if (elecfield_axis == geomtools::ROTATION_AXIS_Y)
          {
            elecfield_direction.setY (1.0);
          }
        else if (elecfield_axis == geomtools::ROTATION_AXIS_Z)
          {
            elecfield_direction.setZ (1.0);
          }

        _uniform_electric_field_ = elecfield_amp * elecfield_direction;
      }

    _uniform_electric_field_ *= e_unit; 

    _set_initialized (true);
    return;
  } 
   
} // end of namespace emfield

// end of uniform_electric_field.cc
