#pragma once
#include <boost/lexical_cast.hpp>

#include "../common/basic_utils.h"
#include "../common/gpu_clone.h"
#include "../common/gt_assert.h"
#include "../common/is_temporary_storage.h"
#include "../stencil-composition/backend_traits_host.h"
#include "../stencil-composition/backend_traits_cuda.h"
#include "hybrid_pointer.h"
#include <iostream>

/**@file
   @brief Implementation of the main storage class, used by all backends, for temporary and non-temporary storage
 */
namespace gridtools {

    namespace _impl
    {
        template <int I, typename OtherLayout, int X>
        struct get_stride_
        {
            GT_FUNCTION
            static int get(const int* s) {
                return s[OtherLayout::template at_<I>::value];
            }
        };

        template <int I, typename OtherLayout>
        struct get_stride_<I, OtherLayout, 2>
        {
            GT_FUNCTION
            static int get(const int* ) {
#ifndef __CUDACC__
#ifndef NDEBUG
                //                std::cout << "U" ;//<< std::endl;
#endif
#endif
                return 1;
            }
        };

        template <int I, typename OtherLayout>
        struct get_stride
          : get_stride_<I, OtherLayout, OtherLayout::template at_<I>::value>
        {};

/**@brief Functor updating the pointers on the device */
        struct update_pointer {
            template <typename StorageType>
            GT_FUNCTION_WARNING
            void operator()(StorageType* s) const {}

#ifdef __CUDACC__
            template < typename T, typename U, bool B
                      >
            //GT_FUNCTION_WARNING
            void operator()(base_storage<enumtype::Cuda,T,U,B
                            > *& s) const {
                if (s) {
                    s->m_data.update_gpu();
                    s->clone_to_gpu();
                    s = s->gpu_object_ptr;
                }
            }
#endif
        };
    }//namespace _impl

    namespace _debug{
#ifndef NDEBUG
        struct print_pointer {
            template <typename StorageType>
            GT_FUNCTION_WARNING
            void operator()(StorageType* s) const {
                printf("CIAOOO TATATA %x\n",  s);
            }

#ifdef __CUDACC__
            template < typename T, typename U, bool B
                      >
            GT_FUNCTION_WARNING
            void operator()(base_storage<enumtype::Cuda,T,U,B
                            > *& s) const {
                printf("CIAO POINTER %X\n", s);
            }
#endif
        };
#endif
    }//namespace _debug

/**
   @biref main class for the storage
 */
    template < enumtype::backend Backend,
               typename ValueType,
               typename Layout,
               bool IsTemporary = false
               >
    struct base_storage : public clonable_to_gpu<base_storage<Backend, ValueType, Layout, IsTemporary> > {
        typedef Layout layout;
        typedef ValueType value_type;
        typedef value_type* iterator_type;
        typedef value_type const* const_iterator_type;
        typedef backend_from_id <Backend> backend_traits_t;

    public:
        explicit base_storage(int dim1, int dim2, int dim3,
                              value_type init = value_type(), std::string const& s = std::string("default name") ):
            m_size( ((layout::template at_<0>::value < 0)?1:dim1) * ((layout::template at_<1>::value < 0)?1:dim2) * ((layout::template at_<2>::value < 0)?1:dim3) ),
            is_set( true ),
            m_data( m_size ),
            m_name(s)
            {
                m_dims[0]=( dim1 );
                m_dims[1]=( dim2 );
                m_dims[2]=( dim3 );
                m_strides[0]=( layout::template find_val<2,int,1>(m_dims[0],m_dims[1],m_dims[2])*layout::template find_val<1,int,1>(m_dims[0],m_dims[1],m_dims[2]) );
                m_strides[1]=( (m_strides[2]==1)?0:layout::template find_val<2,int,1>(m_dims[0],m_dims[1],m_dims[2]) );
                m_strides[2]=( (m_strides[1]==1)?0:1 );
#ifdef _GT_RANDOM_INPUT
                srand(12345);
#endif
                //m_data = new value_type[m_size];
                for (int i = 0; i < m_size; ++i)
#ifdef _GT_RANDOM_INPUT
                    m_data[i] = init * rand();
#else
                m_data[i] = init;
#endif
                m_data.update_gpu();

            }


