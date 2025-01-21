#include "generator/generator.h"
#include "gui/models/nodeGraph/generatorGraphModel.h"
#include "gui/models/nodeGraph/instances/generator.h"
#include "gui/models/nodeGraph/nodeWrapper.h"

// The name of the type (for delegate dispatch)
std::string nodeTypeName(Generator *const &value) { return "Generator"; }

// The path to the icon for the node
std::string nodeTypeIcon(Generator *const &value) { return "qrc:/Dissolve/icons/generator.svg"; }

// The title of the node
std::string nodeName(Generator *const &value) { return "Generator"; }

// Get a specific piece of information from a node by index
QVariant nodeData(Generator *value, int role)
{
    using names = GeneratorGraphModel::PropertyIndex;
    if (!value)
        return {};
    switch (role)
    {
        case names::Value:
            return QVariant::fromValue(value);
        case names::Size:
            return QVariant::fromValue(value->rootSequence().nNodes());
        default:
            return {};
    }
}

// Set a specific piece of information from a node by index
bool nodeSetData(Generator *item, const QVariant &value, int role) { return false; }

// Delete the node
bool nodeDelete(Generator *item, CoreData *coreData) { return false; }
