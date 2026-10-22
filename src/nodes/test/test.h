// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "classes/structure.h"
#include "nodes/node.h"

// Test Node
class TestNode : public Node
{
    public:
    TestNode(Graph *parentGraph);
    ~TestNode() override = default;

    /*
     * Definition
     */
    private:
    // Configuration pointer input
    Configuration *configuration_{nullptr};
    // Optional configuration output
    std::optional<Configuration> optionalConfiguration_;
    // Whether our processing loop creates a valid optional Configuration data
    bool createConfiguration_{false};
    // Number
    Number number_;
    // Number vector
    std::vector<Number> numberVector_;
    // Optional number
    std::optional<Number> optionalNumber_;
    // Variant
    using TestVariant = VariantParameterData<Structure, Number, std::string, Configuration *>;
    TestVariant variant_;
    // Test string
    char char_;
    char *charPtr_;
    std::string message_;
    std::vector<char> messageParts_;

    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;
    // Return the optional Configuration
    const std::optional<Configuration> &optionalConfiguration() const;
    // Set the variant
    template <class T> void setVariant(T value)
    {
        variant_.data = value;
        setUpdateRequired();
    }
    // Return the variant
    TestVariant variant();

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
