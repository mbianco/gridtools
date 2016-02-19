#pragma once
#include "computation.hpp"
#include "fill_conditionals_cxx03.hpp"
#include "if_.hpp"

namespace gridtools {

    namespace _impl {

#ifdef __CUDACC__
#define _POINTER_ computation*
#define _MAKE_POINTER_(data) new data
#else
#define _POINTER_ boost::shared_ptr<computation>
#define _MAKE_POINTER_(data) boost::make_shared< data >
#endif

#define _PAIR_(count, N, data)                  \
        data ## Type ## N data ## Value ## N

#define _MAKE_COMPUTATION_IMPL(z, n, nil) \
        template < \
            bool Positional, \
            typename Backend, \
            typename Domain, \
            typename Grid, \
            BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), typename MssType) \
            > \
        _POINTER_ make_computation_impl ( \
            Domain& domain, \
            const Grid& grid, \
            BOOST_PP_ENUM(BOOST_PP_INC(n), _PAIR_, Mss) \
            ) { \
            typedef typename boost::mpl::fold< \
                BOOST_PP_CAT( boost::mpl::vector, BOOST_PP_INC(n)) <BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), MssType)> \
                , boost::mpl::vector0<> \
                , boost::mpl::if_< \
                    is_condition<boost::mpl::_2> \
                , construct_conditionals_set< \
                    boost::mpl::_1 \
                      , boost::mpl::_2 > \
                , boost::mpl::_1> \
                >::type conditionals_set_mpl_t; \
 \
            typedef typename boost::mpl::fold< conditionals_set_mpl_t \
                                               , boost::mpl::set0<> \
                                               , boost::mpl::insert< \
                                                   boost::mpl::_1 \
                                                     , boost::mpl::_2> \
                                               >::type conditionals_check_t; \
 \
            GRIDTOOLS_STATIC_ASSERT((boost::mpl::size<conditionals_check_t>::type::value \
                                     == boost::mpl::size<conditionals_set_mpl_t>::type::value), \
                                    "Either you yoused the same switch_variable (or conditional) twice, or you used in the same computation two or more switch_variable (or conditional) with the same index. The index Id in condition_variable<Type, Id> (or conditional<Id>) must be unique to the computation, and can be used only in one switch_ statement."); \
 \
            typedef typename boost::fusion::result_of::as_set<conditionals_set_mpl_t>::type conditionals_set_t; \
            conditionals_set_t conditionals_set_; \
 \
            fill_conditionals(conditionals_set_, \
                              BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), MssValue) ); \
 \
            typedef intermediate<              \
                                          Backend             \
                                          , meta_array<(typename meta_array_vector## BOOST_PP_INC(n) <boost::mpl::vector0<>, \
                                                                                                             BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), MssType) \
                                                                                                             >::type), (boost::mpl::quote1<is_mss_descriptor>) > \
                                          , Domain \
                                          , Grid \
                                          , conditionals_set_t \
                                          , Positional \
                                               > intermediate_t;        \
            return _MAKE_POINTER_( intermediate_t )(boost::ref(domain), grid, conditionals_set_); \
    }

    } //namespace _impl

    // BOOST_PP_REPEAT(GT_MAX_MSS, _MAKE_COMPUTATION_IMPL, _)










    // 1 MSS
    template <
        bool Positional,
        typename Backend,
        typename Domain,
        typename Grid,
        typename Mss
        >
    _POINTER_ make_computation_impl (
        Domain& domain,
        Grid const& grid,
        Mss mss_
        ) {
        typedef typename boost::mpl::fold<
            boost::mpl::vector1<Mss>
            , boost::mpl::vector0<>
            , boost::mpl::if_<
                is_condition<boost::mpl::_2>
                  , construct_conditionals_set<
                      boost::mpl::_1
                        , boost::mpl::_2 >
                  , boost::mpl::_1>
            >::type conditionals_set_mpl_t;

        typedef typename boost::mpl::fold< conditionals_set_mpl_t
                                           , boost::mpl::set0<>
                                           , boost::mpl::insert<
                                               boost::mpl::_1
                                                 , boost::mpl::_2>
                                           >::type conditionals_check_t;

        GRIDTOOLS_STATIC_ASSERT((boost::mpl::size<conditionals_check_t>::type::value
                                 == boost::mpl::size<conditionals_set_mpl_t>::type::value),
                                "Either you yoused the same switch_variable (or conditional) twice, or you used in the same computation two or more switch_variable (or conditional) with the same index. The index Id in condition_variable<Type, Id> (or conditional<Id>) must be unique to the computation, and can be used only in one switch_ statement.");

        typedef typename boost::fusion::result_of::as_set<conditionals_set_mpl_t>::type conditionals_set_t;
        conditionals_set_t conditionals_set_;

        fill_conditionals(conditionals_set_,
                          mss_ );

        typedef intermediate<
            Backend
            , meta_array<typename meta_array_vector1 <boost::mpl::vector0<>,
                                                      Mss
                                                      >::type, boost::mpl::quote1<is_mss_descriptor> >
            , Domain
            , Grid
            , conditionals_set_t
            , Positional
            > intermediate_t;

        return _MAKE_POINTER_( intermediate_t )(boost::ref(domain), grid, conditionals_set_);
    }


    //2 MSS
    template <
        bool Positional,
        typename Backend,
        typename Domain,
        typename Grid,
        typename Mss1,
        typename Mss2
        >
    _POINTER_ make_computation_impl (
        Domain& domain,
        const Grid& grid,
        Mss1 mss1_,
        Mss1 mss2_
        ) {
        typedef typename boost::mpl::fold<
            boost::mpl::vector<Mss1, Mss2>
            , boost::mpl::vector0<>
            , boost::mpl::if_<
                is_condition<boost::mpl::_2>
                  , construct_conditionals_set<
                      boost::mpl::_1
                        , boost::mpl::_2 >
                  , boost::mpl::_1>
            >::type conditionals_set_mpl_t;

        typedef typename boost::mpl::fold< conditionals_set_mpl_t
                                           , boost::mpl::set0<>
                                           , boost::mpl::insert<
                                               boost::mpl::_1
                                                 , boost::mpl::_2>
                                           >::type conditionals_check_t;

        GRIDTOOLS_STATIC_ASSERT((boost::mpl::size<conditionals_check_t>::type::value
                                 == boost::mpl::size<conditionals_set_mpl_t>::type::value),
                                "Either you yoused the same switch_variable (or conditional) twice, or you used in the same computation two or more switch_variable (or conditional) with the same index. The index Id in condition_variable<Type, Id> (or conditional<Id>) must be unique to the computation, and can be used only in one switch_ statement.");

        typedef typename boost::fusion::result_of::as_set<conditionals_set_mpl_t>::type conditionals_set_t;
        conditionals_set_t conditionals_set_;

        fill_conditionals(conditionals_set_,
                          mss1_, mss2_ );

        typedef intermediate<
            Backend
                                      , meta_array<typename meta_array_vector2 <boost::mpl::vector0<>,
                                                                                Mss1, Mss2
                                                                                >::type, boost::mpl::quote1<is_mss_descriptor> >
                                      , Domain
                                      , Grid
                                      , conditionals_set_t
                                      , Positional
            > intermediate_t;
        return _MAKE_POINTER_( intermediate_t )(boost::ref(domain), grid, conditionals_set_);
    }



    //3 MSS
    template <
        bool Positional,
        typename Backend,
        typename Domain,
        typename Grid,
        typename Mss1,
        typename Mss2,
        typename Mss3
        >
    _POINTER_ make_computation_impl (
        Domain& domain,
        const Grid& grid,
        Mss1 mss1_,
        Mss1 mss2_,
        Mss1 mss3_
        ) {
        typedef typename boost::mpl::fold<
            boost::mpl::vector<Mss1, Mss2, Mss3>
            , boost::mpl::vector0<>
            , boost::mpl::if_<
                is_condition<boost::mpl::_2>
                  , construct_conditionals_set<
                      boost::mpl::_1
                        , boost::mpl::_2 >
                  , boost::mpl::_1>
            >::type conditionals_set_mpl_t;

        typedef typename boost::mpl::fold< conditionals_set_mpl_t
                                           , boost::mpl::set0<>
                                           , boost::mpl::insert<
                                               boost::mpl::_1
                                                 , boost::mpl::_2>
                                           >::type conditionals_check_t;

        GRIDTOOLS_STATIC_ASSERT((boost::mpl::size<conditionals_check_t>::type::value
                                 == boost::mpl::size<conditionals_set_mpl_t>::type::value),
                                "Either you yoused the same switch_variable (or conditional) twice, or you used in the same computation two or more switch_variable (or conditional) with the same index. The index Id in condition_variable<Type, Id> (or conditional<Id>) must be unique to the computation, and can be used only in one switch_ statement.");

        typedef typename boost::fusion::result_of::as_set<conditionals_set_mpl_t>::type conditionals_set_t;
        conditionals_set_t conditionals_set_;

        fill_conditionals(conditionals_set_,
                          mss1_, mss2_, mss3_ );

        typedef intermediate<
            Backend
                                      , meta_array<typename meta_array_vector3 <boost::mpl::vector0<>,
                                                                                Mss1, Mss2, Mss3
                                                                                >::type, boost::mpl::quote1<is_mss_descriptor> >
                                      , Domain
                                      , Grid
                                      , conditionals_set_t
                                      , Positional
            > intermediate_t;
        return _MAKE_POINTER_( intermediate_t )(boost::ref(domain), grid, conditionals_set_);
    }



#undef _MAKE_COMPUTATION_IMPL
#undef _PAIR_
#undef _POINTER_
#undef _MAKE_POINTER_

} //namespace gridtools
