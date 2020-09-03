/*
    *** Procedure Node - Box
    *** src/procedure/nodes/box.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "procedure/nodes/box.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "keywords/types.h"

BoxProcedureNode::BoxProcedureNode(Vec3<double> lengths, Vec3<double> angles, bool nonPeriodic)
    : ProcedureNode(ProcedureNode::BoxNode)
{
    keywords_.add("Definition", new Vec3NodeValueKeyword(this, lengths, Vec3Labels::ABCLabels), "Lengths", "Box lengths");
    keywords_.add("Definition", new Vec3NodeValueKeyword(this, angles, Vec3Labels::AlphaBetaGammaLabels), "Angles",
                  "Box angles");
    keywords_.add("Definition", new BoolKeyword(false), "NonPeriodic", "Whether the box is non-periodic");
}

BoxProcedureNode::~BoxProcedureNode() {}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool BoxProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::GenerationContext);
}

// Return whether a name for the node must be provided
bool BoxProcedureNode::mustBeNamed() const { return false; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool BoxProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) { return true; }

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult BoxProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                                             GenericList &targetList)
{
    // Retrieve necessary parameters
    auto lengths = keywords_.asVec3Double("Lengths");
    auto angles = keywords_.asVec3Double("Angles");
    auto nonPeriodic = keywords_.asBool("NonPeriodic");

    // Create a Box in the target Configuration with our lengths and angles
    if (!cfg->createBox(lengths, angles, nonPeriodic))
        return ProcedureNode::Failure;

    Messenger::print("[Box] Volume is {} cubic Angstroms (reciprocal volume = {:e})\n", cfg->box()->volume(),
                     cfg->box()->reciprocalVolume());
    lengths = cfg->box()->axisLengths();
    angles = cfg->box()->axisAngles();
    Messenger::print(
        "[Box] Type is {}: A = {:10.4e} B = {:10.4e} C = {:10.4e}, alpha = {:10.4e} beta = {:10.4e} gamma = {:10.4e}\n",
        Box::boxTypes().keyword(cfg->box()->type()), lengths.x, lengths.y, lengths.z, angles.x, angles.y, angles.z);

    return ProcedureNode::Success;
}
