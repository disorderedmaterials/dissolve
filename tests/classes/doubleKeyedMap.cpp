// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "templates/doubleKeyedMap.h"
#include "tests/testing.h"

namespace UnitTest
{

TEST(DoubleKeyedMap, BasicMirrored)
{
    DoubleKeyedMap<int> map(true);

    // Add and retrieve an entry
    DoubleKeyedMapKey key("A", "B");
    map[key] = 10;
    EXPECT_TRUE(map.contains(key));
    EXPECT_TRUE(map.contains("A", "B"));
    EXPECT_EQ(map[key], 10);

    // Triangular mapping...
    EXPECT_TRUE(map.contains("B", "A"));

    // Remove key
    ASSERT_NO_THROW(map.erase("A", "B"));
    EXPECT_FALSE(map.contains("A", "B"));
}

TEST(DoubleKeyedMap, BasicFull)
{
    DoubleKeyedMap<int> map(false);

    // Add and retrieve an entry
    DoubleKeyedMapKey key("A", "B");
    map[key] = 10;
    EXPECT_TRUE(map.contains(key));
    EXPECT_TRUE(map.contains("A", "B"));
    EXPECT_EQ(map[key], 10);

    // Full mapping...
    EXPECT_FALSE(map.contains("B", "A"));
}

TEST(DoubleKeyedMap, SerialisationInts)
{
    DoubleKeyedMap<int> a(false), b;
    a[DoubleKeyedMapKey{"A", "A"}] = 10;
    a[DoubleKeyedMapKey{"A", "B"}] = 22;
    a[DoubleKeyedMapKey{"B", "B"}] = 11;

    auto keys = DissolveSys::splitString("A//A", "//");
    std::cout << std::format("Keys = {}\n", keys.size());

    EXPECT_NO_THROW(tomlRoundTrip(a, b));
    EXPECT_EQ(a.size(), b.size());
    for (auto &[key, value] : a)
    {
        ASSERT_TRUE(b.contains(key));
        EXPECT_EQ(value, b[key]);
    }
}

TEST(DoubleKeyedMap, SerialisationClass)
{
    DoubleKeyedMap<Data1D> a(false), b;
    Data1D data;
    for (auto n = 0; n < 100; ++n)
        data.addPoint(n * (M_PI * 2.0) / 100.0, sin(n * M_PI * 2.0 / 100));

    a[DoubleKeyedMapKey{"A", "A"}] = data;
    data += 1.0;
    a[DoubleKeyedMapKey{"A", "B"}] = data;
    data *= M_PI;
    a[DoubleKeyedMapKey{"B", "B"}] = data;

    EXPECT_NO_THROW(tomlRoundTrip(a, b));

    EXPECT_EQ(a.size(), b.size());
    for (auto &[key, value] : a)
    {
        ASSERT_TRUE(b.contains(key));

        EXPECT_LT(Error::error(Error::ErrorType::RMSEError, value, b[key]).error, 1.0e-6);
    }
}

} // namespace UnitTest