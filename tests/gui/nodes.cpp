// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/models/dissolveModel.h"
#include "gui/models/nodeGraph/graphModel.h"
#include "main/dissolve.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace UnitTest
{
class NodeModelTest : public ::testing::Test
{
    public:
    NodeModelTest() = default;

    protected:
    void SetUp() override {}
};

TEST_F(NodeModelTest, BasicNodes)
{
    CoreData coreData;
    Dissolve dissolve(coreData);
    DissolveModel dissolveModel;
    dissolveModel.setDissolve(dissolve);

    GraphModel model;
    auto nodes = model.nodes();
    auto edges = model.edges();
    model.setGraph(dissolveModel.graph());

    ASSERT_EQ(model.count(), 0);

    model.emplace_back(100, 111, "Add", "Summing Node");
    ASSERT_EQ(model.count(), 1);
    ASSERT_EQ(nodes->rowCount(), 1);
    ASSERT_EQ(edges->rowCount(), 0);

    auto rootIndex = nodes->index(0, 0);
    EXPECT_EQ(nodes->data(rootIndex, Qt::UserRole).toString().toStdString(), "Summing Node");
    EXPECT_EQ(nodes->data(rootIndex, Qt::UserRole + 1).toInt(), 100);
    EXPECT_EQ(nodes->data(rootIndex, Qt::UserRole + 2).toInt(), 111);
}
} // namespace UnitTest
