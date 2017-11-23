/* lb-blackbox-type.h
 * Copyright (C) 2005 Pascal Giorgi
 *
 * Written by Pascal Giorgi <pgiorgi@uwaterloo.ca>
 *
 * ========LICENCE========
 * This file is part of the library LinBox.
 *
  * LinBox is free software: you can redistribute it and/or modify
 * it under the terms of the  GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 */

#ifndef __LINBOX_lb_blackbox_type_H
#define __LINBOX_lb_blackbox_type_H

#include "linbox/matrix/dense-matrix.h"
#include "linbox/matrix/sparse-matrix.h"
#include "linbox/matrix/polynomial-matrix.h"

/****************************************
 * Define the list of all Blackbox Type *
 ****************************************/

// (NEED TO USE ENVELOPE TO DEFINE A CONCRETE TYPE)
typedef LinBoxTypelist < BlackboxEnvelope< LinBox::DenseMatrix > , LinBoxDumbType> BL1;

template<typename Field>
using SparseMat=LinBox::SparseMatrix<Field,LinBox::SparseMatrixFormat::SparseSeq>; 
typedef LinBoxTypelist < BlackboxEnvelope< SparseMat > , BL1> BL2;


template<typename Field>
using PolynomialMat=LinBox::PolynomialMatrix<LinBox::PMType::polfirst,LinBox::PMStorage::plain,Field>;
typedef LinBoxTypelist < BlackboxEnvelope< PolynomialMat > , BL2> BL3;



// define the blackbox typelist
typedef BL2 BlackboxList;




#endif

// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,:0,t0,+0,=s
// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
