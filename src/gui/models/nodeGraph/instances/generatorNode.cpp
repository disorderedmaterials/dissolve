#include "generator/node.h"
#include "gui/models/nodeGraph/nodeWrapper.h"

template <> std::string nodeTypeName<GeneratorNode *>(GeneratorNode *const &value) { return "GeneratorNode"; }

template <> std::string nodeTypeIcon<GeneratorNode *>(GeneratorNode *const &value)
{
    auto name = GeneratorNode::nodeTypes().keyword(value->type());
    return "file:/home/adam/Code/dissolve/src/gui/icons/nodes/" + name + ".svg";
}
