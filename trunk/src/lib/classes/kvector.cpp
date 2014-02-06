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
#include "braggpeak.h"

/*!
 * \brief Constructor
 * \details Constructor for KVector. 
 */
KVector::KVector(int h, int k, int l, BraggPeak* bp, int nAtomTypes)
{
	hkl_.set(h, k, l);
	braggPeak_ = bp;

	// Create atomtype contribution arrays
	cosTerms_.initialise(nAtomTypes);
	sinTerms_.initialise(nAtomTypes);

	prev = NULL;
	next = NULL;
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
	braggPeak_ = source.braggPeak_;
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
 * \brief Return associated BraggPeak
 */
const BraggPeak* KVector::braggPeak() const
{
	return braggPeak_;
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
 * \brief Calculate intensities from stored cos and sin term arrays
 * \details Calculate the intensities, per atom type pair, from the accumulated cos and sin arrays, and sum the results into the associated BraggPeak.
 */
void KVector::calculateIntensities()
{
#ifdef CHECKS
	if (braggPeak_ == NULL)
	{
		msg.print("NULL_POINTER - NULL braggPeak_ found in KVector::calculateIntensities().\n");
		return;
	}
#endif
	// Calculate final intensities from stored cos/sin terms
	// Take account of the half-sphere, doubling intensities of all k-vectors not on h == 0
	// Take account of the half-matrix of atom types, doubling contributions from dissimilar types
	int i, j, nTypes = cosTerms_.nItems(), halfSphereNorm = (hkl_.x == 0 ? 1 : 2);
	double intensity;
	msg.print("KVector %3i %3i %3i  %f  ", hkl_.x, hkl_.y, hkl_.z, braggPeak_->q());
	braggPeak_->addKVectors(halfSphereNorm);
	for (i = 0; i<nTypes; ++i)
	{
		for (j = i; j < nTypes; ++j)
		{
			intensity = (cosTerms_[i]*cosTerms_[j] + sinTerms_[i]*sinTerms_[j]) * halfSphereNorm;
			braggPeak_->addIntensity(i, j, intensity);
// 			msg.print("%f   ", (i == j ? intensity : 2.0*intensity) / 9.0);
		}
	}
	msg.print("\n");
}

/*!
 * \brief Return specified intensity
 */
double KVector::intensity(int typeI, int typeJ)
{
	return (cosTerms_[typeI]*cosTerms_[typeJ] + sinTerms_[typeI]*sinTerms_[typeJ]) * (hkl_.x == 0 ? 1 : 2);
}
