/*
	*** Keyword - Link To Other Keyword
	*** src/keywords/linkto.h
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

#ifndef DISSOLVE_KEYWORD_LINKTO_H
#define DISSOLVE_KEYWORD_LINKTO_H

#include "keywords/data.h"

// Forward Declarations
/* none */

// Keyword with Link to Other Keyword
template <class D> class LinkToKeyword : public KeywordData<D>
{
	public:
	// Constructors
	LinkToKeyword(KeywordData<D>* keywordData) : KeywordData< KeywordData<D>* >(KeywordBase::LinkToKeywordData, keywordData)
	{
	}
	// Destructor
	~LinkToKeyword()
	{
	}


	/*
	 * Base Pointer Return
	 */
	public:
	// Return base pointer for this (may be overloaded to provide access to other KeywordBase instance)
	KeywordBase* base()
	{
		return KeywordData< KeywordData<D>* >::data_;
	}


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	int minArguments()
	{
		return KeywordData< KeywordData<D>* >::data_->minArguments();
	}
	// Return maximum number of arguments accepted
	int maxArguments()
	{
		return KeywordData< KeywordData<D>* >::data_->maxArguments();
	}
	// Parse arguments from supplied LineParser, starting at given argument offset
	bool read(LineParser& parser, int startArg, const CoreData& coreData)
	{
		return KeywordData< KeywordData<D>* >::data_->read(parser, startArg, coreData);
	}
	// Write keyword data to specified LineParser
	bool write(LineParser& parser, const char* prefix)
	{
		return KeywordData< KeywordData<D>* >::data_->write(parser, prefix);
	}


	/*
	 * Conversion
	 */
	public:
	// Return value (as bool)
	bool asBool()
	{
		return KeywordData< KeywordData<D>* >::data_->asBool();
	}
	// Return value (as int)
	int asInt()
	{
		return KeywordData< KeywordData<D>* >::data_->asInt();
	}
	// Return value (as double)
	double asDouble()
	{
		return KeywordData< KeywordData<D>* >::data_->asDouble();
	}
	// Return value (as string)
	const char* asString()
	{
		return KeywordData< KeywordData<D>* >::data_->asString();
	}
	// Return value as Vec3<int>
	Vec3<int> asVec3Int()
	{
		return KeywordData< KeywordData<D>* >::data_->asVec3Int();
	}
	// Return value as Vec3<double>
	Vec3<double> asVec3Double()
	{
		return KeywordData< KeywordData<D>* >::data_->asVec3Double();
	}


	/*
	 * Object Management
	 */
	protected:
	// Prune any references to the supplied AtomType in the contained data
	void removeReferencesTo(AtomType* at)
	{
		KeywordData< KeywordData<D>* >::data_->removeReferencesTo(at);
	}
	// Prune any references to the supplied Configuration in the contained data
	void removeReferencesTo(Configuration* cfg)
	{
		KeywordData< KeywordData<D>* >::data_->removeReferencesTo(cfg);
	}
	// Prune any references to the supplied Isotopologue in the contained data
	void removeReferencesTo(Isotopologue* iso)
	{
		KeywordData< KeywordData<D>* >::data_->removeReferencesTo(iso);
	}
	// Prune any references to the supplied Module in the contained data
	void removeReferencesTo(Module* module)
	{
		KeywordData< KeywordData<D>* >::data_->removeReferencesTo(module);
	}
	// Prune any references to the supplied Species in the contained data
	void removeReferencesTo(Species* sp)
	{
		KeywordData< KeywordData<D>* >::data_->removeReferencesTo(sp);
	}
	// Prune any references to the supplied SpeciesSite in the contained data
	void removeReferencesTo(SpeciesSite* spSite)
	{
		KeywordData< KeywordData<D>* >::data_->removeReferencesTo(spSite);
	}
	// Prune any references to the supplied ProcedureNode in the contained data
	void removeReferencesTo(ProcedureNode* node)
	{
		KeywordData< KeywordData<D>* >::data_->removeReferencesTo(node);
	}
};

#endif

