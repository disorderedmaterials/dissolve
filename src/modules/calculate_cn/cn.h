/*
    *** CalculateCN Module
    *** src/modules/calculate_cn/cn.h
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

#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class Process1DProcedureNode;
class Sum1DProcedureNode;
class OperateSitePopulationNormaliseProcedureNode;

// CalculateCN Module
class CalculateCNModule : public Module
{
    public:
    CalculateCNModule();
    ~CalculateCNModule();

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const;
    // Return category for module
    std::string_view category() const;
    // Return brief description of module
    std::string_view brief() const;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const;

    /*
     * Initialisation
     */
    protected:
    // Perform any necessary initialisation for the Module
    void initialise();

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool);

    /*
     * Functions / Data
     */
    private:
    // Analysis procedure to be run
    Procedure analyser_;
    // Sum1D node
    Sum1DProcedureNode *sum1D_;
    // Process1D node
    Process1DProcedureNode *process1D_;
    // Site normalisation node
    OperateSitePopulationNormaliseProcedureNode *siteNormaliser_;

    public:
    // Return specified coordination number (from Sum1D node)
    const SampledDouble &coordinationNumber(int index);
    // Return whether range B is enabled
    bool isRangeBEnabled() const;
    // Return whether range C is enabled
    bool isRangeCEnabled() const;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
