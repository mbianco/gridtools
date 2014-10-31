#pragma once
#include "iteration_policy.h"

namespace gridtools {
    namespace _impl {

        namespace{
            /**
               @brief generic forward declaration of the execution_policy struct.
            */
            template < typename RunF >
                struct execution_policy;

            /**
               @brief forward declaration of the execution_policy struct
            */
            template <
                typename Arguments, typename T,
                template < typename U, typename Argument > class Back
                >
                struct execution_policy<Back<T,Arguments> >
            {
                typedef Arguments traits_t;
                typedef T execution_engine_t;
            };
        }//unnamed namespace

/**
   @brief basic token of execution responsible of handling the discretization over the vertical dimension. This may be done with a loop over k or by partitoning the k axis and executing in parallel, depending on the execution_policy defined in the multi-stage stencil. The base class is then specialized using the CRTP pattern for the different policies.
*/
        template < typename Derived >
        struct run_f_on_interval_base {

            /**\brief necessary because the Derived class is an incomplete type at the moment of the instantiation of the base class*/
            typedef typename execution_policy<Derived>::traits_t traits;
            typedef typename execution_policy<Derived>::execution_engine_t execution_engine;

            GT_FUNCTION
            explicit run_f_on_interval_base(typename traits::local_domain_t & domain, typename traits::coords_t const& coords)
                : m_coords(coords)
                , m_domain(domain)
            {}

            template <typename Interval>
            GT_FUNCTION
            void operator()(Interval const&) const {
                typedef typename index_to_level<typename Interval::first>::type from;
                typedef typename index_to_level<typename Interval::second>::type to;

                typedef iteration_policy<from, to, execution_engine::type::iteration> iteration_policy;

                if (boost::mpl::has_key<typename traits::interval_map_t, Interval>::type::value) {
                    typedef typename boost::mpl::at<typename traits::interval_map_t, Interval>::type interval_type;

                    int from=m_coords.template value_at<typename iteration_policy::from>();
                    int to=m_coords.template value_at<typename iteration_policy::to>();
                    // std::cout<<"from==> "<<from<<std::endl; 
                    // std::cout<<"to==> "<<to<<std::endl; 
                    static_cast<const Derived*>(this)->template loop<iteration_policy, interval_type>(from, to);
                }

            }
        protected:
            typename traits::coords_t const &m_coords;
            typename traits::local_domain_t const &m_domain;
        };

    } // namespace _impl
} // namespace gridtools