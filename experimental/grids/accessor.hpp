#pragma once
#include "location_type.hpp"
#include <type_traits>


template <typename F1, typename F2>
struct make_mapreduce {
    using map_type = F1;
    using reduce_type = F2;
};

/**
   This struct is the one holding the function to apply when iterating
   on neighbors
 */
template <typename ValueType, typename Accessor, typename Lambda, typename Nested = void>
struct on_neighbors_impl {
    using accessor = Accessor;
    using function = Lambda;
    using location_type = typename accessor::location_type;
    using value_type = ValueType;

    const function ff;
    const value_type value;

    on_neighbors_impl(const function l, value_type value)
        : ff(l)
        , value(value)
    {}

    on_neighbors_impl(on_neighbors_impl const& other)
        : ff(other.ff)
        , value(other.value)
    {}
};

/**
   This struct is the one holding the function to apply when iterating
   on neighbors of neighbors
 */
template <typename ValueType, typename Accessor, typename MapReduce, typename NestedValueType, typename NestedAccessor, typename NestedLambda, typename NestedNext>
struct on_neighbors_impl<ValueType, Accessor, MapReduce, on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda, NestedNext> > {
    using accessor = Accessor;
    using next_accessor = on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda>;
    using map_function = typename MapReduce::map_type;
    using reduce_function = typename MapReduce::reduce_type;
    using location_type = typename accessor::location_type;
    using value_type = ValueType;

    const map_function map;
    const reduce_function reduction;
    const next_accessor nested_guy;
    const value_type value;

    on_neighbors_impl(map_function m, reduce_function r, next_accessor ng, value_type value)
        : map(m)
        , reduction(r)
        , nested_guy(ng)
        , value(value)
    {}

    on_neighbors_impl(on_neighbors_impl const& other)
        : map(other.map)
        , reduction(other.reduction)
        , nested_guy(other.nested_guy)
        , value(other.value)
    {}

    next_accessor next() const {
        return nested_guy;
    }
};

/**
   User friendly interface to let iterate on neighbors of a location of a location type
   (the one of the iteraion space). The neighbors are of a location type Accessor::location_type
 */
template <typename Accessor, typename Lambda, typename ValueType>
on_neighbors_impl<ValueType, Accessor, Lambda, void>
on_neighbors(Accessor, Lambda l, ValueType initial)
{
    return on_neighbors_impl<ValueType, Accessor, Lambda, void>(l, initial);
}

template <typename ValueType, typename Accessor, typename NestedAccessor, typename NestedLambda, typename NestedValueType, typename Map, typename Reduce, typename Rest>
on_neighbors_impl<ValueType, Accessor, make_mapreduce<Map, Reduce>, on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda, Rest> >
on_neighbors(Accessor, Map l, on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda, Rest> nested_guy, Reduce reduce, ValueType initial)
{
    return on_neighbors_impl<ValueType, Accessor, make_mapreduce<Map, Reduce>, on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda, Rest> >(l, reduce, nested_guy, initial);
}


/**
   User friendly interface to let iterate on neighbor cells of a cell
 */
template <typename ValueType, typename Accessor, typename Lambda>
on_neighbors_impl<ValueType, Accessor, Lambda>
on_cells(Accessor, Lambda l, ValueType initial) {
    static_assert(std::is_same<typename Accessor::location_type, gridtools::location_type<0>>::value,
        "The accessor provided to 'on_cells' is not on cells");
    return on_neighbors_impl<ValueType, Accessor, Lambda>(l, initial);
}

template <typename ValueType, typename Accessor, typename NestedAccessor, typename NestedLambda, typename NestedValueType, typename Map, typename Reduce, typename Rest>
on_neighbors_impl<ValueType, Accessor, make_mapreduce<Map, Reduce>, on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda, Rest> >
on_cells(Accessor, Map l, on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda, Rest> nested_guy, Reduce reduce, ValueType initial)
{
    static_assert(std::is_same<typename Accessor::location_type, gridtools::location_type<0>>::value,
        "The accessor (for a nested call) provided to 'on_cells' is not on cells");
    return on_neighbors_impl<ValueType, Accessor, make_mapreduce<Map, Reduce>, on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda, Rest> >(l, reduce, nested_guy, initial);
}

