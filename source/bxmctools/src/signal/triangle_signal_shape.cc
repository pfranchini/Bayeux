// triangle_signal_shape.cc
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2015-04-10
 * Last modified : 2015-04-10
 *
 * Copyright (C) 2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <mctools/signal/triangle_signal_shape.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
// - Bayeux/mygsl:
#include <mygsl/triangle_function.h>

namespace mctools {

  namespace signal {

    MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(triangle_signal_shape,
                                               "mctools::signal::triangle_signal_shape")

    void triangle_signal_shape::_set_defaults()
    {
      _polarity_ = POL_INVALID;
      datatools::invalidate(_rise_time_);
      datatools::invalidate(_fall_time_);

      _iq_  = mygsl::parameter_store::INVALID_INDEX;
      _it0_ = mygsl::parameter_store::INVALID_INDEX;
      datatools::invalidate(_q_);
      datatools::invalidate(_t0_);

      datatools::invalidate(_amplitude_);
      datatools::invalidate(_tpeak_);
      datatools::invalidate(_t1_);
      return;
    }

    void triangle_signal_shape::set_polarity(polarity_type p_)
    {
      DT_THROW_IF(p_ == POL_INVALID, std::logic_error, "Invalid polarity!");
      _polarity_ = p_;
      return;
    }

    polarity_type triangle_signal_shape::get_polarity() const
    {
      return _polarity_;
    }

    void triangle_signal_shape::set_rise_time(double tr_)
    {
      DT_THROW_IF(tr_ <= 0.0, std::logic_error, "Invalid rise time value!");
      _rise_time_ = tr_;
      return;
    }

    double triangle_signal_shape::get_rise_time() const
    {
      return _rise_time_;
    }

    void triangle_signal_shape::set_fall_time(double tf_)
    {
      DT_THROW_IF(tf_ <= 0.0, std::logic_error, "Invalid fall time value!");
      _fall_time_ = tf_;
      return;
    }

    double triangle_signal_shape::get_fall_time() const
    {
      return _fall_time_;
    }

    double triangle_signal_shape::get_t0() const
    {
      return _t0_;
    }

    double triangle_signal_shape::get_t1() const
    {
      return _t1_;
    }

    double triangle_signal_shape::get_tpeak() const
    {
      return _tpeak_;
    }

    double triangle_signal_shape::get_duration() const
    {
      return _t1_ - _t0_;
    }

    double triangle_signal_shape::get_amplitude() const
    {
      return _amplitude_;
    }

    triangle_signal_shape::triangle_signal_shape()
    {
      _set_defaults();
      return;
    }

    triangle_signal_shape::~triangle_signal_shape()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    double triangle_signal_shape::get_non_zero_domain_min() const
    {
      return _t0_;
    }

    double triangle_signal_shape::get_non_zero_domain_max() const
    {
      return _t1_;
    }

    bool triangle_signal_shape::is_valid() const
    {
      return datatools::is_valid(_t0_)
        && datatools::is_valid(_q_)
        && datatools::is_valid(_amplitude_)
        && datatools::is_valid(_rise_time_)
        && datatools::is_valid(_fall_time_)
        && polarity_is_set(_polarity_);
    }

    void triangle_signal_shape::reset()
    {
      _shape_.reset();
      _set_defaults();
      this->i_unary_function_with_parameters::reset();
      return;
    }

    bool triangle_signal_shape::is_initialized() const
    {
      return this->i_unary_function_with_parameters::is_initialized()
        && (_iq_ != mygsl::parameter_store::INVALID_INDEX)
        && (_it0_ != mygsl::parameter_store::INVALID_INDEX)
        && polarity_is_set(_polarity_)
        && datatools::is_valid(_fall_time_)
        && datatools::is_valid(_rise_time_);
    }

