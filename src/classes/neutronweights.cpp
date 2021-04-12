// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/neutronweights.h"
#include "base/lineparser.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/isotopes.h"
#include "genericitems/deserialisers.h"
#include "genericitems/serialisers.h"
#include "templates/algorithms.h"

NeutronWeights::NeutronWeights()
{
    boundCoherentSquareOfAverage_ = 0.0;
    boundCoherentAverageOfSquares_ = 0.0;
    valid_ = false;
}

NeutronWeights::NeutronWeights(const NeutronWeights &source) { (*this) = source; }

void NeutronWeights::operator=(const NeutronWeights &source)
{
    // Isotopologue Mix
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
void NeutronWeights::clear()
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
void NeutronWeights::addIsotopologue(const Species *sp, int speciesPopulation, const Isotopologue *iso,
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
bool NeutronWeights::containsIsotopologues(const Species *sp) const
{
    return std::any_of(isotopologueMixtures_.cbegin(), isotopologueMixtures_.cend(),
                       [sp](const Isotopologues &mix) { return mix.species() == sp; });
}

// Print atomtype / weights information
void NeutronWeights::print() const
{
    Messenger::print("  Species          Isotopologue     nTotMols    Fraction\n");
    Messenger::print("  ------------------------------------------------------\n");
    for (auto &topes : isotopologueMixtures_)
    {
        for (auto it = topes.mix().begin(); it != topes.mix().end(); ++it)
        {
            if (it == topes.mix().begin())
                Messenger::print("  {:<15}  {:<15}  {:<10d}  {}\n", topes.species()->name(), it->isotopologue()->name(),
                                 topes.speciesPopulation(), it->weight());
            else
                Messenger::print("                   {:<15}              {}\n", it->isotopologue()->name(), it->weight());
        }
    }

    // Print atomtypes table
    Messenger::print("\n");
    atomTypes_.print();

    Messenger::print("\nCalculated average scattering lengths: <b>**2 = {:.5f}, <b**2> = {:.5f}\n",
                     boundCoherentSquareOfAverage_, boundCoherentAverageOfSquares_);
}

/*
 * Data
 */

// Calculate weighting matrices based on current AtomType / Isotope information
void NeutronWeights::calculateWeightingMatrices()
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
    for_each_pair(atomTypes_.begin(), atomTypes_.end(),
                  [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2) {
                      ci = atd1.fraction();
                      bi = atd1.boundCoherent() * 0.1;

                      // Update average scattering values
                      if (typeI == typeJ)
                      {
                          boundCoherentSquareOfAverage_ += ci * bi;
                          boundCoherentAverageOfSquares_ += ci * bi * bi;
                      }

                      cj = atd2.fraction();
                      bj = atd2.boundCoherent() * 0.1;

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
    Array2D<char> intraFlag(atomTypes_.nItems(), atomTypes_.nItems(), true);
    Array2D<char> globalFlag(atomTypes_.nItems(), atomTypes_.nItems(), true);
    intraNorm = 0.0;
    globalFlag = false;
    for (auto &topes : isotopologueMixtures_)
    {
        // Get weighting for associated Species population
        auto speciesWeight = double(topes.speciesPopulation());

        // Using the underlying Species, construct a flag matrix which states the AtomType interactions we have present
        const Species *sp = topes.species();
        const AtomTypeList &speciesAtomTypes = sp->usedAtomTypes();
        intraFlag = false;
        for_each_pair(atomTypes_.begin(), atomTypes_.end(),
                      [&](int i_, const AtomTypeData &atd1, int j_, const AtomTypeData &atd2) {
                          // Find this AtomType in our local AtomTypeList
                          int typeI = atomTypes_.indexOf(atd1.atomType());
                          if (typeI == -1)
                              Messenger::error("Failed to find AtomType '{}' in local NeutronWeights.\n", atd1.atomTypeName());

                          // Get AtomType for this Atom and find it in our local AtomTypeList
                          int typeJ = atomTypes_.indexOf(atd2.atomType());
                          if (typeJ == -1)
                              Messenger::error("Failed to find AtomType '{}' in local NeutronWeights.\n", atd2.atomTypeName());

                          intraFlag[{typeI, typeJ}] = true;
                      });

        // Loop over Isotopologues defined for this mixture
        for (auto &isoWeight : topes.mix())
        {
            // Sum the scattering lengths of each pair of AtomTypes, weighted by the speciesWeight and the
            // fractional Isotopologue weight in the mix.
            double weight = speciesWeight * isoWeight.weight();

            const Isotopologue *tope = isoWeight.isotopologue();

            for (auto atd1 = speciesAtomTypes.begin(); atd1 != speciesAtomTypes.end(); ++atd1)
            {
                // Get the local index of this AtomType, as well as its pointer
                auto typeI = atomTypes_.indexOf(atd1->atomType());
                auto &localI = atomTypes_[typeI];

                // If this AtomType is exchangeable, add the averaged scattering length from the local
                // AtomTypesList instead of its actual isotopic length.
                if (localI.exchangeable())
                    bi = localI.boundCoherent();
                else
                {
                    // Get the Isotope associated to this AtomType in the current Isotopologue
                    Isotope *isotope = tope->atomTypeIsotope(atd1->atomType());
                    bi = isotope->boundCoherent();
                }
                bi *= 0.1;

                // Inner loop
                for (auto atd2 = atd1; atd2 != speciesAtomTypes.end(); ++atd2)
                {
                    // Get the local index of this AtomType, as well as its pointer
                    auto typeJ = atomTypes_.indexOf(atd2->atomType());
                    auto &localJ = atomTypes_[typeJ];

                    // Check to see if this interaction is present in the current Species
                    if (!intraFlag[{typeI, typeJ}])
                        continue;

                    // If this AtomType is exchangeable, add the averaged scattering length from the local
                    // AtomTypesList instead of its actual isotopic length.
                    if (localJ.exchangeable())
                        bj = localJ.boundCoherent();
                    else
                    {
                        // Get the Isotope associated to this AtomType in the current Isotopologue
                        Isotope *isotope = tope->atomTypeIsotope(atd2->atomType());
                        bj = isotope->boundCoherent();
                    }
                    bj *= 0.1;

                    intramolecularWeights_[{typeI, typeJ}] += weight * bi * bj;
                    intraNorm[{typeI, typeJ}] += weight;
                    globalFlag[{typeI, typeJ}] = true;
                }
            }
        }
    }

    // Normalise the boundWeights_ array, and multiply by atomic concentrations and Kronecker delta
    for_each_pair(atomTypes_.begin(), atomTypes_.end(),
                  [&](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2) {
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
void NeutronWeights::createFromIsotopologues(const AtomTypeList &exchangeableTypes)
{
    // Loop over Isotopologues entries and ensure relative populations of Isotopologues sum to 1.0
    for (auto &topes : isotopologueMixtures_)
        topes.normalise();

    // Fill atomTypes_ list with AtomType populations, based on Isotopologues relative populations and associated Species
    // populations
    atomTypes_.clear();
    for (auto &topes : isotopologueMixtures_)
    {
        // We must now loop over the Isotopologues in the topesture
        for (auto isoWeight : topes.mix())
        {
            const Isotopologue *tope = isoWeight.isotopologue();

            // Loop over Atoms in the Species, searching for the AtomType/Isotope entry in the isotopes list of the
            // Isotopologue
            for (const auto &i : topes.species()->atoms())
            {
                Isotope *iso = tope->atomTypeIsotope(i.atomType());
                atomTypes_.addIsotope(i.atomType(), iso, isoWeight.weight() * topes.speciesPopulation());
            }
        }
    }
    atomTypes_.finalise(exchangeableTypes);

    calculateWeightingMatrices();

    valid_ = true;
}

// Reduce data to be naturally-weighted
void NeutronWeights::naturalise()
{
    atomTypes_.naturalise();

    calculateWeightingMatrices();

    valid_ = true;
}

// Return AtomTypeList
const AtomTypeList &NeutronWeights::atomTypes() const { return atomTypes_; }

// Return number of used AtomTypes
int NeutronWeights::nUsedTypes() const { return atomTypes_.nItems(); }

// Return concentration product for types i and j
double NeutronWeights::concentrationProduct(int i, int j) const { return concentrationProducts_[{i, j}]; }

// Return bound coherent scattering product for types i
double NeutronWeights::boundCoherentProduct(int i, int j) const { return boundCoherentProducts_[{i, j}]; }

// Return full weighting for types i and j (ci * cj * bi * bj * [2-dij])
double NeutronWeights::weight(int i, int j) const { return weights_[{i, j}]; }

// Return full intramolecular weighting for types i and j
double NeutronWeights::intramolecularWeight(int i, int j) const { return intramolecularWeights_[{i, j}]; }

// Return full weights matrix
const Array2D<double> &NeutronWeights::weights() const { return weights_; }

// Return full intramolecular scattering weights matrix
const Array2D<double> &NeutronWeights::intramolecularWeights() const { return intramolecularWeights_; }

// Return bound coherent average squared scattering (<b>**2)
double NeutronWeights::boundCoherentSquareOfAverage() const { return boundCoherentSquareOfAverage_; }

// Return bound coherent squared average scattering (<b**2>)
double NeutronWeights::boundCoherentAverageOfSquares() const { return boundCoherentAverageOfSquares_; }

// Return whether the structure is valid (i.e. has been finalised)
bool NeutronWeights::isValid() const { return valid_; }

/*
 * Serialisation
 */

// Read data through specified LineParser
bool NeutronWeights::deserialise(LineParser &parser, const CoreData &coreData)
{
    clear();

    // Read AtomTypeList
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
bool NeutronWeights::serialise(LineParser &parser) const
{
    // Write AtomTypeList
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
