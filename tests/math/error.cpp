// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/error.h"
#include "math/data1D.h"
#include <gtest/gtest.h>
#include <numeric>

namespace UnitTest
{

const std::vector<Range> ranges = {Range(0, 4), Range(5, 22), Range(23, 30), Range(31, 33), Range(34, 50)};
const Range totalRange{0, 50};

Data1D basicData()
{
    Data1D d;
    for (int x = totalRange.minimum(); x < totalRange.maximum(); ++x)
    {
        d.addPoint(x, std::rand());
    }
    return d;
}

TEST(ErrorRangeTest, Range)
{
    // Generating random one dimensional datasets
    auto A = basicData();
    auto B = basicData();

    // Vector of error reports for an ErrorType
    std::vector<double> errTypeReports;
    for (auto range : ranges)
    {
        errTypeReports.push_back(Error::error(Error::ASEError, A, B, range).error);
    }

    // Check to see that the sum of ranged errors is equal to the unranged error
    EXPECT_DOUBLE_EQ(std::accumulate(errTypeReports.begin(), errTypeReports.end(), 0.0),
                     Error::error(Error::ASEError, A, B).error);
}

} // namespace UnitTest
