// -*- mode: c++; -*- 
/* sphere.cc
 */

#include <geomtools/sphere.h>

#include <cmath>
#include <stdexcept>
#include <sstream>

namespace geomtools {

  const std::string sphere::SPHERE_LABEL = "sphere";
  
  double 
  sphere::get_r () const
  {
    return _r_;
  }
  
  double 
  sphere::get_radius () const
  {
    return get_r ();
  }
  
  void 
  sphere::set_r (double new_value_)
  {
    if  (new_value_ < 0.0) 
      {
        std::ostringstream message;
        message << "sphere::set_r: Invalid '" 
                << new_value_ << "' R value!";
        throw std::logic_error (message.str ());
      }
    _r_ =new_value_;
  }

  void 
  sphere::set_radius (double new_value_)
  {
    set_r (new_value_);
  }
  
  void 
  sphere::set (double r_)
  {
    set_r (r_);
  }
  
  // ctor/dtor:
  sphere::sphere ()
  {
    reset ();
  }

  sphere::sphere (double r_)
  {
    set (r_);
  }
    
  sphere::~sphere ()
  {
  }

  double 
  sphere::get_surface (int mask_) const
  {
    double s = 0.0;
    if  (mask_ & FACE_SIDE) 
      {
        s += 4.0 * M_PI * _r_ * _r_;
      }
    return s;
  }

  double 
  sphere::get_volume () const 
  {
    return 4.0 * M_PI * _r_ * _r_ * _r_ / 3.0;
  }

  bool 
  sphere::is_valid () const
  {
    return  (_r_ > 0.0);
  }

  void 
  sphere::reset ()
  {
    _r_ = -1.0;
  }

  std::string 
  sphere::get_shape_name () const
  {
    return SPHERE_LABEL;
  }

  bool 
  sphere::is_inside (const vector_3d & point_, double skin_) const
  {
    double skin = get_skin ();
    if  (skin_ > USING_PROPER_SKIN) skin = skin_;

    double r = point_.mag ();
    if  (r > _r_ + 0.5 * skin) return false;
    return true;
  }

  double 
  sphere::get_parameter (const std::string & flag_) const
  {
    if  (flag_ == "r") return get_r ();
    if  (flag_ == "radius") return get_r ();
    if  (flag_ == "volume") return get_volume ();
    if  (flag_ == "surface.side") return get_surface (FACE_SIDE);
    if  (flag_ == "surface") return get_surface (FACE_ALL);

    throw std::runtime_error ("sphere::get_parameter: Unknown flag!");
  }

