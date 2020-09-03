/*
    *** EPSR Module - I/O
    *** src/modules/epsr/io.cpp
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

#include "classes/atomtype.h"
#include "genericitems/listhelper.h"
#include "keywords/types.h"
#include "main/dissolve.h"
#include "modules/epsr/epsr.h"

// Return enum options for EPSRPCofKeyword
EnumOptions<EPSRModule::EPSRPCofKeyword> &EPSRModule::epsrPCofKeywords()
{
    static EnumOptionsList PCOFKeywordOptions =
        EnumOptionsList()
        << EnumOption(EPSRModule::AddPotTypePCofKeyword, "addpottype") << EnumOption(EPSRModule::ExpecFPCofKeyword, "expecf")
        << EnumOption(EPSRModule::GaussianPCofKeyword, "gaussian") << EnumOption(EPSRModule::NCoeffPPCofKeyword, "ncoeffp")
        << EnumOption(EPSRModule::NPItSSPCofKeyword, "npitss") << EnumOption(EPSRModule::PAcceptPCofKeyword, "paccept")
        << EnumOption(EPSRModule::PDMaxPCofKeyword, "pdmax") << EnumOption(EPSRModule::PDStepPCofKeyword, "pdstep")
        << EnumOption(EPSRModule::PowerPCofKeyword, "power") << EnumOption(EPSRModule::PSigma2PCofKeyword, "psigma2")
        << EnumOption(EPSRModule::QuitPCofKeyword, "q") << EnumOption(EPSRModule::RBroadPCofKeyword, "rbroad")
        << EnumOption(EPSRModule::RChargePCofKeyword, "rcharge") << EnumOption(EPSRModule::RefPotFacPCofKeyword, "refpotfac")
        << EnumOption(EPSRModule::RepPotTypePCofKeyword, "reppottype") << EnumOption(EPSRModule::RMaxPtPCofKeyword, "rmaxpt")
        << EnumOption(EPSRModule::RMinFacPCofKeyword, "rminfac") << EnumOption(EPSRModule::RMinPtPCofKeyword, "rminpt")
        << EnumOption(EPSRModule::ROverlapPCofKeyword, "roverlap");
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

        EPSRModule::EPSRPCofKeyword keyword = epsrPCofKeywords().enumeration(parser.argsv(0));
        switch (keyword)
        {
            case (EPSRModule::AddPotTypePCofKeyword):
                break;
            case (EPSRModule::ExpecFPCofKeyword):
                break;
            case (EPSRModule::GaussianPCofKeyword):
                keywords_.set<EPSRModule::ExpansionFunctionType>(
                    "expansionfunction",
                    (DissolveSys::sameString(parser.argsv(1), "Poisson") || DissolveSys::sameString(parser.argsv(1), "T")
                         ? EPSRModule::PoissonExpansionFunction
                         : EPSRModule::GaussianExpansionFunction));
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
    auto &potentialCoefficients = GenericListHelper<Array2D<Array<double>>>::realise(
        dissolve.processingModuleData(), "PotentialCoefficients", uniqueName_, GenericItem::InRestartFileFlag);
    potentialCoefficients.initialise(dissolve.nAtomTypes(), dissolve.nAtomTypes(), true);
    for (int n = 0; n < potentialCoefficients.linearArraySize(); ++n)
    {
        potentialCoefficients.linearArray()[n].initialise(ncoeffp);
        potentialCoefficients.linearArray()[n] = 0.0;
    }

    // Now we are ready to read in the potential coefficients - first line contains the number of pair potentials to expect
    // coefficients for
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return Messenger::error("Failed to read number of pair potentials from pcof file.\n");
    auto nPots = parser.argi(0);
    Messenger::print("Number of potentials in pcof file = {}\n", nPots);
    for (int n = 0; n < nPots; ++n)
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
        Array<double> &coefficients = potentialCoefficients.at(at1->index(), at2->index());
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return Messenger::error("Failed to read coefficients from pcof file.\n");
        if (parser.nArgs() != ncoeffp)
            return Messenger::error("Number of potential coefficients ({}) does not match ncoeffp ({}).\n", parser.nArgs(),
                                    ncoeffp);
        for (int i = 0; i < ncoeffp; ++i)
            coefficients[i] = parser.argd(i);

        // Zero the first coefficient, which EPSR ignores
        coefficients[0] = 0.0;
    }

    return true;
}
