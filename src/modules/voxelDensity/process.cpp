#include "module/context.h"
#include "voxelDensity.h"
#include "math/data3D.h"
#include "math/matrix3.h"
#include "data/atomicMasses.h"
#include "data/isotopes.h"

Module::ExecutionResult VoxelDensityModule::process(ModuleContext &context)
{
    auto &processingData = moduleContext.dissolve().processingModuleData();

    // Calculate target property density
    auto [density, status] = processingData.realiseIf<Data3D>("Density", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created) { density.initialise(numPoints_); }
    density.zero();

    auto unitCell = targetConfiguration_->box();
    auto atoms = targetConfiguration_->atoms();

    auto unaryOp = [&density, &unitCell](const auto &atom)
    {
        auto foldedAtomCoords = atom.set(unitCell->fold(atom.r()));
        auto x = atom.x(), y = atom.y(), z = atom.z();
        auto atomicNumber = atom.speciesAtom()->Z();

        switch(targetProperty_)
        {
            case TargetPropertyType::Mass: 
            { 
                auto atomicMass = AtomicMass::mass(atomicNumber);
                density.addPoint(
                    (double)x*numPoints_, 
                    (double)y*numPoints_, 
                    (double)z*numPoints_,
                    atomicMass
                );
            }
            case TargetPropertyType::AtomicNumber: 
            {
                density.addPoint(
                    (double)x*numPoints_, 
                    (double)y*numPoints_, 
                    (double)z*numPoints_,
                    atomicNumber
                );
            }
            case TargetPropertyType::ScatteringLengthDensity:
            {
                auto naturalIsotope = Sears91::naturalIsotope(atomicNumber)
                density.addPoint(
                    (double)x*numPoints_, 
                    (double)y*numPoints_, 
                    (double)z*numPoints_,
                    Sears91::boundCoherent(naturalIsotope);
                ); 
            }
            default:
            {
                throw(std::runtime_error(
                fmt::format("'{}' not a valid property.\n", targetProperty_)));
            }
        }
    }
    
    dissolve::for_each(std::execution::seq, atoms.begin(), atoms.end(), unaryOp);

}