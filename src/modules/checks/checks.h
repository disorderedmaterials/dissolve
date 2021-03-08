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
    ~ChecksModule() override = default;

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const override;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const override;
    // Return category for module
    std::string_view category() const override;
    // Return brief description of module
    std::string_view brief() const override;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const override;

    /*
     * Initialisation
     */
    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * Checks
     */
    private:
    // Distances between atoms
    List<Geometry> distances_;
    // Angles between atoms
    List<Geometry> angles_;
};
