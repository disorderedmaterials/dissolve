/*
	*** Torsion Definition
	*** src/classes/torsion.cpp
	Copyright T. Youngs 2012-2017

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

#include "classes/torsion.h"
#include "classes/speciestorsion.h"
#include "classes/atom.h"
#include "base/processpool.h"

// Constructor
Torsion::Torsion() : DynamicArrayObject<Torsion>()
{
	speciesTorsion_ = NULL;
	molecule_ = NULL;
	i_ = NULL;
	j_ = NULL;
	k_ = NULL;
	l_ = NULL;
	nAttached_[0] = 0;
	nAttached_[1] = 0;
	attached_[0] = NULL;
	attached_[1] = NULL;
}

// Destructor
Torsion::~Torsion()
{
}

/*
 * Basic Data
 */

// Set Molecule in which this Torsion exists
void Torsion::setMolecule(Molecule* parent)
{
	molecule_ = parent;
}

// Return Molecule in which this Torsion exists
Molecule* Torsion::molecule() const
{
	return molecule_;
}

/*
 * Atom Information
 */

// Set Atoms involved in Torsion
void Torsion::setAtoms(Atom* i, Atom* j, Atom* k, Atom* l)
{
	i_ = i;
	j_ = j;
	k_ = k;
	l_ = l;
#ifdef CHECKS
	if (i_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* i in Torsion::set().\n");
	if (j_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* j in Torsion::set().\n");
	if (k_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* k in Torsion::set().\n");
	if (l_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* l in Torsion::set().\n");
#endif
}

// Return first Atom involved in Torsion
Atom* Torsion::i() const
{
	return i_;
}

// Return second (central) Atom involved in Torsion
Atom* Torsion::j() const
{
	return j_;
}

// Return third Atom involved in Torsion
Atom* Torsion::k() const
{
	return k_;
}

// Return fourth Atom involved in Torsion
Atom* Torsion::l() const
{
	return l_;
}

// Return whether Atoms in Torsion match those specified
bool Torsion::matches(Atom* i, Atom* j, Atom* k, Atom* l) const
{
	if (i_ == i)
	{
		if (j_ != j) return false;
		if (k_ != k) return false;
		if (l_ == l) return true;
	}
	else if (i_ == l)
	{
		if (j_ != k) return false;
		if (k_ != j) return false;
		if (l_ == i) return true;
	}

	return false;
}

/*
 * Connections
 */

// Create attached Atom array
void Torsion::createAttachedAtomArray(int terminus, int size)
{
	if (attached_[terminus] != NULL) delete[] attached_[terminus];
	attached_[terminus] = NULL;
	nAttached_[terminus] = size;

	if (nAttached_[terminus] != 0)
	{
		attached_[terminus] = new Atom*[nAttached_[terminus]];
		for (int n=0; n<nAttached_[terminus]; ++n) attached_[terminus][n] = NULL;
	}
}

// Set attached Atoms for terminus specified
void Torsion::setAttachedAtoms(int terminus, const RefList<Atom,int>& atoms)
{
	createAttachedAtomArray(terminus, atoms.nItems());
	int index = 0;
	for (RefListItem<Atom,int>* refAtom = atoms.first(); refAtom != NULL; refAtom = refAtom->next) attached_[terminus][index++] = refAtom->item;
}

// Return number of attached Atoms for terminus specified
int Torsion::nAttached(int terminus) const
{
	return nAttached_[terminus];
}

// Return array of attached Atoms for terminus specified
Atom** Torsion::attached(int terminus) const
{
	return attached_[terminus];
}

/*
 * Energy / Force
 */

// Set SpeciesTorsion reference
void Torsion::setSpeciesTorsion(SpeciesTorsion* torsionRef)
{
	speciesTorsion_ = torsionRef;
}

// Return SpeciesTorsion reference
SpeciesTorsion* Torsion::speciesTorsion()
{
	return speciesTorsion_;
}

// Return energy for specified torsion
double Torsion::energy(double torsionInDegrees) const
{
	return speciesTorsion_->energy(torsionInDegrees);
}

// Return force multiplier for specified torsion
double Torsion::force(double torsionInDegrees) const
{
	return speciesTorsion_->force(torsionInDegrees);
}

// // Torsion energy
// void Pattern::torsionEnergy(Model* srcmodel, EnergyStore* estore, int molecule)
// {
// 	// Calculate the energy of the torsions in this pattern with coordinates from *xcfg
// 	Messenger::enter("Pattern::torsionEnergy");
// 	int i,j,k,l,aoff,m1;
// 	static double k0, k1, k2, k3, k4, k5, k6, k7, k8, k9, eq, phi, energy, period, s, chi;
// 	PatternBound* pb;
// 	ForcefieldBound* ffb;
// 	Vec3<double> vecij, veckj;
// 	energy = 0.0;
// 	aoff = (molecule == -1 ? startAtom_ : startAtom_ + molecule*nAtoms_);
// 	for (m1=(molecule == -1 ? 0 : molecule); m1<(molecule == -1 ? nMolecules_ : molecule + 1); m1++)
// 	{
// 		for (pb = torsions_.first(); pb != NULL; pb = pb->next)
// 		{
// 			// Grab atom indices and calculate torsion angle (in radians)
// 			i = pb->atomId(0) + aoff;
// 			j = pb->atomId(1) + aoff;
// 			k = pb->atomId(2) + aoff;
// 			l = pb->atomId(3) + aoff;
// 			phi = srcmodel->torsion(i,j,k,l) / DEGRAD;
// 			ffb = pb->data();
// 			// Calculate energy
// 			switch (pb->data()->torsionForm())
// 			{
// 				case (TorsionFunctions::None):
// 					Messenger::print("Warning: No function is specified for torsion energy %i-%i-%i-%i.", i, j, k, l);
// 				case (TorsionFunctions::Ignore):
// 					break;
// 				case (TorsionFunctions::Cosine): 
// 					// U(phi) = forcek * (1 + s*cos(period*phi - eq))
// 					k1 = ffb->parameter(TorsionFunctions::CosineK);
// 					eq = ffb->parameter(TorsionFunctions::CosineEq) / DEGRAD;
// 					period = ffb->parameter(TorsionFunctions::CosineN);
// 					s = ffb->parameter(TorsionFunctions::CosineS);
// 					energy += k1 * (1.0 + s * cos(period*phi - eq));
// 					break;
// 				case (TorsionFunctions::Cos3):
// 					// U(phi) = 0.5 * ( k1*(1+cos(phi)) + k2*(1-cos(2*phi)) + k3*(1+cos(3*phi)) )
// 					k1 = ffb->parameter(TorsionFunctions::Cos3K1);
// 					k2 = ffb->parameter(TorsionFunctions::Cos3K2);
// 					k3 = ffb->parameter(TorsionFunctions::Cos3K3);
// 					energy += 0.5 * (k1 * (1.0 + cos(phi)) + k2 * (1.0 - cos(2.0*phi)) + k3 * (1.0 + cos(3.0*phi)));
// 					break;
// 				case (TorsionFunctions::Cos4):
// 					// U(phi) = 0.5 * ( k1*(1+cos(phi)) + k2*(1-cos(2*phi)) + k3*(1+cos(3*phi)) + k4*(1-cos(4*phi)) )
// 					k1 = ffb->parameter(TorsionFunctions::Cos4K1);
// 					k2 = ffb->parameter(TorsionFunctions::Cos4K2);
// 					k3 = ffb->parameter(TorsionFunctions::Cos4K3);
// 					k4 = ffb->parameter(TorsionFunctions::Cos4K4);
// 					energy += 0.5 * (k1*(1.0+cos(phi)) + k2*(1.0-cos(2.0*phi)) + k3*(1.0+cos(3.0*phi)) + k4*(1.0-cos(4.0*phi)) );
// 					break;
// 				case (TorsionFunctions::Cos3C):
// 					// U(phi) = k0 + 0.5 * ( k1*(1+cos(phi)) + k2*(1-cos(2*phi)) + k3*(1+cos(3*phi)) )
// 					k0 = ffb->parameter(TorsionFunctions::Cos3CK0);
// 					k1 = ffb->parameter(TorsionFunctions::Cos3CK1);
// 					k2 = ffb->parameter(TorsionFunctions::Cos3CK2);
// 					k3 = ffb->parameter(TorsionFunctions::Cos3CK3);
// 					energy += k0 + 0.5 * (k1*(1.0+cos(phi)) + k2*(1.0-cos(2.0*phi)) + k3*(1.0+cos(3.0*phi)) );
// 					break;
// 				case (TorsionFunctions::CosCos):
// 					// U(phi) = 0.5 * k * (1 - cos(n*eq) * cos(n*theta))
// 					k1 = ffb->parameter(TorsionFunctions::CosCosK);
// 					period = ffb->parameter(TorsionFunctions::CosCosN);
// 					eq = ffb->parameter(TorsionFunctions::CosCosEq) / DEGRAD;
// 					energy += 0.5 * k1 * (1.0 - cos(period*eq)*cos(period*phi));
// 					break;
// 				case (TorsionFunctions::Dreiding):
// 					// U(phi) = 0.5 * k * (1 - cos(n*(theta-eq))
// 					k1 = ffb->parameter(TorsionFunctions::DreidingK);
// 					period = ffb->parameter(TorsionFunctions::DreidingN);
// 					eq = ffb->parameter(TorsionFunctions::DreidingEq) / DEGRAD;
// 					energy += 0.5 * k1 * (1.0 - cos(period*(phi - eq)));
// 					break;
// 				case (TorsionFunctions::Pol9):
// 					// U(chi) = sum_{i=0,8} k_i (cos(chi))^i
// 					chi = PI - phi;
// 					k1 = ffb->parameter(TorsionFunctions::Pol9K1);
// 					k2 = ffb->parameter(TorsionFunctions::Pol9K2);
// 					k3 = ffb->parameter(TorsionFunctions::Pol9K3);
// 					k4 = ffb->parameter(TorsionFunctions::Pol9K4);
// 					k5 = ffb->parameter(TorsionFunctions::Pol9K5);
// 					k6 = ffb->parameter(TorsionFunctions::Pol9K6);
// 					k7 = ffb->parameter(TorsionFunctions::Pol9K7);
// 					k8 = ffb->parameter(TorsionFunctions::Pol9K8);
// 					k9 = ffb->parameter(TorsionFunctions::Pol9K9);
// 					energy += k1 + cos(chi)*(k2 + cos(chi)*(k3 + cos(chi)*(k4 + cos(chi)*(k5 + cos(chi)*(k6 + cos(chi)*(k7 + cos(chi)*(k8 + cos(chi)*k9)))))));
// 				default:
// 					Messenger::print("No equation coded for torsion energy of type '%s'.",  TorsionFunctions::functionData[ffb->torsionForm()].name);
// 					break;
// 			}
// 			//printf("TENG - molstart = %i: %i-%i-%i-%i (%i-%i-%i-%i) = %f (tot = %f)\n",aoff,i,j,k,l,pb->atomId(0),pb->atomId(1),pb->atomId(2),pb->atomId(3), phi,energy);
// 		}
// 		aoff += nAtoms_;
// 	}
// 	// Increment energy for pattern
// 	estore->add(EnergyStore::TorsionEnergy,energy,id_);
// 	Messenger::exit("Pattern::torsionEnergy");
// }
// 
// // Returns a unit vector in the specified direction
// Vec3<double> unit_vector(int n)
// {
// 	Vec3<double> result;
// 	result.zero();
// 	result.set(n,1.0);
// 	return result;
// }
// 
// // Return cyclic permutation of the integer provided
// int cp(int n)
// {
// 	return (n%3);
// }
// 
// // Construct 'cross-product' vector of the supplied vector using cyclic permutations
// Matrix make_cp_mat(Vec3<double>& v)
// {
// 	Matrix result;
// 	Vec3<double> temp;
// 	for (int n=0; n<3; ++n)
// 	{
// 		temp = unit_vector(cp(n+1)) * v.get(cp(n+2)) - unit_vector(cp(n+2)) * v.get(cp(n+1));
// 		result.setColumn(n,temp.x,temp.y,temp.z,0.0);
// 	}
// 	return result;
// }
// 
// // Torsion forces
// void Pattern::torsionForces(Model* srcmodel)
// {
// 	// Calculate force contributions from the torsions in this pattern with coordinates from *xcfg
// 	Messenger::enter("Pattern::torsionForces");
// 	int i,j,k,l,aoff,m1;
// 	static Vec3<double> vec_ji, vec_jk, vec_kl, xpj, xpk, dcos_dxpj, dcos_dxpk, temp;
// 	Matrix dxpj_dij, dxpj_dkj, dxpk_dkj, dxpk_dlk;
// 	static double phi, dp, forcek, period, eq, mag_ji, mag_jk, mag_kl, mag_xpj, mag_xpk, du_dphi, dphi_dcosphi;
// 	static Vec3<double> fi, fj, fk, fl;
// 	ForcefieldBound* ffb;
// 	static double k1, k2, k3, k4, s;
// 	PatternBound* pb;
// 	Atom** modelatoms = srcmodel->atomArray();
// 	UnitCell& cell = srcmodel->cell();
// 	aoff = startAtom_;
// 	for (m1=0; m1<nMolecules_; m1++)
// 	{
// 		for (pb = torsions_.first(); pb != NULL; pb = pb->next)
// 		{
// 			// Calculate general components of the torsion forces
// 			// Grab atomic indices
// 			i = pb->atomId(0) + aoff;
// 			j = pb->atomId(1) + aoff;
// 			k = pb->atomId(2) + aoff;
// 			l = pb->atomId(3) + aoff;
// 			ffb = pb->data();
// 
// 			// Calculate vectors between atoms
// 			vec_ji = cell.mimVector(modelatoms[j]->r(), modelatoms[i]->r());
// 			vec_jk = cell.mimVector(modelatoms[j]->r(), modelatoms[k]->r());
// 			vec_kl = cell.mimVector(modelatoms[k]->r(), modelatoms[l]->r());
// 			mag_ji = vec_ji.magnitude();
// 			mag_jk = vec_jk.magnitude();
// 			mag_kl = vec_kl.magnitude();
// // 			printf("i-j-k-l %i-%i-%i-%i MAGs %f %f %f\n",i,j,k,l, mag_ij,mag_kj, mag_lk);
// 
// 			// Calculate cross products and torsion angle formed (in radians)
// 			xpj = vec_ji * vec_jk;
// 			xpk = vec_kl * vec_jk;
// 			mag_xpj = xpj.magAndNormalise();
// 			mag_xpk = xpk.magAndNormalise();
// 			dp = xpj.dp(xpk);
// 			if (dp < -1.0) dp = -1.0;
// 			else if (dp > 1.0) dp = 1.0;
// 			phi = acos(dp);
// // 			printf("i-j-k-l %i-%i-%i-%i DP %16.13f %16.13f %16.13f %16.13f\n",i,j,k,l, mag_xpj, mag_xpk, dp, phi);
// 			if (phi < 0.0) { if (phi > -1e-8) phi = -1e-8; }
// 			else if (phi < 1e-8) phi = 1e-8;
// 
// 			// Derivative w.r.t. change in torsion angle
// 			dphi_dcosphi = -1.0 / sin(phi);
// 			// Pathological case where phi = 0...
// 			if (phi < 1E-8) dphi_dcosphi = 0.0;
// 
// 			/* Construct derivatives of perpendicular axis (cross product) w.r.t. component vectors.
// 			E.g.
// 			    d (rij x rkj) 
// 			    ------------- = rij[cp(n+2)] * U[cp(n+1)] - rij[cp(n+1)] * U[cp(n+2)]
// 			       d rkj[n]  
// 			
// 			where cp is a cylic permutation spanning {0,1,2} == {x,y,z}, and U[n] is a unit vector in the n direction.
// 			So,
// 			    d (rij x rkj) 
// 			    ------------- = rij[2] * U[1] - rij[1] * U[2]
// 			       d rkj[0]  
// 					  = rij[z] * (0,1,0) - rij[y] * (0,0,1)
// 
// 					  = (0,rij[z],0) - (0,0,rij[y])
// 
// 					  = (0,rij[z],-rij[y])
// 			*/
// 			dxpj_dij = make_cp_mat(vec_jk);
// 			temp = -vec_ji;
// 			dxpj_dkj = make_cp_mat(temp);
// 			temp = -vec_kl;
// 			dxpk_dkj = make_cp_mat(temp);
// 			dxpk_dlk = make_cp_mat(vec_jk);
// 			// Construct derivatives of cos(phi) w.r.t. perpendicular axes
// 			dcos_dxpj = (xpk - xpj * dp) / mag_xpj;
// 			dcos_dxpk = (xpj - xpk * dp) / mag_xpk;
// 
// 			// Generate derivative of energy function (placed in 'du_dphi')
// 			switch (pb->data()->torsionForm())
// 			{
// 				case (TorsionFunctions::None):
// 					Messenger::print("Warning: No function is specified for torsion force %i-%i-%i-%i.", i, j, k, l);
// 				case (TorsionFunctions::Ignore):
// 					du_dphi = 0.0;
// 					break;
// 				case (TorsionFunctions::Cosine): 
// 					// dU/dphi = forcek * period * s * -sin(period*phi - eq)
// 					forcek = ffb->parameter(TorsionFunctions::CosineK);
// 					eq = ffb->parameter(TorsionFunctions::CosineEq) / DEGRAD;
// 					period = ffb->parameter(TorsionFunctions::CosineN);
// 					s = ffb->parameter(TorsionFunctions::CosineS);
// 					du_dphi = dphi_dcosphi * period * forcek * s * -sin(period*phi - eq);
// 					break;
// 				case (TorsionFunctions::Cos3):
// 					// dU/dphi = 0.5 * ( -k1*sin(phi) + 2 * k2*sin(2*phi) - 3 * k3*(sin(3*phi)) )
// 					k1 = ffb->parameter(TorsionFunctions::Cos3K1);
// 					k2 = ffb->parameter(TorsionFunctions::Cos3K2);
// 					k3 = ffb->parameter(TorsionFunctions::Cos3K3);
// 					du_dphi = dphi_dcosphi * 0.5 * ( -k1*sin(phi) + 2.0*k2*sin(2.0*phi) - 3.0*k3*sin(3.0*phi));
// 					break;
// 				case (TorsionFunctions::Cos3C):
// 					// dU/dphi = 0.5 * ( -k1*sin(phi) + 2 * k2*sin(2*phi) - 3 * k3*(sin(3*phi)) )
// 					k1 = ffb->parameter(TorsionFunctions::Cos3CK1);
// 					k2 = ffb->parameter(TorsionFunctions::Cos3CK2);
// 					k3 = ffb->parameter(TorsionFunctions::Cos3CK3);
// 					du_dphi = dphi_dcosphi * 0.5 * ( -k1*sin(phi) + 2.0*k2*sin(2.0*phi) - 3.0*k3*sin(3.0*phi));
// 					break;
// 				case (TorsionFunctions::Cos4):
// 					// dU/dphi = 0.5 * ( -k1*sin(phi) + 2 * k2*sin(2*phi) - 3 * k3*(sin(3*phi)) + 4 * k4*(sin(4*phi)))
// 					k1 = -ffb->parameter(TorsionFunctions::Cos4K1);
// 					k2 = 2.0 * ffb->parameter(TorsionFunctions::Cos4K2);
// 					k3 = -3.0 * ffb->parameter(TorsionFunctions::Cos4K3);
// 					k4 = 4.0 * ffb->parameter(TorsionFunctions::Cos4K4);
// 					du_dphi = dphi_dcosphi * 0.5 * ( k1*sin(phi) + k2*sin(2.0*phi) + k3*sin(3.0*phi) + k4*sin(4.0*phi));
// 					break;
// 				case (TorsionFunctions::CosCos):
// 					// dU/dphi = 0.5 * k * n * cos(n*eq) * sin(n*phi)
// 					k1 = ffb->parameter(TorsionFunctions::CosCosK);
// 					period = ffb->parameter(TorsionFunctions::CosCosN);
// 					eq = ffb->parameter(TorsionFunctions::CosCosEq) / DEGRAD;
// 					du_dphi = dphi_dcosphi * 0.5 * k1 * period * cos(period*eq)*sin(period*phi);
// 					break;
// 				case (TorsionFunctions::Dreiding):
// 					// dU/dphi = 0.5 * k * (1 - cos(n*(theta-eq))
// 					k1 = ffb->parameter(TorsionFunctions::DreidingK);
// 					period = ffb->parameter(TorsionFunctions::DreidingN);
// 					eq = ffb->parameter(TorsionFunctions::DreidingEq) / DEGRAD;
// 					du_dphi = dphi_dcosphi * 0.5 * k1 * sin(period*(phi - eq));
// 					break;
// 				default:
// 					printf("No equation coded for torsion force of type '%s'.\n",  TorsionFunctions::functionData[ffb->torsionForm()].name);
// 					break;
// 			}
// // 			printf("i-j-k-l %i-%i-%i-%i %f %f %f\n",i,j,k,l, phi, dphi_dcosphi, du_dphi);
// 			// Calculate forces
// 			fi.x = -du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(0));
// 			fi.y = -du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(1));
// 			fi.z = -du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(2));
// 
// 			fj.x = -du_dphi * ( dcos_dxpj.dp( -dxpj_dij.columnAsVec3(0) - dxpj_dkj.columnAsVec3(0) ) - dcos_dxpk.dp(dxpk_dkj.columnAsVec3(0)) );
// 			fj.y = -du_dphi * ( dcos_dxpj.dp( -dxpj_dij.columnAsVec3(1) - dxpj_dkj.columnAsVec3(1) ) - dcos_dxpk.dp(dxpk_dkj.columnAsVec3(1)) );
// 			fj.z = -du_dphi * ( dcos_dxpj.dp( -dxpj_dij.columnAsVec3(2) - dxpj_dkj.columnAsVec3(2) ) - dcos_dxpk.dp(dxpk_dkj.columnAsVec3(2)) );
// 
// 			fk.x = -du_dphi * ( dcos_dxpk.dp( dxpk_dkj.columnAsVec3(0) - dxpk_dlk.columnAsVec3(0) ) + dcos_dxpj.dp(dxpj_dkj.columnAsVec3(0)) );
// 			fk.y = -du_dphi * ( dcos_dxpk.dp( dxpk_dkj.columnAsVec3(1) - dxpk_dlk.columnAsVec3(1) ) + dcos_dxpj.dp(dxpj_dkj.columnAsVec3(1)) );
// 			fk.z = -du_dphi * ( dcos_dxpk.dp( dxpk_dkj.columnAsVec3(2) - dxpk_dlk.columnAsVec3(2) ) + dcos_dxpj.dp(dxpj_dkj.columnAsVec3(2)) );
// 
// 			fl.x = -du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(0));
// 			fl.y = -du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(1));
// 			fl.z = -du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(2));
// 
// 			modelatoms[i]->f() -= fi;
// 			modelatoms[j]->f() -= fj;
// 			modelatoms[k]->f() -= fk;
// 			modelatoms[l]->f() -= fl;
// 
// 		}
// 		aoff += nAtoms_;
// 	}
// 	Messenger::exit("Pattern::torsionForces");
// }
