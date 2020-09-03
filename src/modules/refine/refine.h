/*
    *** Refine Module
    *** src/modules/refine/refine.h
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

#include "classes/scatteringmatrix.h"
#include "math/broadeningfunction.h"
#include "math/interpolator.h"
#include "math/windowfunction.h"
#include "module/groups.h"
#include "module/module.h"

// Forward Declarations
class AtomType;
class PartialSet;

// Refine Module
class RefineModule : public Module
{
    /*
     * Performs adjustment of interatomic potentials, based on differences between calculated and reference datasets.
     */

    public:
    RefineModule();
    ~RefineModule();

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
    public:
    // Potential Inversion Method Enum
    enum PotentialInversionMethod
    {
        DirectFourierPotentialInversion,    /* Invert delta S(Q) by direct FT, and treat as addition to potential */
        DirectGaussianPotentialInversion,   /* Invert delta S(Q) by approximating with Gaussians, and treat as addition to
                               potential */
        PercusYevickPotentialInversion,     /* Use Percus-Yevick closure to generate potential from delta S(Q) */
        HypernettedChainPotentialInversion, /* Use the hypernetted chain closure to generate potential from delta S(Q)
                                             */
        nPotentialInversionMethods
    };
    // Return enum options for PotentialInversionMethod
    static EnumOptions<RefineModule::PotentialInversionMethod> potentialInversionMethods();
    // Matrix Augmentation Style
    enum MatrixAugmentationStyle
    {
        NoAugmentation,       /* Do not supplement the scattering matrix with any additional data */
        PartialsAugmentation, /* Augment scattering matrix with individual partial S(Q), as EPSR does */
        nMatrixAugmentationStyles
    };
    // Return enum options for MatrixAugmentationStyle
    static EnumOptions<RefineModule::MatrixAugmentationStyle> matrixAugmentationStyles();

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
     * Functions
     */
    private:
    // Target Modules, divided into groups
    ModuleGroups groupedTargets_;
    // Full scattering matrix containing reference dat
    ScatteringMatrix scatteringMatrix_;
    // Simulated data added as reference data
    Array<Data1D> simulatedReferenceData_;
    // Current data being fit by modifyBondTerms
    Data1D fitData_;
    // Interpolation of current data being fit by modifyBondTerms
    Interpolator interpolatedFitData_;
    // Starting position and maximum delta to allow on the x-intercept while fitting
    double xCentreStart_, xCentreDeltaLimit_;

    private:
    // Calculate c(r) from supplied S(Q)
    Data1D calculateCR(const Data1D &sq, double normFactor, double rMin, double rStep, double rMax,
                       WindowFunction windowFunction = WindowFunction(), BroadeningFunction broadening = BroadeningFunction(),
                       bool unbroaden = false);
    // Determine modification to bonds based on supplied delta g(r)
    bool modifyBondTerms(CoreData &coreData, const Data1D &deltaGR, std::shared_ptr<AtomType> typeI,
                         std::shared_ptr<AtomType> typeJ, Data1D &deltaBond);
    // Return value of fit equation given specified parameters
    inline double fitEquation(double x, double xCentre, double delta, double widthSquared, double AL, double AC, double AR);
    // Two-exponential, 5-parameter cost function for modifyBondTerms() fitting
    double costFunction2Exp(const Array<double> &alpha);
    // Three-exponential, 6-parameter cost function for modifyBondTerms() fitting
    double costFunction3Exp(const Array<double> &alpha);
    // Sum fitting equation with the specified parameters into the specified Data1D
    void sumFitEquation(Data1D &target, double xCentre, double delta, double widthSquared, double AL, double AC, double AR);

    public:
    // Return list of target Modules / data for fitting process
    const RefDataList<Module, ModuleGroup *> &allTargets() const;
    // Return grouped target Modules
    const ModuleGroups &groupedTargets() const;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
