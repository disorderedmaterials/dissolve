// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "base/version.h"
#include "classes/atomType.h"
#include "classes/speciesSiteInstance.h"
#include "data/elements.h"
#include "neta/neta.h"
#include "templates/vector3.h"
#include <map>
#include <vector>

// Forward Declarations
class LineParser;
class Site;
class Species;
class SpeciesAtom;

// Species Site Definition
class SpeciesSite : public Serialisable<CoreData &>
{
    public:
    // Site Type
    enum SiteType
    {
        Static,  /* Site is based on fixed atom indices within the species */
        Dynamic, /* Site is atomic and based on elements and atom types */
        Fragment /* Site is based on a NETA description */
    };
    // EnumOptions for SiteType
    static EnumOptions<SiteType> siteTypes();

    explicit SpeciesSite(const Species *parent, SiteType type = SiteType::Static);
    SpeciesSite(const Species *parent, std::string name, SiteType type = SiteType::Static);
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
     * Basic Definition
     */
    private:
    // Type of site
    SiteType type_;
    // Whether the origin should be calculated with mass-weighted positions
    bool originMassWeighted_;

    public:
    // Clear definition data from site
    void clearDefinition();
    // Set type of site
    void setType(SiteType type);
    // Return type of site
    SiteType type() const;
    // Return whether the site has defined axes
    bool hasAxes() const;
    // Set whether the origin should be calculated with mass-weighted positions
    void setOriginMassWeighted(bool b);
    // Return whether the origin should be calculated with mass-weighted positions
    bool originMassWeighted() const;

    /*
     * Static Site Definition
     */
    private:
    // Species atoms whose average position is the origin of the site
    std::vector<const SpeciesAtom *> staticOriginAtoms_;
    // Species atom(s) that indicate the x axis with the origin
    std::vector<const SpeciesAtom *> staticXAxisAtoms_;
    // Species atom(s) that indicate the y axis with the origin, after orthogonalisation
    std::vector<const SpeciesAtom *> staticYAxisAtoms_;

    public:
    // Add origin atom
    bool addStaticOriginAtom(const SpeciesAtom *originAtom);
    // Add origin atom from index
    bool addStaticOriginAtom(int atomIndex);
    // Set origin atoms
    bool setStaticOriginAtoms(const std::vector<const SpeciesAtom *> &atoms);
    // Return origin atom vector
    const std::vector<const SpeciesAtom *> &staticOriginAtoms() const;
    // Return integer array of indices from which the origin should be formed
    std::vector<int> staticOriginAtomIndices() const;
    // Add x-axis atom
    bool addStaticXAxisAtom(const SpeciesAtom *xAxisAtom);
    // Add x-axis atom from index
    bool addStaticXAxisAtom(int atomIndex);
    // Set x-axis atoms
    bool setStaticXAxisAtoms(const std::vector<const SpeciesAtom *> &atoms);
    // Return x-axis atom vector
    const std::vector<const SpeciesAtom *> &staticXAxisAtoms() const;
    // Return integer array of indices from which x-axis should be formed
    std::vector<int> staticXAxisAtomIndices() const;
    // Add y-axis atom
    bool addStaticYAxisAtom(const SpeciesAtom *yAxisAtom);
    // Add y-axis atom from index
    bool addStaticYAxisAtom(int atomIndex);
    // Set y-axis atoms
    bool setStaticYAxisAtoms(const std::vector<const SpeciesAtom *> &atoms);
    // Return y-axis atom vector
    const std::vector<const SpeciesAtom *> &staticYAxisAtoms() const;
    // Return integer array of indices from which y-axis should be formed
    std::vector<int> staticYAxisAtomIndices() const;

    /*
     * Dynamic Site Definition
     */
    private:
    // Target elements for selection as sites
    std::vector<Elements::Element> dynamicElements_;
    // Target atom types for selection as sites
    std::vector<std::shared_ptr<AtomType>> dynamicAtomTypes_;

    public:
    // Add target elements for selection as sites
    bool addDynamicElement(Elements::Element el);
    // Set target elements for selection as sites
    bool setDynamicElements(const std::vector<Elements::Element> &els);
    // Return elements for selection as sites
    const std::vector<Elements::Element> dynamicElements() const;
    // Add target atom type for selection as sites
    bool addDynamicAtomType(const std::shared_ptr<AtomType> &at);
    // Set target atom types for selection as sites
    bool setDynamicAtomTypes(const std::vector<std::shared_ptr<AtomType>> &types);
    // Return atom types for selection as sites
    const std::vector<std::shared_ptr<AtomType>> &dynamicAtomTypes() const;

    /*
     * Fragment Site Definition
     */
    private:
    NETADefinition fragment_;

    public:
    const NETADefinition &fragment() const;
    bool setFragmentDefinitionString(std::string_view definitionString);

    /*
     * Generation
     */
    private:
    // Instances for this site within the Species
    std::vector<SpeciesSiteInstance> instances_;

    private:
    // Calculate geometric centre of atoms in the parent Species
    Vec3<double> centreOfGeometry(std::vector<int> &indices) const;
    // Calculate (mass-weighted) coordinate centre of atoms in the parent Species
    Vec3<double> centreOfMass(std::vector<int> &indices) const;

    public:
    // Create and return Site description from parent Species
    std::vector<std::shared_ptr<Site>> createFromParent() const;
    // Generate unique sites
    bool generateUniqueSites();
    // Return site instances
    const std::vector<SpeciesSiteInstance> &instances() const;

    /*
     * Read / Write
     */
    public:
    // Site Block Keyword Enum
    enum SiteKeyword
    {
        AtomTypeKeyword,           /* 'AtomType' - Specify allowed atom type(s) for dynamic sites */
        DynamicKeyword,            /* 'Dynamic' - States that this is a dynamic site */
        ElementKeyword,            /* 'Element' - Specify allowed element(s) for dynamic sites */
        FragmentKeyword,           /* 'Fragment' - States that this is a fragment site */
        DescriptionKeyword,        /* 'Description' - Defines the NETA description for fragment sites */
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
    bool read(LineParser &parser, const CoreData &coreData);
    // Write site definition to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);

    SerialisedValue serialise() const override;
    void deserialise(const SerialisedValue &node, CoreData &coreData) override;
};
