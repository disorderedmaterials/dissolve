// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/regionBase.h"
#include "keywords/bool.h"
#include "keywords/double.h"

RegionGeneratorNodeBase::RegionGeneratorNodeBase(GeneratorNode::NodeType nodeType) : GeneratorNode(nodeType)
{
    keywords_.setOrganisation("Options", "Grid");
    keywords_.add<DoubleKeyword>("VoxelSize", "Voxel size (length) guiding the coarseness / detail of the region", voxelSize_,
                                 0.1);
    keywords_.add<BoolKeyword>("Invert", "Invert the logic used to determine free space in the region", invert_);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool RegionGeneratorNodeBase::mustBeNamed() const { return true; }

/*
 * Region Data
 */

// Return current region data
const Region &RegionGeneratorNodeBase::region() const { return region_; }

/*
 * Execute
 */

// Execute node
bool RegionGeneratorNodeBase::execute(const GeneratorContext &generatorContext)
{
    return region_.generate(generatorContext.configuration(), voxelSize_, invert_, [&]() { return createVoxelKernel(); });
}
