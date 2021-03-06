/* simple_brio_data_sink.cc
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

// Ourselves:
#include <dpp/simple_brio_data_sink.h>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/things.h>
// 2015-02-11, FM: Mandatory:
#include <datatools/things.ipp>
#include <datatools/io_factory.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
// - Bayeux/brio:
#include <brio/writer.h>

#if DATATOOLS_WITH_BIO == 0
#include <datatools/archives_instantiation.h>
#include <datatools/properties.ipp>
#include <datatools/things.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(datatools::things);
#endif

// This project:
#include <dpp/dpp_config.h>
#include <dpp/brio_common.h>

namespace dpp {

  bool simple_brio_data_sink::is_random () const
  {
    return true;
  }

  void simple_brio_data_sink::reset ()
  {
    if (is_open ()) {
      this->simple_brio_data_sink::close ();
    }
    _sink_record.reset ();
    return;
  }

  void simple_brio_data_sink::close ()
  {
    if (_sink_record.status == sink_record::STATUS_CLOSED) {
      return;
    }
    if (_brio_file_writer_ != 0)  {
      this->simple_brio_data_sink::_close_file_sink ();
    }
    return;
  }

  void simple_brio_data_sink::open ()
  {
    if (_sink_record.status == sink_record::STATUS_OPENED) {
      return;
    }
    std::string label = _sink_record.effective_label;
    std::string file_name;
    bool file_mode = false;
    bool upload_mode = false;
    if (boost::find_first (label, "://")) {
      if (boost::starts_with (label, "file://")) {
        file_mode = true;
        file_name = label;
        boost::replace_first (file_name, "file://", "");
      } else if (boost::starts_with (label, "http://")) {
        upload_mode = true;
#if BOOST_FILESYSTEM_VERSION == 3
        file_name = boost::filesystem::path (label).filename ().string ();
#else
        file_name = boost::filesystem::path (label).filename ();
#endif
      } else if (boost::starts_with (label, "https://")) {
        upload_mode = true;
#if BOOST_FILESYSTEM_VERSION == 3
        file_name = boost::filesystem::path (label).filename ().string ();
#else
        file_name = boost::filesystem::path (label).filename ();
#endif
      } else if (boost::starts_with (label, "ftp://")) {
        upload_mode = true;
#if BOOST_FILESYSTEM_VERSION == 3
        file_name = boost::filesystem::path (label).filename ().string ();
#else
        file_name = boost::filesystem::path (label).filename ();
#endif
      } else {
        DT_THROW_IF(true,std::logic_error,
                    "Sink labelled '" << _sink_record.effective_label << "' needs an unknown protocol !");
      }
    } else {
      file_mode = true;
      file_name = _sink_record.effective_label;
    }

    DT_THROW_IF (upload_mode,  std::runtime_error,
                 "Sink file upload mode is not implemented yet !");
    if (file_mode)
      {
        this->simple_brio_data_sink::_open_file_sink ();
      }

    return;
  }

  void simple_brio_data_sink::_close_file_sink ()
  {
    if (_brio_file_writer_ != 0) {
      if (_brio_file_writer_->is_opened()) _brio_file_writer_->close ();
      delete _brio_file_writer_;
      _brio_file_writer_ = 0;
      _sink_record.status = sink_record::STATUS_CLOSED;
      _sink_record.reset ();
    }
    return;
  }

  void simple_brio_data_sink::_open_file_sink ()
  {
    namespace ds = datatools;
    if (boost::filesystem::exists (_sink_record.effective_label)) {
      DT_THROW_IF (is_preserve_existing_sink (),
                   std::runtime_error,
                   "File '" << _sink_record.effective_label << "' already exists !");
      DT_LOG_WARNING(get_logging_priority (),
                     "File '" << _sink_record.effective_label << "' already exists !");
    }
    int mode = 0;
    int status =
      brio::store_info::guess_mode_from_filename (_sink_record.effective_label, mode);
    DT_THROW_IF (status == ds::io_factory::ERROR,
                 std::logic_error,
                 "File format not recognized for '" << _sink_record.effective_label << "' !");

    if (_brio_file_writer_ == 0) {
      _brio_file_writer_ = new brio::writer;
      _brio_file_writer_->open (_sink_record.effective_label);
      _brio_file_writer_->add_store (brio_common::general_info_store_label(),
                                     datatools::properties::serial_tag(),
                                     32000);
      _brio_file_writer_->add_store (brio_common::event_record_store_label(),
                                     datatools::things::serial_tag(),
                                     256000);
      _brio_file_writer_->select_store (brio_common::event_record_store_label());
      if (get_logging_priority () >= datatools::logger::PRIO_INFORMATION)
        {
          DT_LOG_INFORMATION (get_logging_priority (),
                              "Create brio file with the following setup:");
          _brio_file_writer_->tree_dump (std::clog, "[information]: ");
        }
      _sink_record.status = sink_record::STATUS_OPENED;
    }
    return;
  }

  bool simple_brio_data_sink::store_next_record (const datatools::things & a_event_record)
  {
    bool done = false;
    if (_brio_file_writer_ != 0) {
      _brio_file_writer_->store (a_event_record);
      done = true;
    } else {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Cannot store the event record !");
    }
    return done;
  }

  bool simple_brio_data_sink::can_store_meta_data() const
  {
    return true;
  }

  bool simple_brio_data_sink::store_metadata (const datatools::properties & a_metadata)
  {
    bool done = false;
    if (_brio_file_writer_ != 0) {
      _brio_file_writer_->select_store(brio_common::general_info_store_label());
      _brio_file_writer_->store(a_metadata);
      _brio_file_writer_->select_store(brio_common::event_record_store_label());
      done = true;
    } else {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "Cannot store the metadata !");
    }
    return done;
  }

  simple_brio_data_sink::simple_brio_data_sink (datatools::logger::priority a_priority)
    : i_data_sink (a_priority)
  {
    _brio_file_writer_ = 0;
    return;
  }

  simple_brio_data_sink::simple_brio_data_sink (const std::string & a_sink_label,
                                                datatools::logger::priority a_priority)
    : i_data_sink (a_sink_label, a_priority)
  {
    _brio_file_writer_ = 0;
    this->simple_brio_data_sink::open ();
    return;
  }

  simple_brio_data_sink::~simple_brio_data_sink ()
  {
    this->simple_brio_data_sink::reset ();
    return;
  }

}  // end of namespace dpp

/*
** Local Variables: --
** mode: c++ --
** indent-tabs-mode: nil --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
