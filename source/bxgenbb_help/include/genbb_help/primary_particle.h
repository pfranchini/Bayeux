/// \file genbb_help/primary_particle.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-11
 * Last modified: 2014-06-19
 *
 * License:
 * Copyright 2007-2014 F. Mauger
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
 *   Primary particle for MC generators
 *
 * History:
 *
 */

#ifndef GENBB_HELP_PRIMARY_PARTICLE_H
#define GENBB_HELP_PRIMARY_PARTICLE_H 1

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_serializable.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>
// - Bayeux/genbb_help:
#include <genbb_help/genbb_help_config.h>

namespace genbb {

  /// \brief A primary particle generated by a Monte Carlo generator process
  class primary_particle
    : DATATOOLS_SERIALIZABLE_CLASS,
      public datatools::i_tree_dumpable
  {
  public:

    /// \brief Extended Geant3 particle
    // Using GEANT3 definition from:
    //   http://wwwasdoc.web.cern.ch/wwwasdoc/geant_html3/node72.html#SECTION024000000000000000000000
    //   http://www.star.bnl.gov/public/comp/simu/gstar/Manual/particle_id.html
    enum particle_type {
      UNDEF              = -1, /// Obsolete code
      PARTICLE_UNDEFINED = -1,
      PARTICLE_UNKNOWN = 0,  /// A valid particle but unknown from the genbb library
      GAMMA      = 1,
      POSITRON   = 2,
      ELECTRON   = 3,
      NEUTRINO   = 4,
      MUON_PLUS  = 5,
      MUON_MINUS = 6,
      PION_0     = 7,
      PION_PLUS  = 8,
      PION_MINUS = 9,
      KAON_0_LONG  = 10,
      KAON_PLUS    = 11,
      KAON_MINUS   = 12,
      NEUTRON      = 13,
      PROTON       = 14,
      ANTI_PROTON  = 15,
      KAON_0_SHORT = 16,
      ETA          = 17,
      LAMBDA       = 18,
      SIGMA_PLUS   = 19,
      SIGMA_0      = 20,
      SIGMA_MINUS  = 21,
      XI_0         = 22,
      XI_MINUS     = 23,
      OMEGA_MINUS      = 24,
      ANTI_NEUTRON     = 25,
      ANTI_LAMBDA      = 26,
      ANTI_SIGMA_MINUS = 27,
      ANTI_SIGMA_0     = 28,
      ANTI_SIGMA_PLUS  = 29,
      ANTI_XI_0        = 30,
      ANTI_XI_PLUS     = 31,
      ANTI_OMEGA_PLUS  = 32,
      DEUTERON   = 45,
      TRITIUM    = 46,
      TRITON     = 46,
      ALPHA      = 47,
      GEANTINO   = 48,
      HE3        = 49,
      CERENKOV   = 50,
      // Additional codes:
      NUCLEUS    = 1000000000, // Fully ionized ion (naked nucleus)
      ION        = 1100000000  // An ion/atom with arbitrary ionization state
    };

    /// Invalid PDG particle code
    /// This constant should be moved in some "PDG tools" class in the future
    static const int32_t PDG_CODE_UNDEFINED = 0;

    /// Build the label describing a nucleus ion from Z, A, and excitation energy
    static std::string nucleus_to_label(int z_, int a_, double excitation_energy_ = 0.0);

    /// Build the label describing an charged ion from Z, A, excitation energy and charge
    static std::string ion_to_label(int z_, int a_, double excitation_energy_ = 0.0, int charge_ = 0);

    /// Build the ion's characteristics from a label
    static bool label_to_ion(const std::string label_, int & z_, int & a_, double & excitation_energy_, int & charge_);

    /// Build the nucleus'characteristics from a label
    static bool label_to_nucleus(const std::string label_, int & z_, int & a_, double & excitation_energy_);

    /// Reset the full data structure of the particle
    void reset();

    /// Check if the extended Geant3 type of the particle is defined
    bool has_type() const;

    /// Return the extended Geant3 type of the particle
    int get_type() const;

    /// Set the extended Geant3 type of the particle and set the corresponding particle label if it is empty
    void set_type(int type_);

    /// Define a nucleus
    void set_nucleus(int z_, int a_, double excitation_energy_ = 0.0, bool pdg_ = false);

    /// Define an ion
    void set_ion(int z_, int a_, double excitation_energy_ = 0.0, int charge_ = 0, bool pdg_ = false);

    /// Define a neutrino
    void set_neutrino(const std::string & label_);

    /// Check if the PDG code of the particle is defined
    bool has_pdg_code() const;

    /// Return the PDG code of the particle
    int get_pdg_code() const;

    /// Set the PDG code of the particle
    // see also: http://pdg.lbl.gov/2011/reviews/rpp2011-rev-monte-carlo-numbering.pdf
    void set_pdg_code(int);

    /// Return the creation time of the particle
    double get_time() const;

    /// Return  mutable reference on the creation time of the particle
    double & grab_time();

    /// Check is the particle should have a specific identifier label because it lacks a type or code
    bool needs_particle_label() const;

    /// Check is the particle has a specific identifier label
    bool has_particle_label() const;

    // Return the identifier label of the particle
    const std::string & get_particle_label() const;

    // Set the identifier label of the particle (for type == PARTICLE_UNKNWON, NUCLEUS, ION)
    void set_particle_label(const std::string &);

    /// Set the creation time of the particle
    void set_time(double time_);

    /// Shift the creation time of the particle
    void shift_time(double delta_time_);

    /// Check if the description of the particle is valid
    /**
     *  Here we test a minimal set of attributes to consider
     *  that the particle can be used by a client application:
     *   - identifier: extended Geant3 'type' or 'PDG code' or 'particle label'
     *   - time
     *   - momentum
     *  Other attributes (mass, vertex...) are not mandatory.
     */
    bool is_valid() const;

    /// Check if the particle is of unknown type (but possibly valid for a client application)
    bool is_unknown() const;

    /// Check if the particle is a gamma
    bool is_gamma() const;

    /// Check if the particle is a positron
    bool is_positron() const;

    /// Check if the particle is an electron
    bool is_electron() const;

    /// Check if the particle is a neutrino
    bool is_neutrino() const;

    /// Check if the particle is an alpha
    bool is_alpha() const;

    /// Check if the particle is a neutron
    bool is_neutron() const;

    /// Check if the particle is muon+
    bool is_muon_plus() const;

    /// Check if the particle is minus-
    bool is_muon_minus() const;

    /// Check if the particle is an nucleus (not the light ones: D, T, He3, He4)
    bool is_nucleus() const;

    /// Check if the particle is an ion (not the light ones: D, T, He3, He4)
    bool is_ion() const;

    /// Set the momentum of the particle
    void set_momentum(const geomtools::vector_3d & m_);

    /// Return the const momentum of the particle
    const geomtools::vector_3d & get_momentum() const;

    /// Return the mutable momentum of the particle
    geomtools::vector_3d & grab_momentum();

    /// Set the vertex of the particle
    void set_vertex(const geomtools::vector_3d & v_);

    /// Return the const vertex of the particle
    const geomtools::vector_3d & get_vertex() const;

    /// Return the mutable vertex of the particle
    geomtools::vector_3d & grab_vertex();

    /// Check if the particle has its own vertex
    bool has_vertex() const;

    /// Invalidate the vertex of the particle
    void invalidate_vertex();

    /// Check if the charge of the particle is known
    bool charge_is_known() const;

    /// Return the charge of the particle
    double get_charge() const; // in unit of e

    /// Set the mass of the particle (in unit of energy)
    void set_mass(double);

    /// Reset the mass of the particle
    void reset_mass();

    /// Return the mass of the particle (in unit of energy)
    double get_mass() const;

    /// Check if the mass of the particle is known
    bool mass_is_known() const;

    /// Return the total energy
    double get_total_energy() const;

    /// Return the kinetic energy
    double get_kinetic_energy() const;

    /// Return the relativistic beta coefficient
    double get_beta() const;

    /// Return a const reference to the auxiliary properties
    const datatools::properties & get_auxiliaries() const;

    /// Return a mutable reference to the auxiliary properties
    datatools::properties & grab_auxiliaries();

    /// Constructor
    primary_particle();

    /// Constructor
    primary_particle(int32_t a_type,
                     double a_time,
                     const geomtools::vector_3d &);

    /// Destructor
    virtual ~primary_particle();

    /// Smart print
    virtual void
    tree_dump(std::ostream      & out_    = std::clog,
              const std::string & title_  = "",
              const std::string & indent_ = "",
              bool inherit_               = false) const;

    /// Basic print
    void dump(std::ostream & out_ = std::clog,
              const std::string & a_indent = "") const;

    /// Return the label associated to a particle code
    static std::string particle_label_from_type(int a_type);

    /// Return the particle code associated to a label
    static int particle_type_from_label(const std::string & a_label);

  protected:

    /// Set the default values for attributes
    void _set_defaults();

  private:

    int32_t              _type_;     /// Extended Geant3 type of the particle (using historical Geant3 particle Ids + additional codes)
    int32_t              _pdg_code_; /// Standard PDG code of the particle (NOT USED YET)
    std::string          _particle_label_; /// String identifier of the particle in case it is not supported by the table of Geant3 codes (type) or PDG standards
    double               _mass_;     /// The mass in CLHEP energy unit
    double               _time_;     /// The mass in CLHEP time unit
    geomtools::vector_3d _momentum_; /// The mass in CLHEP momentum unit
    geomtools::vector_3d _vertex_;   /// The mass in CLHEP position unit
    datatools::properties _auxiliaries_; /// Auxiliary properties

    //! Support for Boost-based serialization
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(primary_particle)

    //! Support for backward compatibility serialization tag
    DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT()

    //! Reflection interface
    DR_CLASS_RTTI();

  };

} // end of namespace genbb

DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(::genbb::primary_particle)

// Activate reflection layer for the genbb::primary_particle class :
DR_CLASS_INIT(::genbb::primary_particle);

#endif // GENBB_HELP_PRIMARY_PARTICLE_H

// Local Variables: --
// mode: c++ --
// End: --
