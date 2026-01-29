// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/cgNeutronWeights.h"
#include "base/lineParser.h"
#include "classes/atomType.h"
#include "classes/species.h"
#include "data/isotopes.h"
#include "items/deserialisers.h"
#include "items/serialisers.h"
#include "templates/algorithms.h"

CGNeutronWeights::CGNeutronWeights()
{
    boundCoherentSquareOfAverage_ = 0.0;
    boundCoherentAverageOfSquares_ = 0.0;
    valid_ = false;
}

CGNeutronWeights::CGNeutronWeights(const CGNeutronWeights &source) { (*this) = source; }

void CGNeutronWeights::operator=(const CGNeutronWeights &source)
{
    isotopologueMixtures_ = source.isotopologueMixtures_;
    atomTypes_ = source.atomTypes_;
    boundCoherentProducts_ = source.boundCoherentProducts_;
    concentrationProducts_ = source.concentrationProducts_;
    weights_ = source.weights_;
    intramolecularWeights_ = source.intramolecularWeights_;
    boundCoherentSquareOfAverage_ = source.boundCoherentSquareOfAverage_;
    boundCoherentAverageOfSquares_ = source.boundCoherentAverageOfSquares_;
    valid_ = source.valid_;
}

/*
 * Construction
 */

// Clear contents
void CGNeutronWeights::clear()
{
    isotopologueMixtures_.clear();
    atomTypes_.clear();
    concentrationProducts_.clear();
    boundCoherentProducts_.clear();
    weights_.clear();
    intramolecularWeights_.clear();
    boundCoherentSquareOfAverage_ = 0.0;
    boundCoherentAverageOfSquares_ = 0.0;
    valid_ = false;
}

// Add Isotopologue for Species
void CGNeutronWeights::addIsotopologue(const Species *sp, double speciesPopulation, const Isotopologue *iso,
                                     double isotopologueRelativePopulation)
{
    // Does an Isotopologues definition already exist for the supplied Species?
    auto it = std::find_if(isotopologueMixtures_.begin(), isotopologueMixtures_.end(),
                           [sp](auto &data) { return data.species() == sp; });

    if (it == isotopologueMixtures_.end())
    {
        isotopologueMixtures_.emplace_back(sp, speciesPopulation);
        isotopologueMixtures_.back().add(iso, isotopologueRelativePopulation);
    }
    else
        it->add(iso, isotopologueRelativePopulation);
}

// Return whether an Isotopologues definition exists for the provided Species
bool CGNeutronWeights::containsIsotopologues(const Species *sp) const
{
    return std::any_of(isotopologueMixtures_.cbegin(), isotopologueMixtures_.cend(),
                       [sp](const Isotopologues &mix) { return mix.species() == sp; });
}

// Print atomtype / weights information
void CGNeutronWeights::print() const
{
    Messenger::print("  Species          Isotopologue     nTotMols    Fraction\n");
    Messenger::print("  ------------------------------------------------------\n");
    for (auto &topes : isotopologueMixtures_)
    {
        for (auto it = topes.mix().begin(); it != topes.mix().end(); ++it)
        {
            if (it == topes.mix().begin())
                Messenger::print("  {:<15}  {:<15}  {:<10g}  {}\n", topes.species()->name(), it->isotopologue()->name(),
                                 topes.speciesPopulation(), it->weight());
            else
                Messenger::print("                   {:<15}              {}\n", it->isotopologue()->name(), it->weight());
        }
    }

    // Print atomtypes table
    Messenger::print("\n\n");
    atomTypes_.print();
    Messenger::print("\n");
    beadMap_.print();

    Messenger::print("\nCalculated average scattering lengths: <b>**2 = {:.5f}, <b**2> = {:.5f}\n",
                     boundCoherentSquareOfAverage_, boundCoherentAverageOfSquares_);
}

/*
 * Data
 */