/**
   User friendly interface to let iterate on neighbor edges of a edge
 */
template <typename ValueType, typename Accessor, typename Lambda>
on_neighbors_impl<ValueType, Accessor, Lambda>
on_edges(Accessor, Lambda l, ValueType initial) {
    static_assert(std::is_same<typename Accessor::location_type, gridtools::location_type<1>>::value,
        "The accessor provided to 'on_edges' is not on edges");
    return on_neighbors_impl<ValueType, Accessor, Lambda>(l, initial);
}

template <typename ValueType, typename Accessor, typename NestedAccessor, typename NestedLambda, typename NestedValueType, typename Map, typename Reduce, typename Rest>
on_neighbors_impl<ValueType, Accessor, make_mapreduce<Map, Reduce>, on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda, Rest> >
on_edges(Accessor, Map l, on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda, Rest> nested_guy, Reduce reduce, ValueType initial)
{
    static_assert(std::is_same<typename Accessor::location_type, gridtools::location_type<1>>::value,
        "The accessor (for a nested call) provided to 'on_edges' is not on cells");
    return on_neighbors_impl<ValueType, Accessor, make_mapreduce<Map, Reduce>, on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda, Rest> >(l, reduce, nested_guy, initial);
}



/**
   This is the type of the accessors accessed by a stencil functor.
   It's a pretty minima implementation.
 */
template <int I, typename LocationType>
struct accessor {
    using this_type = accessor<I, LocationType>;
    using location_type = LocationType;
    static const int value = I;

    template <typename ValueType, typename Lambda>
    on_neighbors_impl<ValueType, this_type, Lambda>
    operator()(Lambda l, ValueType initial) const {
        return on_neighbors_impl<ValueType, this_type, Lambda>(l, initial);
    }
    template <typename ValueType, typename Lambda>
    static
    on_neighbors_impl<ValueType, this_type, Lambda>
    neighbors(Lambda l, ValueType initial) {
        return on_neighbors_impl<ValueType, this_type, Lambda>(l, initial);
    }

    template <typename ValueType,
              typename Map,
              typename Reduce,
              typename NestedValueType,
              typename NestedAccessor,
              typename NestedLambda>
    on_neighbors_impl<ValueType
                      , this_type
                      , make_mapreduce<Map, Reduce>
                      , on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda>
                      >
    operator()(Map l,
               on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda> nested_guy,
               Reduce reduce,
               ValueType initial) const
    {
        return on_neighbors_impl<ValueType, this_type, make_mapreduce<Map, Reduce>, on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda> >(l, reduce, nested_guy, initial);
    }

    template <typename ValueType,
              typename Map,
              typename Reduce,
              typename NestedValueType,
              typename NestedAccessor,
              typename NestedLambda>
    static
    on_neighbors_impl<ValueType
                      , this_type
                      , make_mapreduce<Map, Reduce>
                      , on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda> >
    neighbors(Map l,
              on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda> nested_guy,
              Reduce reduce,
              ValueType initial) {
        return on_neighbors_impl<ValueType, this_type, make_mapreduce<Map, Reduce>, on_neighbors_impl<NestedValueType, NestedAccessor, NestedLambda> >(l, reduce, nested_guy, initial);
    }
};


/**
   This class is basically the iterate domain. It contains the
   ways to access data and the implementation of iterating on neighbors.
 */
template <typename PlcVector, typename GridType, typename LocationType>
struct accessor_type {

private:

    template <typename GridType_>
    struct get_pointer {
        template <typename PlcType>
        struct apply {
            using type = typename GridType_::template pointer_to<typename PlcType::location_type>::type;
        };
    };
    template <typename GridType_>
    struct get_storage {
        template <typename PlcType>
        struct apply {
            using type = typename GridType_::template storage_type<typename PlcType::location_type>::type;
        };
    };

public:
    using mpl_storage_types = typename boost::mpl::transform<PlcVector,
                                                             get_storage<GridType>
                                                             >::type;

    using storage_types = typename boost::fusion::result_of::as_vector<mpl_storage_types>::type;

