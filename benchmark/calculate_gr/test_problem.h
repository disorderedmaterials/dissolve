
#include "classes/box.h"
#include "classes/coredata.h"
#include "main/dissolve.h"
#include "modules/rdf/rdf.h"

enum ProblemSize
{
    Small,
    Medium
};

namespace Method
{
constexpr auto CellsMethod = RDFModule::PartialsMethod::CellsMethod;
constexpr auto SimpleMethod = RDFModule::PartialsMethod::SimpleMethod;
} // namespace Method

template <ProblemSize problem = Small> struct Problem
{
    Problem() : dissolve_(coredata_)
    {

        Messenger::setQuiet(true);
        dissolve_.registerMasterModules();
        if constexpr (problem == ProblemSize::Small)
            dissolve_.loadInput("hexane1k.txt");
        else if constexpr (problem == ProblemSize::Medium)
            dissolve_.loadInput("water10k.txt");
        else
            return;
        dissolve_.prepare();
        setUp();
    }

    void setUp()
    {
        auto *benchmarkModule = dissolve_.processingLayers().first()->find("Benchmark");
        auto &procPool = dissolve_.worldPool();
        cfg_ = benchmarkModule->targetConfigurations().first()->item();
        procPool.assignProcessesToGroups(cfg_->processPool());
        cfg_->generate(dissolve_.worldPool(), dissolve_.pairPotentialRange());

        rdfmodule_ = std::make_unique<RDFModule>();
        rdfmodule_->addTargetConfiguration(cfg_);
        cfg_->incrementContentsVersion();
        srand(dissolve_.seed());
    }

    template <RDFModule::PartialsMethod method> void iterateCells()
    {
        double rdfRange = cfg_->box()->inscribedSphereRadius();
        bool upToDate = false;
        rdfRange = 5;
        rdfmodule_->calculateGR(dissolve_.processingModuleData(), dissolve_.worldPool(), cfg_, method, rdfRange, 0.05,
                                upToDate);
        dissolve_.processingModuleData().remove(fmt::format("{}//OriginalGR", cfg_->niceName()), rdfmodule_->uniqueName());
    }
    CoreData coredata_;
    Dissolve dissolve_;
    std::unique_ptr<RDFModule> rdfmodule_;
    Configuration *cfg_;
};