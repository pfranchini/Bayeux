// -*- mode: c++; -*- 
/* genbb_utils.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-28
 * Last modified: 2012-06-21
 * 
 * License: 
 * Copyright 2007-2012 F. Mauger
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Description: 
 *
 *   Utilities related to the  Fortran GENBB generator
 *
 */

#ifndef GENBB_HELP_GENBB_UTILS_H_
#define GENBB_HELP_GENBB_UTILS_H_

#include <string>
#include <vector>
#include <map>
#include <boost/scoped_ptr.hpp>

namespace genbb
{

  class utils
  {
  public:
 
    enum decay_type_t
    {
      INVALID_DECAY_TYPE = 0,
      DECAY_TYPE_DBD = 1,
      DECAY_TYPE_BACKGROUND = 2
    };
   
    static const double DEFAULT_ENERGY_RANGE_MIN;

    static const double DEFAULT_ENERGY_RANGE_MAX;
    
    static const std::vector<std::string> & get_dbd_nuclides ();
    
    static const std::map<int, std::string> & get_dbd_modes ();
    
    static const std::vector<int> & get_dbd_modes_with_energy_range ();
    
  };

}

#endif // GENBB_HELP_GENBB_UTILS_H_
