// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/fit1d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "expression/variable.h"
#include "io/export/data1d.h"
#include "math/mc.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/process1d.h"

Fit1DProcedureNode::Fit1DProcedureNode(Collect1DProcedureNode *target) : ProcedureNode(ProcedureNode::NodeType::Fit1D)
{
    dataNode_.addAllowableNodeType(ProcedureNode::NodeType::Process1D);

    // Create variables, and add them to the vector
    xVariable_ = std::make_shared<ExpressionVariable>("x");
    variables_.emplace_back(xVariable_);

    dataNode_ = target;
    saveData_ = false;
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Fit1DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Node Keywords
 */

// Return enum option info for Fit1DNodeKeyword
EnumOptions<Fit1DProcedureNode::Fit1DNodeKeyword> Fit1DProcedureNode::fit1DNodeKeywords()
{
    return EnumOptions<Fit1DProcedureNode::Fit1DNodeKeyword>(
        "Fit1DNodeKeyword", {{Fit1DProcedureNode::ConstantKeyword, "Constant", 1},
                             {Fit1DProcedureNode::EndFit1DKeyword, "EndFit1D"},
                             {Fit1DProcedureNode::EquationKeyword, "Equation", 1},
                             {Fit1DProcedureNode::FitKeyword, "Fit", 1},
                             {Fit1DProcedureNode::MethodKeyword, "Method"},
                             {Fit1DProcedureNode::SaveKeyword, "Save", 1},
                             {Fit1DProcedureNode::SourceDataKeyword, "SourceData", OptionArguments::OptionalSecond}});
}

/*
 * Data
 */

// Return the named fit target, if it exists
std::shared_ptr<ExpressionVariable> Fit1DProcedureNode::getFitTarget(std::string_view name)
{
    for (auto var : fitTargets_)
        if (DissolveSys::sameString(name, var->name()))
            return var;

    return nullptr;
}

// Return the named constant, if it exists
std::shared_ptr<ExpressionVariable> Fit1DProcedureNode::getConstant(std::string_view name)
{
    for (auto var : constants_)
        if (DissolveSys::sameString(name, var->name()))
            return var;

    return nullptr;
}

// Fitting cost function
double Fit1DProcedureNode::equationCost(const std::vector<double> &alpha)
{
    // We assume that the minimiser has 'pokeBeforeCost' set, so our Expression's variables are up-to-date with new test
    // values.
    double cost = 0.0;
    const auto &x = referenceData_.xAxis();
    const auto &y = referenceData_.values();
    double equationY;
    for (auto n = 0; n < referenceData_.nValues(); ++n)
    {
        // Set axis value
        xVariable_->setValue(x[n]);

        // Evaluate expression
        equationY = equation_.asDouble();

        // Sum squared error
        cost += (equationY - y[n]) * (equationY - y[n]);
    }

    cost /= referenceData_.nValues();

    return sqrt(cost);
}

// Set whether to save processed data
void Fit1DProcedureNode::setSaveData(bool on) { saveData_ = on; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Fit1DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) { return true; }

// Finalise any necessary data after execution
bool Fit1DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Copy reference data from the associated node
    Collect1DProcedureNode *collect1DNode;
    Process1DProcedureNode *process1DNode;
    switch (dataNode_.type())
    {
        case (ProcedureNode::NodeType::Collect1D):
            collect1DNode = dynamic_cast<Collect1DProcedureNode *>(dataNode_.node());
            if (collect1DNode)
                return Messenger::error("Failed to cast dataNode_ into a Collect1DProcedureNode.\n");
            referenceData_ = collect1DNode->accumulatedData();
            break;
        case (ProcedureNode::NodeType::Process1D):
            process1DNode = dynamic_cast<Process1DProcedureNode *>(dataNode_.node());
            if (process1DNode)
                return Messenger::error("Failed to cast dataNode_ into a Process1DProcedureNode.\n");
            referenceData_ = process1DNode->processedData();
            break;
        default:
            return Messenger::error("No suitable data to extract from a node of type '{}'.\n",
                                    ProcedureNode::nodeTypes().keyword(dataNode_.type()));
    }

    // Print equation info
    Messenger::print("Expression to fit is: {}\n", equation_.expressionString());
    Messenger::print("  {:10}                (axis variable)\n", xVariable_->name());
    for (auto var : fitTargets_)
        Messenger::print("  {:10} = {:e} (fit)\n", var->name(), var->value().asDouble());
    for (auto var : constants_)
        Messenger::print("  {:10} = {:e} (constant)\n", var->name(), var->value().asDouble());
    Messenger::print("\n");

    // Check number of variable parameters
    if (fitTargets_.size() > 0)
    {
        // Create a minimiser
        MonteCarloMinimiser<Fit1DProcedureNode> mcMinimiser(*this, &Fit1DProcedureNode::equationCost, true);
        mcMinimiser.setMaxIterations(1000);
        mcMinimiser.setStepSize(0.1);
        for (auto var : fitTargets_)
            mcMinimiser.addTarget(var);

        mcMinimiser.minimise();

        Messenger::print("Optimised values:\n");
        for (auto var : fitTargets_)
            Messenger::print("  {:10} = {:e}\n", var->name(), var->value().asDouble());
    }

    // Generate final fit data
    // Retrieve / realise the data from the supplied list
    auto &data =
        targetList.realise<Data1D>(fmt::format("{}//{}", name(), cfg->niceName()), prefix, GenericItem::InRestartFileFlag);

    data.setTag(name());
    data.clear();

    const auto &x = referenceData_.xAxis();
    for (auto n = 0; n < referenceData_.nValues(); ++n)
    {
        // Set axis value
        xVariable_->setValue(x[n]);

        // Add point
        data.addPoint(x[n], equation_.asDouble());
    }

    // Save data?
    if (saveData_)
    {
        if (procPool.isMaster())
        {
            LineParser parser;
            if (!parser.openOutput(fmt::format("{}_{}.fit", name(), cfg->name())))
                return procPool.decideFalse();
            if (!parser.writeLineF("# Fit Equation : {}\n", equation_.expressionString()))
                return procPool.decideFalse();
            if (!parser.writeLineF("#  {:10}                (axis variable)\n", xVariable_->name()))
                return procPool.decideFalse();
            for (auto var : fitTargets_)
                if (!parser.writeLineF("#  {:10} = {:e} (fit)\n", var->name(), var->value().asDouble()))
                    return procPool.decideFalse();
            for (auto var : constants_)
                if (!parser.writeLineF("#  {:10} = {:e} (constant)\n", var->name(), var->value().asDouble()))
                    return procPool.decideFalse();

            Data1DExportFileFormat exportFormat(name());
            if (exportFormat.exportData(data))
                procPool.decideTrue();
            else
                return procPool.decideFalse();
        }
        else if (!procPool.decision())
            return false;
    }

    return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool Fit1DProcedureNode::deserialise(LineParser &parser, const CoreData &coreData)
{
    // The current line in the parser may contain a node name for us
    if (parser.nArgs() == 2)
        setName(parser.argsv(1));

    // Read until we encounter the EndFit1D keyword, or we fail for some reason
    while (!parser.eofOrBlank())
    {
        // Read and parse the next line
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!fit1DNodeKeywords().isValid(parser.argsv(0)))
            return fit1DNodeKeywords().errorAndPrintValid(parser.argsv(0));
        Fit1DNodeKeyword nk = fit1DNodeKeywords().enumeration(parser.argsv(0));
        if (!fit1DNodeKeywords().validNArgs(nk, parser.nArgs() - 1))
            return false;

        // All OK, so process it
        switch (nk)
        {
            case (Fit1DProcedureNode::ConstantKeyword):
                if (getConstant(parser.argsv(1)) || getFitTarget(parser.argsv(1)))
                    return Messenger::error("Failed to create constant, as a variable by this name already exists.\n");
                constants_.emplace_back(std::make_shared<ExpressionVariable>(parser.argsv(1), parser.argd(2)));
                break;
            case (Fit1DProcedureNode::EndFit1DKeyword):
                return true;
            case (Fit1DProcedureNode::EquationKeyword):
                if (!equation_.create(parser.argsv(1)))
                    return Messenger::error("Failed to create expression.\n");
                break;
            case (Fit1DProcedureNode::FitKeyword):
                if (getConstant(parser.argsv(1)) || getFitTarget(parser.argsv(1)))
                    return Messenger::error("Failed to create fit target, as a variable by this name already exists.\n");
                fitTargets_.emplace_back(std::make_shared<ExpressionVariable>(parser.argsv(1), parser.argd(2)));
                break;
            case (Fit1DProcedureNode::MethodKeyword):
                return false;
                break;
            case (Fit1DProcedureNode::SaveKeyword):
                saveData_ = parser.argb(1);
                break;
            case (Fit1DProcedureNode::SourceDataKeyword):
                if (!dataNode_.read(parser, 1, coreData, procedure()))
                    return Messenger::error("Couldn't set source data for node.\n");
                break;
            case (Fit1DProcedureNode::nFit1DNodeKeywords):
                return Messenger::error("Unrecognised Fit1D node keyword '{}' found.\n", parser.argsv(0));
                break;
            default:
                return Messenger::error("Epic Developer Fail - Don't know how to deal with the Fit1D node keyword '{}'.\n",
                                        parser.argsv(0));
        }
    }

    return true;
}

