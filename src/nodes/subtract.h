#pragma once

#include "nodes/node.h"

// SubractNode Node
template <typename T> class SubractNode : public Node
{
    public:
    SubractNode()
    {
        addInput<T>("A", "First operand to the subtraction", a_);
        addInput<T>("B", "Second operand to the subtraction, subtracted from A", b_);
    }
    ~SubractNode() override = default;

    public:
    std::string_view name() override { return "Subtract"; }
    std::string_view summary() override { return "Performs the subtraction A - B"; }

    /*
     * Definition
     */
    private:
    // Factor A
    T a_;
    // Factor B
    T b_;
    // Product of A and B
    T result_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext)
    {
        result_ = std::minus<T>(a_, b_);

        return Module::ExecutionResult::Success;
    }
};
