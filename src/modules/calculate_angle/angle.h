/*
    *** Calculate Angle Module
    *** src/modules/calculate_angle/angle.h
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
class Collect1DProcedureNode;
class Collect2DProcedureNode;
class Collect3DProcedureNode;
class Process1DProcedureNode;
class Process2DProcedureNode;
class Process3DProcedureNode;
class SelectProcedureNode;
class SpeciesSite;

// Calculate Angle Module
class CalculateAngleModule : public Module
{
    public:
    CalculateAngleModule();
    ~CalculateAngleModule();

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
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool);
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool);

    /*
     * Functions / Data
     */
    private:
    // Analysis procedure to be run
    Procedure analyser_;
    // SelectNode for site A
    SelectProcedureNode *selectA_;
    // SelectNode for site B
    SelectProcedureNode *selectB_;
    // SelectNode for site C
    SelectProcedureNode *selectC_;
    // Collect1DNode for A-B RDF
    Collect1DProcedureNode *collectAB_;
    // Collect1DNode for B-C RDF
    Collect1DProcedureNode *collectBC_;
    // Collect1DNode for A-B-C angle histogram
    Collect1DProcedureNode *collectABC_;
    // Collect2DNode for (A-B)-C distance-angle data
    Collect2DProcedureNode *collectDAngleAB_;
    // Collect2DNode for A-(B-C) distance-angle data
    Collect2DProcedureNode *collectDAngleBC_;
    // Collect3DNode for A-B vs B-C vs A-B-C distance-distance-angle data
    Collect3DProcedureNode *collectDDA_;
    // Process1DNode for A-B RDF
    Process1DProcedureNode *processAB_;
    // Process1DNode for B-c RDF
    Process1DProcedureNode *processBC_;
    // Process1DNode for A-B-C angle histogram
    Process1DProcedureNode *processAngle_;
    // Process2DNode for (A-B)-C distance-angle data
    Process2DProcedureNode *processDAngleAB_;
    // Process2DNode for A-(B-C) distance-angle data
    Process2DProcedureNode *processDAngleBC_;
    // Process3DNode for A-B vs B-C vs A-B-C distance-distance-angle data
    Process3DProcedureNode *processDDA_;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
