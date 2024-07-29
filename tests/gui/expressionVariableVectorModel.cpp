// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/expressionVariableVectorModel.h"
#include "generator/generator.h"
#include "generator/parameters.h"
#include "main/dissolve.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(ExpressionVariableVectorModelTest, Basic)
{
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Create a simple generator with a parameters node
    Generator generator;
    auto parameters = generator.createRootNode<ParametersGeneratorNode>({});
    parameters->addParameter("Alpha", 1.2345);
    parameters->addParameter("Beta", 99);
    parameters->addParameter("Gamma", -10);

    // Create a second set of data since we can't get the ParametersGeneratorNode data non-const
    std::vector<std::shared_ptr<ExpressionVariable>> data;
    data.push_back(std::make_shared<ExpressionVariable>("alf", 5.4321));
    data.push_back(std::make_shared<ExpressionVariable>("bert", 1));
    data.push_back(std::make_shared<ExpressionVariable>("gemma", 10));

    // Set up the model, supplying our mutable data and the parameters node as the parent
    ExpressionVariableVectorModel model;
    model.setData(data, parameters.get());

    // Size
    EXPECT_EQ(model.columnCount(QModelIndex()), 3);
    EXPECT_EQ(model.rowCount(QModelIndex()), 3);
    EXPECT_EQ(model.columnCount(model.index(0, 0)), 0);
    EXPECT_EQ(model.rowCount(model.index(0, 0)), 0);

    // Data
    EXPECT_EQ(model.data(model.index(0, 0), Qt::DisplayRole).toString().toStdString(), "alf");
    EXPECT_EQ(model.data(model.index(0, 1), Qt::DisplayRole).toString().toStdString(), "Real");
    EXPECT_DOUBLE_EQ(model.data(model.index(0, 2), Qt::DisplayRole).toDouble(), 5.4321);

    // Set
    EXPECT_TRUE(model.setData(model.index(0, 0), "colin", Qt::EditRole));
    EXPECT_EQ(model.data(model.index(0, 0), Qt::DisplayRole).toString().toStdString(), "colin");
    EXPECT_FALSE(model.setData(model.index(0, 1), "Anything", Qt::EditRole));
    EXPECT_TRUE(model.setData(model.index(0, 2), 4.567, Qt::EditRole));
    EXPECT_DOUBLE_EQ(model.data(model.index(0, 2), Qt::DisplayRole).toDouble(), 4.567);
    EXPECT_TRUE(model.setData(model.index(0, 2), 50, Qt::EditRole));
    EXPECT_EQ(model.data(model.index(0, 2), Qt::DisplayRole).toInt(), 50);
    EXPECT_DOUBLE_EQ(model.data(model.index(0, 2), Qt::DisplayRole).toDouble(), 50.0);
    EXPECT_FALSE(model.setData(model.index(1, 0), "Alpha", Qt::EditRole));
    EXPECT_FALSE(model.setData(model.index(1, 0), "Beta", Qt::EditRole));
    EXPECT_FALSE(model.setData(model.index(1, 0), "Gamma", Qt::EditRole));
    EXPECT_TRUE(model.setData(model.index(1, 0), "Gemma", Qt::EditRole));
}

} // namespace UnitTest
