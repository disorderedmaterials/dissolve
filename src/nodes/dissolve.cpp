#include "nodes/dissolve.h"

// Return dissolve
Dissolve& DissolveGraph::dissolve() const
{ 
	return dissolve_;
}

// Return world pool
ProcessPool& DissolveGraph::processPool() const
{ 
	return dissolve_.worldPool();
}