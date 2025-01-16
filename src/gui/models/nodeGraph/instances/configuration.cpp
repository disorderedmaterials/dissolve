#include "classes/configuration.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"
#include "gui/models/nodeGraph/instances/configuration.h"
#include "gui/models/nodeGraph/nodeWrapper.h"

// The name of the type (for delegate dispatch)
std::string nodeTypeName(const Configuration *value) { return "Configuration"; }

// The path to the icon for the node
std::string nodeTypeIcon(const Configuration *value)
{
    return "qrc:/Dissolve/icons/configuration.svg";
}

// The title of the node
std::string nodeName(const Configuration *value)
{

    if (!value)
        return std::string("Null");
    std::string s = {value->niceName().begin(), value->niceName().end()};
    return s;
}

// Get a specific piece of information from a node by index
QVariant nodeData(const Configuration *value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    if (!value)
        return {};
    switch (role)
    {
        case names::Value:
            return QVariant::fromValue(value);
        case names::Temperature:
            return value->temperature();
        case names::AtomicDensity:
            return value->atomicDensity().value_or(0.0);
        default:
            return {};
    }
}

// Set a specific piece of information from a node by index
bool nodeSetData(Configuration *item, const QVariant &value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    switch (role)
    {
        case names::Temperature:
            item->setTemperature(value.toDouble());
            return true;
        default:
            return false;
    }
}

// Delete the node
bool nodeDelete(Configuration *item, CoreData *coreData)
{
    coreData->removeConfiguration(item);
    return true;
}
