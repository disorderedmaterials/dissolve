/*
    *** Species Site
    *** src/classes/speciessite.h
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

#include "base/enumoptions.h"
#include "base/version.h"
#include "templates/array.h"
#include "templates/listitem.h"
#include "templates/reflist.h"
#include "templates/vector3.h"

// Forward Declarations
class LineParser;
class Site;
class Species;
class SpeciesAtom;

// Species Site Definition
class SpeciesSite : public ListItem<SpeciesSite>
{
    public:
    SpeciesSite();
    ~SpeciesSite();

    /*
     * Basic Information
     */
    private:
    // Name of site
    std::string name_;
    // Parent Species
    Species *parent_;
    // Version of the SpeciesSite
    VersionCounter version_;

    public:
    // Set name of site
    void setName(std::string_view newName);
    // Return anme of site
    std::string_view name() const;
    // Set Species parent
    void setParent(Species *sp);
    // Return species parent
    Species *parent();
    // Return version
    int version() const;

    /*
     * Definition
     */
    private:
    // List of SpeciesAtoms whose average position is the origin of the site
    RefList<SpeciesAtom> originAtoms_;
    // Whether the origin should be calculated with mass-weighted positions
    bool originMassWeighted_;
    // SpeciesAtom(s) that indicate the x axis with the origin
    RefList<SpeciesAtom> xAxisAtoms_;
    // SpeciesAtom(s) that indicate the y axis with the origin, after orthogonalisation
    RefList<SpeciesAtom> yAxisAtoms_;

    public:
    // Add origin atom
    bool addOriginAtom(SpeciesAtom *originAtom);
    // Add origin atom from index
    bool addOriginAtom(int atomIndex);
    // Remove origin atom
    void removeOriginAtom(SpeciesAtom *originAtom);
    // Set origin atoms
    bool setOriginAtoms(const RefList<SpeciesAtom> atoms);
    // Return list of origin atoms
    const RefList<SpeciesAtom> &originAtoms();
    // Return integer array of indices from which the origin should be formed
    Array<int> originAtomIndices() const;
    // Set whether the origin should be calculated with mass-weighted positions
    void setOriginMassWeighted(bool b);
    // Return whether the origin should be calculated with mass-weighted positions
    bool originMassWeighted() const;
    // Add x-axis atom
    bool addXAxisAtom(SpeciesAtom *xAxisAtom);
    // Add x-axis atom from index
    bool addXAxisAtom(int atomIndex);
    // Remove x-axis atom
    void removeXAxisAtom(SpeciesAtom *xAxisAtom);
    // Set x-axis atoms
    bool setXAxisAtoms(const RefList<SpeciesAtom> atoms);
    // Return list of x-axis atoms
    const RefList<SpeciesAtom> &xAxisAtoms();
    // Return integer array of indices from which x-axis should be formed
    Array<int> xAxisAtomIndices() const;
    // Add y-axis atom
    bool addYAxisAtom(SpeciesAtom *yAxisAtom);
    // Add y-axis atom from indey
    bool addYAxisAtom(int atomIndex);
    // Remove y-axis atom
    void removeYAxisAtom(SpeciesAtom *yAxisAtom);
    // Set y-axis atoms
    bool setYAxisAtoms(const RefList<SpeciesAtom> atoms);
    // Return list of y-axis atoms
    const RefList<SpeciesAtom> &yAxisAtoms();
    // Return integer array of indices from which y-axis should be formed
    Array<int> yAxisAtomIndices() const;
    // Return whether the site has defined axes sites
    bool hasAxes() const;

    /*
     * Generation from Parent
     */
    public:
    // Create and return Site description from parent Species
    Site *createFromParent() const;

    /*
     * Read / Write
     */
    public:
    // Site Block Keyword Enum
    enum SiteKeyword
    {
        EndSiteKeyword,            /* 'EndSite' - Signals the end of the Site */
        OriginKeyword,             /* 'Origin' - Set the atom indices whose average coordinates reflect the site origin */
        OriginMassWeightedKeyword, /* 'OriginMassWeighted' - Control whether the origin should be calculated with
                          mass-weighted coordinates */
        XAxisKeyword, /* 'XAxis' - Define one or more atoms whose average coordinates reflect the direction of the x
                 axis */
        YAxisKeyword, /* 'YAxis' - Define one or more atoms whose average coordinates reflect the direction of the y
                 axis */
        nSiteKeywords /* Number of keywords defined for this block */
    };
    // Return enum option info for SiteKeyword
    static EnumOptions<SpeciesSite::SiteKeyword> keywords();
    // Read site definition from specified LineParser
    bool read(LineParser &parser);
    // Write site definition to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);
};
