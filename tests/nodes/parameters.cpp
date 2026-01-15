// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/dissolve.h"
#include "nodes/numberNode.h"
#include "nodes/test.h"
#include "tests/graphData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(ParametersTest, OptionalPointerOutput)
{
    GraphTestData data;

    // Create a couple of TestNodes
    auto *a = dynamic_cast<TestNode *>(data.graphRoot.addNode(std::make_unique<TestNode>(&data.graphRoot), "TestA"));
    ASSERT_TRUE(a);
    auto createOptA = a->findInput("CreateConfiguration");
    ASSERT_TRUE(createOptA);
    auto *b = dynamic_cast<TestNode *>(data.graphRoot.addNode(std::make_unique<TestNode>(&data.graphRoot), "TestB"));
    ASSERT_TRUE(b);
    auto configInputB = b->findInput("ConfigurationInput");
    ASSERT_TRUE(configInputB);

    // Create an edge between nodes
    ASSERT_TRUE(data.graphRoot.addEdge({"TestA", "OptionalConfiguration", "TestB", "ConfigurationInput"}));

    // Inputs to TestB should be valid (there is no optional data yet, but the Edge definitions are correct)
    EXPECT_TRUE(b->inputsAreValid());
    EXPECT_FALSE(b->isUpToDate());
    EXPECT_EQ(b->versionIndex(), NodeConstants::InvalidVersion);

    // Running TestB should fail since there is no optional data to retrieve via the Edge
    EXPECT_EQ(b->run(), NodeConstants::ProcessResult::Failed);

    // Flag TestA to create the optional data if it doesn't exist, and rerun TestB
    createOptA->set(true);
    EXPECT_EQ(b->run(), NodeConstants::ProcessResult::Success);

    // Double-check the value of the pointer for sanity's sake
    EXPECT_EQ(&a->optionalConfiguration().value(), configInputB->get<Configuration *>());
}

TEST(ParametersTest, VectorParameter)
{
    GraphTestData data;

    // Create a couple of TestNodes
    auto *a = dynamic_cast<TestNode *>(data.graphRoot.addNode(std::make_unique<TestNode>(&data.graphRoot), "TestA"));
    ASSERT_TRUE(a);
    auto numbersABase = a->findInput("NumberVector");
    ASSERT_TRUE(numbersABase);
    auto numbersA = a->findInput("NumberVector");
    ASSERT_TRUE(numbersA);

    // Try to set the base class with a vector
    EXPECT_NO_THROW(numbersABase->set(std::vector<Number>{{1.0}, {2.0}, {3.0}}));

    // We cannot set a vector from a single correctly-typed element, but this is probably only useful in unit tests anyway
    EXPECT_ANY_THROW(numbersABase->set(Number{1.0}));

    // Create a Number node
    auto n1 = dynamic_cast<NumberNode *>(data.graphRoot.addNode(std::make_unique<NumberNode>(&data.graphRoot), "Number1"));
    ASSERT_TRUE(n1);
    auto number1 = n1->findOption("X");
    number1->set(Number{1.0});

    // Assign the number node to the vector
    EXPECT_TRUE(numbersA->assign(number1.get()));

    // Assign another number
    number1->set(Number{4.0});
    EXPECT_TRUE(numbersA->assign(number1.get()));

    // Check vector contents
    ASSERT_EQ(numbersA->get<std::vector<Number>>().size(), 5);
    EXPECT_DOUBLE_EQ(numbersA->get<std::vector<Number>>()[3].asDouble(), 1.0);
    EXPECT_DOUBLE_EQ(numbersA->get<std::vector<Number>>()[4].asDouble(), 4.0);
}

