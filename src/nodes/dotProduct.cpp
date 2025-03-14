#include "dotProduct.h"

DotProductNode::DotProductNode()
{
    addInput<Vec3<double>>("Vector1", "Vector dot product factor", v1_);
    addInput<Vec3<double>>("Vector2", "Vector dot product factor", v2_);
    addOutput<double>("Product", "The inner product of the vectors", dotProduct_);
}

std::string_view DotProductNode::name() const { return "Dot Product"; }

std::string_view DotProductNode::summary() const { return "Computes the dot product of a pair of vectors v1 and v2"; }

// Run main processing
Module::ExecutionResult DotProductNode::process(ModuleContext &moduleContext)
{
    dotProduct_ = v1_.dp(v2_);
    validate();
    return Module::ExecutionResult::Success;
}
