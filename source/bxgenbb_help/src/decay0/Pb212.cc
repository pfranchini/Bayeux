
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Pb212.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/decay0/alpha.h>
#include <genbb_help/decay0/gamma.h>
#include <genbb_help/decay0/electron.h>
#include <genbb_help/decay0/positron.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/pair.h>
#include <genbb_help/decay0/nucltransK.h>
#include <genbb_help/decay0/nucltransKL.h>
#include <genbb_help/decay0/nucltransKLM.h>
#include <genbb_help/decay0/nucltransKLM_Pb.h>
#include <genbb_help/decay0/beta.h>
#include <genbb_help/decay0/beta1.h>
#include <genbb_help/decay0/beta2.h>
#include <genbb_help/decay0/beta_1fu.h>
#include <genbb_help/decay0/PbAtShell.h>

namespace genbb {
  namespace decay0 {

    // Pb212.f 
    // This file was extracted from the 'decay0' program by V.I. Tretyak
    // Copyright 1995-2011 V.I. Tretyak
    // This program is free software
    // it under the terms of the GNU General Public License as published by
    // the Free Software Foundation
    // your option) any later version.
    // 
    // This program is distributed in the hope that it will be useful, but
    // WITHOUT ANY WARRANTY
    // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    // General Public License for more details.
    // 
    // You should have received a copy of the GNU General Public License
    // along with this program
    // Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
    // 

    void Pb212(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double p;
      double pbeta;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of Pb212 decay ("Table of Isotopes", 7th ed., 1978).
      // Three-figured labels correspond to energies of 212Bi excited
      // levels in keV.
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 5.08.1992, 22.10.1995.
      thnuc=38304.;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      pbeta=100.*prng_();
      if (pbeta <=  5.) goto label_415;  // 5%
      if (pbeta <= 88.) goto label_239;  // 83%
      goto label_10000;  // 12%
    label_415  :
      decay0_beta(prng_, event_, 0.158,83.,0.,0.,t);
      thlev=0.;
      p=100.*prng_();
      if (p <=  0.5) goto label_41501;  // 0.5%
      if (p <= 98.5) goto label_41502;  // 98.0%
      goto label_41503;  // 1.5%
    label_41501  :
      decay0_nucltransK(prng_, event_, 0.415,0.091,0.24,0.,tclev,thlev,tdlev);
      return;
    label_41502  :
      decay0_nucltransK(prng_, event_, 0.300,0.091,0.55,0.,tclev,thlev,tdlev);
      goto label_115;
    label_41503  :
      decay0_nucltransK(prng_, event_, 0.177,0.091,2.4,0.,tclev,thlev,tdlev);
      goto label_23900;
    label_239  :
      decay0_beta(prng_, event_, 0.334,83.,0.,0.,t);
    label_23900  :
      thlev=1.e-12;
      decay0_nucltransK(prng_, event_, 0.239,0.091,1.1,0.,tclev,thlev,tdlev);
      return;
    label_115  :
      thlev=8.e-12;
      decay0_nucltransK(prng_, event_, 0.115,0.091,8.0,0.,tclev,thlev,tdlev);
      return;
    label_10000  :
      decay0_beta(prng_, event_, 0.573,83.,0.,0.,t);
      return;
    }
    // end of Pb212.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Pb212.cc
// Local Variables: --
// mode: c++ --
// End: --
