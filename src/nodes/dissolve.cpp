#include "nodes/dissolve.h"

// Return dissolve
Dissolve& DissolveGraph::dissolve() const { return dissolve_; }

// Return world pool
const ProcessPool& DissolveGraph::processPool() const { return dissolve_.worldPool(); }