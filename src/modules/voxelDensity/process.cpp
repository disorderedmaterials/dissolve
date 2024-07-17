#include "data/atomicMasses.h"
#include "data/isotopes.h"
#include "math/data3D.h"
#include "math/matrix3.h"
#include "module/context.h"
#include "voxelDensity.h"
#include <cmath>

Module::ExecutionResult VoxelDensityModule::process(ModuleContext &context)
{
    auto &processingData = moduleContext.dissolve().processingModuleData();

    // Calculate target property density
    auto [density, status] = processingData.realiseIf<Data3D>(
        fmt::format("Voxel{}", targetPropertyTypes().descriptionByIndex(static_cast<int>(targetProperty_))), name(),
        GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
    {
        density.initialise(numPoints_);
    }
    density.zero();

    auto unitCell = targetConfiguration_->box();
    auto atoms = targetConfiguration_->atoms();

    auto unaryOp =
        [&density, &unitCell](const auto &atom)
    {
        atom.set(unitCell->foldFractional(atom.r()));
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
                throw(std::runtime_error(fmt::format("'{}' not a valid property.\n", targetProperty_)));
            }
        }

        auto toIndex = [&numPoints_](const auto pos) { return (double)std::round(pos * numPoints_); }

                       density.addPoint(toIndex(x), toIndex(y), toIndex(z), value);
    }

    dissolve::for_each(std::execution::seq, atoms.begin(), atoms.end(), unaryOp);

    return ExecutionResult::Success;
}