// Calculate weighting matrices based on current AtomType / Isotope information
void CGNeutronWeights::calculateWeightingMatrices()
{
    // Create weights matrices and calculate average scattering lengths
    // Note: Multiplier of 0.1 on b terms converts from units of fm (1e-11 m) to barn (1e-12 m)
    concentrationProducts_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
    boundCoherentProducts_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
    weights_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
    intramolecularWeights_.initialise(atomTypes_.nItems(), atomTypes_.nItems(), true);
    boundCoherentAverageOfSquares_ = 0.0;
    boundCoherentSquareOfAverage_ = 0.0;

    double ci, cj, bi, bj;

    // Determine atomic concentration products, bound coherent products, and full scattering weights
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes_.begin(), atomTypes_.end(),
                            [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2)
                            {
                                ci = atd1.fraction();
                                // bi = atd1.boundCoherent() * 0.1;
                                bi = beadMap_[typeI].scatteringLength();

                                // Update average scattering values
                                if (typeI == typeJ)
                                {
                                    boundCoherentSquareOfAverage_ += ci * bi;
                                    boundCoherentAverageOfSquares_ += ci * bi * bi;
                                }

                                cj = atd2.fraction();
                                // bj = atd2.boundCoherent() * 0.1;
                                bj = beadMap_[typeJ].scatteringLength();

                                concentrationProducts_[{typeI, typeJ}] = ci * cj;
                                boundCoherentProducts_[{typeI, typeJ}] = bi * bj;
                                weights_[{typeI, typeJ}] = ci * cj * bi * bj * (typeI == typeJ ? 1 : 2);
                            });

    // Finalise <b>**2
    boundCoherentSquareOfAverage_ *= boundCoherentSquareOfAverage_;

    // Determine bound (intramolecular) scattering weights
    // Loop over defined Isotopologues in our defining mixtures, summing terms from (intramolecular) pairs of Atoms
    intramolecularWeights_ = 0.0;
    Array2D<double> intraNorm(atomTypes_.nItems(), atomTypes_.nItems(), true);
    Array2D<char> globalFlag(atomTypes_.nItems(), atomTypes_.nItems(), true);
    intraNorm = 0.0;
    globalFlag = false;
    for (auto &topes : isotopologueMixtures_)
    {
        // Get weighting for associated Species population
        auto speciesWeight = double(topes.speciesPopulation());

        // Using the underlying Species, construct a flag matrix which states the AtomType interactions we have present
        const auto *sp = topes.species();
        const auto &speciesAtomTypes = sp->atomTypes();

        // Loop over Isotopologues defined for this mixture
        for (auto &isoWeight : topes.mix())
        {
            // Sum the scattering lengths of each pair of AtomTypes, weighted by the speciesWeight and the
            // fractional Isotopologue weight in the mix.
            auto weight = speciesWeight * isoWeight.weight();
            const auto *tope = isoWeight.isotopologue();

            dissolve::for_each_pair(ParallelPolicies::seq, speciesAtomTypes.begin(), speciesAtomTypes.end(),
                                    [&](int spTypeI, const AtomTypeData &atd1, int spTypeJ, const AtomTypeData &atd2)
                                    {
                                        // First, check that both of atom types used in the species are present in the weights
                                        // atomTypes_. They may legitimately not be if, for example, they are phantom atoms.
                                        auto optPairIndex = atomTypes_.indexOf(atd1.atomType(), atd2.atomType());
                                        if (!optPairIndex)
                                            return;
                                        auto &[typeI, typeJ] = *optPairIndex;

                                        auto &localI = atomTypes_[typeI];
                                        auto &localJ = atomTypes_[typeJ];

                                        // If an AtomType is exchangeable, add the averaged scattering length from the local
                                        // AtomTypesList instead of its actual isotopic length.
                                        if (localI.exchangeable()) 
                                        {
                                            bi = beadMap_[typeI].scatteringLength();
                                        }
                                        else 
                                        {

                                            bi = beadMap_[typeI].scatteringLength(
                                                tope->atomTypeIsotope(localI.atomType()) != Sears91::naturalIsotope(localI.atomType()->Z())
                                            );
                                        }
                                        
                                        if (localJ.exchangeable()) 
                                        {
                                            bj = beadMap_[typeJ].scatteringLength();
                                        }
                                        else 
                                        {
                                            bj = beadMap_[typeJ].scatteringLength(
                                                tope->atomTypeIsotope(localJ.atomType()) != Sears91::naturalIsotope(localJ.atomType()->Z())
                                            );
                                        }

                                        // Convert from fm to barns
                                        // bi *= 0.1;
                                        // bj *= 0.1;

                                        intramolecularWeights_[{typeI, typeJ}] += weight * bi * bj;
                                        intraNorm[{typeI, typeJ}] += weight;
                                        globalFlag[{typeI, typeJ}] = true;
                                    });
        }
    }

    // Normalise the boundWeights_ array, and multiply by atomic concentrations and Kronecker delta
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes_.begin(), atomTypes_.end(),
                            [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2)
                            {
                                // Skip this pair if there are no such intramolecular interactions
                                if (!globalFlag[{typeI, typeJ}])
                                    return;

                                ci = atd1.fraction();
                                cj = atd2.fraction();

                                intramolecularWeights_[{typeI, typeJ}] /= intraNorm[{typeI, typeJ}];
                                intramolecularWeights_[{typeI, typeJ}] *= ci * cj * (typeI == typeJ ? 1 : 2);
                            });
}

// Create AtomType list and matrices based on stored Isotopologues information
void CGNeutronWeights::createFromIsotopologues(const std::vector<std::shared_ptr<AtomType>> &exchangeableTypes)
{
    // Loop over Isotopologues entries and ensure relative populations of Isotopologues sum to 1.0
    for (auto &topes : isotopologueMixtures_)
        topes.normalise();

    double dFraction = 0.0;

    // Fill atomTypes_ list with AtomType populations, based on Isotopologues relative populations and associated Species
    // populations
    atomTypes_.clear();
    for (auto &topes : isotopologueMixtures_)
    {
        // Loop over the Isotopologues in the mixture
        for (const auto &isoWeight : topes.mix())
        {
            const auto *top = isoWeight.isotopologue();

            // Loop over Atoms in the Species, searching for the AtomType/Isotope entry in the isotopes list of the
            // Isotopologue
            for (const auto &i : topes.species()->atoms())
            {
                if (i.isPresence(SpeciesAtom::Presence::Physical))
                {
                        atomTypes_.addIsotope(i.atomType(), top->atomTypeIsotope(i.atomType()),
                                          isoWeight.weight() * topes.speciesPopulation());
                }
                
                if (i.Z() == Elements::H)
                {
                    if (top->atomTypeIsotope(i.atomType()) != Sears91::H_Natural)
                    {
                        dFraction = isoWeight.weight();
                    }
                }
            }
        }
    }
    atomTypes_.finalise(exchangeableTypes);

    beadMap_.initialiseFromFile(dFraction);

    calculateWeightingMatrices();

    valid_ = true;
}

