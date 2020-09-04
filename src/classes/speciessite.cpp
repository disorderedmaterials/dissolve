/*
    *** SpeciesSite Definition
    *** src/classes/speciessite.cpp
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

#include "classes/speciessite.h"
#include "base/lineparser.h"
#include "classes/site.h"
#include "classes/species.h"
#include "data/atomicmass.h"

SpeciesSite::SpeciesSite() : ListItem<SpeciesSite>()
{
    parent_ = nullptr;
    originMassWeighted_ = false;
}

SpeciesSite::~SpeciesSite() {}

/*
 * Basic Information
 */

// Set name of site
void SpeciesSite::setName(std::string_view newName) { name_ = newName; }

// Return anme of site
std::string_view SpeciesSite::name() const { return name_; }

// Set Species parent
void SpeciesSite::setParent(Species *sp) { parent_ = sp; }

// Return species parent
Species *SpeciesSite::parent() { return parent_; }

// Return version
int SpeciesSite::version() const { return version_; }

/*
 * Definition
 */

// Add origin atom
bool SpeciesSite::addOriginAtom(SpeciesAtom *originAtom)
{
    if (!originAtom)
        return Messenger::error("NULL SpeciesAtom passed to SpeciesSite::addOriginAtom().\n");

    // If the SpeciesAtom already exists in the list, complain
    if (originAtoms_.contains(originAtom))
        return Messenger::error("Origin atom index {} specified twice for site '{}'.\n", originAtom->index(), name_);

    originAtoms_.append(originAtom);

    ++version_;

    return true;
}

// Remove origin atom
void SpeciesSite::removeOriginAtom(SpeciesAtom *originAtom)
{
    if (originAtoms_.contains(originAtom))
    {
        originAtoms_.remove(originAtom);

        ++version_;
    }
    else
        Messenger::error("Tried to remove a SpeciesAtom from the origin list that isn't present\n");
}

// Add origin atom from index
bool SpeciesSite::addOriginAtom(int atomIndex)
{
#ifdef CHECKS
    if (!parent_)
        return Messenger::error("Tried to add an origin atom by index to a SpeciesSite, but no parent Species is set.\n");
#endif
    return addOriginAtom(parent_->atom(atomIndex));
}

// Set origin atoms
bool SpeciesSite::setOriginAtoms(const RefList<SpeciesAtom> atoms)
{
    originAtoms_.clear();

    ++version_;

    for (SpeciesAtom *i : atoms)
        if (!addOriginAtom(i))
        {
            originAtoms_.clear();
            return false;
        }

    return true;
}

// Return list of origin atoms
const RefList<SpeciesAtom> &SpeciesSite::originAtoms() { return originAtoms_; }

// Return integer array of indices from which the origin should be formed
Array<int> SpeciesSite::originAtomIndices() const
{
    Array<int> indices;
    for (SpeciesAtom *atom : originAtoms_)
        indices.add(atom->index());

    return indices;
}

// Set whether the origin should be calculated with mass-weighted positions
void SpeciesSite::setOriginMassWeighted(bool b)
{
    originMassWeighted_ = b;

    ++version_;
}

// Return whether the origin should be calculated with mass-weighted positions
bool SpeciesSite::originMassWeighted() const { return originMassWeighted_; }

// Add x-axis atom
bool SpeciesSite::addXAxisAtom(SpeciesAtom *xAxisAtom)
{
    if (!xAxisAtom)
        return Messenger::error("NULL SpeciesAtom passed to SpeciesSite::addXAxisAtom().\n");

    // If the SpeciesAtom already exists in the list, complain
    if (xAxisAtoms_.contains(xAxisAtom))
        return Messenger::error("X-axis atom index {} specified twice for site '{}'.\n", xAxisAtom->index(), name_);

    xAxisAtoms_.append(xAxisAtom);

    ++version_;

    return true;
}

// Add x-axis atom from index
bool SpeciesSite::addXAxisAtom(int atomIndex)
{
#ifdef CHECKS
    if (!parent_)
        return Messenger::error("Tried to add an x-axis atom by index to a SpeciesSite, but no parent Species is set.\n");
#endif
    return addXAxisAtom(parent_->atom(atomIndex));
}

// Remove x-axis atom
void SpeciesSite::removeXAxisAtom(SpeciesAtom *xAxisAtom)
{
    if (xAxisAtoms_.contains(xAxisAtom))
    {
        xAxisAtoms_.remove(xAxisAtom);

        ++version_;
    }
    else
        Messenger::error("Tried to remove a SpeciesAtom from the x-axis list that isn't present\n");
}

// Set x-axis atoms
bool SpeciesSite::setXAxisAtoms(const RefList<SpeciesAtom> atoms)
{
    xAxisAtoms_.clear();

    ++version_;

    for (SpeciesAtom *i : atoms)
        if (!addXAxisAtom(i))
        {
            xAxisAtoms_.clear();
            return false;
        }

    return true;
}