        __device__
        base_storage(base_storage const& other)
            : m_size(other.m_size)
            , is_set(other.is_set)
            , m_name(other.m_name)
            , m_data(other.m_data)
        {
            m_dims[0] = other.m_dims[0];
            m_dims[1] = other.m_dims[1];
            m_dims[2] = other.m_dims[2];

            m_strides[0] = other.m_strides[0];
            m_strides[1] = other.m_strides[1];
            m_strides[2] = other.m_strides[2];
        }

        explicit base_storage(): m_name("default_name"), m_data((value_type*)NULL) {
            is_set=false;
        }

        ~base_storage() {
            if (is_set) {
                //std::cout << "deleting " << std::hex << data << std::endl;
                //backend_traits_t::delete_storage( m_data );
	      //m_data.free_it();
                //delete[] m_data;
            }
        }

        std::string const& name() const {
            return m_name;
        }

        static void text() {
            std::cout << BOOST_CURRENT_FUNCTION << std::endl;
        }

        void h2d_update(){
            m_data.update_gpu();
            }

        void d2h_update(){
            m_data.update_cpu();
        }

        void info() const {
            // std::cout << m_dims[0] << "x"
            //           << m_dims[1] << "x"
            //           << m_dims[2] << ", "
            //           << std::endl;
        }

        GT_FUNCTION
        const_iterator_type min_addr() const {
            return &(m_data[0]);
        }


        GT_FUNCTION
        const_iterator_type max_addr() const {
            return &(m_data[m_size]);
        }

        GT_FUNCTION
        value_type& operator()(int i, int j, int k) {
            /* std::cout<<"indices= "<<i<<" "<<j<<" "<<k<<std::endl; */
            backend_traits_t::assertion(_index(i,j,k) >= 0);
            backend_traits_t::assertion(_index(i,j,k) < m_size);
            return m_data[_index(i,j,k)];
        }


        GT_FUNCTION
        value_type const & operator()(int i, int j, int k) const {
            backend_traits_t::assertion(_index(i,j,k) >= 0);
            backend_traits_t::assertion(_index(i,j,k) < m_size);
            return m_data[_index(i,j,k)];
        }

        template <int I>
        GT_FUNCTION
        int stride_along() const {
            return _impl::get_stride<I, layout>::get(m_strides); /*layout::template at_<I>::value];*/
        }

        GT_FUNCTION
        int offset(int i, int j, int k) const {
            return m_strides[0] * layout::template find_val<0,int,0>(i,j,k) +
                m_strides[1] * layout::template find_val<1,int,0>(i,j,k) +
                layout::template find_val<2,int,0>(i,j,k);
        }

	GT_FUNCTION
	inline int size() const {
	  return m_size;
	}

	GT_FUNCTION
	inline int stride_k() const {
        // std::cout << "Strides"
        //           << " " << m_strides[0]
        //           << " " << m_strides[1]
        //           << " " << m_strides[2]
        //           << std::endl;
        // std::cout << "Stride in k " << layout::template find_val<2,int,0>(m_strides) << std::endl;
        // return layout::template find_val<2,int,0>(m_strides);//e.g. (GPU test) =512*512=262144
        if (layout::template at_default<2,-1>::value == -1) {
            return 0;
        }

        if (layout::template at_default<2,-1>::value == 2) {
            return 1;
        }

        return m_strides[layout::template at_default<2,2>::value];
	}

        void print() const {
            print(std::cout);
        }
	void print_value(int i, int j, int k){ printf("value(%d, %d, %d)=%f, at index %d on the data\n", i, j, k, m_data[_index(i, j, k)], _index(i, j, k));}

