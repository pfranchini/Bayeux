// -*- mode: c++; -*- 
/* brio::test::the_serializable.h */

#ifndef __brio__test__the_serializable_h__
#define __brio__test__the_serializable_h__ 1

#include <boost/serialization/export.hpp>
#include <brio/test/the_serializable.ipp>

/****************************
 * brio::test::data_t *
 ****************************/

BOOST_CLASS_EXPORT_KEY2(brio::test::data_t, "brio::test::data_t")
BOOST_CLASS_EXPORT_IMPLEMENT(brio::test::data_t) 

#endif // __brio__test__the_serializable_h__

/* end of brio::test::the_serializable.h */
