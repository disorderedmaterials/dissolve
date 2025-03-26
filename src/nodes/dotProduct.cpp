#include "dotProduct.h"

DotProductNode::DotProductNode()
{
    addInput<Vec3<double>>("U", "Vector dot product factor", u_);
    addInput<Vec3<double>>("V", "Vector dot product factor", v_);
    addOutput<double>("Product", "The inner product of the vectors", dotProduct_);
}

std::string_view DotProductNode::name() const { return "Dot Product"; }

std::string_view DotProductNode::summary() const { return "Computes the dot product of a pair of vectors u and v"; }

// Run main processing
Module::ExecutionResult DotProductNode::process(ModuleContext &moduleContext)
{
    dotProduct_ = u_.dp(v_);

    return Module::ExecutionResult::Success;
}
