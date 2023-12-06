// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/export/dlPolyField.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/speciesAtom.h"
#include "data/atomicMasses.h"


#include "classes/atom.h"
#include "classes/atomType.h"
#include "classes/atomTypeMix.h"
#include "classes/molecule.h"
#include <vector>
#include <iterator>

#include "base/serialiser.h"
#include "base/version.h"
#include "classes/atom.h"
#include "classes/atomTypeMix.h"
#include "classes/box.h"
#include "classes/cellArray.h"
#include "classes/molecule.h"
#include "classes/siteStack.h"
#include "io/import/coordinates.h"
#include "items/list.h"
#include "kernels/potentials/base.h"
#include "math/data1D.h"
#include "math/histogram1D.h"
#include "math/interpolator.h"
#include "module/layer.h"
#include "procedure/procedure.h"
#include "templates/vector3.h"
#include <deque>
#include <map>
#include <memory>
#include <vector>

#include "io/export/coordinates.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/speciesAtom.h"
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
    // Export title
    if (!parser.writeLineF("title {} @ {}\n", cfg->name(), cfg->contentsVersion()))
        return false;
    if (!parser.writeLineF("units Kj\n"))
        return false;
        
    const std::vector<std::pair<const Species *, int>> speciesPopulations = cfg->speciesPopulations();
    
    if (!parser.writeLineF("moleculer types {}\n", speciesPopulations.size()))
        return false;
    
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
            //const auto params = bond.interactionParameters();
            //Messenger::error("{}",params);
            if (!parser.writeLineF("{}   {}   {} {} {}\n", BondFunctions::forms().keyword(bond.interactionForm()).substr(0, 4), bond.indexI() + 1, bond.indexJ() + 1), "bond.interactionParameters()[0]", "bond.interactionParameters()[1]")
                return false;
        }
        if (!parser.writeLineF("angles {}\n", species.first->nAngles()))
            return false;
        for (const auto &angle : species.first->angles()){
            if (!parser.writeLineF("{}   {}   {} {} {}\n", AngleFunctions::forms().keyword(angle.interactionForm()).substr(0, 4), angle.indexI() + 1, angle.indexJ() + 1), "angle.interactionParameters()[0]", "angle.interactionParameters()[1]")
                return false;
        }       
    }
    
    if (!parser.writeLineF("finish"))
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
