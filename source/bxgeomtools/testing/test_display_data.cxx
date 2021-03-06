// test_display_data.cxx

// Ourselves :
#include <geomtools/display_data.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

// Utilities :
#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
#include <datatools/io_factory.h>

// This project:
#include <geomtools/box.h>
#include <geomtools/rectangle.h>
#include <geomtools/circle.h>
#include <geomtools/disk.h>
#include <geomtools/cylinder.h>
#include <geomtools/tube.h>
#include <geomtools/line_3d.h>
#include <geomtools/polyline_3d.h>
#include <geomtools/blur_spot.h>
#include <geomtools/placement.h>
#include <geomtools/color.h>
#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

std::string make_name (const std::string & prefix_, unsigned int i_)
{
  std::ostringstream out;
  out << prefix_ << '[' << i_ << ']';
  return out.str ();
}

std::string make_name2 (const std::string & prefix_,
                        unsigned int i_, unsigned int j_)
{
  return make_name (make_name (prefix_, i_), j_);
}

std::string make_name3 (const std::string & prefix_,
                        unsigned int i_, unsigned int j_, unsigned int k_)
{
  return make_name2 (make_name (prefix_, i_), j_, k_);
}

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  // long seed = 314159;
  // bool debug = false;
  bool draw = false;
  std::string draw_group;

  int iarg = 1;
  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-D" || arg == "--draw") {
      draw = true;
    } else if (arg == "-g" || arg == "--draw-group") {
      draw = true;
      draw_group = argv_[++iarg];
    }

    iarg++;
  }
  try {
    std::clog << "Test program for class 'display_data' !" << std::endl;

    // A display data object :
    geomtools::display_data DD;


    // Draw a blur spot :
    {
      geomtools::blur_spot a_spot (geomtools::blur_spot::DIMENSION_ZERO,
                                   15.0 * CLHEP::mm);
      geomtools::display_data::display_item & spot_DI
        = DD.add_static_item ("spot",
                              "group::misc",
                              "red");
      // Compute position of the circle :
      geomtools::placement plcmt;
      plcmt.set_translation (-10 * CLHEP::cm, +15 * CLHEP::cm, 30 * CLHEP::cm);
      plcmt.set_orientation (geomtools::AXIS_X, 30.*CLHEP::degree);

      // Fill the 'wires' structure of this circle :
      a_spot.generate_wires(spot_DI.wires, plcmt);
    }

    // Draw 1D blur spot :
    {
      geomtools::blur_spot a_spot (geomtools::blur_spot::DIMENSION_ONE,
                                   10.0 * CLHEP::mm);
      a_spot.set_errors(85.0 * CLHEP::mm);
      geomtools::display_data::display_item & spot_DI
        = DD.add_static_item ("spot1",
                              "group::misc",
                              "magenta");
      // Compute position of the 1D-spot :
      geomtools::placement plcmt;
      plcmt.set_translation (-30 * CLHEP::cm, +15 * CLHEP::cm, 20 * CLHEP::cm);
      plcmt.set_orientation (geomtools::AXIS_X, -15.*CLHEP::degree);

      // Fill the 'wires' structure of this circle :
      a_spot.generate_wires (spot_DI.wires, plcmt);
    }

    // Draw 2D blur spot :
    {
      geomtools::blur_spot a_spot (geomtools::blur_spot::DIMENSION_TWO,
                                   10.0 * CLHEP::mm);
      a_spot.set_errors(65.0 * CLHEP::mm, 135.0 * CLHEP::mm);
      geomtools::display_data::display_item & spot_DI
        = DD.add_static_item ("spot2",
                              "group::misc",
                              "blue");
      // Compute position of the circle :
      geomtools::placement plcmt;
      plcmt.set_translation (-10 * CLHEP::cm, -15 * CLHEP::cm, -20 * CLHEP::cm);
      plcmt.set_orientation (geomtools::AXIS_X, -15.*CLHEP::degree);

      // Fill the 'wires' structure of this circle :
      a_spot.generate_wires (spot_DI.wires, plcmt);
    }

    // Draw 3D blur spot :
    {
      geomtools::blur_spot a_spot (geomtools::blur_spot::DIMENSION_THREE,
                                   10.0 * CLHEP::mm);
      a_spot.set_errors(65.0 * CLHEP::mm, 65.0 * CLHEP::mm, 100.0 * CLHEP::mm);
      geomtools::display_data::display_item & spot_DI
        = DD.add_static_item ("spot3",
                              "group::misc",
                              "green");
      // Compute position of the circle :
      geomtools::placement plcmt;
      plcmt.set_translation (-10 * CLHEP::cm, -35 * CLHEP::cm, -20 * CLHEP::cm);
      plcmt.set_orientation (geomtools::AXIS_Y, 75.*CLHEP::degree);

      // Fill the 'wires' structure of this circle :
      a_spot.generate_wires (spot_DI.wires, plcmt);
    }

    // Draw a circle :
    {
      geomtools::circle a_circle (5. * CLHEP::cm);
      geomtools::display_data::display_item & circle_DI
        = DD.add_static_item ("circle",
                              "group::misc",
                              "green");
      // Compute position of the circle :
      geomtools::placement plcmt;
      plcmt.set_translation (-20 * CLHEP::cm, +25 * CLHEP::cm, 40 * CLHEP::cm);
      plcmt.set_orientation (geomtools::AXIS_Y, 45.*CLHEP::degree);

      // Fill the 'wires' structure of this circle :
      a_circle.generate_wires (circle_DI.wires, plcmt);
    }


    // Draw a disk :
    {
      geomtools::disk a_disk (8. * CLHEP::cm);
      geomtools::display_data::display_item & disk_DI
        = DD.add_static_item ("disk",
                              "group::misc",
                              "blue");
      // Compute position of the disk :
      geomtools::placement plcmt;
      plcmt.set_translation (-20 * CLHEP::cm, -75 * CLHEP::cm, 40 * CLHEP::cm);
      plcmt.set_orientation (geomtools::AXIS_Y, 45.*CLHEP::degree);

      // Fill the 'wires' structure of this disk :
      a_disk.generate_wires (disk_DI.wires, plcmt);
    }


    // Draw a cylinder :
    {
      geomtools::cylinder a_cylinder (10. *CLHEP::cm, 15. * CLHEP::cm);
      geomtools::display_data::display_item & cylinder_DI
        = DD.add_static_item ("cylinder",
                              "group::misc",
                              "blue");
      // Compute position of the cylinder :
      geomtools::placement plcmt;
      plcmt.set_translation (20 * CLHEP::cm, 75 * CLHEP::cm, 30 * CLHEP::cm);
      plcmt.set_orientation (geomtools::AXIS_Y, 30.*CLHEP::degree);

      // Fill the 'wires' structure of this cylinder:
      a_cylinder.generate_wires (cylinder_DI.wires, plcmt);
    }


    // Draw a tube :
    {
      geomtools::tube a_tube (10. *CLHEP::cm, 20. *CLHEP::cm, 8. * CLHEP::cm);
      geomtools::display_data::display_item & tube_DI
        = DD.add_static_item ("tube",
                              "group::misc",
                              "blue");
      // Compute position of the tube :
      geomtools::placement plcmt;
      plcmt.set_translation (-30 * CLHEP::cm, -50 * CLHEP::cm, 40 * CLHEP::cm);
      plcmt.set_orientation (geomtools::AXIS_Y, 30.*CLHEP::degree);

      // Fill the 'wires' structure of this tube:
      a_tube.generate_wires (tube_DI.wires, plcmt);
    }

    // Draw the calorimeter walls :
    {
      // The 3D volume of a calorimeter block :
      geomtools::box calo_block_box (254. *CLHEP::mm,254. *CLHEP::mm, 180. *CLHEP::mm);

      // Layout of the segmented calorimeter (2 walls) :
      int ncols = 4;
      int nrows = 3;
      for (int side = 0; side < 2; side++)  {
        for (int i = 0; i < ncols; i++) {
          for (int j = 0; j < nrows; j++) {
            // Adding a static display item corresponding to each block :
            geomtools::display_data::display_item & calo_block
              = DD.add_static_item (make_name3 ("calo_block", side, i, j),
                                    "group::detector",
                                    "cyan");
            // Compute position of the block :
            geomtools::placement plcmt;
            plcmt.set_translation ((1 - 2 * side) * 52 * CLHEP::cm,
                                   -0.5 * ncols * calo_block_box.get_y () + (i + 0.5) * calo_block_box.get_y (),
                                   -0.5 * nrows * calo_block_box.get_x () + (j + 0.5) * calo_block_box.get_x ()
                                   );
            plcmt.set_orientation (geomtools::AXIS_Y, 90.*CLHEP::degree);

            // Fill the 'wires' structure of this block :
            calo_block_box.generate_wires (calo_block.wires, plcmt);
          }
        }
      }
    }

    // Draw the source foil :
    {
      geomtools::rectangle source_foil_rect (50*CLHEP::cm, 80*CLHEP::cm);
      geomtools::display_data::display_item & srcfoil
        = DD.add_static_item ("source_foil",
                              "group::detector",
                              "magenta");
      // Compute position of the block :
      geomtools::placement plcmt;
      plcmt.set_translation (0, 0, 0);
      plcmt.set_orientation (geomtools::AXIS_Y, 90.*CLHEP::degree);

      // Fill the 'wires' structure of the source foil :
      source_foil_rect.generate_wires (srcfoil.wires, plcmt);
    }

    geomtools::vector_3d true_vertex (0.0 * CLHEP::mm,
                                      35 * CLHEP::mm,
                                      -105 * CLHEP::mm);
    // Draw some Geiger hits :
    {
      for (int i = 0; i < 9; i++) {
        geomtools::display_data::display_item & gghit
          = DD.add_static_item (make_name ("gg_hit", i),
                                "group::gg_hits",
                                "blue");

        double x = (10 + (0.5 + i) * 44) * CLHEP::mm;
        double y = 45 * CLHEP::mm + i * 44 * CLHEP::mm;
        double z = (-100 + 20 * i) * CLHEP::mm;
        double r = (10 + 30 * drand48 ())*CLHEP::mm;
        double dr = 0.5*CLHEP::mm;
        double dz = 1.5*CLHEP::cm;
        geomtools::circle gg_hit_circ1 (r+dr);
        geomtools::circle gg_hit_circ2 (r-dr);
        geomtools::line_3d gg_hit_segmt (geomtools::vector_3d (0, 0, - dz),
                                         geomtools::vector_3d (0, 0, + dz));

        // Compute position of the hit :
        geomtools::placement plcmt;
        plcmt.set_translation (x, y, z);
        plcmt.set_orientation (0, 0, 0);

        // Fill the 'wires' structure of the the hit :
        gg_hit_circ1.generate_wires(gghit.wires, plcmt);
        gg_hit_circ2.generate_wires(gghit.wires, plcmt);
        gg_hit_segmt.generate_wires(gghit.wires, plcmt);

      }
    }

    // Draw some data from the steps of an algorithm
    int n_algo_steps = 5;
    {
      for (int istep = 0; istep < n_algo_steps; istep++) {
          geomtools::display_data::display_item & algovtx
            = DD.add_framed_item ("vertex",
                                  istep,
                                  "group::algo::vertex",
                                  "red");
          {
            std::ostringstream frame_info_oss;
            frame_info_oss << "Algorithm step #" << istep;
            DD.add_frame_info (istep,frame_info_oss.str());
          }
          geomtools::circle vertex_circ (15.0 * CLHEP::mm);
          // Compute position of the vertex :
          geomtools::vector_3d algo_vertex = true_vertex + (n_algo_steps - istep) * geomtools::vector_3d (4, -3, 6) * CLHEP::cm;
          geomtools::placement plcmt;
          plcmt.set_translation (algo_vertex);
          plcmt.set_orientation (0, 0, 0);
          // Fill the 'wires' structure of the the hit :
          std::ostringstream frame_info_ss;
          frame_info_ss << "Vertex estimated @ loop #" << istep;
          algovtx.frame_info = frame_info_ss.str ();
          vertex_circ.generate_wires (algovtx.wires, plcmt);
        }
    }

    DD.tree_dump (std::clog, "Display data : ");

    {
      datatools::data_writer writer;
      writer.init("test_display_data.data.gz", datatools::using_multi_archives);
      writer.store(DD);
    }

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    if (draw) {
        geomtools::vector_3d scene_origin (0, 0, 0);
        geomtools::rotation_3d scene_rotation;

        std::string the_group = draw_group;
        /*
          the_group = "group::detector";
          the_group = "group::algo::vertex";
          the_group = "group::gg_hits";
          the_group = "group::misc";
        */
        std::string the_name;
        for (int frame_index = 0; frame_index < n_algo_steps; frame_index++) {
          datatools::temp_file tmp_file;
          tmp_file.set_remove_at_destroy (true);
          tmp_file.create ("/tmp", "test_display_data_static_");
          int plot_index = 0;
          for (std::vector<std::string>::const_iterator icolor
                 = DD.get_colors ().begin ();
               icolor != DD.get_colors ().end ();
               icolor++) {
            const std::string & the_color = *icolor;
            tmp_file.out () << "#plot_index=" << plot_index << std::endl;
            tmp_file.out () << "#color=" << the_color << std::endl;
            // Gnuplot trick :
            geomtools::gnuplot_draw::basic_draw_point (tmp_file.out (), scene_origin);
            tmp_file.out () << std::endl;

            geomtools::gnuplot_draw::draw_display_data(tmp_file.out (),
                                                       scene_origin,
                                                       scene_rotation,
                                                       DD,
                                                       true,
                                                       frame_index,
                                                       the_color,
                                                       the_group,
                                                       the_name);
            tmp_file.out () << std::endl << std::endl;
            plot_index++;
          }
          tmp_file.close ();
          usleep (200);

          {
            Gnuplot g1 ("lines");
            {
              std::ostringstream title_cmd;
              title_cmd << "set title '";
              title_cmd << "Display data -- " << DD.get_frames().find(frame_index)->second;
              title_cmd << " (3D view)";
              g1.cmd (title_cmd.str ());
            }
            g1.cmd ("set grid");
            g1.cmd ("set size ratio -1");
            g1.cmd ("set view equal xyz");
            g1.cmd ("set xyplane at -500");
            g1.cmd ("set style data lines");
            g1.set_xrange (-1000, +1000).set_yrange (-1000, +1000).set_zrange (-1000, +1000);
            g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");

            std::ostringstream plot_cmd;
            plot_cmd << "splot ";
            for (int pi = 0; pi < plot_index; pi++) {
              if (pi>0) plot_cmd << ", ";
              plot_cmd << "'" << tmp_file.get_filename ();
              plot_cmd << "' index " << pi << " using 1:2:3 notitle with lines lt "
                       <<  geomtools::color::get_color (DD.get_colors()[pi]);
              g1.cmd (plot_cmd.str ());
            }
            g1.showonscreen (); // window output
            geomtools::gnuplot_drawer::wait_for_key ();
            usleep (200);
          }

        }
      }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY


  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
