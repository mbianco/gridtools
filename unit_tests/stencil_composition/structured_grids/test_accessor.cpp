/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <gridtools/stencil_composition/structured_grids/accessor.hpp>
#include <gridtools/stencil_composition/structured_grids/accessor_mixed.hpp>

#include <gtest/gtest.h>

#include <gridtools/common/defs.hpp>
#include <gridtools/common/tuple_util.hpp>
#include <gridtools/stencil_composition/expressions/expressions.hpp>

using namespace gridtools;
using namespace expressions;

static_assert(is_accessor<accessor<6, intent::inout, extent<3, 4, 4, 5>>>::value, "");
static_assert(is_accessor<accessor<2, intent::in>>::value, "");
static_assert(!is_accessor<int>::value, "");
static_assert(!is_accessor<double &>::value, "");
static_assert(!is_accessor<double const &>::value, "");

TEST(accessor, trivial) {
    accessor<0, intent::inout, extent<0, 0, 0, 0>, 3> first(3, 2, -1);

    EXPECT_EQ(3, tuple_util::get<0>(first));
    EXPECT_EQ(2, tuple_util::get<1>(first));
    EXPECT_EQ(-1, tuple_util::get<2>(first));
}

TEST(accessor, array) {
    constexpr accessor<0, intent::inout, extent<0, 0, 0, 0>, 3> first(array<int_t, 3>{3, 2, -1});
    static_assert(tuple_util::get<0>(first) == 3, "");
    static_assert(tuple_util::get<1>(first) == 2, "");
    static_assert(tuple_util::get<2>(first) == -1, "");

    EXPECT_EQ(3, tuple_util::get<0>(first));
    EXPECT_EQ(2, tuple_util::get<1>(first));
    EXPECT_EQ(-1, tuple_util::get<2>(first));
}

/**
 * @brief interface with out-of-order optional arguments
 */
TEST(accessor, alternative1) {
    accessor<0, intent::inout, extent<0, 0, 0, 0>, 6> first(dimension<6>(-6), dimension<4>(12));

    EXPECT_EQ(0, tuple_util::get<0>(first));
    EXPECT_EQ(0, tuple_util::get<1>(first));
    EXPECT_EQ(0, tuple_util::get<2>(first));
    EXPECT_EQ(12, tuple_util::get<3>(first));
    EXPECT_EQ(0, tuple_util::get<4>(first));
    EXPECT_EQ(-6, tuple_util::get<5>(first));
}

/**
 * @brief interface with out-of-order optional arguments, represented as matlab indices
 */
TEST(accessor, alternative2) {
    constexpr dimension<1> i;
    constexpr dimension<2> j;

    constexpr dimension<4> t;
#if !defined(__INTEL_COMPILER) || __INTEL_COMPILER != 1800
    // ICC 18 shows some strange bug here
    constexpr accessor<0, intent::inout, extent<0, 0, 0, 0>, 4> first(i - 5, j, dimension<3>(8), t + 2);
    static_assert(tuple_util::get<0>(first) == -5, "");

    EXPECT_EQ(-5, tuple_util::get<0>(first));
    EXPECT_EQ(0, tuple_util::get<1>(first));
    EXPECT_EQ(8, tuple_util::get<2>(first));
    EXPECT_EQ(2, tuple_util::get<3>(first));
#endif
}

/**
 * @brief interface with aliases defined at compile-time
 * allows to split a single field in its different components, assigning an offset to each component.
 * The aforementioned offset is guaranteed to be treated as compile-time static constant value.
 */
TEST(accessor, static_alias) {
    // mixing compile time and runtime values
    using t = dimension<15>;
    typedef accessor<0, intent::inout, extent<0, 0, 0, 0>, 15> arg_t;
    using alias_t = alias<arg_t, t, dimension<1>, dimension<7>>::set<-3, 4, 2>;

    alias_t first(dimension<8>(23), dimension<3>(-5));

    EXPECT_EQ(2, tuple_util::get<6>(first));
    EXPECT_EQ(4, tuple_util::get<0>(first));
    EXPECT_EQ(-3, tuple_util::get<14>(first));
    EXPECT_EQ(23, tuple_util::get<7>(first));
    EXPECT_EQ(-5, tuple_util::get<2>(first));
}
