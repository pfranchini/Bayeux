// -*- mode: c++ ; -*-
/* model_factory.cc
 */

#include <geomtools/model_factory.h>

#include <stdexcept>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include <datatools/utils.h>
#include <datatools/exception.h>

#include <geomtools/detail/model_tools.h>
#include <geomtools/i_model.h>
#include <geomtools/logical_volume.h>
#include <geomtools/visibility.h>

namespace geomtools {

  using namespace std;

  const string model_factory::DEFAULT_WORLD_LABEL = "world";

  bool model_factory::is_locked () const
  {
    return _locked_;
  }

  bool model_factory::is_debug () const
  {
    return _logging_priority_ >= datatools::logger::PRIO_DEBUG;
  }

  bool model_factory::is_trace () const
  {
    return _logging_priority_ >= datatools::logger::PRIO_TRACE;
  }

  void model_factory::set_debug (bool new_value_)
  {
    if (new_value_) {
      _logging_priority_ = datatools::logger::PRIO_DEBUG;
    } else {
      _logging_priority_ = datatools::logger::PRIO_WARNING;
    }
    return;
  }

  datatools::logger::priority model_factory::get_logging_priority () const
  {
    return _logging_priority_;
  }

  void model_factory::set_logging_priority (datatools::logger::priority p_)
  {
    _logging_priority_ = p_;
    return;
  }

  const models_col_type & model_factory::get_models () const
  {
    return _models_;
  }

  const logical_volume::dict_type & model_factory::get_logicals () const
  {
    return _logicals_;
  }

  void model_factory::_basic_construct_ ()
  {
    _locked_ = false;
    _mp_.set_key_label ("name");
    _mp_.set_meta_label ("type");
    _mp_.set_description ("Geometry models setup");
    bool preload = true;
    if (preload) {
      _factory_register_.import (DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (::geomtools::i_model));
    }
  }

  // Constructor:
  model_factory::model_factory (datatools::logger::priority lp_)
  {
    _logging_priority_ = lp_;
    _basic_construct_ ();
    return;
  }


  // Constructor:
  model_factory::model_factory (bool debug_, bool core_factory_verbose_)
    : _factory_register_ ("geomtools::i_model/model_factory",
                          core_factory_verbose_ ? i_model::factory_register_type::verbose : 0)

  {
    _logging_priority_ = datatools::logger::PRIO_WARNING;
    if (debug_) _logging_priority_ = datatools::logger::PRIO_DEBUG;
    _basic_construct_ ();
    return;
  }

  // Destructor:
  model_factory::~model_factory ()
  {
    if (_locked_) {
      reset ();
    }
    return;
  }

  // 2012-05-25 FM : add support for loading a file that contains a list of geometry filenames :
  void model_factory::load_geom_list (const std::string & geom_list_file_)
  {
    string geom_lis_filename = geom_list_file_;
    datatools::fetch_path_with_env (geom_lis_filename);

    ifstream finlist (geom_lis_filename.c_str ());
    DT_THROW_IF (! finlist, logic_error,
                 "Cannot open file '" << geom_lis_filename << "' !");
    while (finlist) {
      string line;
      std::getline (finlist, line);
      DT_THROW_IF (! finlist, logic_error,
                   "I/O error while reading file '" << geom_lis_filename << "' !");
      string word;
      istringstream line_iss (line);
      line_iss >> word;
      if (word.length () < 1) {
        // skip blank line
        continue;
      }
      if (word[0] == '#') {
        continue;
      }
      string geom_filename = word;
      datatools::fetch_path_with_env (geom_filename);
      load (geom_filename);
      finlist >> ws;
      if (finlist.eof ()) {
        break;
      }
    }

  }

  void model_factory::load (const string & mprop_file_)
  {
    DT_THROW_IF (_locked_, logic_error, "Model factory is locked !");
    _mp_.read (mprop_file_);
    if (is_trace()) {
      DT_LOG_TRACE(_logging_priority_, "Model factory multi-configuration: ");
      _mp_.dump (std::cerr);
    }
    return;
  }