// Return list of x-axis atoms
const RefList<SpeciesAtom> &SpeciesSite::xAxisAtoms() { return xAxisAtoms_; }

// Return integer array of indices from which x-axis should be formed
Array<int> SpeciesSite::xAxisAtomIndices() const
{
    Array<int> indices;
    for (SpeciesAtom *atom : xAxisAtoms_)
        indices.add(atom->index());

    return indices;
}

// Add y-axis atom
bool SpeciesSite::addYAxisAtom(SpeciesAtom *yAxisAtom)
{
    if (!yAxisAtom)
        return Messenger::error("NULL SpeciesAtom passed to SpeciesSite::addYAxisAtom().\n");

    // If the SpeciesAtom already exists in the list, complain
    if (yAxisAtoms_.contains(yAxisAtom))
        return Messenger::error("Y-axis atom index {} specified twice for site '{}'.\n", yAxisAtom->index(), name_);

    yAxisAtoms_.append(yAxisAtom);

    ++version_;

    return true;
}

// Add y-axis atom from index
bool SpeciesSite::addYAxisAtom(int atomIndex)
{
#ifdef CHECKS
    if (!parent_)
        return Messenger::error("Tried to add a y-axis atom by index to a SpeciesSite, but no parent Species is set.\n");
#endif
    return addYAxisAtom(parent_->atom(atomIndex));
}

// Remove y-axis atom
void SpeciesSite::removeYAxisAtom(SpeciesAtom *yAxisAtom)
{
    if (yAxisAtoms_.contains(yAxisAtom))
    {
        yAxisAtoms_.remove(yAxisAtom);

        ++version_;
    }
    else
        Messenger::error("Tried to remove a SpeciesAtom from the y-axis list that isn't present\n");
}

// Set y-axis atoms
bool SpeciesSite::setYAxisAtoms(const RefList<SpeciesAtom> atoms)
{
    yAxisAtoms_.clear();

    ++version_;

    for (SpeciesAtom *i : atoms)
        if (!addYAxisAtom(i))
        {
            yAxisAtoms_.clear();
            return false;
        }

    return true;
}

// Return list of y-axis atoms
const RefList<SpeciesAtom> &SpeciesSite::yAxisAtoms() { return yAxisAtoms_; }

// Return integer array of indices from which y-axis should be formed
Array<int> SpeciesSite::yAxisAtomIndices() const
{
    Array<int> indices;
    for (SpeciesAtom *atom : yAxisAtoms_)
        indices.add(atom->index());

    return indices;
}

// Return whether the site has defined axes sites
bool SpeciesSite::hasAxes() const
{
    if ((xAxisAtoms_.nItems() == 0) || (yAxisAtoms_.nItems() == 0))
        return false;
    return true;
}

/*
 * Generation from Parent
 */

// Create and return Site description from parent Species
Site *SpeciesSite::createFromParent() const
{
    // Get origin atom indices from site
    Array<int> originIndices = originAtomIndices();
    if (originIndices.nItems() == 0)
        return nullptr;

    Site *site = nullptr;

    // Calculate origin
    Vec3<double> origin;
    double mass;
    if (originMassWeighted_)
    {
        double massNorm = 0.0;
        for (int m = 0; m < originIndices.nItems(); ++m)
        {
            mass = AtomicMass::mass(parent_->atom(originIndices[m])->element());
            origin += parent_->atom(originIndices[m])->r() * mass;
            massNorm += mass;
        }
        origin /= massNorm;
    }
    else
    {
        for (int m = 0; m < originIndices.nItems(); ++m)
            origin += parent_->atom(originIndices[m])->r();
        origin /= originIndices.nItems();
    }

    // Calculate axes and store data if required
    if (hasAxes())
    {
        // If the site has axes, grab the atom indices involved
        Array<int> xAxisIndices, yAxisIndices;
        if (hasAxes())
        {
            xAxisIndices = xAxisAtomIndices();
            yAxisIndices = yAxisAtomIndices();
        }

        Vec3<double> v;

        // Get average position of supplied x-axis atoms
        for (int m = 0; m < xAxisIndices.nItems(); ++m)
            v += parent_->atom(xAxisIndices[m])->r();
        v /= xAxisIndices.nItems();

        // Get vector from site origin and normalise it
        auto x = v - origin;
        x.normalise();

        // Get average position of supplied y-axis atoms
        v.zero();
        for (int m = 0; m < yAxisIndices.nItems(); ++m)
            v += parent_->atom(yAxisIndices[m])->r();
        v /= yAxisIndices.nItems();

        // Get vector from site origin, normalise it, and orthogonalise
        auto y = v - origin;
        y.orthogonalise(x);
        y.normalise();

        // Calculate z vector from cross product of x and y
        Vec3<double> z = x * y;

        // Store data
        site = new OrientedSite(nullptr, origin, x, y, z);
    }
    else
        site = new Site(nullptr, origin);

    return site;
}

/*
 * Read / Write
 */

