/*
	*** KVector
	*** src/lib/classes/kvector.cpp
	Copyright T. Youngs 2012-2014

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/kvector.h"

/*!
 * \brief Constructor
 * \details Constructor for KVector. 
 */
KVector::KVector(int h, int k, int l, double magnitude, int nAtomTypes)
{
	hkl_.set(h, k, l);
	magnitude_ = magnitude;
	squaredIntegerMagnitude_ = h*h + k*k + l*l;

	// Create atomtype contribution arrays
	cosTerms_.createEmpty(nAtomTypes);
	sinTerms_.createEmpty(nAtomTypes);
}

/*!
 * \brief Destructor
 * \details Destructor for KVector. 
 */
KVector::~KVector()
{
}

/*!
 * \brief Copy constructor
 */
KVector::KVector(const KVector& source)
{
	(*this) = source;
}

/*!
 * \brief Operator=
 */
void KVector::operator=(const KVector& source)
{
	hkl_ = source.hkl_;
	magnitude_ = source.magnitude_;
	squaredIntegerMagnitude_ = source.squaredIntegerMagnitude_;
	cosTerms_ = source.cosTerms_;
	sinTerms_ = source.sinTerms_;
}

/*
// KVector Data
*/

/*!
 * \brief Return integer hkl indices
 */
const Vec3<int>& KVector::hkl() const
{
	return hkl_;
}

/*!
 *\brief Return h index
 */
const int KVector::h() const
{
	return hkl_.x;
}

/*!
 * \brief Return k index
 */
const int KVector::k() const
{
	return hkl_.y;
}

/*!
 * \brief Return l index
 */
const int KVector::l() const
{
	return hkl_.z;
}

/*!
 * \brief Return magnitude
 */
const double KVector::magnitude() const
{
	return magnitude_;
}

/*!
 * \brief Return index (squaredIntegerMagnitude_)
 * \details Return the squaredIntegerMagnitude_ of the k-vector, so that they can be stored usefully in an OrderedPointerList.
 */
const int KVector::index() const
{
	return squaredIntegerMagnitude_;
}

/*!
 * \brief Zero cos/sin term arrays
 */
void KVector::zeroCosSinTerms()
{
	cosTerms_ = 0.0;
	sinTerms_ = 0.0;
}

/*!
 * \brief Add value to cosTerm index specified
 */
void KVector::addCosTerm(int atomTypeIndex, double value)
{
#ifdef CHECKS
	if (atomTypeIndex >= cosTerms_.nItems())
	{
		msg.print("BAD_USAGE - KVector::cosTerms_ index supplied (%i) is greated than the size of the array (%i).\n", atomTypeIndex, cosTerms_.nItems());
		return;
	}
#endif
	cosTerms_[atomTypeIndex] += value;
}

/*!
 * \brief Add value to sinTerm index specified
 */
void KVector::addSinTerm(int atomTypeIndex, double value)
{
#ifdef CHECKS
	if (atomTypeIndex >= sinTerms_.nItems())
	{
		msg.print("BAD_USAGE - KVector::sinTerms_ index supplied (%i) is greated than the size of the array (%i).\n", atomTypeIndex, sinTerms_.nItems());
		return;
	}
#endif
	sinTerms_[atomTypeIndex] += value;
}

/*!
 * \brief Return cosTerm array
 */
Array<double>& KVector::cosTerms()
{
	return cosTerms_;
}

/*!
 * \brief Return sinTerm array
 */
Array<double>& KVector::sinTerms()
{
	return sinTerms_;
}
