// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/export/dlPolyField.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/configuration.h"
#include "data/atomicMasses.h"


DlPolyFieldExportFileFormat::DlPolyFieldExportFileFormat(std::string_view filename, DlPolyFieldExportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<DlPolyFieldExportFileFormat::DlPolyFieldExportFormat>(
        "FieldExportFileFormat", {{DlPolyFieldExportFormat::DLPOLY, "dlpoly", "DL_POLY FIELD File"}});
}


// Export DlPolyField as Field
bool DlPolyFieldExportFileFormat::exportDLPOLY(LineParser &parser, Configuration *cfg)
{
    if (!parser.writeLineF("title {} @ {}\n", cfg->name(), cfg->contentsVersion()))
        return false;
    if (!parser.writeLineF("units Kj\n"))
        return false;
        
    const std::vector<std::pair<const Species *, int>> speciesPopulations = cfg->speciesPopulations();
    
    // Export number of moleculer types
    if (!parser.writeLineF("moleculer types {}\n", speciesPopulations.size()))
        return false;
    
    // Export number and name of molecules, and number of atoms, bonds and angles with respective properties
    for (const auto &species : speciesPopulations){
        if (!parser.writeLineF("{}\nnummols {}\natoms {}\n", species.first->name(), species.second, species.first->nAtoms())){
            return false;
        }
        for (const auto &atom : species.first->atoms()){
            if (!parser.writeLineF("{}        {}        {} 1 0\n", atom.atomType()->name(), AtomicMass::mass(atom.Z()), atom.charge()))
                return false;
        }
        if (!parser.writeLineF("bonds {}\n", species.first->nBonds()))
            return false;
        for (const auto &bond : species.first->bonds()){
            const auto params = bond.interactionParameters();
            if (!parser.writeLineF("{}   {}   {} {} {}\n", BondFunctions::forms().keyword(bond.interactionForm()).substr(0, 4), bond.indexI() + 1, bond.indexJ() + 1, params[0], params[1]))
                return false;
        }
        if (!parser.writeLineF("angles {}\n", species.first->nAngles()))
            return false;
        for (const auto &angle : species.first->angles()){
            const auto params = angle.interactionParameters();
            if (!parser.writeLineF("{}   {}   {}   {} {} {}\n", AngleFunctions::forms().keyword(angle.interactionForm()).substr(0, 4), angle.indexI() + 1, angle.indexJ() + 1, angle.indexK() + 1, params[0], params[1]))
                return false;
        }       
    }
    
    if (!parser.writeLineF("finish\n"))
        return false;
        
    int vdw = 0;
    
    // Calculate vdw based on interaction potentials for each atom
    for (const auto &species : speciesPopulations){
        for (const auto &atom : species.first->atoms()){
            const auto params = atom.atomType()->interactionPotential().parameters();
            if (params[0] != 0 || params[1] != 0)
                vdw++;
        }
    }
    
    if (!parser.writeLineF("vdw {}\n", vdw))
        return false;
    
    // Export interaction potentials and respective properties
    for (const auto &species : speciesPopulations){
        for (const auto &atom : species.first->atoms()){
            const auto params = atom.atomType()->interactionPotential().parameters();
            if (params[0] != 0 || params[1] != 0)
                if (!parser.writeLineF("{}        {}        {} {} {}\n", atom.atomType()->name(), atom.atomType()->name(), "LJ", params[0],  params[1]))
                    return false;
        }
    }
    if (!parser.writeLineF("close\n"))
        return false;    
    
    return true;
}

// Export DlPolyField using current filename and format
bool DlPolyFieldExportFileFormat::exportData(Configuration *cfg)
{
    // Open the file
    LineParser parser;
    if (!parser.openOutput(filename_))
    {
        parser.closeFiles();
        return false;
    }

    // Write data
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (DlPolyFieldExportFormat::DLPOLY):
            result = exportDLPOLY(parser, cfg);
            break;
        default:
            throw(std::runtime_error(fmt::format("DlPolyField format '{}' export has not been implemented.\n",
                                                 formats_.keywordByIndex(*formatIndex_))));
    }

    return true;
}