// Return enum option info for SiteKeyword
EnumOptions<SpeciesSite::SiteKeyword> SpeciesSite::keywords()
{
    static EnumOptionsList SiteKeywords = EnumOptionsList()
                                          << EnumOption(SpeciesSite::EndSiteKeyword, "EndSite")
                                          << EnumOption(SpeciesSite::OriginKeyword, "Origin", EnumOption::OneOrMoreArguments)
                                          << EnumOption(SpeciesSite::OriginMassWeightedKeyword, "OriginMassWeighted", 1)
                                          << EnumOption(SpeciesSite::XAxisKeyword, "XAxis", EnumOption::OneOrMoreArguments)
                                          << EnumOption(SpeciesSite::YAxisKeyword, "YAxis", EnumOption::OneOrMoreArguments);

    static EnumOptions<SpeciesSite::SiteKeyword> options("SiteKeyword", SiteKeywords);

    return options;
}

// Read site definition from specified LineParser
bool SpeciesSite::read(LineParser &parser)
{
    Messenger::printVerbose("\nReading information for Site '{}'...\n", name());

    auto blockDone = false, error = false;

    while (!parser.eofOrBlank())
    {
        // Read in a line, which should contain a keyword and a minimum number of arguments
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!keywords().isValid(parser.argsv(0)))
            return keywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = keywords().enumeration(parser.argsv(0));
        if (!keywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            case (SpeciesSite::EndSiteKeyword):
                Messenger::print("Found end of Site '{}'.\n", name());
                blockDone = true;
                break;
            case (SpeciesSite::OriginKeyword):
                for (int n = 1; n < parser.nArgs(); ++n)
                {
                    if (!addOriginAtom(parser.argi(n) - 1))
                    {
                        Messenger::error("Failed to add origin atom for site '{}'.\n", name());
                        error = true;
                        break;
                    }
                }
                break;
            case (SpeciesSite::OriginMassWeightedKeyword):
                setOriginMassWeighted(parser.argb(1));
                break;
            case (SpeciesSite::XAxisKeyword):
                for (int n = 1; n < parser.nArgs(); ++n)
                {
                    if (!addXAxisAtom(parser.argi(n) - 1))
                    {
                        Messenger::error("Failed to add x-axis atom for site '{}'.\n", name());
                        error = true;
                        break;
                    }
                }
                break;
            case (SpeciesSite::YAxisKeyword):
                for (int n = 1; n < parser.nArgs(); ++n)
                {
                    if (!addYAxisAtom(parser.argi(n) - 1))
                    {
                        Messenger::error("Failed to add y-axis atom for site '{}'.\n", name());
                        error = true;
                        break;
                    }
                }
                break;
            default:
                Messenger::error("Site block keyword '{}' not accounted for.\n", keywords().keyword(kwd));
                error = true;
                break;
        }

        // Error encountered?
        if (error)
            break;

        // End of block?
        if (blockDone)
            break;
    }

    // If there's no error and the blockDone flag isn't set, return an error
    if (!error && !blockDone)
    {
        Messenger::error("Unterminated Site block found.\n");
        error = true;
    }

    return (!error);
}

// Write site definition to specified LineParser
bool SpeciesSite::write(LineParser &parser, std::string_view prefix)
{
    // Write start of site definition
    if (!parser.writeLineF("{}Site  '{}'\n", prefix, name()))
        return false;

    // Origin atom indices
    if (originAtoms_.nItems() > 0)
    {
        Array<int> indices = originAtomIndices();

        std::string atomIndices;
        for (int n = 0; n < indices.nItems(); ++n)
            atomIndices += fmt::format("  {}", indices[n] + 1);

        if (!parser.writeLineF("{}  {}{}\n", prefix, keywords().keyword(OriginKeyword), atomIndices))
            return false;
    }

    // Origin mass weighted?
    if (originMassWeighted_ && (!parser.writeLineF("{}  {}  True\n", prefix, keywords().keyword(OriginMassWeightedKeyword))))
        return false;

    // X-Axis atom indices
    if (xAxisAtoms_.nItems() > 0)
    {
        Array<int> indices = xAxisAtomIndices();

        std::string atomIndices;
        for (int n = 0; n < indices.nItems(); ++n)
            atomIndices += fmt::format("  {}", indices[n] + 1);

        if (!parser.writeLineF("{}  {}{}\n", prefix, keywords().keyword(XAxisKeyword), atomIndices))
            return false;
    }

    // Y-Axis atom indices
    if (yAxisAtoms_.nItems() > 0)
    {
        Array<int> indices = yAxisAtomIndices();

        std::string atomIndices;
        for (int n = 0; n < indices.nItems(); ++n)
            atomIndices += fmt::format("  {}", indices[n] + 1);

        if (!parser.writeLineF("{}  {}{}\n", prefix, keywords().keyword(YAxisKeyword), atomIndices))
            return false;
    }

    // Write start of site definition
    if (!parser.writeLineF("{}{}\n", prefix, keywords().keyword(EndSiteKeyword)))
        return false;

    return true;
}
