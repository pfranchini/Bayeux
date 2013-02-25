/* manager.cc */

// Ourselves
#include <genbb_help/manager.h>
#include <genbb_help/i_genbb.h>

// Standard Library
#include <stdexcept>
#include <sstream>

// Third Party

// Datatools
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>

namespace genbb {

  //----------------------------------------------------------------------
  // Public Interface Definitions
  //

  bool manager::is_debug() const {
    return _debug_;
  }


  void manager::set_debug(bool debug) {
    _debug_ = debug;
  }


  bool manager::is_initialized() const {
    return _initialized_;
  }


  void manager::load(const std::string& name,
                     const std::string& id,
                     const datatools::properties& config) {
    this->_load_pg(name, id, config);
  }


  void manager::load(const datatools::multi_properties& config) {
    bool debug = this->is_debug();
    if (debug) {
      std::clog << datatools::io::debug
                << "genbb::manager::load: "
                << "Entering..."
                << std::endl;
    }

    if (this->is_initialized()) {
      std::ostringstream message;
      message << "manager::load: " 
              << "Particle generator manager is already initialized !";
      throw std::logic_error(message.str());
    }


    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i 
           = config.ordered_entries().begin();
         i != config.ordered_entries().end();
         ++i) {
      datatools::multi_properties::entry* mpe = *i;
      const std::string& pg_name = mpe->get_key();
      const std::string& pg_id = mpe->get_meta();
      if (debug) {
        std::clog << datatools::io::debug
                  << "genbb::manager::load: "
                  << "Configuration for pg named '" 
                  << pg_name << "' "
                  << " with ID '" 
                  << pg_id << "'..."
                  << std::endl;
      }
      this->_load_pg(pg_name, pg_id, mpe->get_properties());
    }
    return;
  }


  void manager::initialize(const datatools::properties& config) {
    if (this->is_initialized()) {
      std::ostringstream message;
      message << "genbb::manager::initialize: "
              << "Manager is already initialized !";
      throw std::logic_error(message.str());
    }

    if (!this->is_debug()) {
      if (config.has_flag("debug")) {
        this->set_debug(true);
      }
    }

    // Particle generators :
    {
      typedef std::vector<std::string> CFList;
      typedef CFList::iterator CFListIterator;
      std::string conf_file_key("generators.configuration_files");

      CFList conf_file_list;

      if (config.has_key(conf_file_key)) {
        config.fetch(conf_file_key, conf_file_list);
      }

      for (CFListIterator i = conf_file_list.begin();
           i < conf_file_list.end();
           ++i) {
        datatools::fetch_path_with_env(*i);
        datatools::multi_properties mconfig;
        mconfig.read(*i);
        this->load(mconfig);
      }
    }

    _initialized_ = true;
    return;
  }


  void manager::reset() {
    if (this->is_debug()) {
      std::clog << datatools::io::debug
                << "genbb::manager::reset: "
                << "Entering..."
                << std::endl;
    }

    if (!_initialized_) {
      std::ostringstream message;
      message << "genbb::manager::reset: "
              << "Manager is not initialized !";
      throw std::logic_error(message.str());
    }

    size_t count = _particle_generators_.size();
    size_t initial_size = _particle_generators_.size();
    while (_particle_generators_.size() > 0) {
      for (detail::pg_dict_type::iterator it = _particle_generators_.begin();
           it != _particle_generators_.end();
           ++it) {
        detail::pg_entry_type& entry = it->second;
        if (this->is_debug()) {
          std::clog << datatools::io::debug
                    << "genbb::manager::reset: "
                    << "Removing particle generator '" 
                    << entry.get_name ()
                    << "'..."
                    << std::endl;
        }
        this->_reset_pg(entry);
        _particle_generators_.erase(it);
        --count;
        break;
      }
      if (count == initial_size) {
        break;
      }
    }

    if (_particle_generators_.size() > 0) {
      std::clog << datatools::io::warning
                << "genbb::manager::reset: "
                << "There are some left particle generators !"
                << std::endl;
    }
    _particle_generators_.clear();
    _factory_register_.reset();
    _force_initialization_at_load_ = false;
    _preload_ = true;

    _initialized_ = false;
    if (this->is_debug()) {
      std::clog << datatools::io::debug
                << "genbb::manager::reset: "
                << "Exiting."
                << std::endl;
    }
    return;
  }

  
  bool manager::has_service_manager() const
  {
    return _service_mgr_ != 0;
  }

  void manager::reset_service ()
  {
    _service_mgr_ = 0;
    return;
  }
  
  void manager::set_service_manager (datatools::service_manager & service_mgr_)
  {
    _service_mgr_ = &service_mgr_;
    return;
  }

  // Constructor :
  manager::manager(uint32_t flag)
    : _factory_register_("genbb::i_genbb/pg_factory",
                        flag & VERBOSE ? 
                        i_genbb::factory_register_type::verbose : 0) {
    _service_mgr_ = 0;
    _initialized_ = false;
    this->set_debug(flag & DEBUG);
    _force_initialization_at_load_ = false;

    if (flag & FORCE_INITIALIZATION_AT_LOAD) {
      _force_initialization_at_load_ = true;
    }

    bool preload = true;
    if (flag & NO_PRELOAD) {
      preload = false;
    }

    this->_set_preload_(preload);
    return;
  }


  // Destructor :
  manager::~manager () {
    if (_initialized_) this->reset();
    return;
  }


  /***************************
   *   Particle generators   *
   ***************************/

  bool manager::has(const std::string& name) const {
    return _particle_generators_.find(name) != _particle_generators_.end();
  }

  bool manager::is_initialized(const std::string& name) const {
    detail::pg_dict_type::const_iterator found = _particle_generators_.find(name);
    return found != _particle_generators_.end() && found->second.is_initialized();
  }

  bool manager::can_drop(const std::string& name) const {
    detail::pg_dict_type::const_iterator found = _particle_generators_.find(name);
    if (found == _particle_generators_.end()) {
      std::ostringstream message;
      message << "genbb::manager::can_drop: "
              << "Particle generator '" 
              << name 
              << "' does not exist !";
      throw std::logic_error(message.str());
    }
    return true;
  }


  void manager::drop(const std::string& name) {
    detail::pg_dict_type::iterator found = _particle_generators_.find(name);
    if (found == _particle_generators_.end()) {
      std::ostringstream message;
      message << "genbb::manager::drop: "
              << "Particle generator '" 
              << name 
              << "' does not exist !";
      throw std::logic_error(message.str());
    }

    detail::pg_entry_type& entry = found->second;
    // if (!found->second.can_be_dropped()) {
    //   std::ostringstream message;
    //   message << "genbb::manager::drop: "
    //           << "Particle generator '" 
    //           << name
    //           << "' cannot be dropped !";
    //   throw std::logic_error(message.str());
    // }

    if (this->is_debug()) {
      std::clog << datatools::io::debug
                << "genbb::manager::drop: "
                << "Reset & remove particle generator '" 
                << name 
                << "' !"
                << std::endl;
    }

    this->_reset_pg(entry);
    _particle_generators_.erase(found);
    return;
  }


  void manager::dump_particle_generators(std::ostream& out,
                              const std::string& title,
                              const std::string& an_indent) const {
    std::string indent;
    if (!an_indent.empty()) indent = an_indent;

    if (!title.empty()) out << title << ":" << std::endl;

    // Particle generators:
    {
      size_t sz = _particle_generators_.size();
      size_t count = 0;
      for (detail::pg_dict_type::const_iterator it = _particle_generators_.begin();
           it != _particle_generators_.end();
           ++it) {
        count++;
        out << indent;
        if (count == sz) {
          out << "`-- ";
        } else {
          out << "|-- ";
        }
        const std::string& pg_name = it->first;
        const detail::pg_entry_type& pg_record = it->second;
      
        out << "Name : '" << pg_name << "' "
            << "Type : '" << pg_record.get_id () << "' ";
        out << '(';
        int count = 0;
        if (pg_record.is_initialized()) {
          out << "initialized";
          count++;
        }
        out << ')';
        out << std::endl;
      }
    }
    return;
  }

  bool manager::has_pg_type(const std::string& id) const {
    return _factory_register_.has(id);
  }


  void manager::unregister_pg_type(const std::string& id) {
    _factory_register_.unregistration(id);
    return;
  }

  void manager::tree_dump(std::ostream& out,
                          const std::string& title,
                          const std::string& a_indent,
                          bool inherit) const {
    std::string indent;
    if (!a_indent.empty()) indent = a_indent;

    if (!title.empty()) out << indent << title << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Debug          : " 
        << _debug_ << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Preload        : " 
        << _preload_ 
        << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Force initialization : " 
        << _force_initialization_at_load_ 
        << std::endl;

    out << indent << i_tree_dumpable::tag
        << "List of registered particle generators : " << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent << i_tree_dumpable::skip_tag;

      _factory_register_.print(out, indent_oss.str());
    }

    {
      out << indent << i_tree_dumpable::tag
          << "Particle generators       : ";
      size_t sz = _particle_generators_.size();
      if (sz == 0) {
        out << "<none>";
      }

      out << std::endl;
      for (detail::pg_dict_type::const_iterator i = _particle_generators_.begin();
           i != _particle_generators_.end();
           ++i) {
        const std::string& pg_name = i->first;
        const detail::pg_entry_type& pg_entry = i->second;
        out << indent << i_tree_dumpable::skip_tag;

        std::ostringstream indent_oss;
        indent_oss << indent << i_tree_dumpable::skip_tag;
        detail::pg_dict_type::const_iterator j = i;
        j++;
        if (j == _particle_generators_.end()) {
          out << i_tree_dumpable::last_tag;
          indent_oss << i_tree_dumpable::last_skip_tag;
        } else {
          out << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }
        out << "Particle generator : '" << pg_name << "'" << std::endl;
        pg_entry.tree_dump(out, "", indent_oss.str());
      }
    }

    out << indent << i_tree_dumpable::inherit_tag(inherit)
        << "Initialized    : " 
        << this->is_initialized() 
        << std::endl;
  }


  //----------------------------------------------------------------------
  // Protected Interface Definitions
  //

  void manager::_load_pg(const std::string& name,
                             const std::string& id,
                             const datatools::properties& config) {
    bool debug = this->is_debug();
    if (debug) {
      std::clog << datatools::io::debug
                << "genbb::manager::_load_pg: "
                << "Entering..."
                << std::endl;
    }
    if (this->has(name)) {
      std::ostringstream message;
      message << "genbb::manager::_load_pg: "
              << "Particle generator '" 
              << name
              << "' already exists !";
      throw std::logic_error(message.str());
    }

    {
      // Add a new entry :
      detail::pg_entry_type tmp_entry;
      tmp_entry.set_name(name);
      if (debug) {
        std::clog << datatools::io::debug
                  << "genbb::manager::_load_pg: "
                  << "Add an entry for particle generator '" 
                  << name 
                  << "'..."
                  << std::endl;
      }
      _particle_generators_[name] = tmp_entry;
    }
    // fetch a reference on it and update :
    detail::pg_entry_type& new_entry = _particle_generators_.find(name)->second;
    new_entry.set_id(id);
    new_entry.set_config(config);

    if (debug) {
      std::clog << datatools::io::debug
                << "genbb::manager::_load_pg: "
                << "Fetch..."
                << std::endl;
    }

    this->_create_pg(new_entry);
    if (_force_initialization_at_load_) {
      this->_initialize_pg(new_entry);
    }

    if(debug) {
      std::clog << datatools::io::debug
                << "genbb::manager::_load_pg: "
                << "Exiting."
                << std::endl;
    }
    return;
  }


  void manager::_preload_global_dict() {
    bool devel = false;
    if (devel) {
      std::clog << datatools::io::devel
                << "genbb::manager::_preload_global_dict: "
                << "Entering..."
                << std::endl;
    }
    _factory_register_.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(genbb::i_genbb));
    return;
  }


  void manager::_create_pg(detail::pg_entry_type& entry) {
    if (!entry.is_created()) {
      if (this->is_debug()) {
        std::clog << datatools::io::debug
                  << "genbb::manager::_create_pg: "
                  << "Creating particle generator named '"
                  <<  entry.get_name()
                  << "'..."
                  << std::endl;
      }

      // search for the PG's label in the factory dictionary:
      if (!_factory_register_.has(entry.get_id())) {
        std::ostringstream message;
        message << "genbb::manager::_create_pg: "
                << "Cannot find PG factory with ID '"
                << entry.get_id() 
                << "' for particle generator named '"
                << entry.get_name() << "' !";
        throw std::logic_error(message.str());
      }

      typedef 
        genbb::i_genbb::factory_register_type::factory_type FactoryType;

      const FactoryType& the_factory = _factory_register_.get(entry.get_id());

      i_genbb* ptr = the_factory();

      entry.grab_handle().reset(ptr);
      entry.update_status(detail::pg_entry_type::STATUS_CREATED);

      if (this->is_debug()) {
        std::clog << datatools::io::debug
                  << "genbb::manager::_create_pg: "
                  << "Particle generator named '"
                  <<  entry.get_name()
                  << "' has been created !"
                  << std::endl;
      }
    }
    return;
  }


  void manager::_initialize_pg(detail::pg_entry_type& entry) {
    // If not created, first do it :
    if (!entry.is_created()) {
      this->_create_pg(entry);
    }

    // If not initialized, do it :
    if (!entry.is_initialized()) {
      if (this->is_debug()) {
        std::clog << datatools::io::debug
                  << "genbb::manager::_initialize_pg: "
                  << "Initializing particle generator named '"
                  << entry.get_name()
                  << "'..."
                  << std::endl;
      }
      i_genbb& the_pg = entry.grab_handle().grab();
      if (has_service_manager())
        {
          the_pg.initialize(entry.get_config(), 
                            *_service_mgr_, 
                            _particle_generators_);
        }
      else
        {
          the_pg.initialize_with_dictionary_only(entry.get_config(), 
                                                 _particle_generators_);
        }
      entry.update_status(detail::pg_entry_type::STATUS_INITIALIZED);
    }
  }


  void manager::_reset_pg(detail::pg_entry_type& entry) {
    if (entry.is_initialized()) {
      i_genbb& the_pg = entry.grab_handle().grab();
      the_pg.reset();
      entry.reset_status(detail::pg_entry_type::STATUS_INITIALIZED);
    }
    return;
  }


  //----------------------------------------------------------------------
  // Private Interface Definitions

  void manager::_set_preload_(bool preload) {
    _preload_ = preload;
    if (_preload_) {
      this->_preload_global_dict();
    }
    return;
  }


}  // end of namespace genbb