  void model_factory::_lock_ ()
  {
    DT_THROW_IF (_locked_, logic_error, "Model factory is locked !");
    _construct_ ();
    _mp_.reset ();
    return;
  }

  void model_factory::_unlock_ ()
  {
    return;
  }

  void model_factory::unlock ()
  {
    if (! _locked_) return;
    _unlock_ ();
    _locked_ = false;
    return;
  }

  void model_factory::lock ()
  {
    DT_THROW_IF (_locked_, logic_error, "Model factory is locked !");
    _lock_ ();
    _locked_ = true;
    return;
  }

  void model_factory::reset ()
  {
    if (_locked_) {
      unlock ();
    }
    // The container of logicals does not have ownership of the pointers :
    _logicals_.clear ();
    // Memory leak to be fixed:
    for (models_col_type::iterator i = _models_.begin ();
         i != _models_.end();
         i++)  {
      const string & model_name = i->first;
      i_model * model_ptr = i->second;
      if (model_ptr != 0) {
        DT_LOG_DEBUG(_logging_priority_,"Deleting registered model '" << model_name << "'.");
        delete model_ptr;
      }
    }
    _models_.clear ();
    _mp_.reset ();
    return;
  }

  /// Add a property prefix to be preserved in logicals
  void model_factory::add_property_prefix(const std::string & prefix_, int level_)
  {
    DT_THROW_IF (_property_prefixes_.find (prefix_) != _property_prefixes_.end(),
                 std::logic_error,
                 "Property prefix to be preserved '" << prefix_<< "' already exists !");
    _property_prefixes_[prefix_] = level_;
    return;
  }

