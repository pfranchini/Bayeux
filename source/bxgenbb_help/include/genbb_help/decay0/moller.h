#ifndef GENBB_DECAY0_MOLLER_H_
#define GENBB_DECAY0_MOLLER_H_ 1

namespace mygsl {
  class rng;
}
namespace genbb {
  class primary_event;
  namespace decay0 {

    /// \brief To sample the momentum of initial electron for MOLLER1 subroutine and store the momenta of scattered electron and delta ray
    // Input : E1,E2       - range of kinetic energy of gamma (MeV);
    //         teta1,teta2 - range of teta angle (radians);
    //         phi1,phi2   - range of phi  angle (radians);
    //         dcute       - lower energy threshold for emitted delta ray (MeV);
    // Output: see description of primary_event. Times emission of scattered
    //         electron and delta ray are supposed to be 0.
    // VIT, 11.12.1995.
    void decay0_moller(mygsl::rng & prng, primary_event & event,
                       double E1, double E2,
                       double teta1, double teta2,
                       double phi1, double phi2,
                       double dcute);

  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_MOLLER_H_
//
// Local Variables: --
// mode: c++ --
// End: --
