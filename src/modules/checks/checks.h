// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/geometry.h"
#include "module/module.h"

// Forward Declarations
/* none */

// Checks Module
class ChecksModule : public Module
{
    /*
     * Module for making simple checkss
     */

    public:
    ChecksModule();
    ~ChecksModule() = default;

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const;
    // Return category for module
    std::string_view category() const;
    // Return brief description of module
    std::string_view brief() const;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const;

    /*
     * Initialisation
     */
    protected:
    // Perform any necessary initialisation for the Module
    void initialise();

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool);

    /*
     * Checks
     */
    private:
    // Distances between atoms
    List<Geometry> distances_;
    // Angles between atoms
    List<Geometry> angles_;
};
