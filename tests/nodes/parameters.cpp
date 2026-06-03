// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/dissolve.h"
#include "nodes/numberNode.h"
#include "nodes/test.h"
#include "tests/graphData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class ParametersTest : public ::testing::Test
{
    public:
    ParametersTest()
    {
        a_ = dynamic_cast<TestNode *>(testGraph_.addNode(std::make_unique<TestNode>(&testGraph_), "TestA"));
        EXPECT_TRUE(a_);
        b_ = dynamic_cast<TestNode *>(testGraph_.addNode(std::make_unique<TestNode>(&testGraph_), "TestB"));
        EXPECT_TRUE(b_);
    }

    protected:
    TestGraph testGraph_;
    TestNode *a_{nullptr}, *b_{nullptr};
};

TEST_F(ParametersTest, NumberToOptionalNumber)
{
    auto numberOutput = a_->findOutput("Number");
    ASSERT_TRUE(numberOutput);
    auto optionalNumberImput = b_->findInput("OptionalNumber");
    ASSERT_TRUE(optionalNumberImput);

    // Create an edge between nodes
    ASSERT_TRUE(testGraph_.addEdge({"TestA", "Number", "TestB", "OptionalNumber"}));

    // Set number value input (shared with output)
    ASSERT_TRUE(a_->setInput<Number>("Number", 1.234));

    // Run b
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(b_->versionIndex(), 0);

    // Check optional value
    auto optional = b_->getInputValue<std::optional<Number>>("OptionalNumber");
    ASSERT_TRUE(optional.has_value());
    EXPECT_DOUBLE_EQ(1.234, optional->asDouble());
}

TEST_F(ParametersTest, OptionalNumberToNumber)
{
    auto optionalNumberOutput = a_->findOutput("OptionalNumber");
    ASSERT_TRUE(optionalNumberOutput);
    auto numberImput = b_->findInput("Number");
    ASSERT_TRUE(numberImput);

    // Create an edge between nodes
    ASSERT_TRUE(testGraph_.addEdge({"TestA", "OptionalNumber", "TestB", "Number"}));

    // Run b - should fail as there is no optional value to pull along the edge
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Failed);
    EXPECT_EQ(b_->versionIndex(), NodeConstants::InvalidVersion);

    // Set number value input (shared with output)
    ASSERT_TRUE(a_->setInput<std::optional<Number>>("OptionalNumber", 1.234));

    // Run again and check value
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(b_->versionIndex(), 0);
    EXPECT_DOUBLE_EQ(1.234, b_->getInputValue<Number>("Number").asDouble());
}

TEST_F(ParametersTest, OptionalPointerOutput)
{
    auto createOptA = a_->findInput("CreateConfiguration");
    ASSERT_TRUE(createOptA);
    auto configInputB = b_->findInput("Configuration");
    ASSERT_TRUE(configInputB);

    // Create an edge between nodes
    ASSERT_TRUE(testGraph_.addEdge({"TestA", "OptionalConfiguration", "TestB", "Configuration"}));

    // Inputs to TestB should be valid (there is no optional data yet, but the Edge definitions are correct)
    EXPECT_TRUE(b_->inputsAreValid());
    EXPECT_FALSE(b_->isUpToDate());
    EXPECT_EQ(b_->versionIndex(), NodeConstants::InvalidVersion);

    // Running TestB should fail since there is no optional data to retrieve via the Edge
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Failed);

    // Flag TestA to create the optional data if it doesn't exist, and rerun TestB
    createOptA->set(true);
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);

    // Double-check the value of the pointer for sanity's sake
    EXPECT_EQ(&a_->optionalConfiguration().value(), configInputB->get<Configuration *>());
}

TEST_F(ParametersTest, VectorParameter)
{
    auto numbersABase = a_->findInput("NumberVector");
    ASSERT_TRUE(numbersABase);
    auto numbersA = a_->findInput("NumberVector");
    ASSERT_TRUE(numbersA);

    // Try to set the base class with a vector
    EXPECT_NO_THROW(numbersABase->set(std::vector<Number>{{1.0}, {2.0}, {3.0}}));

    // We cannot set a vector from a single correctly-typed element, but this is probably only useful in unit tests anyway
    EXPECT_ANY_THROW(numbersABase->set(Number{1.0}));

    // Create a Number node
    auto n1 = testGraph_.createNode("Number", "Number1");
    ASSERT_TRUE(n1);
    auto number1 = n1->findOption("X");
    number1->set(Number{1.0});

    // Assign the number node to the vector
    EXPECT_TRUE(numbersA->assignDataFromSource(number1.get()));

    // Assign another number
    number1->set(Number{4.0});
    EXPECT_TRUE(numbersA->assignDataFromSource(number1.get()));

    // Check vector contents
    ASSERT_EQ(numbersA->get<std::vector<Number>>().size(), 5);
    EXPECT_DOUBLE_EQ(numbersA->get<std::vector<Number>>()[3].asDouble(), 1.0);
    EXPECT_DOUBLE_EQ(numbersA->get<std::vector<Number>>()[4].asDouble(), 4.0);
}

