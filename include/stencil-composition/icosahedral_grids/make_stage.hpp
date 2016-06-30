#pragma once

#include "common/generic_metafunctions/variadic_to_vector.hpp"
#include "stencil-composition/mss_metafunctions.hpp"

namespace gridtools {

    template < template < uint_t > class Functor, typename Grid, typename LocationType, typename... Args >
    esf_descriptor< Functor, Grid, LocationType, nocolor, boost::mpl::vector< Args... > > make_stage(Args &&... /*args_*/) {
        return esf_descriptor< Functor, Grid, LocationType, nocolor, boost::mpl::vector< Args... > >();
    }
} // namespace gridtools