// Reduce data to be naturally-weighted
void CGNeutronWeights::naturalise()
{
    atomTypes_.naturalise();

    calculateWeightingMatrices();

    valid_ = true;
}

// Return AtomTypeMix
const AtomTypeMix &CGNeutronWeights::atomTypes() const { return atomTypes_; }

// Return number of used AtomTypes
int CGNeutronWeights::nUsedTypes() const { return atomTypes_.nItems(); }

// Return concentration product for types i and j
double CGNeutronWeights::concentrationProduct(int i, int j) const { return concentrationProducts_[{i, j}]; }

// Return bound coherent scattering product for types i
double CGNeutronWeights::boundCoherentProduct(int i, int j) const { return boundCoherentProducts_[{i, j}]; }

// Return full weighting for types i and j (ci * cj * bi * bj * [2-dij])
double CGNeutronWeights::weight(int i, int j) const { return weights_[{i, j}]; }

// Return full intramolecular weighting for types i and j
double CGNeutronWeights::intramolecularWeight(int i, int j) const { return intramolecularWeights_[{i, j}]; }

// Return full weights matrix
const Array2D<double> &CGNeutronWeights::weights() const { return weights_; }

// Return full intramolecular scattering weights matrix
const Array2D<double> &CGNeutronWeights::intramolecularWeights() const { return intramolecularWeights_; }

// Return bound coherent average squared scattering (<b>**2)
double CGNeutronWeights::boundCoherentSquareOfAverage() const { return boundCoherentSquareOfAverage_; }

// Return bound coherent squared average scattering (<b**2>)
double CGNeutronWeights::boundCoherentAverageOfSquares() const { return boundCoherentAverageOfSquares_; }

// Return whether the structure is valid (i.e. has been finalised)
bool CGNeutronWeights::isValid() const { return valid_; }

/*
 * Serialisation
 */

// Read data through specified LineParser
bool CGNeutronWeights::deserialise(LineParser &parser, const CoreData &coreData)
{
    clear();

    // Read AtomTypeMix
    if (!atomTypes_.deserialise(parser, coreData))
        return false;

    // Read isotopologue mixtures
    isotopologueMixtures_.clear();
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nItems = parser.argi(0);
    for (auto n = 0; n < nItems; ++n)
    {
        isotopologueMixtures_.emplace_back();
        if (!isotopologueMixtures_.back().deserialise(parser, coreData))
            return false;
    }

    // Read arrays using static methods in the relevant GenericItemContainer
    if (!GenericItemDeserialiser::deserialise<Array2D<double>>(concentrationProducts_, parser))
        return false;
    if (!GenericItemDeserialiser::deserialise<Array2D<double>>(boundCoherentProducts_, parser))
        return false;
    if (!GenericItemDeserialiser::deserialise<Array2D<double>>(weights_, parser))
        return false;
    if (!GenericItemDeserialiser::deserialise<Array2D<double>>(intramolecularWeights_, parser))
        return false;

    // Read averages
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    boundCoherentAverageOfSquares_ = parser.argd(0);
    boundCoherentSquareOfAverage_ = parser.argd(1);

    return true;
}

// Write data through specified LineParser
bool CGNeutronWeights::serialise(LineParser &parser) const
{
    // Write AtomTypeMix
    if (!atomTypes_.serialise(parser))
        return false;

    // Write isotopologue mixtures
    if (!parser.writeLineF("{}  # nItems\n", isotopologueMixtures_.size()))
        return false;
    for (auto &topes : isotopologueMixtures_)
        if (!topes.serialise(parser))
            return false;

    // Write arrays using static methods in the relevant GenericItemContainer
    if (!GenericItemSerialiser::serialise<Array2D<double>>(concentrationProducts_, parser))
        return false;
    if (!GenericItemSerialiser::serialise<Array2D<double>>(boundCoherentProducts_, parser))
        return false;
    if (!GenericItemSerialiser::serialise<Array2D<double>>(weights_, parser))
        return false;
    if (!GenericItemSerialiser::serialise<Array2D<double>>(intramolecularWeights_, parser))
        return false;

    // Write averages
    if (!parser.writeLineF("{} {}\n", boundCoherentAverageOfSquares_, boundCoherentSquareOfAverage_))
        return false;

    return true;
}

void CGNeutronWeights::set_atom_types(const AtomTypeMix& atomTypes)
{
    atomTypes_ = atomTypes;
}