// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/configuration.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(AtomTypeMixTest, Basic)
{
    SmallMolecules molecules_;
    AtomTypeMix mix_;

    // Add atom types to our mix
    mix_.add(molecules_.atN(), 1);
    mix_.add(molecules_.atOW(), 2);
    mix_.add(molecules_.atHW(), 3);
    mix_.finalise();

    // Check basic data
    EXPECT_EQ(mix_.nItems(), 3);
    EXPECT_TRUE(mix_.contains(molecules_.atN()));
    EXPECT_FALSE(mix_.contains(molecules_.atH1()));

    // Retrieve atom type data
    auto optAtdN = mix_.atomTypeData(molecules_.atN());
    EXPECT_TRUE(optAtdN);
    auto optAtdHW = mix_.atomTypeData(molecules_.atHW());
    EXPECT_TRUE(optAtdHW);
    auto optAtdOW = mix_.atomTypeData(molecules_.atOW());
    EXPECT_TRUE(optAtdOW);
    auto optAtdH1 = mix_.atomTypeData(molecules_.atH1());
    EXPECT_FALSE(optAtdH1);

    // Check detailed data
    EXPECT_TRUE(optAtdN->get().atomType() == molecules_.atN());
    EXPECT_EQ(optAtdN->get().atomTypeName(), molecules_.atN()->name());
    EXPECT_DOUBLE_EQ(optAtdN->get().fraction(), 1.0 / 6.0);
    EXPECT_DOUBLE_EQ(optAtdOW->get().fraction(), 1.0 / 3.0);
    EXPECT_DOUBLE_EQ(optAtdHW->get().fraction(), 1.0 / 2.0);
}

TEST(AtomTypeMixTest, Indexing)
{
    SmallMolecules molecules_;
    AtomTypeMix mix_;

    // Add atom types to our mix
    std::vector<AtomTypeData> orderedData;
    orderedData.push_back(mix_.add(molecules_.atN(), 1));
    orderedData.push_back(mix_.add(molecules_.atOW(), 2));
    orderedData.push_back(mix_.add(molecules_.atHW(), 3));
    orderedData.push_back(mix_.add(molecules_.atH1(), 3));
    mix_.finalise();

    EXPECT_EQ(orderedData.size(), mix_.nItems());

    // Check indexed ordering
    auto index = 0;
    for (const auto &atomTypeData : orderedData)
    {
        EXPECT_EQ(atomTypeData.listIndex(), index);
        ++index;
    }

    // Remove one and check indexing again
    mix_.remove(molecules_.atOW());
    mix_.finalise();
    orderedData.erase(std::remove_if(orderedData.begin(), orderedData.end(),
                                     [&](const auto &atd) { return atd.atomType() == molecules_.atOW(); }),
                      orderedData.end());
    EXPECT_EQ(mix_.nItems(), 3);
    EXPECT_EQ(orderedData.size(), mix_.nItems());
    index = 0;
    for (const auto &atomTypeData : orderedData)
    {
        EXPECT_EQ(atomTypeData.listIndex(), index);
        ++index;
    }
}

} // namespace UnitTest
