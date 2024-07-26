#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "data/atomicMasses.h"
#include "data/isotopes.h"
#include "main/dissolve.h"
#include "math/data3D.h"
#include "math/matrix3.h"
#include "module/context.h"
#include "voxelDensity.h"
#include <cmath>

Module::ExecutionResult VoxelDensityModule::process(ModuleContext &context)
{
    auto &processingData = context.dissolve().processingModuleData();

    // Calculate target property density
    auto [data3D, status] = processingData.realiseIf<Data3D>(
        fmt::format("Data3D//{}", targetPropertyTypes().keywordByIndex(static_cast<int>(targetProperty_))), name(),
        GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
    {
        data3D.initialise(numPoints_);
    }
    data3D.zero();

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
    auto voxelVolume = unitCell->volume() / std::pow(numPoints_, 3);
    auto atoms = targetConfiguration_->atoms();

    auto unaryOp = [this, &data3D, &unitCell](auto &atom)
    {
        atom.set(unitCell->foldFrac(atom.r()));
        auto x = atom.x(), y = atom.y(), z = atom.z();
        auto atomicNumber = atom.speciesAtom()->Z();

        double value;

        switch (targetProperty_)
        {
            case TargetPropertyType::Mass:
            {
                // Atomic mass
                value = AtomicMass::mass(atomicNumber);
                break;
            }
            case TargetPropertyType::AtomicNumber:
            {
                // Atomic number
                value = atomicNumber;
                break;
            }
            case TargetPropertyType::ScatteringLengthDensity:
            {
                auto naturalIsotope = Sears91::naturalIsotope(atomicNumber);

                // Bound coherent natural isotope scattering length density
                value = Sears91::boundCoherent(naturalIsotope);
                break;
            }
            default:
            {
                throw(std::runtime_error(fmt::format("'{}' not a valid property.\n", static_cast<int>(targetProperty_))));
            }
        }

        auto toIndex = [&](const auto pos) { return static_cast<int>(std::round(pos * numPoints_)); };

        data3D.addToPoint(toIndex(x), x, toIndex(y), y, toIndex(z), z, value);
    };

    dissolve::for_each(std::execution::seq, atoms.begin(), atoms.end(), unaryOp);

    // Calculate voxel density histogram, normalising bin values by voxel volume (property/cubic angstrom)
    auto &hist = processingData.realise<Histogram1D>(
        fmt::format("Histogram1D//{}/A^3", targetPropertyTypes().keywordByIndex(static_cast<int>(targetProperty_))), name(),
        GenericItem::InRestartFileFlag);

    auto max = data3D.maxValue(), min = ((numPoints_ == 1) || (max == data3D.minValue())) ? 0 : data3D.minValue();
    hist.initialise(min, max, (max - min) / numPoints_);
    hist.zeroBins();

    dissolve::for_each(std::execution::seq, data3D.values().begin(), data3D.values().end(),
                       [&hist](auto &value) { hist.bin(value / voxelVolume); });

    auto &data1D = processingData.realise<Data1D>(
        fmt::format("Data1D//{}/A^3", targetPropertyTypes().keywordByIndex(static_cast<int>(targetProperty_))), name(),
        GenericItem::InRestartFileFlag);
    data1D = hist.accumulatedData();

    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(data1D, exportFileAndFormat_, context.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}