  vector_3d 
  sphere::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate (normal);
    if (is_on_surface (position_, FACE_SIDE)) 
      {
        normal = position_;
        normal.unit ();
      }
    return (normal);
  }

  bool 
  sphere::is_on_surface (const vector_3d & point_, 
                         int mask_    , 
                         double skin_) const
  {
    bool debug = false;
    //debug = true;
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = skin_;

    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    double hskin = 0.5 * skin;
    double r = point_.mag ();
    if (debug)
      {
        std::clog << "DEVEL: sphere::is_on_surface: r= "
                  << r
                  << std::endl;
        std::clog << "DEVEL: sphere::is_on_surface: R= "
                  << _r_
                  << std::endl;
        std::clog << "DEVEL: sphere::is_on_surface: hskin= "
                  << hskin
                  << std::endl;
      }
    if (mask & FACE_SIDE) 
      {
        if (std::abs (r - _r_) < hskin) return true;
      }
    return false;
  }
  
  bool 
  sphere::find_intercept (const vector_3d & from_, 
                          const vector_3d & direction_,
                          intercept_t & intercept_,
                          double skin_) const
  {
    bool debug = false;
    //debug = true;
    const unsigned int NFACES = 1;
    double t[NFACES];
    const int SPH_SIDE = 0;
    t[SPH_SIDE] = -1.0; // side
    double a, b, c;
    double x0 = from_.x ();
    double y0 = from_.y ();
    double z0 = from_.z ();
    double dx = direction_.x ();
    double dy = direction_.y ();
    double dz = direction_.z ();
    a = dx * dx + dy * dy + dz * dz;
    b = 2. * (dx * x0 + dy * y0 + dz * z0);
    c = x0 * x0 + y0 * y0 + z0 * z0 - _r_ * _r_;
    double delta = b * b - 4. * a * c;
    double ts[2];
    if (debug)
      {
        std::clog << "DEVEL: sphere::find_intercept: from= "
                  << from_
                  << std::endl;
        std::clog << "DEVEL: sphere::find_intercept: direction= "
                  << direction_
                  << std::endl;
        std::clog << "DEVEL: sphere::find_intercept: a= "
                  << a
                  << std::endl;
        std::clog << "DEVEL: sphere::find_intercept: b= "
                  << b
                  << std::endl;
        std::clog << "DEVEL: sphere::find_intercept: c= "
                  << c
                  << std::endl;
        std::clog << "DEVEL: sphere::find_intercept: delta= "
                  << delta
                  << std::endl;
      }
    if (delta >= 0.0)
      {
        double q = std::sqrt (delta);
        double n = a + a;
        ts[0] = (- b + q) / n;
        ts[1] = (- b - q) / n;
      }
    if (debug)
      {
        std::clog << "DEVEL: sphere::find_intercept: ts[" << 0 << "]= "
                  << ts[0]
                  << std::endl;
        std::clog << "DEVEL: sphere::find_intercept: ts[" << 1 << "]= "
                  << ts[1]
                  << std::endl;
      }
    for (int i = 0; i < 2; i++)
      {
        double tsi = ts[i];
        if (std::isnormal (tsi) && (tsi > 0.0))
          {
            if (t[SPH_SIDE] < 0) 
              {
                t[SPH_SIDE] = tsi;
              }
            else 
              {
                if (tsi < t[SPH_SIDE]) 
                  {
                    t[SPH_SIDE] = tsi;
                  }
              }
          }
      }
    
    double t_min = -1.0;
    int face_min = 0;
    for (int i = 0; i < (int) NFACES; i++)
      {
        double ti = t[i];
        if (debug)
          {
            std::clog << "DEVEL: sphere::find_intercept: t[" << i << "]= "
                      << ti << " t_min=" << t_min 
                      << " face_min=" << face_min 
                      << std::endl;
          }
        if (std::isnormal (ti) && (ti > 0.0))
          {
            int face_bit = (0x1 << i); // face mask
            vector_3d intercept = from_ + direction_ * ti;
            if (is_on_surface (intercept, face_bit, skin_))
              {
                if ((t_min < 0.0) || (ti < t_min))
                  {
                    t_min = ti;
                    face_min = face_bit;
                  }
              }
          }
        if (debug)
          {
            std::clog << "DEVEL: sphere::find_intercept: t_min=" << t_min 
                      << " face_min=" << face_min 
                      << std::endl;
          }
      }
    intercept_.reset ();
    if (face_min > 0) 
      {
        intercept_.set (0, face_min, from_ + direction_ * t_min);
      }
    return intercept_.is_ok ();
  }

  std::ostream & 
  operator<< (std::ostream & out_, const sphere & s_)
  {
    out_ << '{' << sphere::SPHERE_LABEL << ' ' 
         << s_._r_ << '}'; 
    return out_;
  }

  std::istream & 
  operator>> (std::istream & in_, sphere & s_)
  {
    s_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{') 
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      } 
    std::string name;
    in_ >> name;
    if (name != sphere::SPHERE_LABEL) 
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      } 
    double r;
    in_ >> r;
    if (! in_) 
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      } 
    c = 0;
    in_.get (c);
    if (c != '}') 
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      } 
    try 
      {
        s_.set (r);
      }
    catch (...) 
      {
        s_.reset ();
        in_.clear (std::ios_base::failbit);
      }
    return in_;
  }

  void sphere::tree_dump (ostream & out_, 
                          const string & title_, 
                          const string & indent_, 
                          bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)  
         << "R : " << get_r () / CLHEP::mm << " mm" << endl;
    return;
  }

  
} // end of namespace geomtools

// end of sphere.cc
