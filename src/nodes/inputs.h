// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

// Parameter Holder Base
class ParameterHolderBase
{
};

// Parameter Holder
template <class T> class ParameterHolder : public ParameterHolderBase
{
    public:
    T data;
};

// Inputs Node
class InputsNode : public Node
{
    public:
    InputsNode(Graph *parentGraph);
    ~InputsNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Parameter Maps
     */
    private:
    // Parameter holders
    std::vector<std::shared_ptr<ParameterHolderBase>> parameterHolders_;

    public:
    // Create input and map to output
    std::shared_ptr<ParameterBase> createMappedInput(std::string_view name, std::type_index typeIndex);

    /*
     * I/O
     */
    public:
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
