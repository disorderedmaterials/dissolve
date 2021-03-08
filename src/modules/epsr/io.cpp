// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "keywords/types.h"
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
bool EPSRModule::readPCof(Dissolve &dissolve, ProcessPool &procPool, std::string_view filename)
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
    auto ncoeffp = 0;
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
                    keywords_.setEnumeration<EPSRModule::ExpansionFunctionType>("expansionfunction",
                                                                                EPSRModule::PoissonExpansionFunction);
                else if (DissolveSys::sameString(parser.argsv(1), "T") || DissolveSys::sameString(parser.argsv(1), "Gaussian"))
                    keywords_.setEnumeration<EPSRModule::ExpansionFunctionType>("expansionfunction",
                                                                                EPSRModule::GaussianExpansionFunction);
                else
                    Messenger::warn("Couldn't determine expansion function to use (argument is '{}').\n", parser.argsv(1));
                break;
            case (EPSRModule::NCoeffPPCofKeyword):
                ncoeffp = parser.argi(1);
                keywords_.set<int>("ncoeffp", ncoeffp);
                break;
            case (EPSRModule::NPItSSPCofKeyword):
                keywords_.set<int>("npitss", parser.argi(1));
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
                keywords_.set<double>("psigma1", parser.argd(1));
                keywords_.set<double>("psigma2", parser.argd(1));
                keywords_.set<double>("gsigma1", parser.argd(1));
                // Note - the factor of two applied here is used to reproduce the broadening applied by ESPR to
                // the r-space Gaussian transformation
                keywords_.set<double>("gsigma2", parser.argd(1) * 2.0);
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
                keywords_.set<double>("rmaxpt", parser.argd(1));
                break;
            case (EPSRModule::RMinFacPCofKeyword):
                break;
            case (EPSRModule::RMinPtPCofKeyword):
                keywords_.set<double>("rminpt", parser.argd(1));
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
        "PotentialCoefficients", uniqueName_, GenericItem::InRestartFileFlag);
    potentialCoefficients.initialise(dissolve.nAtomTypes(), dissolve.nAtomTypes(), true);
    for (auto &n : potentialCoefficients)
    {
        n.clear();
        n.resize(ncoeffp, 0.0);
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

        // Find the atom types to which these cofficients relate
        auto at1 = dissolve.findAtomType(parser.argsv(0));
        if (!at1)
            return Messenger::error("Unrecognised AtomType '{}' referenced in pcof file.\n", parser.argsv(0));
        auto at2 = dissolve.findAtomType(parser.argsv(1));
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
        if (parser.nArgs() != ncoeffp)
            return Messenger::error("Number of potential coefficients ({}) does not match ncoeffp ({}).\n", parser.nArgs(),
                                    ncoeffp);
        for (auto i = 0; i < ncoeffp; ++i)
            coefficients[i] = parser.argd(i);

        // Zero the first coefficient, which EPSR ignores
        coefficients[0] = 0.0;
    }

    return true;
}
