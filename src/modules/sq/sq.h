/*
    *** SQ Module
    *** src/modules/sq/sq.h
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

#include "classes/data1dstore.h"
#include "classes/partialset.h"
#include "math/broadeningfunction.h"
#include "math/windowfunction.h"
#include "module/module.h"

// Forward Declarations
class PartialSet;

// SQ Module
class SQModule : public Module
{
    public:
    SQModule();
    ~SQModule();

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
     * Members / Functions
     */
    private:
    // Test data
    Data1DStore testData_;

    public:
    // Calculate unweighted S(Q) from unweighted g(r)
    static bool calculateUnweightedSQ(ProcessPool &procPool, Configuration *cfg, const PartialSet &unweightedgr,
                                      PartialSet &unweightedsq, double qMin, double qDelta, double qMax, double rho,
                                      const WindowFunction &windowFunction, const BroadeningFunction &broadening);
    // Sum unweighted S(Q) over the supplied Module's target Configurations
    static bool sumUnweightedSQ(ProcessPool &procPool, Module *module, GenericList &moduleData, PartialSet &summedUnweightedSQ);

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
