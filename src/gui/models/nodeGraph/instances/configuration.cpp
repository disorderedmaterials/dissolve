#include "classes/configuration.h"
#include "gui/models/nodeGraph/nodeWrapper.h"

template <> std::string nodeTypeName<Configuration *>(Configuration *const &value) { return "Configuration"; }

template <> std::string nodeTypeIcon<Configuration *>(Configuration *const &value)
{
    return "file:/home/adam/Code/dissolve/src/gui/icons/configuration.svg";
}
