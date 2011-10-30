// -*- mode: c++; -*- 
/* datatools/utils/properties.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-02-19
 * Last modified: 2011-03-09
 * 
 * License: 
 * 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 * 
 * Description: 
 *
 *   A simple properties dictionary
 * 
 * History: 
 * 
 */

#ifndef __datatools__utils__properties_h
#define __datatools__utils__properties_h 1

// standard includes:
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>

#include <datatools/serialization/i_serializable.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/i_clear.h>
#include <datatools/utils/i_cloneable.h>

//! \file datatools/utils/properties.h

using namespace std;

namespace datatools {

  namespace utils {

    //! \brief A dictionary of arbitrary properties.
    /**
     *  The properties class provides a container that holds
     *  many different data of different types (bool, int, double, string
     *  as well as arrays (std::vector) of these build-in types. 
     *
     */
    class properties : public datatools::serialization::i_serializable,
		       public datatools::utils::i_tree_dumpable,
		       public datatools::utils::i_clear,
		       public datatools::utils::i_cloneable 
    {

    public:

      //! Global static debug variable.
      static bool g_debug;

      //! Serialization tag.
      //static const std::string SERIAL_TAG;

      //! \brief Internal data stored within the dictionary of the properties class.
      class data : public datatools::serialization::i_serializable     
      {

      public:

	//! Serialization tag.
	//static const std::string SERIAL_TAG;

	static const int  ERROR_SUCCESS = 0;
	static const int  ERROR_FAILURE = 1;
	static const int  ERROR_BADTYPE = 2;
	static const int  ERROR_RANGE   = 3;
	static const int  ERROR_LOCK    = 4;

	static const char MASK_TYPE    = 0x7;
	static const char MASK_LOCK    = 0x40;
	static const char MASK_VECTOR  = 0x80;
      
	static const char TYPE_NONE    = 0x0;
	static const char TYPE_BOOLEAN = 0x1;
	static const char TYPE_INTEGER = 0x2;
	static const char TYPE_REAL    = 0x3;
	static const char TYPE_STRING  = 0x4;

	static const char TYPE_BOOLEAN_SYMBOL = 'B';
	static const char TYPE_INTEGER_SYMBOL = 'I';
	static const char TYPE_REAL_SYMBOL    = 'R';
	static const char TYPE_STRING_SYMBOL  = 'S';

	static const char STRING_FORBIDDEN_CHAR = '"';

	static const int  SCALAR_DEF    = -1;
	static const int  SCALAR_SIZE   =  1;

	static const bool        DEFAULT_VALUE_BOOLEAN;
	static const int         DEFAULT_VALUE_INTEGER;
	static const double      DEFAULT_VALUE_REAL;
	static const std::string DEFAULT_VALUE_STRING;
      
      public:

	typedef std::vector<bool>        vbool;
	typedef std::vector<int32_t>     vint;
	typedef std::vector<double>      vdouble;
	typedef std::vector<std::string> vstring;

      private:
	/*
	 * format: VL000TTT
	 *  V   == vector bit
	 *  L   == lock bit
	 *  TTT == type bits 
	 *  0   == unused
	 */
	uint8_t     _flags_; 
	std::string _description_;
	vbool       _boolean_values_;
	vint        _integer_values_;
	vdouble     _real_values_;
	vstring     _string_values_;

      private:
	void
	clear_values_ ();

	int
	init_values_ (char a_type = TYPE_INTEGER_SYMBOL, int a_size = SCALAR_DEF);

      public:

	void set_description (const std::string &);

	const std::string & get_description () const;

	bool has_type () const;

	bool is_boolean () const;

	bool is_integer () const;

	bool is_real () const;

	bool is_string () const;

	bool is_scalar () const;

	bool is_vector () const;

	bool is_locked () const;

	bool is_unlocked () const;

	int boolean (int a_size = SCALAR_DEF);

	int integer (int a_size = SCALAR_DEF);

	int real (int a_size = SCALAR_DEF);

	int string (int a_size = SCALAR_DEF);

	int lock ();

	int unlock ();

	int32_t get_size () const;

	int32_t size () const;

	bool index_is_valid (int) const;

	bool get_boolean_value (int = 0) const;

	int get_integer_value (int = 0) const;

	double get_real_value (int = 0) const;

	std::string get_string_value (int = 0) const;

	int set_value (bool, int = 0);

	int set_value (int, int = 0);

	int set_value (double, int = 0);

	int set_value (const std::string &, int = 0);

	int set_value (const char *, int = 0);

	int get_value (bool &, int = 0) const;

	int get_value (int &, int = 0) const;

	int get_value (double &, int = 0) const;

	int get_value (std::string &, int = 0) const;

	std::string get_type_label () const;

	std::string get_vector_label () const;

	static bool has_forbidden_char (const std::string &);

	//ctor
	data (char a_type = TYPE_INTEGER_SYMBOL ,
	      int a_size = SCALAR_DEF);

	data (bool a_value,
	      int a_size = SCALAR_DEF);

	data (int a_value,
	      int a_size = SCALAR_DEF);

	data (double a_value,
	      int a_size = SCALAR_DEF);

	data (const std::string & a_value,
	      int a_size = SCALAR_DEF);

	data (const char * a_value ,
	      int a_size = SCALAR_DEF);

	// dtor
	virtual ~data ();

	void
	dump (std::ostream &) const;

	void 
	to_string (std::ostream & a_out) const;

	static std::string
	get_error_message (int);

	//! Method for smart printing (from the datatools::utils::i_tree_dump interface).
	virtual void
	tree_dump (std::ostream & a_out         = std::clog, 
		   const std::string & a_title  = "",
		   const std::string & a_indent = "",
		   bool a_inherit               = false) const;

	//! Return the serialization tag (from the datatools::serialization::i_serializable interface).
	//virtual const std::string & get_serial_tag () const;

	DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()

      private:

	//! Boost.Serialization hook.
	friend class boost::serialization::access; 
        BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

      };

    public: 

      // typedefs declarations:
      typedef std::list<std::string>      vkeys;
      typedef std::map<std::string, data> pmap;
      typedef vkeys keys_col_t;
      typedef pmap  props_col_t;

      //! \brief Pure abstract class for key validator.
      struct basic_key_validator 
	: public std::unary_function<std::string,bool>
      {
	virtual bool operator () (const std::string & a_key_arg) const = 0;
      };

      //! \brief Default abstract class for key validator.
      class default_key_validator 
	: public basic_key_validator
      {
	
      public:

	static const std::string ALLOWED_CHARS;

	// ctor
	default_key_validator ();

	// dtor	
	virtual ~default_key_validator ();

	virtual bool
	operator () (const std::string & a_key_arg) const;

      };

    protected:

      static default_key_validator g_default_key_validator;

      // non static fields declarations:

    private: 

      bool                        _debug_;
      std::string                 _description_;
      pmap                        _props_;
      const basic_key_validator * _key_validator_;
      bool                        _key_validator_deletion_;

      // getters/setters declarations:

    private: 

      void
      _validate_key_ (const std::string & a_key_arg) const;

      void
      _clear_key_validator_ ();

    public: 
      static const std::string PRIVATE_PROPERTY_PREFIX;

      bool
      is_debug () const;

      void
      set_debug (bool);

      //size_t
      int32_t
      size () const;

      void
      set_description (const std::string &);

      const std::string &
      get_description () const;
  
    public: 

      void
      unset_key_validator ();

      void
      set_default_key_validator ();

      void
      set_key_validator (const basic_key_validator &);

      void
      set_key_validator (const basic_key_validator *, 
			 bool a_deletion_on_destroy = true); 

      // ctor
      properties (); // with embedded default key validator

      properties (const std::string & a_desc); // with embedded default key validator

      properties (const std::string & a_desc, 
		  const basic_key_validator &);

      properties (const basic_key_validator &); 

      /* with external key validator (deletion_on_destroy_==false)
       * with internal key validator (deletion_on_destroy_==true) 
       *      validator is deleted in the destructor. 
       */

      properties (const std::string & a_desc, 
		  const basic_key_validator *, 
		  bool deletion_on_destroy_ = true); 
      
      properties (const basic_key_validator *, 
		  bool deletion_on_destroy_ = true); 

      // dtor
      virtual ~properties ();
  
      // methods:

    private: 

      void
      _check_nokey_ (const std::string & a_key) const;

      void
      _check_key_ (const std::string & a_key,
		   data ** a_data);

      void
      _check_key_ (const std::string & a_key,
		   const data ** a_data) const;


    public: 

      //! Returns the list of keys stored in the map (read-only).  
      vkeys keys () const;

      //! Set the list of keys. 
      void
      keys (vkeys &) const;

      //! returns the list of keys stored in the map that start with a_key_prefix. 
      void
      keys_not_starting_with (vkeys &, const std::string & a_key_prefix) const;

      vkeys
      keys_not_starting_with (const std::string & a_key_prefix) const;

      //! returns the list of keys stored in the map that start with a_key_prefix. 
      void
      keys_starting_with (vkeys &, const std::string & a_key_prefix) const;

      vkeys
      keys_starting_with (const std::string & a_key_prefix) const;

      //! returns the list of keys stored in the map that end with a_key_suffix. 
      void
      keys_not_ending_with (vkeys &, const std::string & a_key_suffix) const;

      vkeys
      keys_not_ending_with (const std::string & a_key_suffix) const;

      //! returns the list of keys stored in the map that end with a_key_suffix. 
      void
      keys_ending_with (vkeys &, const std::string & a_key_suffix) const;

      vkeys
      keys_ending_with (const std::string & a_key_suffix) const;

      //! Lock the properties dictionary. 
      void lock (const std::string & a_key);

      //! Unlock the properties dictionary. 
      void unlock (const std::string & a_key);

      //! Check if the instance is locked.
      bool is_locked (const std::string & a_key) const;

      //! Return a static key.
      static std::string make_private_key (const std::string & a_key);

      //! Check if a string matches a private 'a_key'.
      static bool key_is_private (const std::string & a_key);

      //! Check if a string matches a public 'a_key'.
      static bool key_is_public (const std::string & a_key);

      //! Check if data with name 'a_key' is private.
      bool is_private (const std::string & a_key) const;

      //! Check if data with name 'a_key' is public.
      bool is_public (const std::string & a_key) const;

      //! Check if data with name 'a_key' is boolean.
      bool is_boolean (const std::string & a_key) const;

      //! Check if data with name 'a_key' is integer.
      bool is_integer (const std::string & a_key) const;

      //! Check if data with name 'a_key' is real.
      bool is_real (const std::string & a_key) const;

      //! Check if data with name 'a_key' is string.
      bool is_string (const std::string & a_key) const;
      
      //! Check if data with name 'string' is scalar.
      bool is_scalar (const std::string & a_key) const;

      bool is_vector (const std::string & a_key) const;

      int32_t size (const std::string & a_key) const;

      bool has_key (const std::string & a_key) const;

      //! Erase property with name a_key.
      void erase (const std::string & a_key);

      //! Erase all properties.
      void erase_all ();

      //! Erase all properties starting with a_key_prefix
      void erase_all_starting_with (const std::string & a_key_prefix);

      //! Erase all properties not starting with a_key_prefix
      void erase_all_not_starting_with (const std::string & a_key_prefix);

      void export_starting_with (properties & a_props, 
				 const std::string & a_key_prefix) const;

      void export_and_rename_starting_with (properties & a_props, 
					    const std::string & a_key_prefix,
					    const std::string & a_new_prefix) const;

      void export_not_starting_with (properties & a_props, 
				     const std::string & a_key_prefix) const;
      
      //! Erase all properties ending with a_key_suffix
      void erase_all_ending_with (const std::string & a_key_suffix);
      
      //! Erase all properties not ending with a_key_suffix
      void erase_all_not_ending_with (const std::string & a_key_suffix);
      
      void export_ending_with (properties & a_props, 
			       const std::string & a_key_suffix) const;
      
      void export_not_ending_with (properties & a_props, 
				   const std::string & a_key_suffix) const;

      void clean (const std::string & a_key);

      //! Reset method (from the datatools::utils::i_clear interface).
      virtual void clear ();

      //! Reset method
      void reset ();
 
      void store_flag (const std::string & a_key, 
		       const std::string & a_desc = "",
		       bool a_lock = false);

      void set_flag (const std::string & a_key);

      void unset_flag (const std::string & a_key);
 
      void store (const std::string & a_key, 
		  bool a_value, 
		  const std::string & a_desc = "",
		  bool a_lock = false);

      void store_boolean (const std::string & a_key, 
			  bool a_value, 
			  const std::string & a_desc = "",
			  bool a_lock = false);

      void store (const std::string & a_key, 
		  int a_value, 
		  const std::string & a_desc = "",
		  bool a_lock = false);
      
      void store_integer (const std::string & a_key, 
			  int a_value, 
			  const std::string & a_desc = "",
			  bool a_lock = false);

      void store (const std::string & a_key, 
		  double a_value, 
		  const std::string & a_desc = "",
		  bool a_lock = false);

      void store_real (const std::string & a_key, 
		       double a_value, 
		       const std::string & a_desc = "",
		       bool a_lock = false);

      void store (const std::string & a_key, 
		  const std::string & a_value, 
		  const std::string & a_desc = "",
		  bool a_lock = false);

      void store_string (const std::string & a_key, 
			 const std::string & a_value, 
			 const std::string & a_desc = "",
			 bool a_lock = false);
 
      void store (const std::string & a_key, 
		  const char * a_value, 
		  const std::string & a_desc = "",
		  bool a_lock = false);
      
      void store (const std::string & a_key, 
		  const data::vbool & a_values, 
		  const std::string & a_desc = "",
		  bool a_lock = false);
      
      void store (const std::string & a_key, 
		  const data::vint & a_values, 
		  const std::string & a_desc = "",
		  bool a_lock = false);
      
      void store (const std::string & a_key, 
		  const data::vdouble & a_values, 
		  const std::string & a_desc = "",
		  bool a_lock = false);

      void store (const std::string & a_key, 
		  const data::vstring & a_values, 
		  const std::string & a_desc = "",
		  bool a_lock = false);
      
      void change (const std::string & a_key, 
		   bool a_value, 
		   int a_index = 0);

      void change_boolean (const std::string & a_key , 
			   bool a_value , 
			   int a_index = 0);
      
      void change (const std::string & a_key , 
		   int a_value , 
		   int a_index = 0);
      
      void change_integer (const std::string & a_key , 
			   int a_value , 
			   int a_index = 0);
      
      void change (const std::string & a_key, 
		   double a_value, 
		   int a_index = 0);
      
      void change_real (const std::string & a_key , 
			double a_value , 
			int a_index = 0);
      
      void change (const std::string & a_key, 
		   const std::string & a_value, 
		   int a_index = 0);
      
      void change_string (const std::string & a_key , 
			  const std::string & a_value , 
			  int a_index = 0);
      
      void change (const std::string & a_key, 
		   const char * a_value, 
		   int a_index = 0);

      void change (const std::string & a_key, 
		   const data::vbool & a_values);

      void change (const std::string & a_key, 
		   const data::vint & a_values);

      void change (const std::string & a_key, 
		   const data::vdouble & a_values);

      void change (const std::string & a_key, 
		   const data::vstring & a_values);

      /*****/

      void update_flag (const std::string & a_key);

      void update (const std::string & a_key, 
		   bool a_value);

      void update_boolean (const std::string & a_key, 
			   bool a_value);

      void update (const std::string & a_key , 
		   int a_value);

      void update_integer (const std::string & a_key , 
			   int a_value);

      void update (const std::string & a_key, 
		   double a_value);

      void update_real (const std::string & a_key, 
			double a_value);

      void update (const std::string & a_key, 
		   const std::string & a_value);

      void update_string (const std::string & a_key, 
			  const std::string & a_value);

      void update (const std::string & a_key, 
		   const data::vbool & a_values);

      void update (const std::string & a_key, 
		   const data::vint & a_values);

      void update (const std::string & a_key, 
		   const data::vdouble & a_values);

      void update (const std::string & a_key, 
		   const data::vstring & a_values);

      /*****/ 

      bool has_flag (const std::string & a_key) const;

      void fetch (const std::string & a_key, 
		  bool   & a_value, 
		  int a_index = 0) const;
      
      void fetch (const std::string & a_key, 
		  int    & a_value, 
		  int a_index = 0) const;
      
      void fetch (const std::string & a_key, 
		  double & a_value, 
		  int a_index = 0) const;
      
      void fetch (const std::string & a_key, 
		  std::string & a_value, 
		  int a_index = 0) const;
      
      void fetch (const std::string & a_key , 
		  data::vbool & a_values) const;
      
      void fetch (const std::string & a_key, 
		  data::vint & a_values) const;
      
      void fetch (const std::string & a_key,
		  data::vdouble & a_values) const;
      
      void fetch (const std::string & a_key, 
		  data::vstring & a_values) const;
      
      bool fetch_boolean (const std::string &,
			  int a_index = 0) const;
      
      bool fetch_boolean_scalar (const std::string & a_name) const;
 
      bool fetch_boolean_vector (const std::string & a_name , int a_index) const;
 
      int fetch_integer (const std::string &,
			 int a_index = 0) const;

      int fetch_integer_scalar (const std::string & a_name) const;

      int fetch_integer_vector (const std::string & a_name , int a_index) const;

      double fetch_real (const std::string &,
			 int a_index = 0) const;

      double fetch_real_scalar (const std::string & a_name) const;

      double fetch_real_vector (const std::string & a_name , int a_index) const;

      std::string fetch_string (const std::string &,
				int a_index = 0) const;

      std::string fetch_string_scalar (const std::string & a_name) const;

      std::string fetch_string_vector (const std::string & a_name , int a_index) const;

      void dump (std::ostream & = clog) const;

      void dump_def () const;

      void dump_stderr () const;

      void dump_stdout () const;

      virtual void tree_dump (std::ostream & a_out         = std::clog, 
			      const std::string & a_title  = "",
			      const std::string & a_indent = "",
			      bool a_inherit               = false) const;
      
      std::string  key_to_string (const std::string & a_key) const;

      std::string  key_to_property_string (const std::string & a_key) const;


      //! From the datatools::utils::i_cloneable interface :
      DATATOOLS_CLONEABLE_DECLARATION (properties)

      //! From the datatools::serialization::i_serializable interface :
      //virtual const std::string & get_serial_tag () const;
      DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()

    private:

      friend class boost::serialization::access; 
      BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

    public:

      void export_to_string_based_dictionary (map<string, string> & a_dict, 
					      bool a_quoted_strings = true) const;

    public:

      friend class config;

      class config
      {

      public:

	static const char DEFAULT_CONTINUATION_CHAR    = '\\';
	static const char DEFAULT_COMMENT_CHAR = '#';
	static const char DEFAULT_ASSIGN_CHAR  = '=';
	static const char DEFAULT_DESC_CHAR    = ':';
	static const char OPEN_VECTOR          = '[';
	static const char CLOSE_VECTOR         = ']';

	static const int MODE_BARE          = 0;
	static const int MODE_HEADER_FOOTER = 1;
	static const int MODE_DEFAULT       = MODE_HEADER_FOOTER;
	static const int mode_header_footer = MODE_HEADER_FOOTER;
	static const int mode_bare          = MODE_BARE;
	
	static const bool write_private_also;
	static const bool write_public_only;

      private:

	bool   _debug_;
	int    _mode_;
	bool   _use_smart_modulo_;
	bool   _write_public_only_;
	size_t _read_line_count_;
	char   _continuation_char_;
	char   _comment_char_;
	char   _assign_char_;
	char   _desc_char_;

      private:

	void read_ (std::istream & a_in, properties & a_prop);
	
      public:
	
	bool is_debug () const;
	
	void set_debug (bool a_debug);

	// ctor
	config (bool a_use_smart_modulo = true,
		int a_mode = MODE_DEFAULT,
		bool a_write_public_only = write_public_only);

	// dtor
	virtual ~config ();

	void read (std::istream & a_in, properties & a_prop);
	
	void write (std::ostream & a_out, const properties & a_prop);
       
	static bool read_quoted_string (std::istream &,
					std::string &);


      };

      static void write_config (const std::string & a_filename, 
				const properties & a_prop,
				bool a_use_smart_modulo = true,
				bool a_write_public_only = true);
      
      static void  read_config (const std::string & a_filename, 
				properties & a_props);

      static std::string build_property_key (const std::string & a_prefix,
					     const std::string & a_subkey);
      
    };

  } // end of namespace utils 

} // end of namespace datatools 

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(datatools::utils::properties, "datatools::utils::properties")

#endif // __datatools__utils__properties_h

// end of properties.h
