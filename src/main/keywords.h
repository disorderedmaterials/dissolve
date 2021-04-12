// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"

// Forward Declarations
class LineParser;
class CoreData;
class Dissolve;
class Sample;
class Configuration;
class Species;
class Module;
class ModuleLayer;
class GenericList;
class Data;
class SpeciesInfo;
class SpeciesSite;

/*
 * Block Keywords
 */
namespace BlockKeywords
{
// Block Keyword Enum
enum BlockKeyword
{
    ConfigurationBlockKeyword,  /* 'Configuration' - Defines a single Configuration for use in the simulation */
    LayerBlockKeyword,          /* 'Layer' - Defines a sequence of Modules in a processing layer */
    MasterBlockKeyword,         /* 'Master' - Contains master intramolecular terms for use in Species */
    ModuleBlockKeyword,         /* 'Module' - Sets up a Module to run after Configuration processing */
    PairPotentialsBlockKeyword, /* 'PairPotentials' - Contains definitions of the PairPotentials for the simulation */
    SimulationBlockKeyword,     /* 'Simulation' - Setting of simulation variables affecting the calculation */
    SiteBlockKeyword,           /* 'Site' - Defines an analysis site within a Species */
    SpeciesBlockKeyword         /* 'Species' - Begins a definition of a Species */
};
// Return enum option info for BlockKeyword
EnumOptions<BlockKeywords::BlockKeyword> keywords();
}; // namespace BlockKeywords

/*
 * Configuration Block Keywords
 */
namespace ConfigurationBlock
{
// Configuration Block Keyword Enum
enum ConfigurationKeyword
{
    CellDivisionLengthKeyword, /* 'CellDivisionLength' - Set the requested side length for regions when partitioning the
                      unit cell */
    EndConfigurationKeyword,   /* 'EndConfiguration' - Signals the end of the Configuration block */
    GeneratorKeyword,          /* 'Generator' - Define the generator procedure for the Configuration */
    InputCoordinatesKeyword,   /* 'InputCoordinates' - Specifies the file which contains the starting coordinates */
    SizeFactorKeyword,         /* 'SizeFactor' - Scaling factor for Box lengths, Cell size, and Molecule centres-of-geometry */
    TemperatureKeyword         /* 'Temperature' - Defines the temperature of the simulation */
};
// Return enum option info for ConfigurationKeyword
EnumOptions<ConfigurationBlock::ConfigurationKeyword> keywords();
// Parse Configuration block
bool parse(LineParser &parser, Dissolve *dissolve, Configuration *cfg);
}; // namespace ConfigurationBlock

/*
 * Layer Block Keywords
 */
namespace LayerBlock
{
// Layer Block Keyword Enum
enum LayerKeyword
{
    DisabledKeyword,  /* 'Disabled' - Specify that the layer is currently disabled */
    EndLayerKeyword,  /* 'EndLayer' - Signals the end of the Layer block */
    FrequencyKeyword, /* 'Frequency' - Frequency at which the layer is executed, relative to the main iteration counter */
    ModuleKeyword     /* 'Module' - Begin a Module definition within this layer */
};
// Return enum option info for LayerKeyword
EnumOptions<LayerBlock::LayerKeyword> keywords();
// Parse Layer block
bool parse(LineParser &parser, Dissolve *dissolve, ModuleLayer *layer);
}; // namespace LayerBlock

/*
 * Master Block Keywords
 */
namespace MasterBlock
{
// Master Block Keyword Enum
enum MasterKeyword
{
    AngleKeyword,     /* 'Angle' - Define master Angle parameters that can be referred to */
    BondKeyword,      /* 'Bond' - Define master Bond parameters that can be referred to */
    EndMasterKeyword, /* 'EndMaster' - Signals the end of the Master block */
    ImproperKeyword,  /* 'Improper' - Define master Improper parameters that can be referred to */
    TorsionKeyword    /* 'Torsion' - Define master Torsion parameters that can be referred to */
};
// Return enum option info for MasterKeyword
EnumOptions<MasterBlock::MasterKeyword> keywords();
// Parse Master block
bool parse(LineParser &parser, CoreData &coreData);
}; // namespace MasterBlock

/*
 * Module Block Keywords
 */
namespace ModuleBlock
{
// Module Block Keyword Enum
enum ModuleKeyword
{
    DisableKeyword,   /* 'Disable' - Disables the module, preventing it from running */
    EndModuleKeyword, /* 'EndModule' - Signals the end of the Module block */
    FrequencyKeyword  /* 'Frequency' - Frequency at which the Module is run */
};
// Return enum option info for ModuleKeyword
EnumOptions<ModuleBlock::ModuleKeyword> keywords();
// Parse Module block
bool parse(LineParser &parser, Dissolve *dissolve, Module *module, GenericList &targetList, bool moduleInConfiguration);
}; // namespace ModuleBlock

/*
 * PairPotential Block Keywords
 */
namespace PairPotentialsBlock
{
// PairPotential Block Keyword Enum
enum PairPotentialsKeyword
{
    CoulombTruncationKeyword, /* 'CoulombTruncation' - Truncation scheme to apply to Coulomb potential */
    DeltaKeyword,             /* 'Delta' - Gives the spacing between points in the tabulated potentials */
    EndPairPotentialsKeyword, /* 'EndPairPotentials' - Signals the end of the PairPotentials block */
    GenerateKeyword,          /* 'Generate' - Generates a single PairPotential with the specified contributions */
    IncludeCoulombKeyword,    /* 'IncludeCoulomb' - Include Coulomb term in tabulated pair potentials" */
    ParametersKeyword,        /* 'Parameters' - Sets or re-sets the short-range and charge parameters for a specific AtomType */
    RangeKeyword, /* 'Range' - Specifies the total range (inc. truncation width) over which to generate potentials */
    ShortRangeTruncationKeyword,     /* 'ShortRangeTruncation' - Truncation scheme to apply to short-range potential */
    ShortRangeTruncationWidthKeyword /* 'ShortRangeTruncationWidth' - Width of potential tail over which to reduce
                        short-range term to zero */
};
// Return enum option info for PairPotentialsKeyword
EnumOptions<PairPotentialsBlock::PairPotentialsKeyword> keywords();
// Parse PairPotentials block
bool parse(LineParser &parser, Dissolve *dissolve);
}; // namespace PairPotentialsBlock

/*
 * Simulation Block Keywords
 */
namespace SimulationBlock
{
// Simulation Block Keyword Enum
enum SimulationKeyword
{
    EndSimulationKeyword, /* 'EndSimulation' - Signals the end of the Simulation block */
    SeedKeyword           /* 'Seed' - Random seed to use */
};
// Return enum option info for SimulationKeyword
EnumOptions<SimulationBlock::SimulationKeyword> keywords();
// Parse Simulation block
bool parse(LineParser &parser, Dissolve *dissolve);
}; // namespace SimulationBlock
