// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/coreData.h"
#include "main/dissolve.h"
#include "math/error.h"
#include "math/sampledVector.h"

namespace UnitTest
{
class DissolveSystemTest
{
    public:
    DissolveSystemTest() : dissolve_(coreData_){};

    private:
    CoreData coreData_;
    Dissolve dissolve_;

    public:
    // Load, parse, optionally rewrite the specified file, and prepare the simulation
    void setUp(std::string_view inputFile, bool noRewriteCheck = false)
    {
        dissolve_.clear();
        if (!dissolve_.loadInput(inputFile))
            throw(std::runtime_error(fmt::format("Input file '{}' failed to load correctly.\n", inputFile)));

        if (!noRewriteCheck)
        {
            auto newInput = fmt::format("{}.rewrite", inputFile);
            if (!dissolve_.saveInput(newInput))
                throw(std::runtime_error(fmt::format("Input file '{}' failed to rewrite correctly.\n", inputFile)));

            dissolve_.clear();
            if (!dissolve_.loadInput(newInput))
                throw(std::runtime_error(fmt::format("Input file '{}' failed to reload correctly.\n", newInput)));
        }

        if (!dissolve_.prepare())
            throw(std::runtime_error("Failed to prepare simulation.\n"));
    }
    // Load restart file
    void loadRestart(std::string_view restartFile)
    {
        if (!dissolve_.loadRestart(restartFile))
            throw(std::runtime_error(fmt::format("Restart file '{}' failed to load correctly.\n", restartFile)));
    }
    // Iterate for set number of iterations
    bool iterate(int nIterations) { return dissolve_.iterate(nIterations); }
    // Test SampledVector data
    bool checkSampledVector(std::string_view tag, std::vector<double> referenceData, double tolerance = 5.0e-3,
                            Error::ErrorType errorType = Error::ErrorType::EuclideanError)
    {
        // Locate the target reference data
        auto optData = dissolve_.processingModuleData().search<const SampledVector>(tag);
        if (!optData)
        {
            return Messenger::error("No data with tag '{}' exists.\n", tag);
        }

        const SampledVector &data = optData->get();
        Messenger::print("Located reference data '{}'.\n", tag);

        // Generate the error estimate and compare against the threshold value
        auto error = Error::error(errorType, data.values(), referenceData).error;
        auto notOK = isnan(error) || error > tolerance;
        Messenger::print("Target data '{}' has error of {:7.3e} with reference data and is {} (threshold is {:6.3e})\n\n", tag,
                         error, notOK ? "NOT OK" : "OK", tolerance);
        return !notOK;
    }
};
} // namespace UnitTest
