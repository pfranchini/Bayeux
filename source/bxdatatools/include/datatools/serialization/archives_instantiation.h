/* -*- mode: c++; -*-
/* datatools::serialization:: archives_instantiation.h */
/* 
 * Description :
 *
 * Define some macro to instantiate serialization methods for all archives
 * supported by datatools.
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

#ifndef __datatools__serialization__archives_instantiation_h
#define __datatools__serialization__archives_instantiation_h 1

#include <datatools/serialization/archives_list.h>

#define DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,A) \
template void T::serialize(A & ar, const unsigned int version);  \
/**/

#ifdef IOFACTORY_USE_PBA
#define DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(T)		                      \
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::text_oarchive)                  \
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::text_iarchive)                  \
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::xml_oarchive)                   \
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::xml_iarchive)                   \
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::quasi_portable_binary_oarchive) \
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::quasi_portable_binary_iarchive) \
/**/
#else
#define DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(T)		       \
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::text_oarchive)   \
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::text_iarchive)   \
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::xml_oarchive)    \
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::xml_iarchive)    \
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::binary_oarchive) \
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::binary_iarchive) \
/**/
#endif // ! IOFACTORY_USE_PBA

#endif // __datatools__serialization__archives_instantiation_h

/* end of datatools::serialization::archives_instantiation.h */
