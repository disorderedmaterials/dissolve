// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "main/dissolve.h"
#include "modules/epsr/epsr.h"

// Return enum options for EPSRPCofKeyword
EnumOptions<EPSRModule::EPSRPCofKeyword> EPSRModule::epsrPCofKeywords()
{
    return EnumOptions<EPSRModule::EPSRPCofKeyword>("PCOFKeywords", {{EPSRModule::AddPotTypePCofKeyword, "addpottype"},
                                                                     {EPSRModule::ExpecFPCofKeyword, "expecf"},
                                                                     {EPSRModule::GaussianPCofKeyword, "gaussian"},
                                                                     {EPSRModule::NCoeffPPCofKeyword, "ncoeffp"},
                                                                     {EPSRModule::NPItSSPCofKeyword, "npitss"},
                                                                     {EPSRModule::PAcceptPCofKeyword, "paccept"},
                                                                     {EPSRModule::PDMaxPCofKeyword, "pdmax"},
                                                                     {EPSRModule::PDStepPCofKeyword, "pdstep"},
                                                                     {EPSRModule::PowerPCofKeyword, "power"},
                                                                     {EPSRModule::PSigma2PCofKeyword, "psigma2"},
                                                                     {EPSRModule::QuitPCofKeyword, "q"},
                                                                     {EPSRModule::RBroadPCofKeyword, "rbroad"},
                                                                     {EPSRModule::RChargePCofKeyword, "rcharge"},
                                                                     {EPSRModule::RefPotFacPCofKeyword, "refpotfac"},
                                                                     {EPSRModule::RepPotTypePCofKeyword, "reppottype"},
                                                                     {EPSRModule::RMaxPtPCofKeyword, "rmaxpt"},
                                                                     {EPSRModule::RMinFacPCofKeyword, "rminfac"},
                                                                     {EPSRModule::RMinPtPCofKeyword, "rminpt"},
                                                                     {EPSRModule::ROverlapPCofKeyword, "roverlap"}});
}

