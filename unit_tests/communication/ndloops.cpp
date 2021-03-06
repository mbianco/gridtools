/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "gtest/gtest.h"
#include <gridtools/common/array.hpp>
#include <gridtools/common/ndloops.hpp>
#include <iostream>
#include <sys/time.h>

struct sumup {
    mutable double res;
    int N;
    double *storage;
    sumup(int N, double *st) : res(0.0), N(N), storage(st) {}

    template <typename TUPLE>
    void operator()(TUPLE const &tuple) const {
        int idx = tuple[0] + tuple[1] * N + tuple[2] * N * N + tuple[3] * N * N * N;
        res += storage[idx];
    }
};

struct sumup2 {
    mutable double res;
    int N;
    double *storage;
    sumup2(int N, double *st) : res(0.0), N(N), storage(st) {}

    void operator()(int idx) const {
        // std::cout << ++i << " " << idx << "\n";
        res += storage[idx];
    }
};

struct print_tuple {
    template <typename TUPLE>
    void operator()(TUPLE const &tuple) const {
        std::cout << "(";
        for (unsigned int i = 0; i < tuple.size() - 1; ++i) {
            std::cout << tuple[i] << ", ";
        }
        std::cout << tuple[tuple.size() - 1] << ") x\n";
    }
};

struct print_int {
    void operator()(int idx) const { std::cout << "(" << idx << ")\n"; }
};

TEST(Communication, ndloops) {

    gridtools::array<int, 4> indices; /*= {3, 4, 3, 2}; // enabled in C++0x */
    indices[0] = 3;
    indices[1] = 4;
    indices[2] = 3;
    indices[3] = 2;
    gridtools::array<int, 4> dimensions; /*= {5, 5, 5, 5};  // enabled in C++0x */
    dimensions[0] = 5;
    dimensions[1] = 5;
    dimensions[2] = 5;
    dimensions[3] = 5;
    std::cout << gridtools::access_to<4>()(indices, dimensions) << "\n";

    int N = 3;

    gridtools::array<gridtools::bounds, 4> ab;
    ab[0].imin = 0;
    ab[0].imax = N - 1;
    ab[1].imin = 0;
    ab[1].imax = N - 1;
    ab[2].imin = 0;
    ab[2].imax = N - 1;
    ab[3].imin = 0;
    ab[3].imax = N - 1;

    print_int tmp;
    gridtools::access_loop<4, print_int>()(ab, dimensions, tmp);

    struct timeval start_tv;
    struct timeval stop_tv;
    double time;

    std::cout << "\n\n\n\n";
    gridtools::array<int, 4> tuple;
    print_tuple tmp2;
    gridtools::loop<4>()(ab, tmp2, tuple);

    double *storage = new double[N * N * N * N];

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                for (int l = 0; l < N; ++l) {
                    int idx = l + k * N + j * N * N + i * N * N * N;
                    storage[idx] = i + j + k + l;
                }

    ab[0].imin = 0;
    ab[0].imax = N - 1;
    ab[1].imin = 0;
    ab[1].imax = N - 1;
    ab[2].imin = 0;
    ab[2].imax = N - 1;
    ab[3].imin = 0;
    ab[3].imax = N - 1;

    dimensions[0] = N;
    dimensions[1] = N;
    dimensions[2] = N;
    dimensions[3] = N;

    std::cout << "start regular\n";

    gettimeofday(&start_tv, nullptr);
    double res = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                for (int l = 0; l < N; ++l) {
                    int idx = l + k * N + j * N * N + i * N * N * N;
                    res += storage[idx];
                }
    gettimeofday(&stop_tv, nullptr);

    time = (((double)stop_tv.tv_sec + 1 / 1000000.0 * (double)stop_tv.tv_usec) -
               ((double)start_tv.tv_sec + 1 / 1000000.0 * (double)start_tv.tv_usec)) *
           1000.0;

    std::cout << "result " << res << " time " << time << "\n";

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                for (int l = 0; l < N; ++l) {
                    int idx = l + k * N + j * N * N + i * N * N * N;
                    storage[idx] = (i + j + k + l) / 10.;
                }

    std::cout << "start loop\n";

    sumup summ(N, storage);
    gettimeofday(&start_tv, nullptr);
    gridtools::loop<4>()(ab, summ, tuple);
    gettimeofday(&stop_tv, nullptr);

    time = (((double)stop_tv.tv_sec + 1 / 1000000.0 * (double)stop_tv.tv_usec) -
               ((double)start_tv.tv_sec + 1 / 1000000.0 * (double)start_tv.tv_usec)) *
           1000.0;

    std::cout << "result " << summ.res << " time " << time << "\n";

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                for (int l = 0; l < N; ++l) {
                    int idx = l + k * N + j * N * N + i * N * N * N;
                    storage[idx] = (i + j + k + l) / 100.;
                }

    std::cout << "start loop with access function\n";

    sumup2 summ2(N, storage);
    gettimeofday(&start_tv, nullptr);
    gridtools::access_loop<4, sumup2>()(ab, dimensions, summ2);
    gettimeofday(&stop_tv, nullptr);

    time = (((double)stop_tv.tv_sec + 1 / 1000000.0 * (double)stop_tv.tv_usec) -
               ((double)start_tv.tv_sec + 1 / 1000000.0 * (double)start_tv.tv_usec)) *
           1000.0;

    std::cout << "result " << summ2.res << " time " << time << "\n";

    delete[] storage;

    EXPECT_TRUE(true);
}
