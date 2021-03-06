/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "test_accumulate.hpp"
#include "gtest/gtest.h"

TEST(accumulate, test_and) { ASSERT_TRUE(test_accumulate_and()); }

TEST(accumulate, test_or) { ASSERT_TRUE(test_accumulate_or()); }