    using mpl_pointers_t_ = typename boost::mpl::transform<PlcVector,
                                                           get_pointer<GridType>
                                                           >::type;

    using pointers_t = typename boost::fusion::result_of::as_vector<mpl_pointers_t_>::type;

    using grid_type = GridType;
    using location_type = LocationType;
private:
    storage_types storages;
    pointers_t pointers;
    grid_type const& m_grid;
    uint_t m_pos_i;
    uint_t m_pos_j;

    uint_t m_ll_i;
    uint_t m_ll_j;
    uint_t m_ll_k;

    template <typename PointersT, typename StoragesT>
    struct _set_pointers
    {
        PointersT &m_pt;
        StoragesT const &m_st;
        _set_pointers(PointersT& pt, StoragesT const &st): m_pt(pt), m_st(st) {}

        template <typename Index>
        void operator()(Index) {
            double * ptr = const_cast<double*>((boost::fusion::at_c<Index::value>(m_st))->min_addr());
            std::cout << " -------------------------> Pointer " << std::hex << ptr << std::dec << std::endl;
            boost::fusion::at_c<Index::value>(m_pt) = ptr;
        }
    };

    template <typename LocationT, typename PointersT, typename StoragesT, typename GridT>
    struct _set_pointers_to
    {
        PointersT &m_pt;
        StoragesT const &m_st;
        GridT const& m_g;
        const uint_t m_ll_i;
        const uint_t m_ll_j;
        const uint_t m_ll_k;

        _set_pointers_to(PointersT& pt,
                         StoragesT const &st,
                         GridT const& g,
                         const uint_t ll_i,
                         const uint_t ll_j,
                         const uint_t ll_k)
            : m_pt(pt)
            , m_st(st)
            , m_g(g)
            , m_ll_i(ll_i)
            , m_ll_j(ll_j)
            , m_ll_k(ll_k)
        {}

        template <typename Index>
        void operator()(Index) {
            double * ptr = const_cast<double*>((boost::fusion::at_c<Index::value>(m_st))->min_addr())
                + (boost::fusion::at_c<LocationT::value>(m_g.virtual_storages())->_index(m_ll_i, m_ll_j, m_ll_k));
            std::cout << " -------------------------> Pointer_To " << std::hex << ptr << std::dec << std::endl;
            boost::fusion::at_c<Index::value>(m_pt) = ptr;
        }
    };

    template <int Coordinate, typename PointersT, typename GridT>
    struct _move_pointers
    {
        PointersT &m_pt;
        GridT const &m_g;

        _move_pointers(PointersT& m_pt, GridT const& m_g): m_pt(m_pt), m_g(m_g) {}

        template <typename Index>
        void operator()(Index) {
            auto value = boost::fusion::at_c<boost::mpl::at_c<PlcVector, Index::value>::type::location_type::value>
                (m_g.virtual_storages())->template strides<Coordinate>();
            //std::cout << "Stide<" << Index::value << "> for coordinate " << Coordinate << " = " << value << std::endl;
            boost::fusion::at_c<Index::value>(m_pt) += value;
        }
    };

    void _increment_pointers_i()
    {
        using indices = typename boost::mpl::range_c<int, 0, boost::fusion::result_of::size<storage_types>::type::value >;
        boost::mpl::for_each<indices>(_move_pointers<0, pointers_t, grid_type>(pointers, m_grid));
    }

    void _increment_pointers_j()
    {
        using indices = typename boost::mpl::range_c<int, 0, boost::fusion::result_of::size<storage_types>::type::value >;
        boost::mpl::for_each<indices>(_move_pointers<1, pointers_t, grid_type>(pointers, m_grid));
    }

    void _reset_pointers()
    {
        using indices = typename boost::mpl::range_c<int, 0, boost::fusion::result_of::size<storage_types>::type::value >;
        boost::mpl::for_each<indices>(_set_pointers<pointers_t, storage_types>(pointers, storages));
    }

    struct printplc {
        template <typename T>
        void operator()(T const& e) const {
            std::cout << "printing placeholders: " << e << std::endl;
        }
    };


