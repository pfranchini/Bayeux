// datatools/ui/shell_cd_command.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include <datatools/ui/shell_cd_command.h>

// This project:
#include <datatools/ui/utils.h>
#include <datatools/ui/traits.h>
#include <datatools/detail/command_macros.h>

namespace datatools {

  namespace ui {

    shell_cd_command::shell_cd_command(basic_shell & shell_)
      : datatools::ui::target_command<basic_shell>(shell_, "cd", "Change the current interface path")
    {
      return;
    }

    shell_cd_command::~shell_cd_command()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void shell_cd_command::_init(const datatools::properties & config_)
    {
      this->base_command::_init(config_);

      // Arguments description:
      _grab_opts().add_options()

        ("path",
         boost::program_options::value<std::string>()
         ->value_name("path"),
         "Path to be moved in\n"
         "Examples: \n"
         "  cd --path / \n"
         "  cd ~ \n"
         "  cd ./ \n"
         "  cd /"
         "  cd "
         )

        ; // end of options description

      // Positional options:
      _grab_args().add("path", 1);

      return;
    }

    void shell_cd_command::_run(datatools::command::returned_info & cri_, uint32_t /*flags_*/)
    {
      cri_.reset();
      cri_.set_error_code(datatools::command::CEC_SUCCESS);
      DT_LOG_TRACE(get_logging_priority(), "Running command '" << get_name() << "'...");
      try {
        // Options:
        std::string path;
        basic_shell & shell = _grab_target();
        if (_grab_vmap().count("path")) {
          path = _grab_vmap()["path"].as<std::string>();
        }
        std::string full_path = shell.canonical_path(path);
        if (!shell.get_ihs().is_interface(full_path)) {
          DT_COMMAND_RETURNED_ERROR(cri_,
                                    datatools::command::CEC_PARAMETER_INVALID_TYPE,
                                    get_name() + ": " + "Path '" << path << "' is not an interface!");
        }
        if (shell.get_ihs().is_trait(full_path, traits::disabled_label())) {
          DT_COMMAND_RETURNED_ERROR(cri_,
                                    datatools::command::CEC_PARAMETER_INVALID_CONTEXT,
                                    get_name() + ": " + "Cannot move to disabled interface '" << path << "'!");
        } else {
          if (shell.get_ihs().is_trait(full_path, datatools::ui::traits::broken_label())) {
            // Attempt to cd a broken interface...
            std::cerr << get_name() << ": warning: "
                      << "Interface '" << full_path << "' is broken!" << std::endl;
          }
         if (shell.get_ihs().has_interface(full_path)) {
            const base_command_interface & bci = shell.get_ihs().get_interface(full_path);
            if (bci.is_disabled()) {
              DT_COMMAND_RETURNED_ERROR(cri_,
                                        datatools::command::CEC_PARAMETER_INVALID_CONTEXT,
                                        get_name() + ": " + "Cannot move to disabled command interface '" << path << "'!");

            }
          }
        }
        shell.set_current_path(full_path);
      } catch (std::exception & error) {
        DT_COMMAND_RETURNED_ERROR(cri_,
                                  datatools::command::CEC_FAILURE,
                                  get_name() + ": " + error.what());
      }
      DT_LOG_TRACE(get_logging_priority(), "End of command '" << get_name() << "'.");
      return;
    }

  } // namespace ui

} // namespace datatools