  void model_factory::_construct_ ()
  {
    DT_LOG_TRACE(_logging_priority_,"Entering...");
    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i
           = _mp_.ordered_entries ().begin ();
         i != _mp_.ordered_entries ().end ();
         i++) {
      const datatools::multi_properties::entry * ptr_entry = *i;
      const datatools::multi_properties::entry & e = *ptr_entry;
      string model_name = e.get_key ();
      string model_type = e.get_meta ();

      if (! _factory_register_.has (model_type)) {
        DT_LOG_TRACE(_logging_priority_,
                     "No registered class with ID '"
                     << model_type << "' for model named '" << model_name << " !");
        continue;
      }
      const i_model::factory_register_type::factory_type & the_factory
        = _factory_register_.get (model_type);
      DT_LOG_TRACE(_logging_priority_,
                   "About to create a new model of type \"" << model_type
                   << "\" with name \"" << model_name << "\"...");
      i_model * model = the_factory ();
      model->construct (model_name, e.get_properties (), &_models_);
      for (std::map<std::string,int>::const_iterator i = _property_prefixes_.begin ();
           i != _property_prefixes_.end ();
           i++) {
        if (model->has_effective_logical()) {
          e.get_properties ().export_starting_with (model->grab_effective_logical ().grab_parameters (),
                                                    i->first);
        } else {
          e.get_properties ().export_starting_with (model->grab_logical ().grab_parameters (),
                                                    i->first);
        }
      }

      _models_[model_name] = model;
      DT_LOG_DEBUG(_logging_priority_,"Adding model '" << model_name << "'...");
      string log_name = model->get_logical ().get_name ();
      _logicals_[log_name] = &(model->get_logical ());
      if (model->has_effective_logical()) {
        string eff_log_name = model->get_effective_logical ().get_name ();
        _logicals_[eff_log_name] = &(model->get_effective_logical ());
      }
      DT_LOG_DEBUG(_logging_priority_,"New model is:");
      if (is_debug ()) model->tree_dump (clog,"");
    }
    DT_LOG_TRACE(_logging_priority_,"Exiting.");
    return;
  }

  void model_factory::tree_dump (ostream & out_,
                                 const string & title_,
                                 const string & indent_,
                                 bool inherit_) const
  {
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) {
      out_ << indent << title_ << endl;
    }


    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logging priority threshold       : \""
         << datatools::logger::get_priority_label(_logging_priority_) << "\"" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Locked  : " <<  (_locked_? "Yes": "No") << endl;

    // Configurations:
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Multi-properties : ";
      if ( _mp_.entries ().size () == 0) {
        out_ << "<empty>";
      }
      out_ << endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _mp_.tree_dump (out_, "", indent_oss.str ());
      }
    }

    // Logicals:
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Logicals : ";
      if ( _logicals_.size () == 0) {
        out_ << "<empty>";
      } else {
        out_ << "[" << _logicals_.size () << "]";
      }
      out_ << endl;
      for (logical_volume::dict_type::const_iterator i = _logicals_.begin ();
           i != _logicals_.end ();
           i++) {
        const string & key = i->first;
        const logical_volume * a_logical = i->second;
      }
    }

    // Models:
    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Geometry models : ";
      if ( _models_.size () == 0) {
        out_ << "<empty>";
      } else {
        out_ << "[" << _models_.size () << "]";
      }
      out_ << endl;
      for (models_col_type::const_iterator i = _models_.begin ();
           i != _models_.end ();
           i++) {
        const string & key = i->first;
        const i_model * a_model = i->second;
        ostringstream indent_oss;
        out_ << indent << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
        indent_oss << indent << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
        models_col_type::const_iterator j = i;
        j++;
        if (j == _models_.end ()) {
          out_ << datatools::i_tree_dumpable::inherit_tag (inherit_);
          indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
        } else {
          out_ << datatools::i_tree_dumpable::tag;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
        }
        out_ << "Model : " << '"' << key << '"' << endl;
        a_model->tree_dump (out_, "", indent_oss.str ());
      }
    }

    return;
  }

  bool model_factory::print_list_of_models(const geomtools::model_factory & mf_,
                                           std::ostream & out_,
                                           uint32_t options_)
  {
    out_ << std::endl
         << "List of available geometry models : " << std::endl;
    int count = 0;
    for (geomtools::models_col_type::const_iterator i
           = mf_.get_models ().begin ();
         i != mf_.get_models ().end ();
         i++) {
      bool long_name = false;
      size_t max_width = 32;
      if (i->second->get_name ().size () > max_width) {
        long_name = true;
      }
      if ((count % 2) == 0) {
        out_ << std::endl;
      }
      out_  << "  " << std::setw (max_width)
            << std::setiosflags(std::ios::left)
            << std::resetiosflags(std::ios::right)
            << i->second->get_name () << "  ";
      if (long_name) {
        out_ << std::endl;
        count = 0;
      }
      count++;
    }
    if ((count % 2) == 1)  {
      out_ << std::endl;
    }
    out_ << std::endl;
    return true;
  }

} // end of namespace geomtools


/****************************************************************/
// OCD support for class '::geomtools::model_factory' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::model_factory,ocd_)
{
  ocd_.set_class_name("geomtools::model_factory");
  ocd_.set_class_library("geomtools");
  ocd_.set_class_description("A virtual geometry model_factory");
  ocd_.set_class_documentation("The geometry model factory is the core engine of the\n"
                               "'geomtools' geometry system. The factory is able to \n"
                               "load the definitions of many 'geometry models' from \n"
                               "ASCII configuration files.                          \n"
                               "A geometry model is a special object that describes \n"
                               "a geometry volume and itsmain characteristics : shape,\n"
                               "materials, daughter volumes with their placements, and\n"
                               "also arbitrary properties that can be used at some point.\n"
                               "A geometry model must fulfil the interface of the \n"
                               "'geomtools::i_model' class.                       \n"
                               );


  ocd_.set_configuration_hints("A geometry model factory is configured through some configuration     \n"
                               "files that obeys the 'datatools::multi_properties' class.             \n"
                               "Each file contains a list of sections, one section per geometry model.\n"
                               "A geometry model object describes a possibly complex geometry volume  \n"
                               "with its shape, materials, daughter volumes and many other attributes \n"
                               "that can be used for various tasks (ex: display, GID mapping).        \n"
                               "A geometry model is dynamically instantiated and initialized/constructed\n"
                               "by the geometry factory.                                                \n"
                               );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::model_factory,"geomtools::model_factory")

// end of model_factory.cc
