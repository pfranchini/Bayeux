
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Os190low.h>
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

    // Os190low.f 
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

    void Os190low(mygsl::rng & prng_, genbb::primary_event & event_, int levelkev)
    {
      //double t;
      double tdlev;
      double p;
      double tclev;
      double thlev;
      // Subroutine describes the deexcitation process in Os190 nucleus
      // after eb+/2e decay of Pt190 to ground and excited 0+ and 2+ levels
      // of Os190 (in accordance with NDS 99(2003)275 and NNDC on 22.07.2010).
      // Level (0,1,2)+ at 1382 keV is supposed to be 0+.
      // Call : call Os190low(levelkev)
      // Input : levelkev - energy of Os190 level (integer in keV) occupied
      // initially
      // 0+(gs) - 0 keV,
      // 2+(1) - 187 keV,
      // 2+(2) - 558 keV,
      // 0+(1) - 912 keV,
      // 2+(3) - 1115 keV,
      // 0+(2) - 1382 keV.
      // Output: // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 30.07.2010.
      tclev=0.;
      if (levelkev == 1382) goto label_1382;
      if (levelkev == 1115) goto label_1115;
      if (levelkev ==  912) goto label_912;
      if (levelkev ==  558) goto label_558;
      if (levelkev ==  187) goto label_187;
      if (levelkev ==  0) goto label_10000;
      goto label_20000;
    label_1382  :
      thlev=0.;
      decay0_nucltransK(prng_, event_, 1.196,0.074,3.5e-3,4.0e-6,tclev,thlev,tdlev);
      goto label_187;
    label_1115  :
      thlev=0.;
      p=100.*prng_();
      if (p <= 28.8) goto label_11151;
      if (p <= 79.3) goto label_11152;
      if (p <= 97.0) goto label_11153;
      goto label_11154;
    label_11151  :
      decay0_nucltransK(prng_, event_, 1.115,0.074,4.0e-3,3.5e-7,tclev,thlev,tdlev);
      return;
    label_11152  :
      decay0_nucltransK(prng_, event_, 0.928,0.074,5.8e-3,0.,tclev,thlev,tdlev);
      goto label_187;
    label_11153  :
      decay0_nucltransK(prng_, event_, 0.359,0.074,5.4e-2,0.,tclev,thlev,tdlev);
      goto label_756;
    label_11154  :
      decay0_nucltransK(prng_, event_, 0.203,0.074,3.2e-1,0.,tclev,thlev,tdlev);
      goto label_912;
    label_912  :
      thlev=15e-12;
      p=100.*prng_();
      if (p <= 77.3) goto label_91201;
      goto label_91202;
    label_91201  :
      decay0_nucltransK(prng_, event_, 0.725,0.074,9.7e-3,0.,tclev,thlev,tdlev);
      goto label_187;
    label_91202  :
      decay0_nucltransK(prng_, event_, 0.354,0.074,6.0e-2,0.,tclev,thlev,tdlev);
      goto label_558;
    label_756  :
      thlev=0.;
      p=100.*prng_();
      if (p <= 87.1) goto label_75601;
      if (p <= 91.7) goto label_75602;
      goto label_75603;
    label_75601  :
      decay0_nucltransK(prng_, event_, 0.569,0.074,1.7e-2,0.,tclev,thlev,tdlev);
      goto label_187;
    label_75602  :
      decay0_nucltransK(prng_, event_, 0.208,0.074,3.0e-1,0.,tclev,thlev,tdlev);
      goto label_548;
    label_75603  :
      decay0_nucltransK(prng_, event_, 0.198,0.074,3.5e-1,0.,tclev,thlev,tdlev);
      goto label_558;
    label_558  :
      thlev=15.2e-12;
      p=100.*prng_();
      if (p <= 56.7) goto label_55801;
      goto label_55802;
    label_55801  :
      decay0_nucltransK(prng_, event_, 0.558,0.074,1.8e-2,0.,tclev,thlev,tdlev);
      return;
    label_55802  :
      decay0_nucltransK(prng_, event_, 0.371,0.074,5.1e-2,0.,tclev,thlev,tdlev);
      goto label_187;
    label_548  :
      thlev=12.8e-12;
      decay0_nucltransK(prng_, event_, 0.361,0.074,5.4e-2,0.,tclev,thlev,tdlev);
      goto label_187;
    label_187  :
      thlev=375e-12;
      decay0_nucltransK(prng_, event_, 0.187,0.074,4.25e-1,0.,tclev,thlev,tdlev);
      return;
    label_10000  :
      return;
    label_20000  :
      // print *,'Os190: wrong level [keV] ',levelkev
      return;
    }
    // end of Os190low.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Os190low.cc
// Local Variables: --
// mode: c++ --
// End: --
