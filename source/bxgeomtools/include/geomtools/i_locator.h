// -*- mode: c++ ; -*-
/* i_locator.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-09
 * Last modified: 2011-04-13
 *
 * License:
 *
 * Description:
 *   Geometry locator abstract interface
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_LOCATOR_H_
#define GEOMTOOLS_I_LOCATOR_H_ 1

#include <geomtools/i_object_3d.h>
#include <geomtools/geom_id.h>

namespace geomtools {

  class geom_info;

  /**! An abstract interface for simple locators that build the geometry ID
   *   associated to some position for some geometry type.
   */
  class i_base_locator
  {
    virtual bool find_geom_id (const vector_3d & a_position,
                               int a_type,
                               geom_id & a_gid,
                               double a_tolerance = i_object_3d::USING_PROPER_TOLERANCE) const = 0;

  };

  /**! An abstract interface for locators based on some internal lookup table.
   *   Its methods return some reference to some stored geometrical meta data.
   */
  class i_locator
  {

  public:

    /** Given a geometry ID, check if it is valid (known within some internal table).
     */
    virtual bool validate_id (const geom_id & a_id) const = 0;

    /** Given a geometry ID, returns the associated geom_info instance (from some internal table).
     */
    virtual const geom_info & get_geom_info (const geom_id & a_id) const = 0;


    /** Given a position and the type of some geometry volume,
     *  returns the associated geom_id instance (from some internal table).
     */
    virtual const geom_id & get_geom_id (const vector_3d & a_position,
                                         int a_type = geom_id::INVALID_TYPE,
                                         double a_tolerance = i_object_3d::USING_PROPER_TOLERANCE) const = 0;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_LOCATOR_H_

// end of i_locator.h
