// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "items/list.h"
#include "math/data1D.h"
#include <gtest/gtest.h>
#include <string>
#include <templates/algorithms.h>

namespace UnitTest
{

TEST(GenericListTest, GenericList)
{
    GenericList list;

    // POD set / get
    const auto d = 1.23456;
    list.realise<double>("DOUBLE") = d;
    EXPECT_DOUBLE_EQ(d, list.value<double>("DOUBLE"));
    EXPECT_DOUBLE_EQ(d, list.retrieve<double>("DOUBLE"));

    // Class set / get
    Data1D testData;
    for (auto n = 0; n < 100; ++n)
        testData.addPoint(n, rand() * 0.1);
    list.realise<Data1D>("XY") = testData;
    auto xy1 = list.value<Data1D>("XY");
    for (auto &&[a, b] : zip(xy1.values(), testData.values()))
        EXPECT_DOUBLE_EQ(a, b);
    list.retrieve<Data1D>("XY") += xy1;
    auto xy2 = list.value<Data1D>("XY");
    for (auto &&[a, b] : zip(xy2.values(), testData.values()))
        EXPECT_DOUBLE_EQ(a, 2.0 * b);

    // Missing item
    EXPECT_ANY_THROW(list.value<int>("INT"));
    EXPECT_ANY_THROW(list.retrieve<int>("INT"));

    // Bad cast
    EXPECT_ANY_THROW(list.value<std::string>("DOUBLE"));
    EXPECT_ANY_THROW(list.retrieve<std::vector<double>>("DOUBLE"));

    // Bad realise
    list.realise<int>("INT") = 99;
    EXPECT_ANY_THROW(list.realise<std::string>("INT") = "99");
}

} // namespace UnitTest
