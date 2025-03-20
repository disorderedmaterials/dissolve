#include "nodes/dissolve.h"

// Set node context
void DissolveNode::setContext(Dissolve &dissolve) { ctx_.dissolve = dissolve, ctx_.processPool = dissolve.worldPool(); }

// Return node context
NodeContext &DissolveNode::context() const { return ctx_; }