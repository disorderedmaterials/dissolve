#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "main/dissolve.h"
#include "math/gaussFit.h"
#include "module/context.h"
#include "voxelDensity.h"

void VoxelDensityModule::addValue(Vec3<double> coords, double value, Array3D<double> &array)
{
    if (nAxisVoxels_.x == 1 && nAxisVoxels_.y == 1 && nAxisVoxels_.z == 1)
    {
        array.values()[0] += value;
        return;
    }

    auto t = std::make_tuple((int)std::floor(coords.x * nAxisVoxels_.x), (int)std::floor(coords.y * nAxisVoxels_.y),
                             (int)std::floor(coords.z * nAxisVoxels_.z));
    array[t] += value;
}

Vec3<double> VoxelDensityModule::foldedCoordinates(const Vec3<double> &r, const Box *unitCell) { return unitCell->foldFrac(r); }

Module::ExecutionResult VoxelDensityModule::process(ModuleContext &context)
{
    auto &processingData = context.dissolve().processingModuleData();

    // Define voxels
    auto unitCell = targetConfiguration_->box();
    const double boxX = unitCell->axisLengths().x, boxY = unitCell->axisLengths().y, boxZ = unitCell->axisLengths().z;
    voxelVolume_ = voxelSideLength(boxX) * voxelSideLength(boxY) * voxelSideLength(boxZ);
    nAxisVoxels_.x = int(round(boxX / voxelSideLength(boxX)));
    nAxisVoxels_.y = int(round(boxY / voxelSideLength(boxY)));
    nAxisVoxels_.z = int(round(boxZ / voxelSideLength(boxZ)));

    Messenger::print("Volume of unit cell voxels: {}.\n", voxelVolume_);
    Messenger::print("Number of voxels along each axis: nX={}, nY={}, nZ={}.\n", nAxisVoxels_.x, nAxisVoxels_.y,
                     nAxisVoxels_.z);

    // Calculate target property 3d map over unit cell voxels
    array3D_.initialise(nAxisVoxels_.x, nAxisVoxels_.y, nAxisVoxels_.z);

    if (!restrictToSpecies_.empty())
    {
        auto cfgSpecies = targetConfiguration_->speciesPopulations();

        for (const auto &[sp, N] : cfgSpecies)
        {
            bool included = std::any_of(restrictToSpecies_.begin(), restrictToSpecies_.end(), [sp](auto &spTarget) { return sp->name() == spTarget->name(); });

            if (included)
                targetConfiguration_->removeMolecules(sp);
        }
    }

    const auto &atoms = targetConfiguration_->atoms();

    auto massOp = [this, &unitCell](auto &atom)
    { addValue(foldedCoordinates(atom.r(), unitCell), AtomicMass::mass(atom.speciesAtom()->Z()), array3D_); };

    auto atomicNumberOp = [this, &unitCell](auto &atom)
    { addValue(foldedCoordinates(atom.r(), unitCell), atom.speciesAtom()->Z(), array3D_); };

    auto scatteringLengthDensityOp = [this, &unitCell](auto &atom)
    { addValue(foldedCoordinates(atom.r(), unitCell), scatteringLengthDensity(atom.speciesAtom()->Z()), array3D_); };

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

    const auto min = binRange_.x, max = binRange_.y, binWidth = binRange_.z;

    hist.initialise(min, max, binWidth);
    hist.zeroBins();

    for (const auto &value : array3D_.values())
        hist.bin(value / voxelVolume_);

    auto &data1D = processingData.realise<Data1D>("Data1D", name(), GenericItem::InRestartFileFlag);
    hist.accumulate();
    data1D = hist.accumulatedData();

    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(data1D, exportFileAndFormat_, context.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}