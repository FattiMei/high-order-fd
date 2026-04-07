#ifndef __ASSEMBLY_H__
#define __ASSEMBLY_H__


#include <Eigen/Core>
#include <Eigen/Sparse>
#include <vector>


struct CSCMatrix {
	std::vector<double> data;
	std::vector<int> outerStarts;
	std::vector<int> innerIndices;
};


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


template <bool only_allocations = false, bool dump_constant = false>
CSCMatrix
assemble_system_matrix_minimal(int n, const Eigen::MatrixXd& operator_stencils) {
	if constexpr (only_allocations) {
		return CSCMatrix();
	}

	const int stencil_size = operator_stencils.rows();
	const int nnz = 2 + (n-2) * stencil_size;

	std::vector<double> data(nnz);
	std::vector<int> innerIndices(nnz);
	std::vector<int> outerStarts(n);

	int write_idx = 0;
	for (int row = 0; row < n; ++row) {
		outerStarts[row] = write_idx;

		// I'm confident that the loop peeling will take care of this
		if (row == 0 or row == (n-1)) {
			data[write_idx] = 1.0;
			innerIndices[write_idx] = 0;
			++write_idx;
		} else {
			const int lb = std::min(
				std::max(0, row - stencil_size/2), // handles the left boundary
				n-stencil_size                     // handles the right boundary
			);

			// phony writes, only the number of writes is correct
			for (int k = 0; k < stencil_size; ++k) {
				data[write_idx] = operator_stencils(row-lb, k);
				innerIndices[write_idx] = lb+k;
				++write_idx;
			}
		}
	}

	return CSCMatrix(data, innerIndices, outerStarts);
}

#endif // __ASSEMBLY_H__
