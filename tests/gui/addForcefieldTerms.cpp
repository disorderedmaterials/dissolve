// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    ASSERT_TRUE(dissolve.loadInput("dissolve/input/full-benzene.txt"));
    auto species = dissolve.coreData().findSpecies("benzene");
    ASSERT_NE(species, nullptr);

    AddForcefieldDialogModel model;
    model.setDissolve(dissolve);
    model.setSpecies(species);
    model.ready();

    // ForceFieldPicker
    EXPECT_EQ(model.forcefields()->rowCount(), 8);
    auto ff_name = model.forcefields()->data(model.forcefields()->index(1, 0), Qt::DisplayRole).toString();
    EXPECT_EQ(ff_name.toStdString(), "OPLSAA2005/Aromatics");
    model.setFf(model.forcefields()->data(model.forcefields()->index(1, 0), Qt::UserRole + 3).value<Forcefield *>());
    EXPECT_TRUE(model.progressionAllowed());
    model.next();

    // ForceFieldAssign
    //  std::cout << (int) model.atomTypeRadio_ << std::endl;
    EXPECT_TRUE(model.progressionAllowed());
    model.next();

    // ForceFieldAtomType
    auto at_name = model.atomTypes()->data(model.atomTypes()->index(1, 0), Qt::DisplayRole).toString();
    EXPECT_EQ(at_name.toStdString(), "HA");
    EXPECT_TRUE(model.progressionAllowed());
    model.next();

    // ForceFieldIntra
    EXPECT_TRUE(model.progressionAllowed());
    model.next();

    // ForceFieldMaster
    EXPECT_EQ(model.bonds()->data(model.bonds()->index(1, 0), Qt::DisplayRole).toString().toStdString(), "CA-HA");
    EXPECT_EQ(model.angles()->data(model.angles()->index(1, 0), Qt::DisplayRole).toString().toStdString(), "CA-CA-HA");
    EXPECT_EQ(model.torsions()->data(model.torsions()->index(1, 0), Qt::DisplayRole).toString().toStdString(), "CA-CA-CA-HA");
    EXPECT_TRUE(model.progressionAllowed());

    // Prefix a bond
    model.addMasterPrefix(0, 1, "Prefix-");
    EXPECT_EQ(model.bonds()->data(model.bonds()->index(1, 0), Qt::DisplayRole).toString().toStdString(), "Prefix-CA-HA");
    model.addMasterSuffix(1, 1, "-Suffix");
    EXPECT_EQ(model.angles()->data(model.angles()->index(1, 0), Qt::DisplayRole).toString().toStdString(), "CA-CA-HA-Suffix");

    model.next();
}
} // namespace UnitTest
