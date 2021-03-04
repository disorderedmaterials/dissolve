// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "models/xmlAngleModel.h"
#include "models/xmlBondModel.h"
#include <gtest/gtest.h>
#include <tuple>
#include <vector>

TEST(XmlFF, XmlBond)
{
    XmlBondModel bonds;

    bonds.readFile("/home/adam/Code/dissolve/tests/ff/methanol.xml");

    ASSERT_EQ(bonds.columnCount(), 4);
    ASSERT_EQ(bonds.rowCount(), 5);

    std::vector<XmlBondData> reference = {{"O801", "C800", 0.141000, 267776.000000},
					  {"H802", "C800", 0.109000, 284512.000000},
					  {"H803", "C800", 0.109000, 284512.000000},
					  {"H804", "C800", 0.109000, 284512.000000},
					  {"H805", "O801", 0.094500, 462750.400000}};

    int row = 0;
    for (auto b : reference)
    {
	ASSERT_EQ(bonds.data(bonds.index(row, 0)).toString().toStdString(), std::get<0>(b));
	ASSERT_EQ(bonds.data(bonds.index(row, 1)).toString().toStdString(), std::get<1>(b));
	ASSERT_EQ(bonds.data(bonds.index(row, 2)).toDouble(), std::get<2>(b));
	ASSERT_EQ(bonds.data(bonds.index(row, 3)).toDouble(), std::get<3>(b));
	++row;
    }
}

TEST(XmlFF, XmlAngle)
{
    XmlAngleModel angles;

    angles.readFile("/home/adam/Code/dissolve/tests/ff/methanol.xml");

    ASSERT_EQ(angles.columnCount(), 5);
    ASSERT_EQ(angles.rowCount(), 7);

    std::vector<XmlAngleData> reference = {
	{"O801", "C800", "H802", 1.911136, 292.880000}, {"O801", "C800", "H803", 1.911136, 292.880000},
	{"O801", "C800", "H804", 1.911136, 292.880000}, {"C800", "O801", "H805", 1.893682, 460.240000},
	{"H802", "C800", "H803", 1.881465, 276.144000}, {"H802", "C800", "H804", 1.881465, 276.144000},
	{"H803", "C800", "H804", 1.881465, 276.144000}};

    int row = 0;
    for (auto b : reference)
    {
	ASSERT_EQ(angles.data(angles.index(row, 0)).toString().toStdString(), std::get<0>(b));
	ASSERT_EQ(angles.data(angles.index(row, 1)).toString().toStdString(), std::get<1>(b));
	ASSERT_EQ(angles.data(angles.index(row, 2)).toString().toStdString(), std::get<2>(b));
	ASSERT_EQ(angles.data(angles.index(row, 3)).toDouble(), std::get<3>(b));
	ASSERT_EQ(angles.data(angles.index(row, 4)).toDouble(), std::get<4>(b));
	++row;
    }
}
