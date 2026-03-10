#ifndef __STENCIL_H__
#define __STENCIL_H__


#include <Eigen/Dense>


// all the users of this function assemble an Eigen::SparseMatrix<double>
// it makes sense to perform here the conversion
//
// if the others parts of the program don't need to be aware
// of the precision at which stencils are computed, their
// compilation path will be independent
Eigen::MatrixXd compute_laplacian_stencils(int npoints);


#endif // __STENCIL_H__