    void triangle_signal_shape::initialize(const datatools::properties & config_,
                                           mygsl::unary_function_dict_type & functors_)
    {
      this->mygsl::i_unary_function_with_parameters::initialize(config_, functors_);

      double default_time_unit = CLHEP::nanosecond;

      if (!polarity_is_set(_polarity_)) {
        if (config_.has_key("polarity")) {
          const std::string & pol_label =  config_.fetch_string("polarity");
          if (pol_label == "+") {
            set_polarity(POL_POSITIVE);
          } else if (pol_label == "-") {
            set_polarity(POL_NEGATIVE);
          } else {
            DT_THROW(std::logic_error, "Invalid polarity label '" << pol_label << "'!");
          }
        }
      }

      if (!datatools::is_valid(_rise_time_)) {
        if (config_.has_key("rise_time")) {
          double tr = config_.fetch_real("rise_time");
          if (!config_.has_explicit_unit("rise_time")) {
            tr *= default_time_unit;
          }
          set_rise_time(tr);
        }
      }

      if (!datatools::is_valid(_fall_time_)) {
        if (config_.has_key("fall_time")) {
          double tf = config_.fetch_real("fall_time");
          if (!config_.has_explicit_unit("fall_time")) {
            tf *= default_time_unit;
          }
          set_fall_time(tf);
        }
      }

      DT_THROW_IF(!polarity_is_set(_polarity_), std::logic_error,
                  "polarity is not set!")

      DT_THROW_IF(!datatools::is_valid(_rise_time_), std::logic_error,
                  "Rise time is not set!")

      DT_THROW_IF(!datatools::is_valid(_fall_time_), std::logic_error,
                  "Fall time is not set!")

      _shape_.reset(new mygsl::triangle_function);
      _iq_ = get_parameter_index("q");
      _it0_ = get_parameter_index("t0");
      _at_parameters_change();

      return;
    }

    void triangle_signal_shape::_at_parameters_change()
    {
      // Parameters:
      if (is_parameter_set(_iq_)) {
        fetch_parameter(_iq_, _q_);
      }
      if (is_parameter_set(_it0_)) {
        fetch_parameter(_it0_, _t0_);
      }
      double tr = _rise_time_ / 0.8;
      double tf = _fall_time_ / 0.8;
      _tpeak_ = _t0_ + tr;
      _t1_ = _tpeak_ + tf;
      _amplitude_ = _q_ * 2 / (tr + tf);
      _shape_->set_amplitude(_amplitude_);
      _shape_->set_head_width(tr);
      _shape_->set_tail_width(tf);
      _shape_->set_center(_tpeak_);
      return;
    }

    double triangle_signal_shape::_eval_from_parameters(double x_) const
    {
      double res = _shape_->eval(x_);
      if (_polarity_ == POL_NEGATIVE) {
        res *= -1.0;
      }
      return res;
    }

    void triangle_signal_shape::tree_dump(std::ostream & out_,
                                          const std::string & title_,
                                          const std::string & indent_,
                                          bool inherit_) const
    {
      this->i_unary_function_with_parameters::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Polarity: " << _polarity_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Rise time (10-90%): " << _rise_time_ / CLHEP::ns << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Fall time (10-90%): " << _fall_time_ / CLHEP::ns << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Index for q: " << _iq_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Index for t0: " << _it0_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value q : " << _q_ / (CLHEP::volt * CLHEP::ns) << " nV.s" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value t0 : " << _t0_ / (CLHEP::ns) << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current amplitude : " << _amplitude_ / (CLHEP::volt) << " V" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value tpeak : " << _tpeak_ / (CLHEP::ns) << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Current value t1 : " << _t1_ / (CLHEP::ns) << " ns" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Shape : ";
      if (_shape_) {
        out_ << "[@" << _shape_.get()  << "]";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;



      return;
    }

    /*
    double triangle_signal_shape::_eval(double t_) const
    {
      DT_THROW_IF(!is_valid(), std::logic_error, "Signal shape is not valid!");
      double res = 0.0;
      if ((t_ >= _t0_) && (t_ <= _t0_ + _duration_)) {
        double t1 = _t0_ + 0.5 * _duration_;
        if (t_ < t1) {
          res = _amplitude_ * 2 * (t_ - _t0_) / _duration_;
        } else {
          double t2 = 2 * _t0_ + _duration_ - t_;
          res = _amplitude_ * 2 * (t2 - _t0_) / _duration_;
        }
      }
      if (_polarity_ == POL_NEGATIVE) {
        res *= -1.0;
      }
      return res;
    }
    */

  } // end of namespace signal

} // end of namespace mctools
