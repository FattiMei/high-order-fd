#ifndef __STENCIL_H__
#define __STENCIL_H__


#include <vector>
#include <eigen3/Eigen/Dense>


// In the original implementation I used to compute
// the stencils in double precision.
//
// I got second thoughts about the accuracy of the
// stencils when I experimented with rational numbers.
// In this particular case, rational arithmetic would be exact
// and so the stencils would be computed with no error.
//
// The solution proposed was to template the stencil
// calculations with the desired type and I planned on using:
//   * double
//   * boost::rational<long>
// this promptly made me integrate boost::rational<IntType>
// with the Eigen library.
//
// But the rational type was introduced to test the hypothesis
// that some degree of error in the stencil calculations would
// have a measurable effect on the error of the final solutions.
//
// This can be tested with `long doubles` before introducing
// rational numbers!


// this should later be controlled by compile flags
using StencilPrecision = double;


// these are macro definitions for a dynamic Eigen::Matrix with a custom scalar type
#define EIGEN_VECTOR(T) Eigen::Matrix<T, Eigen::Dynamic, 1>
#define EIGEN_MATRIX(T) Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>


EIGEN_VECTOR(StencilPrecision) compute_stencil_vandermonde(int order,
                                                           const StencilPrecision center,
                                                           const std::vector<StencilPrecision>& nodes);
EIGEN_MATRIX(StencilPrecision) compute_laplacian_stencils(int npoints);


#endif // __STENCIL_H__
