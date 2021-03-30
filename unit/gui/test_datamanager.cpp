// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/dataManagerModel.h"
#include "gui/referencepoint.h"
#include "templates/list.h"
#include <gtest/gtest.h>

namespace UnitTest
{

class DataManagerTest : public ::testing::Test
{
    public:
    DataManagerTest() = default;

    protected:
    void SetUp() override {}
};

TEST_F(DataManagerTest, DataManger)
{

    List<ReferencePoint> points;
    DataManagerModel model(points);

    ASSERT_EQ(model.columnCount(), 2);
    ASSERT_EQ(model.rowCount(), 0);

    auto ref = points.add();
    ref->setSuffix("Suffix Example");
    ref->setRestartFile("Restart Example");

    model.update();

    ASSERT_EQ(model.rowCount(), 1);
    ASSERT_EQ(model.data(model.index(0, 0)).toString().toStdString(), "Suffix Example");
    ASSERT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "Restart Example");
}

} // namespace UnitTest
