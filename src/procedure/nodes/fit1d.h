// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "expression/expression.h"
#include "math/data1d.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/nodereference.h"
#include "templates/reflist.h"

// Forward Declarations
class Collect1DProcedureNode;
class SelectProcedureNode;
class Data1D;
class LineParser;
class NodeScopeStack;

// Procedure Node - Fit1D
class Fit1DProcedureNode : public ProcedureNode
{
    public:
    Fit1DProcedureNode(Collect1DProcedureNode *target = nullptr);
    ~Fit1DProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;

    /*
     * Node Keywords
     */
    public:
    // Node Keywords
    enum Fit1DNodeKeyword
    {
        ConstantKeyword,
        EndFit1DKeyword,
        EquationKeyword,
        FitKeyword,
        MethodKeyword,
        SaveKeyword,
        SourceDataKeyword,
        nFit1DNodeKeywords
    };
    // Return enum option info for Fit1DNodeKeyword
    static EnumOptions<Fit1DNodeKeyword> fit1DNodeKeywords();

    /*
     * Data
     */
    private:
    // Node containing source data to fit
    ProcedureNodeReference dataNode_;
    // Fit equation object
    Expression equation_;
    // Data against which to fit
    Data1D referenceData_;
    // Vector of variables accessible by the fitting equation
    std::vector<std::shared_ptr<ExpressionVariable>> variables_;
    // Data variables accessible by the transform equation
    std::shared_ptr<ExpressionVariable> xVariable_;
    // Vector of variables that we use, but are not fitting
    std::vector<std::shared_ptr<ExpressionVariable>> constants_;
    // Vector of variables which we are fitting
    std::vector<std::shared_ptr<ExpressionVariable>> fitTargets_;
    // Whether to save data after normalisation
    bool saveData_;

    private:
    // Return the named fit target, if it exists
    std::shared_ptr<ExpressionVariable> getFitTarget(std::string_view name);
    // Return the named constant, if it exists
    std::shared_ptr<ExpressionVariable> getConstant(std::string_view name);
    // Fitting cost function
    double equationCost(const std::vector<double> &alpha);

    public:
    // Return fitted data
    const Data1D &fittedData() const;
    // Set whether to save processed data
    void setSaveData(bool on);

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Finalise any necessary data after execution
    bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;

    /*
     * Read / Write
     */
    public:
    // Read structure from specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write structure to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);
};
