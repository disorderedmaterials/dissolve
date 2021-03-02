// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "models/xmlAngleModel.h"
#include "models/xmlBondModel.h"
#include <gtest/gtest.h>

TEST(XmlFF, XmlBond)
{
    XmlBondModel bonds;

    bonds.readFile("/home/adam/Code/dissolve/tests/ff/methanol.xml");

    ASSERT_EQ(bonds.columnCount(), 4);
    ASSERT_EQ(bonds.rowCount(), 5);

    ASSERT_EQ(bonds.data(bonds.index(0, 0)), "O801");
}
