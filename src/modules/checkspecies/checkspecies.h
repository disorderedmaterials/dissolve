// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciesintra.h"
#include "module/module.h"
#include "templates/optionalref.h"
#include <fmt/ranges.h>
#include <tuple>
#include <vector>

// CheckSpecies Module
class CheckSpeciesModule : public Module
{
    public:
    CheckSpeciesModule();
    ~CheckSpeciesModule() override = default;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const override;

    /*
     * Control
     */
    private:
    // Atom types to check against indices
    std::vector<std::tuple<std::vector<int>, std::vector<std::string>>> atomTypes_;
    // Bond parameters to check
    std::vector<std::tuple<std::vector<int>, std::vector<double>>> bondParameters_;
    // Angle parameters to check
    std::vector<std::tuple<std::vector<int>, std::vector<double>>> angleParameters_;
    // Torsion parameters to check
    std::vector<std::tuple<std::vector<int>, std::vector<double>>> torsionParameters_;
    // Improper parameters to check
    std::vector<std::tuple<std::vector<int>, std::vector<double>>> improperParameters_;
    // Tolerance beyond which charges are said to differ
    double chargeTolerance_{1.0e-3};
    // Target species to check
    const Species *targetSpecies_;
    // Check the total charge of the species against the specified value
    double totalCharge_{0.0};
    // Tolerance beyond which parameters are said to differ
    double tolerance_{1.0e-3};

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
     * Functions / Data
     */
    private:
    // Check parameters for the supplied intramolecular term against those supplied
    template <class T>
    bool parametersDiffer(std::string_view intraType, OptionalReferenceWrapper<const T> term, const std::vector<int> &indices,
                          const std::vector<double> &refParams, const double tolerance) const
    {
        // Format indices string - do this outside of print() and error() calls as the ref would need to be forwarded
        auto indexString = fmt::format("{}", fmt::join(indices, "-"));

        // Check for an actual intramolecular term
        if (!term)
            return Messenger::error("No {} {} exists in the species.", intraType, indexString);

        const T &intra = *term;
        const auto &sourceParams = intra.parameters();

        // Check parameter values
        auto result = true;

        for (auto n = 0; n < std::min(sourceParams.size(), refParams.size()); ++n)
        {
            if (fabs(sourceParams.at(n) - refParams.at(n)) >= tolerance)
            {
                Messenger::print("  ... parameter {} is incorrect ({:.5e} vs. {:.5e} reference, delta = {:.5e}", n + 1,
                                 sourceParams.at(n), refParams.at(n), fabs(sourceParams.at(n) - refParams.at(n)));
                result = false;
            }
        }

        if (sourceParams.size() != refParams.size())
        {
            Messenger::print("  ... number of parameters supplied to check is inconsistent ({} vs. {})", sourceParams.size(),
                             refParams.size());
            result = false;
        }

        if (result)
            Messenger::print("The {} {} has the correct parameters.", intraType, indexString);
        else
            Messenger::print("Parameters for {} {} are incorrect - see above.", intraType, indexString);

        return !result;
    }
};
