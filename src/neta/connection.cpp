/*
	*** NETA Connection Node
	*** src/neta/connection.cpp
	Copyright T. Youngs 2019

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

#include "neta/connection.h"
#include "data/ffatomtype.h"
#include "classes/speciesatom.h"
#include "templates/dynamicarray.h"
#include "templates/refdatalist.h"

// Constructor
NETAConnectionNode::NETAConnectionNode(NETADefinition* parent, PointerArray<Element> targetElements, PointerArray<ForcefieldAtomType> targetAtomTypes, SpeciesBond::BondType bt, NETANode* innerDefinition) : NETANode(parent, NETANode::ConnectionNode)
{
	allowedElements_ = targetElements;
	allowedAtomTypes_ = targetAtomTypes;
	bondType_ = bt;
	innerDefinition_ = innerDefinition;
}

// Destructor
NETAConnectionNode::~NETAConnectionNode()
{
}

/*
 * Scoring
 */

// Evaluate the node and return its score
int NETAConnectionNode::score(const SpeciesAtom* i, RefList<const SpeciesAtom>& matchPath) const
{
			printf("I AM THE CONNECTION\n");
	// Get directly connected atoms about 'i', excluding any that have already been matched
	RefDataList<const SpeciesAtom, int> neighbours;
	const PointerArray<SpeciesBond>& bonds = i->bonds();
	for (int n=0; n<bonds.nItems(); ++n)
	{
		const SpeciesAtom* j = bonds.at(n)->partner(i);
		if (matchPath.contains(j)) continue;
		neighbours.append(j, NETANode::NoMatch);
	}

	// Loop over neighbour atoms
	const int nRequiredXXX = 1;
	int nMatches = 0;
	RefDataListIterator<const SpeciesAtom,int> neighbourIterator(neighbours);
	while (const SpeciesAtom* j = neighbourIterator.iterate())
	{
		// Evaluate the neighbour against our elements
		int atomScore = NETANode::NoMatch;
		for (int n=0; n<allowedElements_.nItems(); ++n)
		{
			if (j->element() != allowedElements_.at(n)) continue;

			// Score the element matchadd
			atomScore = 1;

			// Process inner definition if present
			if (innerDefinition_)
			{
				int innerScore = innerDefinition_->score(j, matchPath);
				if (innerScore == NETANode::NoMatch) continue;

				atomScore += innerScore;
			}

			// Now have a match, so break out of the loop
			break;
		}
		if (atomScore == NETANode::NoMatch) for (int n=0; n<allowedAtomTypes_.nItems(); ++n)
		{
			// Evaluate the neighbour against the atom type
			atomScore = allowedAtomTypes_.at(n)->neta().score(j);
			if (atomScore == NETANode::NoMatch) continue;

			// Atom was a match - process inner definition if present
			if (innerDefinition_)
			{
				int innerScore = innerDefinition_->score(j, matchPath);
				if (innerScore == NETANode::NoMatch) continue;

				atomScore += innerScore;
			}

			// Now have a match, so break out of the loop
			break;
		}

		// Did we match the atom?
		if (atomScore == NETANode::NoMatch) continue;

		// Found a match, so increase the match count and store the score
		++nMatches;
		neighbourIterator.currentData() = atomScore;

		// Have we matched enough? If so break out early.
		if (nMatches == nRequiredXXX) break;
	}

	// Did we find the required number of matches in the neighbour list?
	if (nMatches != nRequiredXXX) return NETANode::NoMatch;

	// Flag each of the matched atoms as being part of the matched path, and generate total score
	int totalScore = 0;
	neighbourIterator.restart();
	while (const SpeciesAtom* j = neighbourIterator.iterate())
	{
		if (neighbourIterator.currentData() == NETANode::NoMatch) continue;

		totalScore += neighbourIterator.currentData();
		matchPath.append(j);
	}

	return totalScore;

// 	// Check for reverse logic
// 	if (reverseLogic_) totalscore = (totalscore == -1 ? 1 : -1);
// 	RefListItem<Atom,int>* ri;
// 	RefList< RefListItem<Atom,int>, int > scores;
// 	RefListItem< RefListItem<Atom,int>, int > *si;
// 	// Pointer check
// 	if (nbrs == NULL)
// 	{
// 		Messenger::print("NETA Internal Error: Called NETAConnectionNode::score() without a valid neighbour list.");
// 		Messenger::exit("NETAConnectionNode::score");
// 		return -1;
// 	}
// 	// Exactly how we proceed here depends on the current context (i.e. whether in Ring, Chain, etc.)
// 	if (nbrs->nItems() == 0) totalscore = -1;
// 	else
// 	{
// 		for (ri = nbrs->first(); ri != NULL; ri = ri->next)
// 		{
// 			// Does this bound neighbour match the element/type of the BoundNode?
// 			si = scores.add(ri, atomScore(ri->item));
// 			if (si->data == -1) continue;
// 			// Connection type?
// 			if ((context->nodeType() != NETANode::RingNode) && (bondType_ != Bond::Any))
// 			{
// 				Bond* b = target->findBond(ri->item);
// 				if (b == NULL)
// 				{
// 					Messenger::print("NETA Internal Error: Couldn't find bond between atom ids %i and %i to check type.", target->id(), ri->item->id());
// 					Messenger::exit("NETAConnectionNode::score");
// 					return -1;
// 				}
// 				if (b->type() == bondType_) si->data ++;
// 				else si->data = -1;
// 			}
// 			if (si->data == -1) continue;
// 			// Is there an inner NETA description to test?
// 			if (innerNETA_ == NULL) continue;
// 			else
// 			{
// 				// For each bound neighbour here, construct its own list of bound atoms and rings and check the inner NETA score
// 				RefList<SpeciesAtom> boundList;
// 				ri->item->addBoundToRefList(&boundList);
// 				// Construct new ringlist
// 				RefList<Ring> ringList;
// 				for (Ring* r = parent()->targetRingList()->first(); r != NULL; r = r->next) if (r->containsAtom(ri->item)) ringList.add(r);
// 				path.add(target);
// 				boundscore = innerNETA_->score(ri->item, &boundList, &ringList, this, path, level+1);
// 				path.removeLast();
// 				if (boundscore != -1) si->data += boundscore;
// 				else si->data = -1;
// 			}
// 		}
// 		n = 0;
// 		for (si = scores.first(); si != NULL; si = si->next) if (si->data > 0) ++n;
// 		if (n == 0) totalscore = -1;
// 		else if ((repeat_ == -1) || (NETA::netaValueCompare(n, repeatComparison_, repeat_)))
// 		{
// 			n = repeat_ == -1 ? 1 : repeat_;
// 			ri = nbrs->first();
// 			totalscore = 0;
// 			for (si = scores.first(); si != NULL; si = si->next)
// 			{
// 				if (si->data > 0)
// 				{
// 					totalscore += si->data;
// 					nbrs->remove(si->item);
// 					--n;
// 				}
// 				if (n == 0) break;
// 			}
// 		}
// 		else totalscore = -1;
// 		if (totalscore == 0) totalscore = -1;
// 	}
// 	// Check for reverse logic
// 	if (reverseLogic_) totalscore = (totalscore == -1 ? 1 : -1);
// 	NETANode::printScore(level, "Bound Check (%i of %s) = %i", repeat_ == -1 ? 1 : repeat_, qPrintable(elementsAndTypesString()), totalscore);
// 
// 	return totalscore;
}
