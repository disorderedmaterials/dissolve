/*
	*** Module Keyword - Double Triplet
	*** src/module/keywordtypes/vec3double.cpp
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

#include "module/keywordtypes/vec3double.h"
#include "base/lineparser.h"
#include "genericitems/listhelper.h"

// Constructors
Vec3DoubleModuleKeyword::Vec3DoubleModuleKeyword(Vec3<double> value) : ModuleKeywordData< Vec3<double> >(ModuleKeywordBase::Vec3DoubleData, value)
{
	minimumLimit_ = false;
	maximumLimit_ = false;
}

Vec3DoubleModuleKeyword::Vec3DoubleModuleKeyword(Vec3<double> value, Vec3<double> minValue) : ModuleKeywordData< Vec3<double> >(ModuleKeywordBase::Vec3DoubleData, value)
{
	minimumLimit_ = true;
	min_ = minValue;
	maximumLimit_ = false;
}

Vec3DoubleModuleKeyword::Vec3DoubleModuleKeyword(Vec3<double> value, Vec3<double> minValue, Vec3<double> maxValue) : ModuleKeywordData< Vec3<double> >(ModuleKeywordBase::Vec3DoubleData, value)
{
	minimumLimit_ = true;
	min_ = minValue;
	maximumLimit_ = true;
	max_ = maxValue;
}

// Destructor
Vec3DoubleModuleKeyword::~Vec3DoubleModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool Vec3DoubleModuleKeyword::isSet()
{
	return set_;
}

/*
 * Data Validation
 */

// Return whether a minimum validation limit has been set for supplied index
bool Vec3DoubleModuleKeyword::hasValidationMin(int index)
{
	return minimumLimit_[index];
}

// Return validation minimum limit for supplied index
double Vec3DoubleModuleKeyword::validationMin(int index)
{
	return min_[index];
}

// Return whether a maximum validation limit has been set for supplied index
bool Vec3DoubleModuleKeyword::hasValidationMax(int index)
{
	return maximumLimit_[index];
}

// Return validation maximum limit for supplied index
double Vec3DoubleModuleKeyword::validationMax(int index)
{
	return max_[index];
}

// Validate supplied value
bool Vec3DoubleModuleKeyword::isValid(Vec3<double> value)
{
	if (!isValid(0, value.x)) return false;
	if (!isValid(1, value.y)) return false;
	if (!isValid(2, value.z)) return false;

	return true;
}

// Validate supplied value
bool Vec3DoubleModuleKeyword::isValid(int index, double value)
{
	// Check minimum limit
	if (minimumLimit_[index])
	{
		if (value < min_[index]) return false;
	}

	// Check maximum limit
	if (maximumLimit_[index])
	{
		if (value > max_[index]) return false;
	}

	return true;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Vec3DoubleModuleKeyword::minArguments()
{
	return 3;
}

// Return maximum number of arguments accepted
int Vec3DoubleModuleKeyword::maxArguments()
{
	return 3;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool Vec3DoubleModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	if (parser.hasArg(startArg+2))
	{
		// Check individual components of the vector
		for (int n=0; n<3; ++n)
		{
			if (!isValid(n, parser.argd(startArg)))
			{
				if (minimumLimit_[n] && maximumLimit_[n]) Messenger::error("Value %i is out of range for keyword. Valid range is %i <= n <= %i.\n", data_[n], min_[n], max_[n]);
				else if (minimumLimit_[n]) Messenger::error("Value %i is out of range for keyword. Valid range is %i <= n.\n", data_[n], min_[n]);
				else Messenger::error("Value %i is out of range for keyword. Valid range is n <= %i.\n", data_[n], max_[n]);

				return false;
			}
		}

		if (!setData(parser.arg3d(startArg))) return false;

		return true;
	}
	return false;
}

// Write keyword data to specified LineParser
bool Vec3DoubleModuleKeyword::write(LineParser& parser, const char* prefix)
{
	return parser.writeLineF("%s%s  %i  %i  %i\n", prefix, keyword(), data_.x, data_.y, data_.z);
}

/*
 * Conversion
 */

// Return value (as Vec3<int>)
Vec3<int> Vec3DoubleModuleKeyword::asVec3Int()
{
	return Vec3<int>(data_.x, data_.y, data_.z);
}

// Return value (as Vec3<double>)
Vec3<double> Vec3DoubleModuleKeyword::asVec3Double()
{
	return data_;
}
