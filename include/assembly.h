#ifndef __ASSEMBLY_H__
#define __ASSEMBLY_H__


#include <Eigen/Core>
#include <Eigen/Sparse>


// this is an implementation where I assemble the system matrix
// by rows using specialized insertion instructions. This should be the most
// performant routine while still using Eigen API
//
// the function is templated on the storage scheme, namely Eigen::RowMajor
// and Eigen::ColMajor, so that I can perform some benchmarks
template <Eigen::StorageOptions storage_option = Eigen::StorageOptions::ColMajor>
Eigen::SparseMatrix<double, storage_option>
assemble_system_matrix(int n, const Eigen::MatrixXd& operator_stencils) {
	const int m = operator_stencils.rows();

	Eigen::SparseMatrix<double, Eigen::RowMajor> trMat(n,n);
	trMat.reserve(2 + (n-2)*m);


	trMat.startVec(0);
	trMat.insertBackByOuterInner(0,0) = 1.0;

	for (int i = 1; i < n-1; ++i) {
		trMat.startVec(i);

		const int lb = std::min(
			std::max(0, i - m/2), // handles the left boundary
			n-m                   // handles the right boundary
		);

		for (int k = 0; k < m; ++k) {
			trMat.insertBackByOuterInner(i, lb+k) = operator_stencils(i-lb, k);
		}
	}

	trMat.startVec(n-1);
	trMat.insertBackByOuterInner(n-1,n-1) = 1.0;

	trMat.finalize();

	// this is an implicit cast from RowMajor to StorageScheme
	// I could look at the transpose routine to make the performance
	// analysis more complete
	return trMat;
}


// this is the original implementation that used the `.insert(i,j)` method. It is kept
// here for reference and performance comparison
Eigen::SparseMatrix<double>
assemble_system_matrix_original(int n, const Eigen::MatrixXd& operator_stencils);


#endif // __ASSEMBLY_H__
