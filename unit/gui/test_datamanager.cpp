// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/dataManagerReferencePointModel.h"
#include "classes/referencepoint.h"
#include <gtest/gtest.h>
#include <vector>

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

    CoreData coreData;
    Dissolve dissolve(coreData);
    std::vector<ReferencePoint> points;
    DataManagerReferencePointModel model(dissolve, points);

    ASSERT_TRUE(model.addFile("error", "restart/not.exist.restart"));

    auto result = model.addFile("benzene", "restart/benzene.txt.restart.txt");

    ASSERT_FALSE(result);

    ASSERT_EQ(model.columnCount(), 2);
    ASSERT_EQ(model.rowCount(), 0);

    model.update();

    ASSERT_EQ(model.rowCount(), 1);
    ASSERT_EQ(model.data(model.index(0, 0)).toString().toStdString(), "Suffix Example");
    ASSERT_EQ(model.data(model.index(0, 1)).toString().toStdString(), "Restart Example");
}

} // namespace UnitTest
