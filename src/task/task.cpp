#include "task/task.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"

std::vector<std::unique_ptr<Module>> &Task::modules()
{ return modules_; }

SerialisedValue Task::serialise() const
{
    SerialisedValue result;
    Serialisable::fromVectorToTable(modules_, "modules", result);
    return result;
}

void Task::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    Serialisable::toMap(
        node, "modules",
        [&coreData, this](const auto &key, const SerialisedValue &data)
        {
            auto *module =
                addModule(*ModuleTypes::moduleType(std::string_view(std::string(toml::find<std::string>(data, "type"), {}))));
            module->setName(key);
            module->deserialise(data, coreData);
        });
}