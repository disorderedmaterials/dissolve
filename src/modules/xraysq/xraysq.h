// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "classes/data1dstore.h"
#include "classes/partialset.h"
#include "data/formfactors.h"
#include "io/import/data1d.h"
#include "module/module.h"

// Forward Declarations
class PartialSet;
class XRayWeights;

// SQ Module
class XRaySQModule : public Module
{
    public:
    XRaySQModule();
    ~XRaySQModule();

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
    private:
    // Reference F(Q) file and format
    Data1DImportFileFormat referenceFQ_;

    protected:
    // Perform any necessary initialisation for the Module
    void initialise();

    public:
    // Normalisation Type enum
    enum NormalisationType
    {
        NoNormalisation,
        AverageOfSquaresNormalisation,
        SquareOfAverageNormalisation,
        nNormalisationTypes
    };
    // Return enum option info for NormalisationType
    EnumOptions<XRaySQModule::NormalisationType> normalisationTypes();
    // Return file and format for reference total F(Q)
    const Data1DImportFileFormat &referenceFQFileAndFormat();

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool);

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool);

    /*
     * Members / Functions
     */
    private:
    // Test data
    Data1DStore testData_;

    public:
    // Calculate weighted g(r) from supplied unweighted g(r) and Weights
    bool calculateWeightedGR(PartialSet &unweightedgr, PartialSet &weightedgr, const XRayWeights &weights,
                             XRaySQModule::NormalisationType normalisation);
    // Calculate weighted S(Q) from supplied unweighted S(Q) and Weights
    bool calculateWeightedSQ(PartialSet &unweightedsq, PartialSet &weightedsq, const XRayWeights &weights,
                             XRaySQModule::NormalisationType normalisation);
    // Calculate Weights matrix summed over target Configurations
    bool calculateSummedWeights(XRayWeights &summedWeights, XRayFormFactors::XRayFormFactorData formFactors) const;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