TEST_F(ParametersTest, VectorInputOutput)
{
    auto numbersA = a_->findInput("NumberVector");
    ASSERT_TRUE(numbersA);
    auto numbersB = b_->findInput("NumberVector");
    ASSERT_TRUE(numbersB);

    // Create an edge linking the vector output from A to the vector input of B
    ASSERT_TRUE(testGraph_.addEdge({"TestA", "NumberVector", "TestB", "NumberVector"}));

    // Create three Number nodes as inputs for TestA's number vector
    auto *n1 = testGraph_.createNode("Number", "Number1");
    ASSERT_TRUE(n1);
    auto number1 = n1->findOption("X");
    ASSERT_TRUE(number1);
    auto *n2 = testGraph_.createNode("Number", "Number2");
    ASSERT_TRUE(n2);
    auto number2 = n2->findOption("X");
    ASSERT_TRUE(number2);
    auto *n3 = testGraph_.createNode("Number", "Number3");
    ASSERT_TRUE(n3);
    auto number3 = n3->findOption("X");
    ASSERT_TRUE(number3);

    // Set some numbers
    number1->set(Number{5.0});
    number2->set(Number{6.0});
    number3->set(Number{8.0});

    // Link all three numbers in to the TestA vector
    ASSERT_TRUE(testGraph_.addEdge({"Number1", "X", "TestA", "NumberVector"}));
    ASSERT_TRUE(testGraph_.addEdge({"Number2", "X", "TestA", "NumberVector"}));
    ASSERT_TRUE(testGraph_.addEdge({"Number3", "X", "TestA", "NumberVector"}));

    // Run the TestB node to pull the number edge vector from TestA, using the numbers from the three number nodes
    EXPECT_TRUE(a_->inputsAreValid());
    EXPECT_TRUE(b_->inputsAreValid());
    EXPECT_FALSE(a_->isUpToDate());
    EXPECT_FALSE(b_->isUpToDate());
    EXPECT_EQ(b_->versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(b_->versionIndex(), 0);

    // Check the number vectors on TestA and TestB
    EXPECT_EQ(numbersA->get<std::vector<Number>>().size(), 3);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), (std::vector<Number>{{5.0}, {6.0}, {8.0}}));
    EXPECT_EQ(numbersB->get<std::vector<Number>>().size(), 3);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), numbersB->get<std::vector<Number>>());

    // Adjust the numbers - this should invalidate both TestA and TestB
    number1->set(Number{1.0});
    number3->set(Number{2.0});
    EXPECT_TRUE(a_->inputsAreValid());
    EXPECT_TRUE(b_->inputsAreValid());
    EXPECT_FALSE(a_->isUpToDate());
    EXPECT_FALSE(b_->isUpToDate());

    // Run again and check the result
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(b_->versionIndex(), 1);
    EXPECT_EQ(numbersA->get<std::vector<Number>>().size(), 3);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), (std::vector<Number>{{1.0}, {6.0}, {2.0}}));
    EXPECT_EQ(numbersB->get<std::vector<Number>>().size(), 3);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), numbersB->get<std::vector<Number>>());

    // Remove a single edge - this should flag TestA and TestB as being out of date
    EXPECT_TRUE(testGraph_.removeEdge({"Number1", "X", "TestA", "NumberVector"}));
    EXPECT_TRUE(a_->inputsAreValid());
    EXPECT_TRUE(b_->inputsAreValid());
    EXPECT_FALSE(a_->isUpToDate());
    EXPECT_FALSE(b_->isUpToDate());

    // Run again and check the result
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(b_->versionIndex(), 2);
    EXPECT_EQ(numbersA->get<std::vector<Number>>().size(), 2);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), (std::vector<Number>{{6.0}, {2.0}}));
    EXPECT_EQ(numbersB->get<std::vector<Number>>().size(), 2);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), numbersB->get<std::vector<Number>>());

    // Remove both of the other edges
    EXPECT_TRUE(testGraph_.removeEdge({"Number3", "X", "TestA", "NumberVector"}));
    EXPECT_TRUE(testGraph_.removeEdge({"Number2", "X", "TestA", "NumberVector"}));
    EXPECT_TRUE(a_->inputsAreValid());
    EXPECT_TRUE(b_->inputsAreValid());
    EXPECT_FALSE(a_->isUpToDate());
    EXPECT_FALSE(b_->isUpToDate());

    // Run again and check the result
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(b_->versionIndex(), 3);
    EXPECT_EQ(numbersA->get<std::vector<Number>>().size(), 0);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), std::vector<Number>());
    EXPECT_EQ(numbersB->get<std::vector<Number>>().size(), 0);
    EXPECT_EQ(numbersA->get<std::vector<Number>>(), numbersB->get<std::vector<Number>>());
}

