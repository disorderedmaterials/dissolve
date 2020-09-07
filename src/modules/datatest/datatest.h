/*
    *** DataTest Module
    *** src/modules/datatest/datatest.h
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

#include "classes/configuration.h"
#include "classes/data1dstore.h"
#include "classes/data2dstore.h"
#include "classes/data3dstore.h"
#include "genericitems/listhelper.h"
#include "module/module.h"

// Forward Declarations
/* none */

// DataTest Module
class DataTestModule : public Module
{
    public:
    DataTestModule();
    ~DataTestModule();

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
     * Functions
     */
    private:
    // Target module containing / owning data to test
    RefList<Module> targetModule_;
    // Test 1D datasets
    Data1DStore test1DData_;
    // Test 2D datasets
    Data2DStore test2DData_;
    // Test 3D datasets
    Data3DStore test3DData_;

    private:
    // Find reference Data
    template <class T>
    const T &findReferenceData(std::string_view dataIdentifier, Module *targetModule, GenericList &processingModuleData,
                               bool &found)
    {
        static T dummy;

        found = false;

        // If a target module was supplied, search there first
        if (targetModule)
        {
            // Get target module data list
            GenericList &moduleData = targetModule->configurationLocal()
                                          ? targetModule->targetConfigurations().firstItem()->moduleData()
                                          : processingModuleData;

            // The 'dataIdentifier' is the actual name of the data (possibly with module prefix) - does it exist in
            // the target list?
            if (moduleData.contains(dataIdentifier, targetModule->uniqueName()))
            {
                // Try to retrieve the data as the current type
                found = false;
                const T &data =
                    GenericListHelper<T>::value(moduleData, dataIdentifier, targetModule->uniqueName(), T(), &found);

                if (!found)
                {
                    Messenger::error("Data named '{}_{}' exists, but is not of the correct type (is {} rather than "
                                     "{}).\n",
                                     targetModule->uniqueName(), dataIdentifier,
                                     moduleData.find(dataIdentifier, targetModule->uniqueName())->itemClassName(),
                                     T::itemClassName());
                    return dummy;
                }
                else
                    return data;
            }
            else if (moduleData.contains(dataIdentifier))
            {
                // Try to retrieve the data as the current type
                found = false;
                const T &data = GenericListHelper<T>::value(moduleData, dataIdentifier, "", T(), &found);

                if (!found)
                {
                    Messenger::error("Data named '{}' exists, but is not of the correct type (is {} rather than "
                                     "{}).\n",
                                     dataIdentifier,
                                     moduleData.find(dataIdentifier, targetModule->uniqueName())->itemClassName(),
                                     T::itemClassName());
                    return dummy;
                }
                else
                    return data;
            }
        }

        // If we haven't found it yet, try a search by object tag
        if ((!found) && T::findObject(dataIdentifier))
        {
            // The tagged data exists...
            const T &data = *T::findObject(dataIdentifier);
            found = true;
            return data;
        }

        // Failed to find data
        found = false;
        return dummy;
    }

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
