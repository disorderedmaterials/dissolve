/*
    *** Procedure Node - Fit1D
    *** src/procedure/nodes/fit1d.h
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
    ~Fit1DProcedureNode();

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context);

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
    // X variable for equation
    ExpressionVariable *xVariable_;
    // List of variables that we use, but are not fitting
    RefList<ExpressionVariable> constants_;
    // List of variables which we are fitting
    RefList<ExpressionVariable> fitTargets_;
    // Whether to save data after normalisation
    bool saveData_;

    private:
    // Fitting cost function
    double equationCost(const Array<double> &alpha);

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
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList);
    // Execute node, targetting the supplied Configuration
    ProcedureNode::NodeExecutionResult execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                               GenericList &targetList);
    // Finalise any necessary data after execution
    bool finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList);

    /*
     * Read / Write
     */
    public:
    // Read structure from specified LineParser
    bool read(LineParser &parser, CoreData &coreData);
    // Write structure to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);
};
