#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "data/atomicMasses.h"
#include "data/isotopes.h"
#include "main/dissolve.h"
#include "math/matrix3.h"
#include "module/context.h"
#include "templates/array3D.h"
#include "voxelDensity.h"
#include <cmath>

void addValue(const int N, Vec3<double> coords, double value, Array3D<double> &array)
{
    auto t = std::make_tuple((int)std::round(coords.x * N), (int)std::round(coords.y * N), (int)std::round(coords.z * N));
    array[t] += value;
}

Vec3<double> foldedCoordinates(Atom &atom, const Box *unitCell)
{
    atom.set(unitCell->foldFrac(atom.r()));
    auto x = atom.x(), y = atom.y(), z = atom.z();
    return Vec3<double>(x, y, z);
}

double scatteringLengthDensity(Elements::Element Z) { return Sears91::boundCoherent(Sears91::naturalIsotope(Z)); }

Module::ExecutionResult VoxelDensityModule::process(ModuleContext &context)
{
    auto &processingData = context.dissolve().processingModuleData();

    // Calculate target property 3d map over unit cell voxels
    auto array3D = processingData.realise<Array3D<double>>("Array3D", name());
    array3D.initialise(nVoxels_, nVoxels_, nVoxels_);

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
    auto voxelVolume = unitCell->volume() / std::pow(nVoxels_, 3);
    auto atoms = targetConfiguration_->atoms();

    auto massOp = [this, &array3D, &unitCell](auto &atom)
    { addValue(nVoxels_, foldedCoordinates(atom, unitCell), AtomicMass::mass(atom.speciesAtom()->Z()), array3D); };

    auto atomicNumberOp = [this, &array3D, &unitCell](auto &atom)
    { addValue(nVoxels_, foldedCoordinates(atom, unitCell), atom.speciesAtom()->Z(), array3D); };

    auto scatteringLengthDensityOp = [this, &array3D, &unitCell](auto &atom)
    { addValue(nVoxels_, foldedCoordinates(atom, unitCell), scatteringLengthDensity(atom.speciesAtom()->Z()), array3D); };

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

    auto values = array3D.values();
    auto max = *std::max_element(values.begin(), values.end());
    constexpr static auto findMin = [](const auto &v) { return *std::min_element(v.begin(), v.end()); };
    auto min = ((nVoxels_ == 1) || (max == findMin(values))) ? (double)0 : findMin(values);

    hist.initialise(min, max, (max - min) / nVoxels_);
    hist.zeroBins();

    for (const auto &value : array3D.values())
        hist.bin(value / voxelVolume);

    auto &data1D = processingData.realise<Data1D>("Data1D", name(), GenericItem::InRestartFileFlag);
    data1D = hist.accumulatedData();

    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(data1D, exportFileAndFormat_, context.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}