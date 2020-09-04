/*
    *** RDF Module
    *** src/modules/rdf/rdf.h
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
#include "math/pairbroadeningfunction.h"
#include "module/module.h"

// Forward Declarations
class Dissolve;
class ModuleGroup;
class PartialSet;

// RDF Module
class RDFModule : public Module
{
    public:
    RDFModule();
    ~RDFModule();

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

    public:
    // Partial Calculation Method enum
    enum PartialsMethod
    {
        AutoMethod,
        CellsMethod,
        SimpleMethod,
        TestMethod,
        nPartialsMethods
    };
    // Return enum option info for PartialsMethod
    EnumOptions<RDFModule::PartialsMethod> partialsMethods();

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

    private:
    // Calculate partial g(r) in serial with simple double-loop
    bool calculateGRTestSerial(Configuration *cfg, PartialSet &partialSet);
    // Calculate partial g(r) with optimised double-loop
    bool calculateGRSimple(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double rdfRange);
    // Calculate partial g(r) utilising Cell neighbour lists
    bool calculateGRCells(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double binWidth);

    public:
    // (Re)calculate partial g(r) for the specified Configuration
    bool calculateGR(ProcessPool &procPool, Configuration *cfg, RDFModule::PartialsMethod method, const double rdfRange,
                     const double rdfBinWidth, bool &alreadyUpToDate);
    // Calculate smoothed/broadened partial g(r) from supplied partials
    static bool calculateUnweightedGR(ProcessPool &procPool, Configuration *cfg, const PartialSet &originalgr,
                                      PartialSet &weightedgr, PairBroadeningFunction &intraBroadening, int smoothing);
    // Return effective density for specified Module's target Configurations
    static double summedRho(Module *module, GenericList &processingModuleData);
    // Sum unweighted g(r) over the supplied Module's target Configurations
    static bool sumUnweightedGR(ProcessPool &procPool, Module *module, GenericList &processingModuleData,
                                PartialSet &summedUnweightedGR);
    // Sum unweighted g(r) over all Configurations targeted by the specified ModuleGroup
    static bool sumUnweightedGR(ProcessPool &procPool, Module *parentModule, ModuleGroup *moduleGroup,
                                GenericList &processingModuleData, PartialSet &summedUnweightedGR);
    // Test supplied PartialSets against each other
    static bool testReferencePartials(PartialSet &setA, PartialSet &setB, double testThreshold);
    // Test calculated partial against supplied reference data
    static bool testReferencePartial(const PartialSet &partials, double testThreshold, const Data1D &testData,
                                     std::string_view typeIorTotal, std::string_view typeJ = "", std::string_view target = "");
    // Test calculated vs reference data (two source sets)
    static bool testReferencePartials(const Data1DStore &testData, double testThreshold, const PartialSet &partials,
                                      std::string_view prefix);
    // Test calculated vs reference data (two source sets)
    static bool testReferencePartials(const Data1DStore &testData, double testThreshold, const PartialSet &partialsA,
                                      std::string_view prefixA, const PartialSet &partialsB, std::string_view prefixB);

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
