// -*- mode: c++; -*- 
//! \file datatools/event/basic_event.h 
/* basic_event.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-02-25
 * Last modified: 2008-02-25
 * 
 * License: 
 *  
 * Description: 
 *
 *   A generic basic event model
 *
 * History: 
 * 
 */

#ifndef __datatools__event__basic_event_h
#define __datatools__event__basic_event_h 1

// standard includes:
#include <iostream>
#include <string>

#include <boost/cstdint.hpp>

#include <datatools/serialization/i_serializable.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/i_clear.h>

#include <datatools/utils/properties.h>
#include <datatools/event/event_id.h>

namespace datatools {

  namespace event {
    
    /*! \brief A simple basic event model.
     *
     *  The basic_event class is serializable through the Boost.Serialization library.
     *  It owns an event_id instance and a properties instance.
     *
     *  Sample program: #programs/test_basic_event.cxx 
     *
     */
    class basic_event : public datatools::serialization::i_serializable,
                        public datatools::utils::i_tree_dumpable,
                        public datatools::utils::i_clear    
    {

    public: 

      //! Reset method (from the datatools::utils::i_clear interface).
      virtual void clear ();

      //! Set the event ID.
      void id (const event_id & a_id);

      //! Set the event ID.
      void set_id (const event_id & a_id);

      //! Get the event ID (read-only).
      const event_id & id () const;

      //! Get the event ID (read-only).
      const event_id & get_id () const;

      //! Get the event ID (left value).
      event_id & id ();

      //! Get the properties (read-only)..
      const datatools::utils::properties & properties () const;

      const datatools::utils::properties & get_properties_ro () const;

      //! Get the properties (left value).
      datatools::utils::properties & properties ();

      datatools::utils::properties & get_properties_rw ();

    public: 

      //! Default constructor.
      basic_event ();
      
      //! Constructor on event ID.
      basic_event (const event_id & a_id);
      
      //! Destructor.
      virtual ~basic_event ();
      
      //! Method for smart printing (from the datatools::utils::i_tree_dump interface).
      virtual void tree_dump (std::ostream & a_out         = std::cerr, 
                              const std::string & a_title  = "",
                              const std::string & a_indent = "",
                              bool a_inherit               = false) const;
      
      //! Shortcut to tree_dump.
      void dump () const;

      //! Return the serialization tag (from the datatools::serialization::i_serializable interface).
      DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()

      private:

      //! Boost.Serialization hook.
      friend class boost::serialization::access; 
      BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

      private:

      event_id                     _id_;         //!< Event ID.
      datatools::utils::properties _properties_; //!< Dictionary of properties.
     
    };

  } // end of namespace event 

} // end of namespace datatools

#endif // __datatools__event__basic_event_h

// end of basic_event.h
