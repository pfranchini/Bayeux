// ex01_read_plain_simdata.cxx
/* Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>

#include <datatools/io_factory.h>
#include <datatools/properties.h>
#include <datatools/library_loader.h>

#include <geomtools/manager.h>
//#include <geomtools/gnuplot_draw.h>
//#include <geomtools/gnuplot_drawer.h>
//#include <geomtools/display_data.h>
//#include <geomtools/line_3d.h>
//#include <geomtools/blur_spot.h>

#include <mctools/simulated_data.h>

#include <datatools/bio_guard.h>
#include <mygsl/bio_guard.h>
#include <geomtools/bio_guard.h>
#include <mctools/bio_guard.h>

#include <ex01_inspector.h>

int main(int argc_, char **argv_) {
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  try {

    std::string plain_simulated_data_filename;
    std::string geometry_config_filename;
    bool interactive = false;
    bool visualization = false;
    std::vector<std::string> LL_dlls;
    std::string              LL_config;

    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()
      ("help,h", "produce help message")

      ("logging-priority,g",
       po::value<std::string>()
       ->default_value("warning"),
       "set the logging priority threshold")

      ("load-dll,l",
       po::value<std::vector<std::string> > (),
       "load a dynamic library"
       )

      ("dll-config",
       po::value<std::vector<std::string> > (),
       "load a configuration file for dynamic library loading"
       )

      ("input-file,i",
       po::value<std::string>(&plain_simulated_data_filename),
       "set the input plain simulated data file from which to load the Monte-Carlo data")

      ("geometry-config,g",
       po::value<std::string>(&geometry_config_filename),
       "set the main configuration file of the geometry manager")

      ("interactive,I",
       po::value<bool>(&interactive)
       ->zero_tokens()
       ->default_value(false),
       "run in interactive mode")

      ("with-visualization,V",
       po::value<bool>(&visualization)
       ->zero_tokens()
       ->default_value(false),
       "activate visualization")

      ; // end of options description

    // Describe command line arguments :
    po::positional_options_description args;
    args.add("input-file", 1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
      .options(opts)
              .positional(args)
              .run(), vm);
    po::notify(vm);
    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << "Usage : " << std::endl;
      std::cout << opts << std::endl;
      return(1);
    }
    if (vm.count("logging-priority")) {
      std::string logging_label = vm["logging-priority"].as<std::string>();
      logging = datatools::logger::get_priority(logging_label);
      DT_THROW_IF(logging == datatools::logger::PRIO_UNDEFINED, std::logic_error,
                  "Invalid logging priority label '" << logging_label << "' !");
    }

    if (vm.count ("load-dll")) {
      LL_dlls = vm["load-dll"].as<std::vector<std::string> > ();
    }

    if (vm.count ("dll-config")) {
      LL_config = vm["dll-config"].as<std::string> ();
    }


    uint32_t LL_flags = datatools::library_loader::allow_unregistered;
    datatools::library_loader LL(LL_flags, LL_config);
    BOOST_FOREACH (const std::string & dll_name, LL_dlls) {
      DT_LOG_NOTICE(logging,"Loading DLL '" << dll_name << "'...");
      DT_THROW_IF (LL.load(dll_name) != EXIT_SUCCESS,
                   std::runtime_error,
                   "Loading DLL '" << dll_name << "' failed !");
    }

    DT_THROW_IF (plain_simulated_data_filename.empty(), std::logic_error,
                 "Missing plain simulated data input file !");

    // The input plain simulated data file :
    DT_THROW_IF (! boost::filesystem::exists(plain_simulated_data_filename),
                 std::runtime_error,
                 "File '" << plain_simulated_data_filename << "' does not exists !");

    // The  plain simulated data reader :
    datatools::data_reader psd_reader(plain_simulated_data_filename,
                                      datatools::using_multi_archives);

    datatools::properties header;
    datatools::properties footer;
    bool has_header = false;
    bool has_footer = false;

    // Geometry manager :
    DT_LOG_DEBUG(logging, "Initializing the geometry manager...");
    if (geometry_config_filename.empty()) {
      geometry_config_filename = "config/geometry/manager.conf";
    }
    geomtools::manager geo_mgr;
    datatools::fetch_path_with_env(geometry_config_filename);
    datatools::properties geo_mgr_config;
    datatools::properties::read_config(geometry_config_filename, geo_mgr_config);
    geo_mgr.initialize(geo_mgr_config);

    // The simulated data object to be loaded :
    mctools::simulated_data simdata;

    // The simulated data inspector :
    mctools::ex01::simulated_data_inspector SDI;
    SDI.set_interactive(interactive);
    SDI.set_with_visualization(visualization);
    SDI.set_geometry_manager(geo_mgr);

    // The simulated data loading loop :
    int psd_count = 0;
    while (psd_reader.has_record_tag()) {

      if (psd_reader.record_tag_is(datatools::properties::SERIAL_TAG)) {

        // Depending on the Geant4 simulation options, a header and footer could have been
        // stored as the first and last records of the archive file respectively :
        if (! has_header) {
          psd_reader.load(header); // Load the header
          has_header = true;
          header.tree_dump(std::clog, "Plain simulated data header:");
        } else if (! has_footer) {
          psd_reader.load(footer); // Load the footer
          has_footer = true;
          footer.tree_dump(std::clog, "Plain simulated data footer:");
        } else {
          DT_THROW_IF(true, std::logic_error,
                      "Header and footer have already been loaded ! "
                      << "Cannot identify additional record with serial tag '"
                      << datatools::properties::SERIAL_TAG << "' ! Abort !");
        }
      } else if (psd_reader.record_tag_is(mctools::simulated_data::SERIAL_TAG)) {

        // A plain `mctools::simulated_data' object is stored here :
        psd_reader.load(simdata); // Load the simulated data object
        DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
        bool goon = SDI.inspect(simdata);
        if (! goon) {
          break;
        }
        simdata.clear(); // Reset the simulated data object before to read the next one (if any).
        psd_count++;

      } else {

        // Here we are unable to identify the type of the record :
        DT_THROW_IF(true, std::logic_error,
                    "Cannot load record with serial tag '"
                    << psd_reader.get_record_tag() << "' ! Abort !");
      }
    }
  }
  catch(std::exception& x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  }
  catch(...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of ex01_read_plain_simdata
