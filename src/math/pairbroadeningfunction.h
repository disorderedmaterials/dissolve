// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "math/broadeningfunction.h"
#include "templates/array.h"
#include "templates/array2d.h"
#include <memory>

// Forward Declarations
class AtomType;
class LineParser;
class SpeciesIntra;

// Pair Broadening Function
class PairBroadeningFunction
{
    public:
    // Function Types
    enum FunctionType
    {
        NoFunction,
        GaussianFunction,
        GaussianElementPairFunction,
        nFunctionTypes
    };
    // Return FunctionType from supplied string
    static FunctionType functionType(std::string_view s);
    // Return FunctionType name
    static std::string_view functionType(FunctionType func);
    // Return number of parameters needed to define FunctionType
    static int nFunctionParameters(FunctionType func);

    public:
    PairBroadeningFunction(FunctionType function = NoFunction);
    ~PairBroadeningFunction();
    PairBroadeningFunction(const PairBroadeningFunction &source);
    void operator=(const PairBroadeningFunction &source);

    /*
     * Function Data
     */
    private:
    // Function Type
    FunctionType function_;
    // Gaussian FWHM parameter
    double gaussianFWHM_;
    // Elemental pair Gaussian FWHM parameters
    Array2D<double> elementPairGaussianFWHM_;
    // Elemental pair flags (whether a valid value exists)
    Array2D<char> elementPairGaussianFlags_;

    public:
    // Read function data from LineParser source
    bool readAsKeyword(LineParser &parser, int startArg, const CoreData &coreData);
    // Write function data to LineParser source
    bool writeAsKeyword(LineParser &parser, std::string_view prefix, bool writeBlockMarker = true) const;
    // Set function type
    void setFunction(FunctionType function);
    // Return function type
    FunctionType function() const;
    // Set Gaussian FWHM parameters
    void setGaussianFWHM(double fwhm);
    // Return Gaussian FWHM parameter
    double gaussianFWHM() const;
    // Return array of pointers to all adjustable parameters
    std::vector<double *> parameters();
    // Return short summary of function and its parameters
    std::string summary() const;
    // Return a BroadeningFunction tailored to the specified AtomType pair
    BroadeningFunction broadeningFunction(std::shared_ptr<AtomType> at1, std::shared_ptr<AtomType> at2,
                                          SpeciesIntra *intra = nullptr);

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser);
    // Write data through specified LineParser
    bool write(LineParser &parser);
};