TEST_F(ParametersTest, VariantToVariant)
{
    auto variantOutput = a_->findOutput("Variant");
    ASSERT_TRUE(variantOutput);
    auto variantInput = b_->findInput("Variant");
    ASSERT_TRUE(variantInput);

    // Create an edge between nodes
    ASSERT_TRUE(testGraph_.addEdge({"TestA", "Variant", "TestB", "Variant"}));

    // Set variant on A
    a_->setVariant(Number(5.0));

    // Run the graph from B
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(a_->versionIndex(), 0);
    EXPECT_EQ(b_->versionIndex(), 0);
    ASSERT_NO_THROW(std::get<Number>(b_->variant().data));
    EXPECT_EQ(std::get<Number>(b_->variant().data), std::get<Number>(a_->variant().data));

    // Set the variant on A to a different type
    a_->setVariant(Structure());

    // Run the graph from B
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(a_->versionIndex(), 1);
    EXPECT_EQ(b_->versionIndex(), 1);
    ASSERT_NO_THROW(std::get<Structure>(b_->variant().data));
}

TEST_F(ParametersTest, VariantToOther)
{
    auto variantOutput = a_->findOutput("Variant");
    ASSERT_TRUE(variantOutput);
    auto numberInput = b_->findInput("Number");
    ASSERT_TRUE(numberInput);

    // Create an edge between nodes
    ASSERT_TRUE(testGraph_.addEdge({"TestA", "Variant", "TestB", "Number"}));

    // Set variant on A
    a_->setVariant(Number(5.0));

    // Run the graph from B
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(a_->versionIndex(), 0);
    EXPECT_EQ(b_->versionIndex(), 0);
    ASSERT_NO_THROW(std::get<Number>(a_->variant().data));
    EXPECT_EQ(numberInput->get<Number>(), std::get<Number>(a_->variant().data));
}

TEST_F(ParametersTest, OtherToVariant)
{
    auto numberOutput = a_->findOutput("Number");
    ASSERT_TRUE(numberOutput);
    auto variantInput = b_->findInput("Variant");
    ASSERT_TRUE(variantInput);

    // Create an edge between nodes
    ASSERT_TRUE(testGraph_.addEdge({"TestA", "Number", "TestB", "Variant"}));

    // Set value on A
    numberOutput->set<Number>(Number(5.0));

    // Run the graph from B
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(a_->versionIndex(), 0);
    EXPECT_EQ(b_->versionIndex(), 0);
    ASSERT_NO_THROW(std::get<Number>(b_->variant().data));
    EXPECT_EQ(numberOutput->get<Number>(), std::get<Number>(b_->variant().data));
}

TEST_F(ParametersTest, VariantToPointer)
{
    auto variantOutput = a_->findOutput("Variant");
    ASSERT_TRUE(variantOutput);
    auto configurationInput = b_->findInput("Configuration");
    ASSERT_TRUE(configurationInput);

    // Create an edge between nodes
    ASSERT_TRUE(testGraph_.addEdge({"TestA", "Variant", "TestB", "Configuration"}));

    // Set variant on A
    Configuration cfg;
    a_->setVariant(&cfg);

    // Run the graph from B
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(a_->versionIndex(), 0);
    EXPECT_EQ(b_->versionIndex(), 0);
    ASSERT_NO_THROW(std::get<Configuration *>(a_->variant().data));
    EXPECT_EQ(configurationInput->get<Configuration *>(), &cfg);
}

TEST_F(ParametersTest, PointerToVariant)
{
    // Create an edge between nodes
    ASSERT_TRUE(testGraph_.addEdge({"TestA", "Configuration", "TestB", "Variant"}));

    // Run the graph from B - should fail as the pointer in A doesn't exist yet
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Failed);
    EXPECT_EQ(a_->versionIndex(), 0);
    EXPECT_EQ(b_->versionIndex(), NodeConstants::InvalidVersion);

    // Now set the configuration input and run again
    Configuration cfg;
    a_->setInput("Configuration", &cfg);
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(a_->versionIndex(), 1);
    EXPECT_EQ(b_->versionIndex(), 0);
    ASSERT_NO_THROW(std::get<Configuration *>(b_->variant().data));
    EXPECT_EQ(std::get<Configuration *>(b_->variant().data), &cfg);
}

TEST_F(ParametersTest, OptionalPointerToVariant)
{
    // Create an edge between nodes
    ASSERT_TRUE(testGraph_.addEdge({"TestA", "OptionalConfiguration", "TestB", "Variant"}));

    // Run the graph from B - should fail as the optional hasn't been created so there is no pointer to return
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Failed);
    EXPECT_EQ(a_->versionIndex(), 0);
    EXPECT_EQ(b_->versionIndex(), NodeConstants::InvalidVersion);

    // Now set the configuration input and run again
    a_->setInput("CreateConfiguration", true);
    EXPECT_EQ(b_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(a_->versionIndex(), 1);
    EXPECT_EQ(b_->versionIndex(), 0);
    ASSERT_NO_THROW(std::get<Configuration *>(b_->variant().data));
    ASSERT_TRUE(a_->optionalConfiguration().has_value());
    EXPECT_EQ(std::get<Configuration *>(b_->variant().data), &a_->optionalConfiguration().value());
}

} // namespace UnitTest
