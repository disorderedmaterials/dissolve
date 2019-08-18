/*
	*** Procedure Node - Calculate
	*** src/procedure/nodes/calculate.cpp
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "procedure/nodes/calculate.h"
#include "procedure/nodes/select.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
CalculateProcedureNode::CalculateProcedureNode(CalculateProcedureNode::Observable observable, SelectProcedureNode* site0, SelectProcedureNode* site1, SelectProcedureNode* site2, SelectProcedureNode* site3) : ProcedureNode(ProcedureNode::CalculateNode)
{
	observable_ = observable;
	sites_[0] = site0;
	sites_[1] = site1;
	sites_[2] = site2;
	sites_[3] = site3;
	value_ = 0.0;
}

// Destructor
CalculateProcedureNode::~CalculateProcedureNode()
{
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool CalculateProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
	return (context == ProcedureNode::AnalysisContext);
}

/*
 * Node Keywords
 */

// Return enum option info for CalculateNodeKeyword
EnumOptions<CalculateProcedureNode::CalculateNodeKeyword> CalculateProcedureNode::calculateNodeKeywords()
{
	static EnumOptionsList CalculateNodeTypeKeywords = EnumOptionsList() <<
		EnumOption(CalculateProcedureNode::AngleKeyword,		"Angle",	3) <<
		EnumOption(CalculateProcedureNode::DistanceKeyword,		"Distance",	2) <<
		EnumOption(CalculateProcedureNode::EndCalculateKeyword,		"EndCalculate") <<
		EnumOption(CalculateProcedureNode::VectorKeyword,		"Vector",	2);

	static EnumOptions<CalculateProcedureNode::CalculateNodeKeyword> options("CalculateNodeKeyword", CalculateNodeTypeKeywords);

	return options;
}

/*
 * Observables
 */

// Observable keywords
const char* ObservableKeywords[] = { "Angle", "Distance", "Vector" };
int ObservableNSites[] = { 3, 2 , 2 };
int ObservableDimensionality[] = { 1, 1, 3 };

// Convert string to Observable
CalculateProcedureNode::Observable CalculateProcedureNode::observable(const char* s)
{
	for (int nk=0; nk < CalculateProcedureNode::nObservables; ++nk) if (DissolveSys::sameString(s, ObservableKeywords[nk])) return (CalculateProcedureNode::Observable) nk;

	return CalculateProcedureNode::nObservables;
}

// Convert Observable to string
const char* CalculateProcedureNode::observable(CalculateProcedureNode::Observable obs)
{
	return ObservableKeywords[obs];
}

// Number of sites required to calculate Observable
int CalculateProcedureNode::observableNSites(CalculateProcedureNode::Observable obs)
{
	return ObservableNSites[obs];
}

// Dimensionality of data for Observable
int CalculateProcedureNode::observableDimensionality(Observable obs)
{
	return ObservableDimensionality[obs];
}

/*
 * Observable Target
 */

// Return last calculated value of observable
double CalculateProcedureNode::value(int id) const
{
#ifdef CHECKS
	if ((id < 0) || (id >= 3))
	{
		Messenger::error("Observable value index %i is out of range.\n", id);
		return 0.0;
	}
	if (id >= observableDimensionality(observable_))
	{
		Messenger::error("Observable value index %i is out of range for an observable of type '%s' which has dimensionality %i.\n", id, observable(observable_), observableDimensionality(observable_));
		return 0.0;
	}
#endif
	return value_.get(id);
}

// Return last calculated value of observable
Vec3<double> CalculateProcedureNode::values() const
{
	return value_;
}

/* 
 * Execute
 */

// Prepare any necessary data, ready for execution
bool CalculateProcedureNode::prepare(Configuration* cfg, const char* prefix, GenericList& targetList)
{
	return true;
}

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult CalculateProcedureNode::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
#ifdef CHECKS
	for (int n=0; n<observableNSites(observable_); ++n)
	{
		if (!sites_[n])
		{
			Messenger::error("Observable site %i is not set.\n", n);
			return ProcedureNode::Failure;
		}
		if (sites_[n]->currentSite() == NULL)
		{
			Messenger::error("Observable %i has no current site.\n", n);
			return ProcedureNode::Failure;
		}
	}