TEST(ParametersTest, VectorInputOutput)
{
    CoreData coreData_;
    Dissolve dissolve_(coreData_);
    GraphTestData data;

    // Create a couple of TestNodes
    auto *a = dynamic_cast<TestNode *>(data.graphRoot.addNode(std::make_unique<TestNode>(&data.graphRoot), "TestA"));
    ASSERT_TRUE(a);
    auto numbersA = a->findInput("NumberVector");
    ASSERT_TRUE(numbersA);
    auto *b = dynamic_cast<TestNode *>(data.graphRoot.addNode(std::make_unique<TestNode>(&data.graphRoot), "TestB"));
    ASSERT_TRUE(b);
    auto numbersB = b->findInput("NumberVector");
    ASSERT_TRUE(numbersB);

    // Create an edge linking the vector output from A to the vector input of B
    ASSERT_TRUE(data.graphRoot.addEdge({"TestA", "NumberVector", "TestB", "NumberVector"}));

    // Create three Number nodes as inputs for TestA's number vector
    auto *n1 = dynamic_cast<NumberNode *>(data.graphRoot.addNode(std::make_unique<NumberNode>(&data.graphRoot), "Number1"));
    ASSERT_TRUE(n1);
    auto number1 = n1->findOption("X");
    ASSERT_TRUE(number1);
    auto *n2 = dynamic_cast<NumberNode *>(data.graphRoot.addNode(std::make_unique<NumberNode>(&data.graphRoot), "Number2"));
    ASSERT_TRUE(n2);
    auto number2 = n2->findOption("X");
    ASSERT_TRUE(number2);
    auto *n3 = dynamic_cast<NumberNode *>(data.graphRoot.addNode(std::make_unique<NumberNode>(&data.graphRoot), "Number3"));
    ASSERT_TRUE(n3);
    auto number3 = n3->findOption("X");
    ASSERT_TRUE(number3);

    // Set some numbers
    number1->set(Number{5.0});
    number2->set(Number{6.0});
    number3->set(Number{8.0});

    // Link all three numbers in to the TestA vector
    ASSERT_TRUE(data.graphRoot.addEdge({"Number1", "X", "TestA", "NumberVector"}));
    ASSERT_TRUE(data.graphRoot.addEdge({"Number2", "X", "TestA", "NumberVector"}));
    ASSERT_TRUE(data.graphRoot.addEdge({"Number3", "X", "TestA", "NumberVector"}));

    // Run the TestB node to pull the number edge vector from TestA, using the numbers from the three number nodes
    EXPECT_TRUE(a->inputsAreValid());
    EXPECT_TRUE(b->inputsAreValid());
    EXPECT_FALSE(a->isUpToDate());
    EXPECT_FALSE(b->isUpToDate());
    EXPECT_EQ(b->versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(b->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(b->versionIndex(), 0);

    // Check the number vectors on TestA and TestB
    EXPECT_EQ(numbersA->get<std::vector<Number>>().size(), 3);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), (std::vector<Number>{{5.0}, {6.0}, {8.0}}));
    EXPECT_EQ(numbersB->get<std::vector<Number>>().size(), 3);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), numbersB->get<std::vector<Number>>());

    // Adjust the numbers - this should invalidate both TestA and TestB
    number1->set(Number{1.0});
    number3->set(Number{2.0});
    EXPECT_TRUE(a->inputsAreValid());
    EXPECT_TRUE(b->inputsAreValid());
    EXPECT_FALSE(a->isUpToDate());
    EXPECT_FALSE(b->isUpToDate());

    // Run again and check the result
    EXPECT_EQ(b->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(b->versionIndex(), 1);
    EXPECT_EQ(numbersA->get<std::vector<Number>>().size(), 3);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), (std::vector<Number>{{1.0}, {6.0}, {2.0}}));
    EXPECT_EQ(numbersB->get<std::vector<Number>>().size(), 3);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), numbersB->get<std::vector<Number>>());

    // Remove a single edge - this should flag TestA and TestB as being out of date
    EXPECT_TRUE(data.graphRoot.removeEdge({"Number1", "X", "TestA", "NumberVector"}));
    EXPECT_TRUE(a->inputsAreValid());
    EXPECT_TRUE(b->inputsAreValid());
    EXPECT_FALSE(a->isUpToDate());
    EXPECT_FALSE(b->isUpToDate());

    // Run again and check the result
    EXPECT_EQ(b->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(b->versionIndex(), 2);
    EXPECT_EQ(numbersA->get<std::vector<Number>>().size(), 2);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), (std::vector<Number>{{6.0}, {2.0}}));
    EXPECT_EQ(numbersB->get<std::vector<Number>>().size(), 2);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), numbersB->get<std::vector<Number>>());

    // Remove both of the other edges
    EXPECT_TRUE(data.graphRoot.removeEdge({"Number3", "X", "TestA", "NumberVector"}));
    EXPECT_TRUE(data.graphRoot.removeEdge({"Number2", "X", "TestA", "NumberVector"}));
    EXPECT_TRUE(a->inputsAreValid());
    EXPECT_TRUE(b->inputsAreValid());
    EXPECT_FALSE(a->isUpToDate());
    EXPECT_FALSE(b->isUpToDate());

    // Run again and check the result
    EXPECT_EQ(b->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(b->versionIndex(), 3);
    EXPECT_EQ(numbersA->get<std::vector<Number>>().size(), 0);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), std::vector<Number>());
    EXPECT_EQ(numbersB->get<std::vector<Number>>().size(), 0);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), numbersB->get<std::vector<Number>>());
}

} // namespace UnitTest