// Read data from supplied pcof file
bool EPSRModule::readPCof(Dissolve &dissolve, const ProcessPool &procPool, std::string_view filename)
{
    /*
     * Read EPSR potential coefficients from supplied file.
     * All potential coefficients matching pair potentials that exist in the simulation will be set.
     * All others will be set to zero.
     */

    LineParser parser(&procPool);
    if (!parser.openInput(filename))
        return Messenger::error("Couldn't open pcof file for reading.\n");

    // Read keyword section (terminated by the 'q' command)
    auto done = false;
    nCoeffP_ = 0;
    while (!parser.eofOrBlank())
    {
        // Parse arguments, and attempt to convert the first into a keyword
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return Messenger::error("Failed to read pcof file.\n");

        auto keyword = epsrPCofKeywords().enumeration(parser.argsv(0));
        switch (keyword)
        {
            case (EPSRModule::AddPotTypePCofKeyword):
                break;
            case (EPSRModule::ExpecFPCofKeyword):
                break;
            case (EPSRModule::GaussianPCofKeyword):
                if (DissolveSys::sameString(parser.argsv(1), "F") || DissolveSys::sameString(parser.argsv(1), "Poisson"))
                    expansionFunction_ = EPSRModule::PoissonExpansionFunction;
                else if (DissolveSys::sameString(parser.argsv(1), "T") || DissolveSys::sameString(parser.argsv(1), "Gaussian"))
                    expansionFunction_ = EPSRModule::GaussianExpansionFunction;
                else
                    Messenger::warn("Couldn't determine expansion function to use (argument is '{}').\n", parser.argsv(1));
                break;
            case (EPSRModule::NCoeffPPCofKeyword):
                nCoeffP_ = parser.argi(1);
                break;
            case (EPSRModule::NPItSSPCofKeyword):
                nPItSs_ = parser.argi(1);
                break;
            case (EPSRModule::PAcceptPCofKeyword):
                break;
            case (EPSRModule::PDMaxPCofKeyword):
                break;
            case (EPSRModule::PDStepPCofKeyword):
                break;
            case (EPSRModule::PowerPCofKeyword):
                break;
            case (EPSRModule::PSigma2PCofKeyword):
                pSigma1_ = parser.argd(1);
                pSigma2_ = parser.argd(1);
                gSigma1_ = parser.argd(1);
                // Note - the factor of two applied here is used to reproduce the broadening applied by ESPR to
                // the r-space Gaussian transformation
                gSigma2_ = parser.argd(1) * 2.0;
                break;
            case (EPSRModule::QuitPCofKeyword):
                done = true;
                break;
            case (EPSRModule::RBroadPCofKeyword):
                break;
            case (EPSRModule::RChargePCofKeyword):
                break;
            case (EPSRModule::RefPotFacPCofKeyword):
                break;
            case (EPSRModule::RepPotTypePCofKeyword):
                break;
            case (EPSRModule::RMaxPtPCofKeyword):
                rMaxPT_ = parser.argd(1);
                break;
            case (EPSRModule::RMinFacPCofKeyword):
                break;
            case (EPSRModule::RMinPtPCofKeyword):
                rMinPT_ = parser.argd(1);
                break;
            case (EPSRModule::ROverlapPCofKeyword):
                break;
            default:
                Messenger::warn("Unrecognised pcof file keyword '{}'...\n", parser.argsv(0));
                continue;
        }

        // If we have found the 'q' keyword, exit the loop
        if (done)
            break;
    }

    // Retrieve and zero the current potential coefficients file
    auto &potentialCoefficients = dissolve.processingModuleData().realise<Array2D<std::vector<double>>>(
        "PotentialCoefficients", name_, GenericItem::InRestartFileFlag);
    potentialCoefficients.initialise(dissolve.coreData().nAtomTypes(), dissolve.coreData().nAtomTypes(), true);
    for (auto &n : potentialCoefficients)
    {
        n.clear();
        n.resize(nCoeffP_.value_or(0), 0.0);
    }

    // Now we are ready to read in the potential coefficients - first line contains the number of pair potentials to expect
    // coefficients for
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return Messenger::error("Failed to read number of pair potentials from pcof file.\n");
    auto nPots = parser.argi(0);
    Messenger::print("Number of potentials in pcof file = {}\n", nPots);
    for (auto n = 0; n < nPots; ++n)
    {
        // First line of potential contains the two atom types it is related to, and its index (in EPSR)
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return Messenger::error("Failed to read pair potential atom types from pcof file.\n");

        // Find the atom types to which these coefficients relate
        auto at1 = dissolve.coreData().findAtomType(parser.argsv(0));
        if (!at1)
            return Messenger::error("Unrecognised AtomType '{}' referenced in pcof file.\n", parser.argsv(0));
        auto at2 = dissolve.coreData().findAtomType(parser.argsv(1));
        if (!at2)
            return Messenger::error("Unrecognised AtomType '{}' referenced in pcof file.\n", parser.argsv(1));

        Messenger::print("Found {}-{} potential...\n", at1->name(), at2->name());

        // Next line contains ??? and ??? TODO
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return Messenger::error("Failed to read ??? and ??? from pcof file.\n");

        // Grab the coefficient storage from the module data and read the coefficients in - they will all be on one
        // single line in the file.
        auto &coefficients = potentialCoefficients[{at1->index(), at2->index()}];
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return Messenger::error("Failed to read coefficients from pcof file.\n");
        if (parser.nArgs() != nCoeffP_.value_or(-1))
            return Messenger::error("Number of potential coefficients ({}) does not match ncoeffp ({}).\n", parser.nArgs(),
                                    nCoeffP_.value_or(-1));
        for (auto i = 0; i < nCoeffP_; ++i)
            coefficients[i] = parser.argd(i);

        // Zero the first coefficient, which EPSR ignores
        coefficients[0] = 0.0;
    }

    return true;
}

// Read fit coefficients from 'inpa' file
bool EPSRModule::readFitCoefficients(Dissolve &dissolve, const ProcessPool &procPool, std::string_view inpaFilename)
{
    /*
     * Read EPSR fit coefficients from supplied file.
     * All potential coefficients matching pair potentials that exist in the simulation will be set.
     * All others will be set to zero.
     */

    LineParser parser(&procPool);
    if (!parser.openInput(inpaFilename))
        return Messenger::error("Couldn't open inpa file for reading.\n");

    // First line contains parameters relating to energy / pressure calculation which we can skip
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return Messenger::error("Failed to read initial parameters from inpa file.\n");

    // Read fit coefficients, indexed by dataset - we assume that the data in the file is the same size as our target data
    for (auto &target : targets_)
    {
        // Data file index
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return Messenger::error("Failed to read initial parameters from inpa file.\n");
        Messenger::print("Reading fit coefficients for data file {} ({})...\n", parser.argi(0), target->name());

        // Number of coefficients, stepsize in r, sigma2
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return Messenger::error("Failed to read basic coefficient data from inpa file.\n");
        const auto nCoeff = parser.argi(0);

        // Coefficients
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return Messenger::error("Failed to read coefficient data from inpa file.\n");
        if (parser.nArgs() != nCoeff)
            return Messenger::error("Number of potential coefficients ({}) does not match ncoeffp ({}).\n", parser.nArgs(),
                                    nCoeff);
        auto &fitCoefficients = dissolve.processingModuleData().realise<std::vector<double>>(
            fmt::format("FitCoefficients_{}", target->name()), name_, GenericItem::InRestartFileFlag);
        fitCoefficients.resize(nCoeff);
        for (auto i = 0; i < nCoeff; ++i)
            fitCoefficients[i] = parser.argd(i);
    }

    parser.closeFiles();
    return true;
}
