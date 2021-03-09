// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/braggreflection.h"
#include "classes/partialset.h"
#include "math/broadeningfunction.h"
#include "module/module.h"

// Forward Declarations
class PartialSet;

// Bragg Module
class BraggModule : public Module
{
    public:
    BraggModule();
    ~BraggModule() override = default;

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const override;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const override;
    // Return category for module
    std::string_view category() const override;
    // Return brief description of module
    std::string_view brief() const override;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const override;

    /*
     * Initialisation
     */
    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * Members / Functions
     */
    public:
    // Calculate Bragg terms for specified Configuration
    bool calculateBraggTerms(GenericList &moduleData, ProcessPool &procPool, Configuration *cfg, const double qMin,
                             const double qDelta, const double qMax, Vec3<int> multiplicity, bool &alreadyUpToDate);
    // Form partial and total reflection functions from calculated reflection data
    bool formReflectionFunctions(GenericList &moduleData, ProcessPool &procPool, Configuration *cfg, const double qMin,
                                 const double qDelta, const double qMax);
    // Re-bin reflection data into supplied arrays
    bool reBinReflections(GenericList &moduleData, ProcessPool &procPool, Configuration *cfg, Array2D<Data1D> &braggPartials);

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
