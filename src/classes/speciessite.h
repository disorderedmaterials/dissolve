// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "base/version.h"
#include "templates/listitem.h"
#include "templates/reflist.h"
#include "templates/vector3.h"

// Forward Declarations
class LineParser;
class Site;
class Species;
class SpeciesAtom;

// Species Site Definition
class SpeciesSite
{
    public:
    explicit SpeciesSite(const Species *parent);
    ~SpeciesSite() = default;

    /*
     * Basic Information
     */
    private:
    // Name of site
    std::string name_;
    // Parent Species
    const Species *parent_;
    // Version of the SpeciesSite
    VersionCounter version_;

    public:
    // Set name of site
    void setName(std::string_view newName);
    // Return anme of site
    std::string_view name() const;
    // Return species parent
    const Species *parent() const;
    // Return version
    int version() const;

    /*
     * Definition
     */
    private:
    // Species atoms whose average position is the origin of the site
    std::vector<const SpeciesAtom *> originAtoms_;
    // Whether the origin should be calculated with mass-weighted positions
    bool originMassWeighted_;
    // Species atom(s) that indicate the x axis with the origin
    std::vector<const SpeciesAtom *> xAxisAtoms_;
    // Species atom(s) that indicate the y axis with the origin, after orthogonalisation
    std::vector<const SpeciesAtom *> yAxisAtoms_;

    public:
    // Add origin atom
    bool addOriginAtom(const SpeciesAtom *originAtom);
    // Add origin atom from index
    bool addOriginAtom(int atomIndex);
    // Set origin atoms
    bool setOriginAtoms(const std::vector<const SpeciesAtom *> &atoms);
    // Return integer array of indices from which the origin should be formed
    std::vector<int> originAtomIndices() const;
    // Set whether the origin should be calculated with mass-weighted positions
    void setOriginMassWeighted(bool b);
    // Return whether the origin should be calculated with mass-weighted positions
    bool originMassWeighted() const;
    // Add x-axis atom
    bool addXAxisAtom(const SpeciesAtom *xAxisAtom);
    // Add x-axis atom from index
    bool addXAxisAtom(int atomIndex);
    // Set x-axis atoms
    bool setXAxisAtoms(const std::vector<const SpeciesAtom *> &atoms);
    // Return integer array of indices from which x-axis should be formed
    std::vector<int> xAxisAtomIndices() const;
    // Add y-axis atom
    bool addYAxisAtom(const SpeciesAtom *yAxisAtom);
    // Add y-axis atom from index
    bool addYAxisAtom(int atomIndex);
    // Set y-axis atoms
    bool setYAxisAtoms(const std::vector<const SpeciesAtom *> &atoms);
    // Return integer array of indices from which y-axis should be formed
    std::vector<int> yAxisAtomIndices() const;
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
        YAxisKeyword  /* 'YAxis' - Define one or more atoms whose average coordinates reflect the direction of the y
                 axis */
    };
    // Return enum option info for SiteKeyword
    static EnumOptions<SpeciesSite::SiteKeyword> keywords();
    // Read site definition from specified LineParser
    bool read(LineParser &parser);
    // Write site definition to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);
};
