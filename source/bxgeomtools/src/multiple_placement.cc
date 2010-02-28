// -*- mode: c++; -*- 
/* multiple_placement.cc 
 */

#include <geomtools/multiple_placement.h>

namespace geomtools {

  using namespace std;

  bool multiple_placement::is_replica () const
  {
    return false;
  }
 
  bool multiple_placement::is_valid () const
  {
    return __placements.size () > 0;
  }

  void multiple_placement::invalidate ()
  {
    reset ();
  }

  void multiple_placement::add (const placement & p_)
  {
    __placements.push_back (p_);
  }
  
  const placement & multiple_placement::get_placement (int index_) const
  {
    return __placements.at (index_);
  }
  
  placement & multiple_placement::get_placement (int index_)
  {
    return __placements.at (index_);
  }
  
  size_t multiple_placement::get_number_of_items () const
  {
    return __placements.size ();
  }
  
  void multiple_placement::get_placement (int item_, placement & p_) const
  {
    p_ = __placements[item_];
  }
  
  // ctor:  
  multiple_placement::multiple_placement ()
  {
  }
  
  // dtor:
  multiple_placement::~multiple_placement ()
  {
    reset ();
  }
  
  void multiple_placement::reset ()
  {
    __placements.clear ();
  }
    
  void multiple_placement::tree_dump (ostream & out_, 
				      const string & title_, 
				      const string & indent_, 
				      bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    this->i_placement::tree_dump (out_, title_, indent, true);


    out_ << indent << i_tree_dumpable::tag  
	 << "Size    :" << get_number_of_items () << endl;

    out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	 << "Replica :" << is_replica () << endl;

    return;
  }
 

} // end of namespace geomtools

// end of multiple_placement.cc

