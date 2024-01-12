// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

// Module Types
namespace ModuleTypes
{
enum ModuleType
{
    Accumulate,
    Analyse,
    Angle,
    AtomShake,
    AvgMol,
    AxisAngle,
    Benchmark,
    Bragg,
    Checks,
    CheckSpecies,
    Compare,
    DAngle,
    DataTest,
    Energy,
    EPSR,
    ExportCoordinates,
    ExportPairPotentials,
    ExportTrajectory,
    Forces,
    GeometryOptimisation,
    GR,
    HistogramCN,
    ImportTrajectory,
    IntraAngle,
    IntraDistance,
    IntraShake,
    MD,
    MolShake,
    NeutronSQ,
    OrientedSDF,
    QSpecies,
    SDF,
    SiteRDF,
    Skeleton,
    SQ,
    Test,
    XRaySQ
};

// Return module type string for specified type enumeration
std::string moduleType(ModuleTypes::ModuleType type);
// Return the lowerCamelCase name of the module type provided
std::string lccModuleType(ModuleTypes::ModuleType type);
// Return module type enumeration for specified module type string
std::optional<ModuleTypes::ModuleType> moduleType(std::string_view keyword);
}; // namespace ModuleTypes