    template <typename LocationT>
    void _set_pointers_to_ll() {
        using indices = typename boost::mpl::range_c<int, 0, boost::fusion::result_of::size<storage_types>::type::value >;
        boost::mpl::for_each<indices>(_set_pointers_to<LocationT, pointers_t, storage_types, grid_type>(pointers, storages, m_grid, m_ll_i, m_ll_j, m_ll_k));
    }

public:
    accessor_type(storage_types const& storages, GridType const& m_grid, uint_t pos_i, uint_t pos_j)
        : storages(storages)
        , m_grid(m_grid)
        , m_pos_i(pos_i)
        , m_pos_j(pos_j)
    {
        _reset_pointers();
        boost::mpl::for_each<PlcVector>(printplc());
    }

    GridType const& grid() const {return m_grid;}

    void inc_i() {++m_pos_i; _increment_pointers_i();}
    void inc_j() {++m_pos_j; _increment_pointers_j();}

    void inc_ll_k() {_increment_pointers_j();}

    void reset_i() {m_pos_i = 0;}
    void reset_j() {m_pos_j = 0;}
    void set_ij(int i, int j) {
        m_pos_i = i;
        m_pos_j = j;
    }

    template <typename LocationT>
    void set_ll_ij(int i, int j, int k) {
        m_ll_i = i;
        m_ll_j = j;
        m_ll_k = k;
        _set_pointers_to_ll<LocationT>();
    }

    uint_t i() const {return m_pos_i;}
    uint_t j() const {return m_pos_j;}

    template <typename ValueType, typename Arg, typename Accumulator>
    double operator()(on_neighbors_impl<ValueType, Arg, Accumulator, void> onneighbors) const {

        auto neighbors = m_grid.neighbors( {m_pos_i, m_pos_j}, location_type(), typename Arg::location_type() );
        double result = onneighbors.value;
        //std::cout << "on_cells " << Arg::value << std::endl;

        for (int i = 0; i<neighbors.size(); ++i) {
            result = onneighbors.ff(*(boost::fusion::at_c<Arg::value>(pointers)+neighbors[i]), result);
        }
    }

    template <typename ValueType, typename Arg, typename Accumulator, typename NextLevel>
    double operator()(on_neighbors_impl<ValueType, Arg, Accumulator, NextLevel> onneighbors) const {
        auto neighbors = m_grid.ll_indices( {m_pos_i, m_pos_j}, location_type() );
        __begin_iteration<location_type>(onneighbors, neighbors);
    }

    template <int I, typename LT>
    double& operator()(accessor<I,LT> const& arg) const {
        return *(boost::fusion::at_c<I>(pointers));
    }

private:
    template <typename LocationTypeSrc, typename ValueType, typename Arg, typename Accumulator, typename NextLevel>
    double __begin_iteration(on_neighbors_impl<ValueType, Arg, Accumulator, NextLevel> __onneighbors,
                             gridtools::array<uint_t, 3> const& indices) const
    {
        auto neighbors = m_grid.neighbors_indices_3( indices, LocationTypeSrc(), typename Arg::location_type() );
        double result = __onneighbors.value;
        std::cout << "<" << neighbors.size() << ">";
        for (int i = 0; i<neighbors.size(); ++i) {
            result = __onneighbors.reduction(__onneighbors.map
                (*(boost::fusion::at_c<Arg::value>(pointers)
                   +m_grid.ll_offset(neighbors[i], typename Arg::location_type())),
                 __begin_iteration<typename Arg::location_type>(__onneighbors.next(), neighbors[i])), result);
        }
        return result;
    }

    template <typename LocationTypeSrc, typename ValueType, typename Arg, typename Accumulator>
    double __begin_iteration(on_neighbors_impl<ValueType, Arg, Accumulator, void> __onneighbors,
                             gridtools::array<uint_t, 3> const& indices) const
    {
        auto neighbors = m_grid.neighbors_indices_3( indices, LocationTypeSrc(), typename Arg::location_type() );
        double result = __onneighbors.value;
        for (int i = 0; i<neighbors.size(); ++i) {
            result = __onneighbors.ff
                (*(boost::fusion::at_c<Arg::value>(pointers)
                   +m_grid.ll_offset(neighbors[i], typename Arg::location_type())),
                 result
                 );
        }
        return result;
    }

};
