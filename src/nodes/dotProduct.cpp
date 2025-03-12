#include "dotProduct.h"

DotProductNode::DotProductNode()
{ 
	addInput<Vec3<double>>("Vector1", "Vector dot product factor", v1_);
    addInput<Vec3<double>>("Vector2", "Vector dot product factor", v2_);
}

std::string_view DotProductNode::name() { return "Dot Product"; }

std::string_view DotProductNode::summary() { return "Computes the dot product of a pair of vectors v1 and v2"; }

// Run main processing
Module::ExecutionResult DotProductNode::process(ModuleContext& moduleContext)
{ 
	dorProduct_ = v1_.dp(v2_);

	return ExecutionResult::Success;
}