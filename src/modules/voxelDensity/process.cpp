#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "main/dissolve.h"
#include "math/gaussFit.h"
#include "module/context.h"
#include "voxelDensity.h"
#include <cmath>

void VoxelDensityModule::addValue(Vec3<double> coords, double value, Array3D<double> &array)
{
    auto t = std::make_tuple((int)std::floor(coords.x * nAxisVoxels_), (int)std::floor(coords.y * nAxisVoxels_),
                             (int)std::floor(coords.z * nAxisVoxels_));
    array[t] += value;
}

Vec3<double> VoxelDensityModule::foldedCoordinates(Atom &atom, const Box *unitCell)
{
    atom.set(unitCell->foldFrac(atom.r()));
    auto x = atom.x(), y = atom.y(), z = atom.z();
    return Vec3<double>(x, y, z);
}

Module::ExecutionResult VoxelDensityModule::process(ModuleContext &context)
{
    auto &processingData = context.dissolve().processingModuleData();

    // Calculate target property 3d map over unit cell voxels
    auto array3D = processingData.realise<Array3D<double>>("Array3D", name());
    array3D.initialise(nAxisVoxels_, nAxisVoxels_, nAxisVoxels_);

    if (!restrictToSpecies_.empty())
    {
        auto cfgSpecies = targetConfiguration_->speciesPopulations();

        for (const auto &[sp, N] : cfgSpecies)
        {
            bool included = false;

            for (const auto &spTarget : restrictToSpecies_)
            {
                if (sp->name() == spTarget->name())
                {
                    included = true;
                    break;
                }
            }

            if (included)
            {
                targetConfiguration_->removeMolecules(sp);
            }
        }
    }

    auto unitCell = targetConfiguration_->box();
    auto voxelVolume = unitCell->volume() / std::pow(nAxisVoxels_, 3);
    auto atoms = targetConfiguration_->atoms();

    auto massOp = [this, &array3D, &unitCell](auto &atom)
    { addValue(foldedCoordinates(atom, unitCell), AtomicMass::mass(atom.speciesAtom()->Z()), array3D); };

    auto atomicNumberOp = [this, &array3D, &unitCell](auto &atom)
    { addValue(foldedCoordinates(atom, unitCell), atom.speciesAtom()->Z(), array3D); };

    auto scatteringLengthDensityOp = [this, &array3D, &unitCell](auto &atom)
    { addValue(foldedCoordinates(atom, unitCell), scatteringLengthDensity(atom.speciesAtom()->Z()), array3D); };

    switch (targetProperty_)
    {
        case TargetPropertyType::Mass:
            dissolve::for_each(std::execution::seq, atoms.begin(), atoms.end(), massOp);
            break;
        case TargetPropertyType::AtomicNumber:
            dissolve::for_each(std::execution::seq, atoms.begin(), atoms.end(), atomicNumberOp);
            break;
        case TargetPropertyType::ScatteringLengthDensity:
            dissolve::for_each(std::execution::seq, atoms.begin(), atoms.end(), scatteringLengthDensityOp);
            break;
        default:
            throw(std::runtime_error(fmt::format("'{}' not a valid property.\n", static_cast<int>(targetProperty_))));
    }

    // Calculate voxel density histogram, normalising bin values by voxel volume (property/cubic angstrom)
    auto &hist = processingData.realise<Histogram1D>("Histogram1D", name(), GenericItem::InRestartFileFlag);

    hist.initialise(binRange_.x, binRange_.y, binRange_.z);
    hist.zeroBins();

    for (const auto &value : array3D.values())
        hist.bin(value / voxelVolume);

    auto &data1D = processingData.realise<Data1D>("Data1D", name(), GenericItem::InRestartFileFlag);
    hist.accumulate();
    data1D = hist.accumulatedData();

    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData((fitGaussian_) ? GaussFit(data1D).approximation(FunctionSpace::RealSpace, 1.0, binRange_.x, binRange_.z, binRange_.y) : data1D, exportFileAndFormat_, context.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}