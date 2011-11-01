// -*- mode: c++; -*- 
/* color.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-23
 * Last modified: 2010-03-24
 * 
 * License: 
 * 
 * Description: 
 *  Utilities for color manipulation:
 *
 *
 *
 * History: 
 * 
 */

#ifndef __geomtools__color_h
#define __geomtools__color_h 1

#include <iostream>
#include <string>
#include <map>

#include <boost/scoped_ptr.hpp>

namespace geomtools {

  using namespace std;

  class color
  {
  public:

    static const string white;
    static const string black;
    static const string red;
    static const string green;
    static const string blue;
    static const string magenta;
    static const string cyan;
    static const string yellow;
    static const string orange;
    static const string grey;
    static const string default_color;

  public:
    int    code;
    string name;
    int    red_amount, green_amount, blue_amount;

  public:
    // ctor:
    color ();

    // ctor:
    color (int, const string &, int, int, int);

    /*** general color map ***/
    typedef map<string, int> color_map_type;

    class color_db
    {
    public:
      
      color_db ();
      
      virtual ~color_db ();

      //bool has_color (const string & id_) const;
      
      int get_color (const string & a_color_id) const;
      
      //void register_color (color, const string & id_);
      
      //void dump_colors (ostream & out_ = clog);

    private:

      color_map_type map_of_colors_;
     
    };
    
    typedef boost::scoped_ptr<color_db> scoped_color_db_type;
 
  private:

    static scoped_color_db_type _g_color_db_;
    
  public:
    
    static const color_db & get_color_db ();

    static int get_color (const string & a_name);

  };

}  // end of namespace geomtools

#endif // __geomtools__color_h

// end of color.h