#endif
	// Determine the value of the requested observable
	switch (observable_)
	{
		case (CalculateProcedureNode::AngleObservable):
			value_.x = cfg->box()->angleInDegrees(sites_[0]->currentSite()->origin(), sites_[1]->currentSite()->origin(), sites_[2]->currentSite()->origin());
			break;
		case (CalculateProcedureNode::DistanceObservable):
			value_.x = cfg->box()->minimumDistance(sites_[0]->currentSite()->origin(), sites_[1]->currentSite()->origin());
			break;
		case (CalculateProcedureNode::VectorObservable):
			value_ = cfg->box()->minimumVector(sites_[0]->currentSite()->origin(), sites_[1]->currentSite()->origin());
			break;
		default:
			Messenger::error("Mr Developer Man has not implemented calculation of this observable quantity (%i).\n", observable_);
			return ProcedureNode::Failure;
	}

	return ProcedureNode::Success;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool CalculateProcedureNode::read(LineParser& parser, const CoreData& coreData)
{
	// The current line in the parser must contain a label (name) for the node, and which must not currently exist on the scope stack
	if (parser.nArgs() != 2) return Messenger::error("A Calculate node must be given a suitable name.\n");
	setName(parser.argc(1));

	// Read until we encounter the EndCalculate keyword, or we fail for some reason
	while (!parser.eofOrBlank())
	{
		// Read and parse the next line
		if (parser.getArgsDelim() != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (!calculateNodeKeywords().isValid(parser.argc(0))) return calculateNodeKeywords().errorAndPrintValid(parser.argc(0));
		CalculateNodeKeyword nk = calculateNodeKeywords().enumeration(parser.argc(0));
		if (!calculateNodeKeywords().validNArgs(nk, parser.nArgs()-1)) return false;

		// All OK, so process it
		switch (nk)
		{
			case (CalculateProcedureNode::AngleKeyword):
				observable_ = CalculateProcedureNode::AngleObservable;

				// First Site argument (point 'i' in angle i-j-k)
				sites_[0] = dynamic_cast<SelectProcedureNode*>(nodeInScope(parser.argc(1), ProcedureNode::SelectNode));
				if (!sites_[0]) return Messenger::error("Unrecognised site reference '%s' given to %s keyword.\n", parser.argc(1), calculateNodeKeywords().keyword(CalculateProcedureNode::AngleKeyword));

				// Second Site argument (point 'j' in angle i-j-k)
				sites_[1] = dynamic_cast<SelectProcedureNode*>(nodeInScope(parser.argc(2), ProcedureNode::SelectNode));
				if (!sites_[1]) return Messenger::error("Unrecognised site reference '%s' given to %s keyword.\n", parser.argc(2), calculateNodeKeywords().keyword(CalculateProcedureNode::AngleKeyword));

				// Third Site argument (point 'k' in angle i-j-k)
				sites_[2] = dynamic_cast<SelectProcedureNode*>(nodeInScope(parser.argc(3), ProcedureNode::SelectNode));
				if (!sites_[2]) return Messenger::error("Unrecognised site reference '%s' given to %s keyword.\n", parser.argc(3), calculateNodeKeywords().keyword(CalculateProcedureNode::AngleKeyword));
				break;
			case (CalculateProcedureNode::DistanceKeyword):
				observable_ = CalculateProcedureNode::DistanceObservable;

				// First Site argument (point 'i' in distance i-j)
				sites_[0] = dynamic_cast<SelectProcedureNode*>(nodeInScope(parser.argc(1), ProcedureNode::SelectNode));
				if (!sites_[0]) return Messenger::error("Unrecognised site reference '%s' given to %s keyword.\n", parser.argc(1), calculateNodeKeywords().keyword(CalculateProcedureNode::DistanceKeyword));

				// Second Site argument (point 'j' in distance i-j)
				sites_[1] = dynamic_cast<SelectProcedureNode*>(nodeInScope(parser.argc(2), ProcedureNode::SelectNode));
				if (!sites_[1]) return Messenger::error("Unrecognised site reference '%s' given to %s keyword.\n", parser.argc(2), calculateNodeKeywords().keyword(CalculateProcedureNode::DistanceKeyword));
				break;
			case (CalculateProcedureNode::EndCalculateKeyword):
				// Check that valid Observable was set
				if (observable_ == nObservables) return Messenger::error("A quantity to calculate must be specified in %s.\n", ProcedureNode::nodeTypes().keyword(type_));
				return true;
			case (CalculateProcedureNode::nCalculateNodeKeywords):
				return Messenger::error("Unrecognised Calculate node keyword '%s' found.\n", parser.argc(0));
				break;
			default:
				return Messenger::error("Epic Developer Fail - Don't know how to deal with the Calculate node keyword '%s'.\n", parser.argc(0));
		}
	}

	return true;
}

// Write structure to specified LineParser
bool CalculateProcedureNode::write(LineParser& parser, const char* prefix)
{
	// Block Start
	if (!parser.writeLineF("%s%s\n", ProcedureNode::nodeTypes().keyword(type_))) return false;

	// Observable target
	if (observable_ == CalculateProcedureNode::AngleObservable)
	{
		if (!parser.writeLineF("%s  %s  '%s'  '%s'  '%s'\n", prefix, calculateNodeKeywords().keyword(CalculateProcedureNode::AngleKeyword), sites_[0]->name(), sites_[1]->name(), sites_[2]->name())) return false;
	}
	else if (observable_ == CalculateProcedureNode::DistanceObservable)
	{
		if (!parser.writeLineF("%s  %s  '%s'  '%s'\n", prefix, calculateNodeKeywords().keyword(CalculateProcedureNode::DistanceKeyword), sites_[0]->name(), sites_[1]->name())) return false;
	}
	else if (observable_ == CalculateProcedureNode::VectorObservable)
	{
		if (!parser.writeLineF("%s  %s  '%s'  '%s'\n", prefix, calculateNodeKeywords().keyword(CalculateProcedureNode::VectorKeyword), sites_[0]->name(), sites_[1]->name())) return false;
	}

	// Block End
	if (!parser.writeLineF("%s%s\n", calculateNodeKeywords().keyword(CalculateProcedureNode::EndCalculateKeyword))) return false;

	return true;
}