// Write structure to specified LineParser
bool Fit1DProcedureNode::write(LineParser &parser, std::string_view prefix)
{
    // Block Start
    if (!parser.writeLineF("{}{}\n", prefix, ProcedureNode::nodeTypes().keyword(type_)))
        return false;

    // Constants
    for (auto var : constants_)
        if (!parser.writeLineF("{}  {}  {}  {:12.6e}\n", prefix,
                               fit1DNodeKeywords().keyword(Fit1DProcedureNode::ConstantKeyword), var->name(),
                               var->value().asDouble()))
            return false;

    // Fitting targets
    for (auto var : fitTargets_)
        if (!parser.writeLineF("{}  {}  {}  {:12.6e}\n", prefix, fit1DNodeKeywords().keyword(Fit1DProcedureNode::FitKeyword),
                               var->name(), var->value().asDouble()))
            return false;

    // Equation
    if (!parser.writeLineF("{}  {}  '{}'\n", prefix, fit1DNodeKeywords().keyword(Fit1DProcedureNode::EquationKeyword),
                           equation_.expressionString()))
        return false;

    // Source data
    if (!dataNode_.write(parser,
                         fmt::format("{}  {}", prefix, fit1DNodeKeywords().keyword(Fit1DProcedureNode::SourceDataKeyword))))
        return false;

    // Control
    if (saveData_ && !parser.writeLineF("{}  {}  On\n", prefix, fit1DNodeKeywords().keyword(Fit1DProcedureNode::SaveKeyword)))
        return false;

    // Block End
    if (!parser.writeLineF("{}{}\n", prefix, fit1DNodeKeywords().keyword(Fit1DProcedureNode::EndFit1DKeyword)))
        return false;

    return true;
}
