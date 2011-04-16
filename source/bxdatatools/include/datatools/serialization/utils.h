/* datatools::serialization::utils.h */
/* 
 * Description :
 *
 *  Some useful macros related to Boost/Serialisation.
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
 */ 

#ifndef __datatools__serialization__utils_h
#define __datatools__serialization__utils_h 1

#include <boost/serialization/tracking.hpp>
#include <boost/serialization/split_member.hpp>

// declare the template load member function from the header file :
#define BOOST_SERIALIZATION_SPLIT_MEMBER_LOAD_DECLARATION()      \
template<class Archive>                                          \
void load(                                                       \
    Archive &ar,                                                 \
    const unsigned int file_version);				 \
/**/

// declare the template save member function from the header file :
#define BOOST_SERIALIZATION_SPLIT_MEMBER_SAVE_DECLARATION()      \
template<class Archive>                                          \
void save(                                                       \
    Archive &ar,                                                 \
    const unsigned int file_version) const;			 \
/**/

// declare the template serialize member function from the header file :
#define BOOST_SERIALIZATION_SERIALIZE_DECLARATION()              \
template<class Archive>                                          \
void serialize(                                                  \
    Archive &ar,                                                 \
    const unsigned int file_version);				 \
/**/

// split member serialize functions save/load/serialize
// from the header file :
#define BOOST_SERIALIZATION_SPLIT_MEMBER_SERIALIZE_DECLARATIONS() \
BOOST_SERIALIZATION_SPLIT_MEMBER_LOAD_DECLARATION()               \
BOOST_SERIALIZATION_SPLIT_MEMBER_SAVE_DECLARATION()               \
BOOST_SERIALIZATION_SERIALIZE_DECLARATION()                       \
/**/

// split member function serialize function into save/load
// from the implementation file :
#define BOOST_SERIALIZATION_SPLIT_MEMBER_SERIALIZE_IMPLEMENTATION(T) \
template<class Archive>                                          \
void T::serialize(						 \
    Archive &ar,                                                 \
    const unsigned int file_version                              \
){                                                               \
    boost::serialization::split_member(ar, *this, file_version); \
    return;                                                      \
}                                                                \
/**/

#endif // __datatools__serialization__utils_h

/* end of datatools::serialization::utils.h */
