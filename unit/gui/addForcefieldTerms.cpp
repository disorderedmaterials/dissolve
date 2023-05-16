// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/addForcefieldDialogModel.h"
#include "main/dissolve.h"
#include <QTableView>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace UnitTest
{

class AddForcefieldDialogModelTest : public ::testing::Test
{
    public:
    AddForcefieldDialogModelTest() = default;

    protected:
    void SetUp() override{};
};

TEST_F(AddForcefieldDialogModelTest, benzene)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    dissolve.clear();
    ASSERT_TRUE(dissolve.loadInput("inputs/benzene.txt"));
    auto species = dissolve.findSpecies("benzene");
    ASSERT_NE(species, nullptr);

    AddForcefieldDialogModel model;
    model.setDissolve(dissolve);
    model.setSpecies(species);
    model.ready();

    EXPECT_EQ(model.forcefields()->rowCount(), 8);
    auto ff_name = model.forcefields()->data(model.forcefields()->index(1, 0), Qt::DisplayRole).toString();
    EXPECT_EQ(ff_name.toStdString(), "OPLSAA2005/Aromatics");
}
}