    static const std::string info_string;

//private:
        int m_dims[3];
        int m_strides[3];
        int m_size;
        bool is_set;
        const std::string& m_name;
        typename backend_traits_t::template pointer<value_type>::type m_data;
        //iterator_type m_data;

        template <typename Stream>
        void print(Stream & stream) const {
            //std::cout << "Printing " << name << std::endl;
            stream << "(" << m_dims[0] << "x"
                      << m_dims[1] << "x"
                      << m_dims[2] << ")"
                      << std::endl;
            stream << "| j" << std::endl;
            stream << "| j" << std::endl;
            stream << "v j" << std::endl;
            stream << "---> k" << std::endl;

            int MI=12;
            int MJ=12;
            int MK=12;

            for (int i = 0; i < m_dims[0]; i += std::max(1,m_dims[0]/MI)) {
                for (int j = 0; j < m_dims[1]; j += std::max(1,m_dims[1]/MJ)) {
                    for (int k = 0; k < m_dims[2]; k += std::max(1,m_dims[2]/MK)) {
                        stream << "["/*("
                                          << i << ","
                                          << j << ","
                                          << k << ")"*/
                                  << this->operator()(i,j,k) << "] ";
                    }
                    stream << std::endl;
                }
                stream << std::endl;
            }
            stream << std::endl;
        }

        GT_FUNCTION
        int _index(int i, int j, int k) const {
            int index;
            if (IsTemporary) {
                index =
                    m_strides[0]
                    * (modulus(layout::template find_val<0,int,0>(i,j,k),layout::template find<0>(m_dims))) +
                    m_strides[1] * modulus(layout::template find_val<1,int,0>(i,j,k),layout::template find<1>(m_dims)) +
                    modulus(layout::template find_val<2,int,0>(i,j,k),layout::template find<2>(m_dims));
            } else {
                index =
                    m_strides[0]
                    * layout::template find_val<0,int,0>(i,j,k) +
                    m_strides[1] * layout::template find_val<1,int,0>(i,j,k) +
                    layout::template find_val<2,int,0>(i,j,k);
            }
            //assert(index >= 0);
            // assert(index <m_size);
            return index;
        }
    };

    // template <typename T>
    // struct is_temporary_storage {
    //     typedef boost::false_type type;
    // };


//huge waste of space because the C++ standard doesn't want me to initialize static const inline
    template < enumtype::backend B, typename ValueType, typename Layout, bool IsTemporary
        >
    const std::string base_storage<B , ValueType, Layout, IsTemporary
            >::info_string=boost::lexical_cast<std::string>(-1);


    template <enumtype::backend B, typename ValueType, typename Y>
    struct is_temporary_storage<base_storage<B,ValueType,Y,false>*& >
      : boost::false_type
    {};

    template <enumtype::backend X, typename ValueType, typename Y>
    struct is_temporary_storage<base_storage<X,ValueType,Y,true>*& >
      : boost::true_type
    {};

    template <enumtype::backend X, typename ValueType, typename Y>
    struct is_temporary_storage<base_storage<X,ValueType,Y,false>* >
      : boost::false_type
    {};

    template <enumtype::backend X, typename ValueType, typename Y>
    struct is_temporary_storage<base_storage<X,ValueType,Y,true>* >
      : boost::true_type
    {};

    template <enumtype::backend X, typename ValueType, typename Y>
    struct is_temporary_storage<base_storage<X,ValueType,Y,false> >
      : boost::false_type
    {};

    template <enumtype::backend X, typename ValueType, typename Y>
    struct is_temporary_storage<base_storage<X,ValueType,Y,true> >
      : boost::true_type
    {};


    template <enumtype::backend Backend, typename T, typename U, bool B>
    std::ostream& operator<<(std::ostream &s, base_storage<Backend,T,U, B> ) {
        return s << "base_storage <T,U," << " " << std::boolalpha << B << "> ";
            }


} //namespace gridtools