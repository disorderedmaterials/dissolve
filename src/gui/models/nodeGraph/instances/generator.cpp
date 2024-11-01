#include "generator/generator.h"
#include "gui/models/nodeGraph/nodeWrapper.h"

template <> std::string nodeTypeName<Generator *>(Generator *const &value) { return "Generator"; }

template <> std::string nodeTypeIcon<Generator *>(Generator *const &value)
{
    return "file:/home/adam/Code/dissolve/src/gui/icons/generator.svg";
}
