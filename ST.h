#pragma once

#ifndef ST_H
#define ST_H

#define _USE_MATH_DEFINES
#include <iostream>
#include <numeric>
#include <vector>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <limits>
#include <utility>
#include <cstdlib>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <functional>
#include <boost/math/distributions/students_t.hpp>

namespace StatTools
{
	/*
	第一次扩展
	基础统计部分
	*/

	/**
	 * Throws an exception if the vector is empty or contains non-numeric elements.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector to check.
	 * @throws std::invalid_argument If the vector is empty.
	 */
	template <typename T>
	inline void checkVector(const std::vector<T>& vec)
	{
		if (vec.empty())
		{
			throw std::invalid_argument("Vector is empty");
		}
		static_assert(std::is_arithmetic<T>::value, "Vector elements must be numeric");
	}

	/**
	 * Checks if the vector contains any duplicate elements.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector to check.
	 * @return True if duplicates are found, false otherwise.
	 */
	template <typename T>
	inline bool hasDuplicates(const std::vector<T>& vec)
	{
		return vec.size() != std::unordered_set<T>(vec.begin(), vec.end()).size();
	}

	/**
	 * Calculates the sum of all elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @return The sum of the elements.
	 */
	template <typename T>
	inline T sum(const std::vector<T>& vec)
	{
		checkVector(vec);
		return std::accumulate(vec.begin(), vec.end(), T{});
	}

	/**
	 * Finds the maximum value in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @return The maximum value.
	 */
	template <typename T>
	inline T max(const std::vector<T>& vec)
	{
		checkVector(vec);
		return *std::max_element(vec.begin(), vec.end());
	}

	/**
	 * Finds the minimum value in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @return The minimum value.
	 */
	template <typename T>
	inline T min(const std::vector<T>& vec)
	{
		checkVector(vec);
		return *std::min_element(vec.begin(), vec.end());
	}

	/**
	 * Calculates the mean (average) of the elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @return The mean value as a double.
	 */
	template <typename T>
	inline double mean(const std::vector<T>& vec)
	{
		checkVector(vec);
		return static_cast<double>(sum<T>(vec)) / vec.size();
	}

	/**
	 * Calculates the Euclidean norm (magnitude) of the elements in the vector.
	 * This is also known as the L2 norm or the Euclidean length of the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @return The Euclidean norm as a double.
	 */
	template <typename T>
	inline double norm(const std::vector<T>& vec)
	{
		checkVector(vec);
		double sumOfSquares = 0.0;
		for (const auto& x : vec)
		{
			sumOfSquares += x * x;
		}
		return std::sqrt(sumOfSquares);
	}

	/**
	 * Calculates the p-norm of the elements in the vector.
	 * The p-norm is a generalization of the Euclidean norm, where p can be any positive real number.
	 * For p = 2, this is the Euclidean norm (L2 norm).
	 * For p = 1, this is the Manhattan norm (L1 norm).
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @param ord The order of the norm (must be a positive real number).
	 * @return The p-norm as a double.
	 */
	template <typename T>
	inline double norm(const std::vector<T>& vec, int ord)
	{
		checkVector(vec);
		if (ord <= 0)
		{
			throw std::invalid_argument("The order of the norm (ord) must be a positive number");
		}
		switch (ord)
		{
		case 1:
		{
			double sumAbs = 0.0;
			for (const auto& x : vec)
			{
				sumAbs += std::abs(x);
			}
			return sumAbs;
		}
		case 2:
		{
			return norm(vec);
		}
		default:
		{
			double sumOfPowers = 0.0;
			for (const auto& x : vec)
			{
				sumOfPowers += std::pow(std::abs(x), ord);
			}
			return std::pow(sumOfPowers, 1.0 / ord);
		}
		}
	}

	/**
	 * Calculates the median of the elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements (will be modified).
	 * @return The median value.
	 */
	template <typename T>
	inline T median(const std::vector<T>& vec)
	{
		checkVector(vec);
		std::vector<T> sortedVec(vec);
		std::sort(sortedVec.begin(), sortedVec.end());
		size_t size = sortedVec.size();
		if (size % 2 == 0)
		{
			return (sortedVec[size / 2 - 1] + sortedVec[size / 2]) / 2;
		}
		else
		{
			return sortedVec[size / 2];
		}
	}

	/**
	 * Calculates the population variance of the elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @return The population variance.
	 */
	template <typename T>
	inline double populationVar(const std::vector<T>& vec)
	{
		checkVector(vec);
		double meanVal = mean(vec);
		double variance = 0.0;
		for (const auto& x : vec)
		{
			variance += (x - meanVal) * (x - meanVal);
		}
		return variance / vec.size();
	}

	/**
	 * Calculates the sample variance of the elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @return The sample variance.
	 */
	template <typename T>
	inline double sampleVar(const std::vector<T>& vec)
	{
		checkVector(vec);
		double meanVal = mean(vec);
		double variance = 0.0;
		for (const auto& x : vec)
		{
			variance += (x - meanVal) * (x - meanVal);
		}
		return variance / (vec.size() - 1);
	}

	/**
	 * Calculates the variance of the elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @param isSample Indicates whether to calculate sample variance (true) or population variance (false).
	 * @return The variance.
	 */
	template <typename T>
	inline double var(const std::vector<T>& vec, bool isSample = false)
	{
		return isSample ? sampleVar(vec) : populationVar(vec);
	}

	/**
	 * Calculates the population standard deviation of the elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @return The population standard deviation.
	 */
	template <typename T>
	inline double populationStd(const std::vector<T>& vec)
	{
		return std::sqrt(populationVar(vec));
	}

	/**
	 * Calculates the sample standard deviation of the elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @return The sample standard deviation.
	 */
	template <typename T>
	inline double sampleStd(const std::vector<T>& vec)
	{
		return std::sqrt(sampleVar(vec));
	}

	/**
	 * Calculates the standard deviation of the elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @param isSample Indicates whether to calculate sample standard deviation (true) or population standard deviation (false).
	 * @return The standard deviation.
	 */
	template <typename T>
	inline double std(const std::vector<T>& vec, bool isSample = false)
	{
		return std::sqrt(var(vec, isSample));
	}

	/**
	 * Calculates the skewness of the elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @return The skewness.
	 */
	template <typename T>
	inline double skewness(const std::vector<T>& vec)
	{
		checkVector(vec);
		if (vec.size() < 3)
		{
			throw std::invalid_argument("Skewness is undefined for vector of size less than 3");
		}
		double meanVal = mean(vec);
		double stdVal = populationStd(vec);
		double skew = 0.0;
		for (const auto& x : vec)
		{
			skew += std::pow((x - meanVal) / stdVal, 3);
		}
		return skew / vec.size();
	}

	/**
	 * Calculates the kurtosis of the elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @return The kurtosis.
	 */
	template <typename T>
	inline double kurtosis(const std::vector<T>& vec)
	{
		checkVector(vec);
		if (vec.size() < 4)
		{
			throw std::invalid_argument("Kurtosis is undefined for vector of size less than 4");
		}
		double meanVal = mean(vec);
		double stdVal = populationStd(vec);
		double kurt = 0.0;
		for (const auto& x : vec)
		{
			kurt += std::pow((x - meanVal) / stdVal, 4);
		}
		return kurt / vec.size() - 3;
	}

	/**
	 * Calculates the percentile of the elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @param p The percentile to calculate (between 0 and 1).
	 * @return The percentile value.
	 */
	template <typename T>
	inline double percentile(const std::vector<T>& vec, double p)
	{
		checkVector(vec);
		if (p < 0 || p > 1)
		{
			throw std::invalid_argument("Percentile must be between 0 and 1");
		}
		std::vector<T> sortedVec(vec);
		std::sort(sortedVec.begin(), sortedVec.end());
		size_t index = static_cast<size_t>(p * (sortedVec.size() - 1));
		return sortedVec[index];
	}

	/**
	 * Finds the mode of the elements in the vector.
	 *
	 * @tparam T The type of elements in the vector.
	 * @param vec The vector containing the elements.
	 * @return The mode value.
	 */
	template <typename T>
	inline T mode(const std::vector<T>& vec)
	{
		checkVector(vec);
		std::unordered_map<T, int> freqMap;
		for (const auto& x : vec)
		{
			freqMap[x]++;
		}
		T modeValue = vec[0];
		int maxFreq = 0;
		for (const auto& pair : freqMap)
		{
			if (pair.second > maxFreq)
			{
				maxFreq = pair.second;
				modeValue = pair.first;
			}
		}
		return modeValue;
	}

	/**
	 * Calculates the covariance between two vectors.
	 *
	 * @tparam T The type of elements in the vectors.
	 * @param vec1 The first vector.
	 * @param vec2 The second vector.
	 * @param isSample Indicates whether to calculate sample covariance (true) or population covariance (false).
	 * @return The covariance.
	 */
	template <typename T>
	inline double covariance(const std::vector<T>& vec1, const std::vector<T>& vec2, bool isSample = false)
	{
		if (vec1.size() != vec2.size())
		{
			throw std::invalid_argument("Vectors must be of the same size");
		}
		checkVector(vec1);
		checkVector(vec2);
		double mean1 = mean(vec1);
		double mean2 = mean(vec2);
		double cov = 0.0;
		for (size_t i = 0; i < vec1.size(); ++i)
		{
			cov += (vec1[i] - mean1) * (vec2[i] - mean2);
		}
		return isSample ? cov / (vec1.size() - 1) : cov / vec1.size();
	}

	/**
	 * Calculates the correlation coefficient between two vectors.
	 *
	 * @tparam T The type of elements in the vectors.
	 * @param vec1 The first vector.
	 * @param vec2 The second vector.
	 * @return The correlation coefficient.
	 */
	template <typename T>
	inline double correlation(const std::vector<T>& vec1, const std::vector<T>& vec2)
	{
		return covariance(vec1, vec2) / (std(vec1) * std(vec2));
	}

	/*
	第二次扩展
	t-test
	*/
	struct t_testResult
	{
		double t;
		double p_value;
		double log2_fc;
	};

	/**
	 * Performs a two-sample t-test on two vectors.
	 *
	 * @tparam T The type of elements in the vectors.
	 * @param vec1 The first vector.
	 * @param vec2 The second vector.
	 * @return A t_testResult object containing the t-statistic, p-value, and log2 fold change.
	 */
	template <typename T>
	inline t_testResult t_test(const std::vector<T>& vec1, const std::vector<T>& vec2, bool log2_data = true)
	{
		// 计算t统计量
		double m1 = mean(vec1);
		double m2 = mean(vec2);
		double s1 = var(vec1, true) / vec1.size();
		double s2 = var(vec2, true) / vec2.size();

		// 计算log2 fold change
		double log2_fc = 0;
		if (log2_data)
			// 数据已经经过了log2处理，所以直接相减即可
			log2_fc = m1 - m2;
		else
			log2_fc = std::log2(m1 / m2);

		// 计算t值和自由度
		double t = (m1 - m2) / std::sqrt(s1 + s2);
		double df = std::pow(s1 + s2, 2) / (std::pow(s1, 2) / (vec1.size() - 1) + std::pow(s2, 2) / (vec2.size() - 1));

		boost::math::students_t dist(df);
		double p = 2 * boost::math::cdf(boost::math::complement(dist, std::abs(t)));

		return { t, p, log2_fc };
	}

	/**
	 * Adjusts the false discovery rate (FDR) of a set of p-values using the Benjamini-Hochberg method.
	 *
	 * @param p_values A vector of p-values to be adjusted.
	 * @return A vector of adjusted p-values.
	 */
	inline std::vector<double> adjust_fdr(std::vector<double> p_values)
	{
		size_t n = p_values.size();
		std::vector<std::pair<double, int>> pv_with_index;
		for (size_t i = 0; i < n; ++i)
			pv_with_index.push_back({ p_values[i], i });
		std::sort(pv_with_index.begin(), pv_with_index.end());

		std::vector<double> q_values(n);
		double prev_q = 1.0;
		for (int i = n - 1; i >= 0; --i)
		{
			double unadjusted = pv_with_index[i].first * n / (i + 1);
			double q = std::min(unadjusted, prev_q);
			q_values[pv_with_index[i].second] = q;
			prev_q = q;
		}
		return q_values;
	}

	/*
	第三次扩展
	线性代数 + PCA + Kmeans
	*/

	/**
	 * Multiplies two matrices.
	 *
	 * This function performs matrix multiplication, computing the product of two matrices A and B.
	 * The number of columns in matrix A must be equal to the number of rows in matrix B.
	 *
	 * @param A The first matrix, represented as a 2D vector.
	 * @param B The second matrix, represented as a 2D vector.
	 * @return A new matrix that is the product of A and B, represented as a 2D vector.
	 * @throws std::invalid_argument If the number of columns in A does not match the number of rows in B.
	 */
	inline std::vector<std::vector<double>> multiplyMatrix(const std::vector<std::vector<double>>& A, const std::vector<std::vector<double>>& B)
	{
		size_t rowsA = A.size();
		size_t colsA = A[0].size();
		size_t rowsB = B.size();
		size_t colsB = B[0].size();

		if (colsA != rowsB)
		{
			throw std::invalid_argument("Matrix dimensions do not match for multiplication");
		}

		std::vector<std::vector<double>> C(rowsA, std::vector<double>(colsB, 0.0));
		for (size_t i = 0; i < rowsA; ++i)
		{
			for (size_t j = 0; j < colsB; ++j)
			{
				for (size_t k = 0; k < colsA; ++k)
				{
					C[i][j] += A[i][k] * B[k][j];
				}
			}
		}
		return C;
	}

	/**
	 * Finds the position of the largest off-diagonal element in a square matrix.
	 *
	 * This function scans the upper triangular part of the matrix (excluding the diagonal)
	 * and returns the indices of the element with the largest absolute value.
	 *
	 * @param mat The square matrix represented as a 2D vector.
	 * @return A pair of integers representing the indices (row, column) of the largest off-diagonal element.
	 *         The row index is less than the column index, as the function only considers the upper triangle.
	 * @throws std::invalid_argument If the matrix is not square (i.e., number of rows is not equal to the number of columns).
	 */
	inline std::pair<int, int> findMaxOffDiagonal(const std::vector<std::vector<double>>& mat)
	{
		int n = mat.size();
		double max_val = 0.0;
		std::pair<int, int> max_pos = { 0, 1 };
		for (int i = 0; i < n; ++i)
		{
			for (int j = i + 1; j < n; ++j)
			{
				double abs_val = fabs(mat[i][j]);
				if (abs_val > max_val)
				{
					max_val = abs_val;
					max_pos = { i, j };
				}
			}
		}
		return max_pos;
	}

	/**
	 * Performs QR decomposition of a matrix.
	 *
	 * This function decomposes a matrix A into an orthogonal matrix Q and an upper triangular matrix R
	 * such that A = QR. The decomposition is done using the Gram-Schmidt process.
	 *
	 * @param A The matrix to decompose, represented as a 2D vector.
	 * @return A pair of matrices Q and R, represented as 2D vectors.
	 * @throws std::invalid_argument If the matrix A is not square or has incompatible dimensions.
	 */
	inline std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> qrDecomposition(std::vector<std::vector<double>> A)
	{
		size_t rows = A.size();
		size_t cols = A[0].size();
		std::vector<std::vector<double>> Q(rows, std::vector<double>(cols, 0.0));
		std::vector<std::vector<double>> R(rows, std::vector<double>(cols, 0.0));

		for (size_t i = 0; i < rows; ++i)
		{
			Q[i][i] = 1.0;
		}

		for (size_t j = 0; j < cols; ++j)
		{
			double norm = 0.0;
			for (size_t i = 0; i < rows; ++i)
			{
				norm += A[i][j] * A[i][j];
			}
			R[j][j] = sqrt(norm);

			std::vector<double> q_col(rows);
			for (size_t i = 0; i < rows; ++i)
			{
				q_col[i] = A[i][j] / R[j][j];
			}

			for (size_t k = j + 1; k < cols; ++k)
			{
				double dot_prod = 0.0;
				for (size_t i = 0; i < rows; ++i)
				{
					dot_prod += q_col[i] * A[i][k];
				}
				R[j][k] = dot_prod;
				for (size_t i = 0; i < rows; ++i)
				{
					A[i][k] -= q_col[i] * dot_prod;
				}
			}

			for (size_t i = 0; i < rows; ++i)
			{
				Q[i][j] = q_col[i];
			}
		}

		return { Q, R };
	}

	/**
	 * Computes the eigenvalues and eigenvectors of a matrix using QR iteration.
	 *
	 * This function iteratively applies QR decomposition to a matrix and updates it until the matrix
	 * converges to a diagonal form. The diagonal elements are the eigenvalues, and the columns of the
	 * matrix Q are the eigenvectors.
	 *
	 * @param A The matrix to compute eigenvalues and eigenvectors for, represented as a 2D vector.
	 * @param max_iter The maximum number of iterations to perform in the QR algorithm.
	 * @param tol The tolerance value to check for convergence. If the change in eigenvalues is less than this, the algorithm stops.
	 * @return A pair of the eigenvalues (as a vector) and the eigenvectors (as a 2D vector).
	 * @throws std::invalid_argument If the matrix A is not square or has incompatible dimensions.
	 */
	inline std::pair<std::vector<double>, std::vector<std::vector<double>>> eigenvalues_and_eigenvectors(std::vector<std::vector<double>>& A, int max_iter = 1000, double tol = 1e-6)
	{
		size_t n = A.size();
		std::vector<double> eigenvals(n);
		std::vector<std::vector<double>> eigenvectors(n, std::vector<double>(n, 0.0));

		for (size_t i = 0; i < n; ++i)
		{
			eigenvectors[i][i] = 1.0;
		}

		for (int iter = 0; iter < max_iter; ++iter)
		{
			std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> result = qrDecomposition(A);
			std::vector<std::vector<double>> Q = result.first;
			std::vector<std::vector<double>> R = result.second;

			A = multiplyMatrix(R, Q);

			bool converged = true;
			for (size_t i = 0; i < n; ++i)
			{
				eigenvals[i] = A[i][i];
				if (fabs(A[i][i] - eigenvals[i]) > tol)
				{
					converged = false;
				}
			}

			eigenvectors = multiplyMatrix(Q, eigenvectors);

			if (converged)
				break;
		}

		return { eigenvals, eigenvectors };
	}

	/**
	 * Performs Principal Component Analysis (PCA) on a dataset.
	 *
	 * This function standardizes the input data, computes the covariance matrix, and performs
	 * eigenvalue decomposition to find the principal components. The data is then projected onto
	 * the first 'num_components' principal components.
	 *
	 * @param data The dataset to perform PCA on, represented as a 2D vector. Rows are samples and columns are features.
	 * @param num_components The number of principal components to retain after dimensionality reduction.
	 * @return A new matrix with the projected data, with the shape (n, num_components), where n is the number of samples.
	 * @throws std::invalid_argument If the number of components is larger than the number of features.
	 */
	inline std::vector<std::vector<double>> performPCA(std::vector<std::vector<double>> data, int num_components)
	{
		size_t row = data.size();
		size_t col = data[0].size();

		// 数据标准化
		// 计算每列的均值
		std::vector<double> means(col, 0.0);
		for (size_t j = 0; j < col; ++j)
		{
			for (size_t i = 0; i < row; ++i)
			{
				means[j] += data[i][j];
			}
			means[j] /= row;
		}

		// 计算每列的标准差
		std::vector<double> stddevs(col, 0.0);
		for (size_t j = 0; j < col; ++j)
		{
			for (size_t i = 0; i < row; ++i)
			{
				stddevs[j] += pow(data[i][j] - means[j], 2);
			}
			stddevs[j] = sqrt(stddevs[j] / row);
		}

		// 标准化数据
		for (size_t i = 0; i < row; ++i)
		{
			for (size_t j = 0; j < col; ++j)
			{
				data[i][j] = (data[i][j] - means[j]) / stddevs[j];
			}
		}

		// 计算协方差矩阵
		std::vector<std::vector<double>> cov_matrix(col, std::vector<double>(col, 0.0));
		for (size_t i = 0; i < col; ++i)
		{
			for (size_t j = i; j < col; ++j)
			{
				double sum = 0.0;
				for (size_t k = 0; k < row; ++k)
				{
					sum += data[k][i] * data[k][j];
				}
				cov_matrix[i][j] = sum / (row - 1);
				cov_matrix[j][i] = cov_matrix[i][j];
			}
		}

		auto result = eigenvalues_and_eigenvectors(cov_matrix);

		// 选择前num_components个主成分
		std::vector<size_t> indices(col);
		std::iota(indices.begin(), indices.end(), 0);
		sort(indices.begin(), indices.end(), [&result](size_t i1, size_t i2)
			{ return result.first[i1] > result.first[i2]; });

		// 投影到主成分
		std::vector<std::vector<double>> projected_data(row, std::vector<double>(num_components));
		for (size_t i = 0; i < row; ++i)
		{
			for (int j = 0; j < num_components; ++j)
			{
				for (size_t k = 0; k < col; ++k)
				{
					projected_data[i][j] += data[i][k] * result.second[k][indices[j]];
				}
			}
		}

		return projected_data;
	}

	/**
	 * Computes the Euclidean distance between two vectors.
	 *
	 * The Euclidean distance between two vectors `a` and `b` is calculated as the square root of the sum of the squared differences
	 * between corresponding elements in the vectors:
	 *
	 *     distance = sqrt( Σ (a[i] - b[i])^2 )
	 *
	 * This distance measure is commonly used to quantify the similarity or dissimilarity between two data points in Euclidean space.
	 *
	 * @param a The first vector, represented as a 1D vector of `double` values.
	 * @param b The second vector, represented as a 1D vector of `double` values.
	 *
	 * @return The Euclidean distance between the two vectors, as a `double` value.
	 *
	 * @throws std::invalid_argument If the vectors `a` and `b` have different sizes.
	 */
	inline double euclideanDistance(const std::vector<double>& a, const std::vector<double>& b)
	{
		double sum = 0.0;
		for (size_t i = 0; i < a.size(); ++i)
		{
			sum += (a[i] - b[i]) * (a[i] - b[i]);
		}
		return sqrt(sum);
	}

	/**
	 * Performs K-means clustering algorithm on a dataset.
	 *
	 * The K-means algorithm divides the data into K clusters by iteratively assigning data points
	 * to the nearest cluster center and then updating the cluster centers based on the mean of the points
	 * assigned to each cluster. The process continues until the cluster centers no longer change or a maximum
	 * number of iterations is reached.
	 *
	 * @param data A 2D vector representing the dataset, where each row is a data point and each column is a feature.
	 * @param k The number of clusters to divide the data into.
	 * @param max_iter The maximum number of iterations for the algorithm to run. Defaults to 1000 iterations.
	 *
	 * @return A vector of integers where each element corresponds to the cluster label assigned to the respective data point.
	 *         The size of the vector is equal to the number of data points (n).
	 *
	 * @throws std::invalid_argument If the number of clusters (k) is less than 1.
	 * @throws std::invalid_argument If the dataset is empty or not well-formed (e.g., inconsistent row lengths).
	 */
	inline std::vector<int> kMeans(const std::vector<std::vector<double>>& data, int k, int max_iter = 1000)
	{
		size_t n = data.size();
		size_t m = data[0].size();

		// 初始化簇中心
		std::vector<std::vector<double>> centroids(k, std::vector<double>(m));
		std::vector<int> labels(n, -1); // 存储每个数据点的簇标签

		// 随机选择初始簇中心
		srand(time(0));
		std::vector<int> indices(n);
		std::iota(indices.begin(), indices.end(), 0);
		random_shuffle(indices.begin(), indices.end());

		for (int i = 0; i < k; ++i)
		{
			centroids[i] = data[indices[i]]; // 新的簇中心
		}

		for (int iter = 0; iter < max_iter; ++iter)
		{
			bool changed = false;

			// 分配每个点到最近的簇
			for (size_t i = 0; i < n; ++i)
			{
				double min_dist = std::numeric_limits<double>::infinity();
				int closest_centroid = -1;

				for (int j = 0; j < k; ++j)
				{
					double dist = euclideanDistance(data[i], centroids[j]);
					if (dist < min_dist)
					{
						min_dist = dist;
						closest_centroid = j;
					}
				}

				if (labels[i] != closest_centroid)
				{
					labels[i] = closest_centroid;
					changed = true;
				}
			}

			// 更新簇中心
			std::vector<std::vector<double>> new_centroids(k, std::vector<double>(m, 0.0));
			std::vector<int> count(k, 0); // 记录每个簇中点的数量

			for (size_t i = 0; i < n; ++i)
			{
				int cluster = labels[i];
				for (size_t j = 0; j < m; ++j)
				{
					new_centroids[cluster][j] += data[i][j];
				}
				count[cluster]++;
			}

			for (int j = 0; j < k; ++j)
			{
				for (size_t i = 0; i < m; ++i)
				{
					new_centroids[j][i] /= count[j];
				}
			}

			centroids = new_centroids;

			// 如果没有簇中心变化，停止迭代
			if (!changed)
			{
				break;
			}
		}

		return labels;
	}

	/*
	第四次扩展
	层次聚类 + KDE图
	*/

	/**
	 * @struct Linkage
	 * @brief Represents a single merge event in hierarchical clustering.
	 *
	 * Each record contains indices of two clusters merged, the distance between
	 * them at merge time, and the size (number of samples) of the new cluster.
	 */
	struct Linkage
	{
		int idx1, idx2;
		double dist;
		int size;
	};

	/**
	 * @brief Compute pairwise Euclidean distance matrix.
	 * @param data N*D matrix
	 * @return N*N symmetric matrix of distances
	 */
	inline std::vector<std::vector<double>> computeDistanceMatrix(const std::vector<std::vector<double>>& data) {
		int N = data.size();
		std::vector<std::vector<double>> dist(N, std::vector<double>(N, 0.0));
		for (int i = 0; i < N; ++i) {
			for (int j = i + 1; j < N; ++j) {
				double sum = 0;
				for (size_t k = 0; k < data[i].size(); ++k) {
					double d = data[i][k] - data[j][k];
					sum += d * d;
				}
				dist[i][j] = dist[j][i] = std::sqrt(sum);
			}
		}
		return dist;
	}

	/**
	 * @brief Perform single-linkage hierarchical clustering.
	 * @param distMat precomputed distance matrix
	 * @return linkage matrix (size N-1) with merge records
	 */
	inline std::vector<Linkage> hierarchicalClustering(
		std::vector<std::vector<double>> distMat
	) {
		int N = distMat.size();
		// 初始：每个叶子自成一簇
		std::map<int, std::vector<int>> clusters;
		for (int i = 0; i < N; ++i) clusters[i] = { i };

		std::vector<Linkage> Z;
		Z.reserve(N - 1);
		int nextId = N;

		while (clusters.size() > 1) {
			double best = std::numeric_limits<double>::infinity();
			int a = -1, b = -1;
			// 找到最近的簇对 (a, b)
			for (auto it1 = clusters.begin(); it1 != clusters.end(); ++it1) {
				for (auto it2 = std::next(it1); it2 != clusters.end(); ++it2) {
					double dmin = std::numeric_limits<double>::infinity();
					for (int i : it1->second)
						for (int j : it2->second)
							dmin = std::min(dmin, distMat[i][j]);
					if (dmin < best) {
						best = dmin;
						a = it1->first;
						b = it2->first;
					}
				}
			}

			auto merged = clusters[a];
			merged.insert(merged.end(),
				clusters[b].begin(),
				clusters[b].end());

			// 记录 linkage
			Z.push_back({ a, b, best, (int)merged.size() });

			clusters.erase(a);
			clusters.erase(b);
			clusters[nextId++] = std::move(merged);
		}

		return Z;
	}

	/**
	 * @brief Gaussian kernel function
	 * @param u normalized distance (x - xi) / bandwidth
	 */
	inline double gaussianKernel(double u) {
		static const double inv_sqrt2pi = 1.0 / std::sqrt(2 * M_PI);
		return inv_sqrt2pi * std::exp(-0.5 * u * u);
	}

	/**
	 * @brief Compute KDE densities for given samples over a grid
	 * @param data input sample vector
	 * @param grid_x grid points at which to evaluate KDE
	 * @param bandwidth kernel bandwidth
	 * @return vector of density values
	 */
	inline std::vector<double> computeKDE(
		const std::vector<double>& data,
		const std::vector<double>& grid_x,
		double bandwidth
	) {
		int n = data.size();
		std::vector<double> densities(grid_x.size());
		for (size_t i = 0; i < grid_x.size(); ++i) {
			double x = grid_x[i];
			double sum = 0.0;
			for (double xi : data) {
				sum += gaussianKernel((x - xi) / bandwidth);
			}
			densities[i] = sum / (n * bandwidth);
		}
		return densities;
	}

	/**
	 * @brief Plot KDE using matplotlib-cpp
	 * @param samples input data samples
	 * @param bandwidth Gaussian kernel bandwidth
	 * @param grid_n number of grid points (default 200)
	 * @param width figure width in pixels (default 800)
	 * @param height figure height in pixels (default 600)
	 */

	 /*
	 第五次扩展
	 t-SNE降维 LLE降维
	 */

	 /**
	  * @brief 对单行距离平方向量使用二分搜索确定局部高维相似度分布。
	  *
	  * @param dist2_row 长度为 n 的距离平方向量。
	  * @param target_perplexity 目标困惑度。
	  * @param max_iter 最大二分搜索迭代次数。
	  * @param tol 熵误差容限。
	  * @return 长度为 n 的概率分布 P(i|j)。
	  */
	inline std::vector<double> binarySearchSigma(
		const std::vector<double>& dist2_row,
		double target_perplexity,
		int max_iter = 50,
		double tol = 1e-5
	)
	{
		double beta_min = -INFINITY, beta_max = INFINITY;
		double beta = 1.0;
		size_t n = dist2_row.size();
		std::vector<double> P(n);
		for (int iter = 0; iter < max_iter; ++iter) {
			double sumP = 0.0;
			for (size_t j = 0; j < n; ++j) {
				P[j] = std::exp(-dist2_row[j] * beta);
				sumP += P[j];
			}
			double H = 0.0;
			for (size_t j = 0; j < n; ++j) {
				P[j] /= sumP;
				if (P[j] > 1e-12) H -= P[j] * std::log(P[j]);
			}
			double perplexity = std::exp(H);
			double diff = perplexity - target_perplexity;
			if (std::fabs(diff) < tol) break;
			if (diff > 0) {
				beta_min = beta;
				beta = (beta_max == INFINITY ? beta * 2 : 0.5 * (beta + beta_max));
			}
			else {
				beta_max = beta;
				beta = (beta_min == -INFINITY ? beta / 2 : 0.5 * (beta + beta_min));
			}
		}
		return P;
	}

	/**
	 * @brief 计算高维空间的对称化相似度矩阵 P。
	 *
	 * 对于每个样本 i，先调用 binarySearchSigma 获得 P(i|j)，
	 * 然后对称化：P_{ij} = (P(i|j) + P(j|i)) / (2n)。
	 *
	 * @param dist2 n*n 的距离平方矩阵。
	 * @param perplexity 困惑度。
	 * @return n*n 的高维相似度矩阵 P。
	 */
	inline std::vector<std::vector<double>> computeHighDimAffinities(
		const std::vector<std::vector<double>>& dist2,
		double perplexity
	) {
		size_t n = dist2.size();
		std::vector<std::vector<double>> P(n, std::vector<double>(n, 0.0));
		for (size_t i = 0; i < n; ++i) {
			auto Pi = binarySearchSigma(dist2[i], perplexity);
			for (size_t j = 0; j < n; ++j) P[i][j] = Pi[j];
		}
		// 对称化并归一化
		for (size_t i = 0; i < n; ++i) {
			for (size_t j = i + 1; j < n; ++j) {
				double val = (P[i][j] + P[j][i]) / (2.0 * n);
				P[i][j] = P[j][i] = val;
			}
		}
		return P;
	}

	/**
	 * @brief 计算低维空间的学生 t 分布相似度矩阵 Q。
	 *
	 * Q_{ij} = (1 + ||y_i - y_j||^2)^{-1}，然后整体除以总和。
	 *
	 * @param Y 低维坐标矩阵，大小 n*dim。
	 * @return n*n 的相似度矩阵 Q。
	 */
	inline std::vector<std::vector<double>> computeLowDimAffinities(
		const std::vector<std::vector<double>>& Y
	) {
		size_t n = Y.size();
		std::vector<std::vector<double>> Q(n, std::vector<double>(n, 0.0));
		double sum = 0.0;
		for (size_t i = 0; i < n; ++i) {
			for (size_t j = i + 1; j < n; ++j) {
				double dist2 = 0.0;
				for (size_t d = 0; d < Y[i].size(); ++d) {
					double diff = Y[i][d] - Y[j][d];
					dist2 += diff * diff;
				}
				double inv = 1.0 / (1.0 + dist2);
				Q[i][j] = Q[j][i] = inv;
				sum += 2.0 * inv;
			}
			Q[i][i] = 0.0;
		}
		// 归一化
		for (size_t i = 0; i < n; ++i)
			for (size_t j = 0; j < n; ++j)
				Q[i][j] /= sum;
		return Q;
	}

	/**
	 * @brief 执行 t-SNE 降维。
	 *
	 * @param X 原始高维数据矩阵，大小 n*d。
	 * @param dim 目标降维维度，默认为 2。
	 * @param perplexity 困惑度参数。
	 * @param max_iter 最大迭代次数。
	 * @param lr 学习率。
	 * @return 降维后的坐标矩阵，大小 n*dim。
	 * @throws std::invalid_argument 如果输入数据为空。
	 */
	inline std::vector<std::vector<double>> performTSNE(
		const std::vector<std::vector<double>>& X,
		int dim = 2,
		double perplexity = 30.0,
		int max_iter = 1000,
		double lr = 200.0
	) {
		if (X.empty()) {
			throw std::invalid_argument("Input data X is empty.");
		}
		size_t n = X.size();

		auto dist = computeDistanceMatrix(X);
		std::vector<std::vector<double>> dist2(n, std::vector<double>(n, 0.0));
		for (size_t i = 0; i < n; ++i) {
			for (size_t j = 0; j < n; ++j) {
				double d = dist[i][j];
				dist2[i][j] = d * d;
			}
		}
		auto P = computeHighDimAffinities(dist2, perplexity);

		std::vector<std::vector<double>> Y(n, std::vector<double>(dim));
		for (size_t i = 0; i < n; ++i)
			for (int d = 0; d < dim; ++d)
				Y[i][d] = (std::rand() / (double)RAND_MAX - 0.5) * 1e-4;

		std::vector<std::vector<double>> iY(n, std::vector<double>(dim, 0.0));
		double momentum = 0.5;

		for (int iter = 0; iter < max_iter; ++iter) {
			auto Q = computeLowDimAffinities(Y);
			std::vector<std::vector<double>> grad(n, std::vector<double>(dim, 0.0));
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < n; ++j) {
					double mult = P[i][j] - Q[i][j];
					for (int d = 0; d < dim; ++d) {
						grad[i][d] += mult * (Y[i][d] - Y[j][d]);
					}
				}
			}
			for (size_t i = 0; i < n; ++i) {
				for (int d = 0; d < dim; ++d) {
					iY[i][d] = momentum * iY[i][d] - lr * grad[i][d];
					Y[i][d] += iY[i][d];
				}
			}
			if (iter == 250) momentum = 0.8;
		}

		return Y;
	}

	/**
	 * @brief 转置矩阵
	 *
	 * @param A 原始矩阵，大小 rows*cols
	 * @return 转置矩阵，大小 cols*rows
	 */
	inline std::vector<std::vector<double>> transposeMatrix(
		const std::vector<std::vector<double>>& A
	)
	{
		size_t rows = A.size();
		size_t cols = A[0].size();
		std::vector<std::vector<double>> At(cols, std::vector<double>(rows));
		for (size_t i = 0; i < rows; ++i) {
			for (size_t j = 0; j < cols; ++j) {
				At[j][i] = A[i][j];
			}
		}
		return At;
	}

	/**
	 * @brief 局部线性嵌入（LLE）降维算法
	 * @param X 输入数据矩阵，大小 n*d。
	 * @param n_neighbors 每个点的近邻数 k。
	 * @param dim 目标降维维度。
	 * @param reg 正则化系数，防止局部协方差矩阵奇异，默认为 1e-3。
	 * @return 大小 n*dim 的降维嵌入矩阵。
	 * @throws std::invalid_argument 若输入数据为空或参数不合法。
	 */
	inline std::vector<std::vector<double>> performLLE(
		const std::vector<std::vector<double>>& X,
		int n_neighbors,
		int dim,
		double reg = 1e-3
	)
	{
		if (X.empty() || n_neighbors <= 0 || dim <= 0) {
			throw std::invalid_argument("Invalid input to performLLE");
		}
		size_t n = X.size();

		std::vector<std::vector<double>> dist = computeDistanceMatrix(X);

		// k 个最近邻索引
		std::vector<std::vector<int>> neighbors(n);
		for (size_t i = 0; i < n; ++i) {
			// 按距离排序前 k+1（包含自身）
			std::vector<std::pair<double, int>> order;
			order.reserve(n);
			for (size_t j = 0; j < n; ++j) {
				order.push_back(std::make_pair(dist[i][j], static_cast<int>(j)));
			}
			std::nth_element(order.begin(), order.begin() + n_neighbors,
				order.end(),
				std::bind(std::less<std::pair<double, int>>(),
					std::placeholders::_1,
					std::placeholders::_2));
			neighbors[i].reserve(n_neighbors);
			int count = 0;
			for (size_t p = 0; p < order.size() && count < n_neighbors; ++p) {
				int idx = order[p].second;
				if (idx == static_cast<int>(i)) continue;
				neighbors[i].push_back(idx);
				++count;
			}
		}

		std::vector<std::vector<double>> W(n, std::vector<double>(n, 0.0));
		for (size_t i = 0; i < n; ++i) {
			// 局部协方差矩阵 C (k*k)
			std::vector<std::vector<double>> C(n_neighbors,
				std::vector<double>(n_neighbors, 0.0));
			for (int a = 0; a < n_neighbors; ++a) {
				for (int b = 0; b < n_neighbors; ++b) {
					double sum = 0.0;
					for (size_t d = 0; d < X[i].size(); ++d) {
						double da = X[i][d] - X[neighbors[i][a]][d];
						double db = X[i][d] - X[neighbors[i][b]][d];
						sum += da * db;
					}
					C[a][b] = sum;
				}
			}
			// 正则化
			double trace = 0.0;
			for (int a = 0; a < n_neighbors; ++a) {
				trace += C[a][a];
			}
			for (int a = 0; a < n_neighbors; ++a) {
				C[a][a] += reg * trace;
			}
			std::vector<double> ones(n_neighbors, 1.0);
			for (int p = 0; p < n_neighbors; ++p) {
				int maxr = p;
				for (int r = p + 1; r < n_neighbors; ++r) {
					if (std::fabs(C[r][p]) > std::fabs(C[maxr][p])) {
						maxr = r;
					}
				}
				std::swap(C[p], C[maxr]);
				std::swap(ones[p], ones[maxr]);
				double diag = C[p][p];
				if (std::fabs(diag) < 1e-12) {
					throw std::runtime_error("Singular matrix in LLE weight solve");
				}
				for (int c = p; c < n_neighbors; ++c) {
					C[p][c] /= diag;
				}
				ones[p] /= diag;
				for (int r = p + 1; r < n_neighbors; ++r) {
					double factor = C[r][p];
					for (int c = p; c < n_neighbors; ++c) {
						C[r][c] -= factor * C[p][c];
					}
					ones[r] -= factor * ones[p];
				}
			}
			std::vector<double> w(n_neighbors);
			for (int irow = n_neighbors - 1; irow >= 0; --irow) {
				double sum = ones[irow];
				for (int jcol = irow + 1; jcol < n_neighbors; ++jcol) {
					sum -= C[irow][jcol] * w[jcol];
				}
				w[irow] = sum;
			}
			double s = 0.0;
			for (int a = 0; a < n_neighbors; ++a) {
				s += w[a];
			}
			for (int a = 0; a < n_neighbors; ++a) {
				W[i][neighbors[i][a]] = w[a] / s;
			}
		}

		// 构造 M = (I - W)^T * (I - W)
		std::vector<std::vector<double>> IminusW(n,
			std::vector<double>(n, 0.0));
		for (size_t i = 0; i < n; ++i) {
			for (size_t j = 0; j < n; ++j) {
				IminusW[i][j] = ((i == j) ? 1.0 : 0.0) - W[i][j];
			}
		}
		std::vector<std::vector<double>> At = transposeMatrix(IminusW);
		std::vector<std::vector<double>> M = multiplyMatrix(At, IminusW);

		// 特征分解 M
		std::vector<double> evals;
		std::vector<std::vector<double>> evecs;
		std::tie(evals, evecs) = eigenvalues_and_eigenvectors(M, 1000, 1e-6);
		std::vector<int> idx(n);
		std::iota(idx.begin(), idx.end(), 0);
		std::sort(idx.begin(), idx.end(), [&](int a, int b) {
			return evals[a] < evals[b];
			});
		std::vector<std::vector<double>> Y(n, std::vector<double>(dim, 0.0));
		for (int d = 0; d < dim; ++d) {
			int ev = idx[d + 1];
			for (size_t i = 0; i < n; ++i) {
				Y[i][d] = evecs[i][ev];
			}
		}

		return Y;
	}

	/*
	 第六次扩展
	 DESeq2
	 */

	 /**
	  * @struct DESeq2Result
	  * @brief 存储差异表达分析结果：log2FC、pvalue、padj
	  */
	struct DESeq2Result {
		double log2FC;
		double pvalue;
		double padj;
	};

	/**
	 * @brief 计算几何平均
	 * @param counts G×N 原始计数矩阵
	 * @return 长度 G 的几何平均向量
	 */
	inline std::vector<double> computeGeometricMeans(
		const std::vector<std::vector<double>>& counts
	) {
		size_t G = counts.size();
		size_t N = counts.front().size();
		std::vector<double> gm(G);
		for (size_t i = 0; i < G; ++i) {
			double sumLog = 0.0;
			int cnt = 0;
			for (size_t j = 0; j < N; ++j) {
				double x = counts[i][j];
				if (x > 0) {
					sumLog += std::log(x);
					++cnt;
				}
			}
			gm[i] = cnt > 0 ? std::exp(sumLog / cnt) : 0.0;
		}
		return gm;
	}

	/**
	 * @brief 计算 size factors（Median-of-Ratios）
	 * @param counts G×N 原始计数矩阵
	 * @param gm     G 维几何平均向量
	 * @return N 维 size factor 向量
	 */
	inline std::vector<double> computeSizeFactors(
		const std::vector<std::vector<double>>& counts,
		const std::vector<double>& gm
	) {
		size_t G = counts.size();
		size_t N = counts.front().size();
		std::vector<double> sf(N);
		for (size_t j = 0; j < N; ++j) {
			std::vector<double> ratios;
			ratios.reserve(G);
			for (size_t i = 0; i < G; ++i) {
				if (gm[i] > 0.0) {
					ratios.push_back(counts[i][j] / gm[i]);
				}
			}
			if (ratios.empty()) {
				throw std::runtime_error("无法计算 size factors");
			}
			size_t mid = ratios.size() / 2;
			std::nth_element(ratios.begin(), ratios.begin() + mid, ratios.end());
			sf[j] = ratios[mid];
		}
		return sf;
	}

	/**
	 * @brief 归一化 counts
	 * @param counts G×N 原始计数矩阵
	 * @param sf     N 维 size factor 向量
	 * @return G×N 归一化矩阵
	 */
	inline std::vector<std::vector<double>> normalizeCounts(
		const std::vector<std::vector<double>>& counts,
		const std::vector<double>& sf
	) {
		size_t G = counts.size();
		size_t N = sf.size();
		std::vector<std::vector<double>> nc = counts;
		for (size_t i = 0; i < G; ++i) {
			for (size_t j = 0; j < N; ++j) {
				nc[i][j] = counts[i][j] / sf[j];
			}
		}
		return nc;
	}

	/**
	 * @brief 原始 dispersion 方法矩估计
	 * @param normCounts G×N 归一化后矩阵
	 * @param muBar_out  输出：G 维平均表达向量
	 * @return G 维 raw dispersion 向量
	 */
	inline std::vector<double> estimateRawDispersions(
		const std::vector<std::vector<double>>& normCounts,
		std::vector<double>& muBar_out
	) {
		size_t G = normCounts.size();
		size_t N = normCounts.front().size();
		std::vector<double> raw(G);
		muBar_out.assign(G, 0.0);
		for (size_t i = 0; i < G; ++i) {
			double sum = std::accumulate(normCounts[i].begin(), normCounts[i].end(), 0.0);
			double mu = sum / N;
			muBar_out[i] = mu;
			double var = 0.0;
			for (size_t j = 0; j < N; ++j) {
				double diff = normCounts[i][j] - mu;
				var += diff * diff;
			}
			var /= (N > 1 ? N - 1 : 1);
			raw[i] = std::max((var - mu) / (mu * mu), 0.0);
		}
		return raw;
	}

	/**
	 * @brief 收缩 dispersion（简单 trend 收缩）
	 * @param raw    原始 dispersion
	 * @param muBar  平均表达
	 * @return 收缩后 dispersion
	 */
	inline std::vector<double> shrinkDispersions(
		const std::vector<double>& raw,
		const std::vector<double>& muBar
	) {
		size_t G = raw.size();
		std::vector<double> shrink(G);
		for (size_t i = 0; i < G; ++i) {
			shrink[i] = raw[i];
		}
		return shrink;
	}

	/**
	 * @brief 计算 log2FC 与 Wald p-value
	 * @param nc_i   某基因归一化表达向量
	 * @param groups 样本分组向量
	 * @param alpha  dispersion
	 * @return std::pair<double,double> first=log2FC, second=p-value
	 */
	inline std::pair<double, double> computeWaldTest(
		const std::vector<double>& nc_i,
		const std::vector<int>& groups,
		double alpha
	) {
		int n0 = 0, n1 = 0;
		double sum0 = 0, sum1 = 0;
		for (size_t j = 0; j < nc_i.size(); ++j) {
			if (groups[j] == 0) { sum0 += nc_i[j]; ++n0; }
			else { sum1 += nc_i[j]; ++n1; }
		}
		double mu0 = sum0 / n0;
		double mu1 = sum1 / n1;
		double log2FC = std::log2(mu1 / mu0);

		// NB 方差
		double var0 = mu0 + alpha * mu0 * mu0;
		double var1 = mu1 + alpha * mu1 * mu1;
		double se = std::sqrt(var0 / (n0 * mu0 * mu0)
			+ var1 / (n1 * mu1 * mu1))
			/ std::log(2.0);

		double w = log2FC / se;
		double p = 2 * (1 - 0.5 * (1 + std::erf(std::abs(w) / std::sqrt(2.0))));
		return std::make_pair(log2FC, p);
	}

	/**
	 * @brief Benjamini-Hochberg FDR 校正
	 * @param res 输入/输出结果向量，函数会填充 res[i].padj
	 */
	inline void adjustPValues(std::vector<DESeq2Result>& res) {
		int M = static_cast<int>(res.size());
		std::vector<std::pair<double, int>> pv;
		pv.reserve(M);
		for (int i = 0; i < M; ++i) {
			pv.push_back(std::make_pair(res[i].pvalue, i));
		}
		std::sort(pv.begin(), pv.end());
		std::vector<double> adj(M);
		double minq = 1.0;
		for (int k = M; k >= 1; --k) {
			double q = pv[k - 1].first * M / k;
			if (q < minq) minq = q;
			adj[pv[k - 1].second] = minq;
		}
		for (int i = 0; i < M; ++i) {
			res[i].padj = adj[i];
		}
	}

	/**
	 * @brief 简化版 DESeq2 主接口
	 * @param counts G×N 原始计数矩阵
	 * @param groups 长度 N 样本分组
	 * @return 长度 G 的差异表达结果向量
	 */
	inline std::vector<DESeq2Result> performDESeq2(
		const std::vector<std::vector<double>>& counts,
		const std::vector<int>& groups
	) {
		if (counts.empty()) {
			throw std::invalid_argument("counts 为空");
		}
		size_t G = counts.size();
		size_t N = counts.front().size();
		/*if (groups.size() != N) {
			throw std::invalid_argument("groups 长度不匹配");
		}*/

		std::vector<double> gm = computeGeometricMeans(counts);
		std::vector<double> sf = computeSizeFactors(counts, gm);
		std::vector<std::vector<double>> nc = normalizeCounts(counts, sf);
		std::vector<double> muBar;
		std::vector<double> raw = estimateRawDispersions(nc, muBar);
		std::vector<double> disp = shrinkDispersions(raw, muBar);

		std::vector<DESeq2Result> results(G);
		for (size_t i = 0; i < G; ++i) {
			std::pair<double, double> pr = computeWaldTest(nc[i], groups, disp[i]);
			results[i].log2FC = pr.first;
			results[i].pvalue = pr.second;
		}
		adjustPValues(results);
		return results;
	}

	/*
	 第七次扩展
	 降维DBSCAN kMeansPP
	 */
	 /**
	  * @brief 找到样本 idx 在数据集中的 eps-邻域
	  * @param data 数据集 n×m 矩阵
	  * @param idx  样本索引
	  * @param eps  邻域半径
	  * @return 距离 <= eps 的所有样本索引列表
	  */
	inline std::vector<size_t> regionQuery(
		const std::vector<std::vector<double>>& data,
		size_t idx,
		double eps
	) {
		std::vector<size_t> neighbors;
		for (size_t j = 0; j < data.size(); ++j) {
			if (euclideanDistance(data[idx], data[j]) <= eps) {
				neighbors.push_back(j);
			}
		}
		return neighbors;
	}

	/**
	 * @brief 扩展簇：将所有密度可达的点加入同一簇
	 * @param data      数据集 n×m
	 * @param labels    样本簇标签向量，-1 表示未分类或噪声
	 * @param visited   样本访问标记向量
	 * @param idx       当前样本索引
	 * @param clusterId 当前簇 ID
	 * @param eps       邻域半径
	 * @param minPts    构成核心点的最小邻居数
	 */
	inline void expandCluster(
		const std::vector<std::vector<double>>& data,
		std::vector<int>& labels, std::vector<bool>& visited,
		size_t idx,
		int clusterId,
		double eps,
		int minPts
	) {
		// 获取邻域
		std::vector<size_t> seedSet = regionQuery(data, idx, eps);
		labels[idx] = clusterId;

		for (size_t k = 0; k < seedSet.size(); ++k) {
			size_t cur = seedSet[k];
			if (!visited[cur]) {
				visited[cur] = true;
				// 如果是核心点，则将其邻域加入 seedSet
				std::vector<size_t> curNeighbors = regionQuery(data, cur, eps);
				if (curNeighbors.size() >= static_cast<size_t>(minPts)) {
					seedSet.insert(seedSet.end(), curNeighbors.begin(), curNeighbors.end());
				}
			}
			// 如果该点尚未分配簇，则分配到当前簇
			if (labels[cur] == -1) {
				labels[cur] = clusterId;
			}
		}
	}

	/**
	 * @brief DBSCAN 密度聚类算法
	 * @param data   数据集 n×m 矩阵
	 * @param eps    邻域半径
	 * @param minPts 构成核心点的最小邻域数
	 * @return 每个样本的簇标签向量，-1 表示噪声，其余 0,1,2... 为簇 ID
	 */
	inline std::vector<int> dbscan(
		const std::vector<std::vector<double>>& data,
		double eps,
		int minPts
	) {
		size_t n = data.size();
		std::vector<int> labels(n, -1);
		std::vector<bool> visited(n, false);
		int clusterId = 0;

		for (size_t i = 0; i < n; ++i) {
			if (visited[i]) continue;
			visited[i] = true;
			std::vector<size_t> neighbors = regionQuery(data, i, eps);
			// 如果邻居数不足，则标记为噪声
			if (neighbors.size() < static_cast<size_t>(minPts)) {
				labels[i] = -1;
			}
			else {
				// 扩展新簇
				expandCluster(data, labels, visited, i, clusterId, eps, minPts);
				++clusterId;
			}
		}

		return labels;
	}

	/**
	 * @brief 使用 K-means++ 算法初始化质心
	 * @param data   输入数据，大小 n×m (n 个样本，每个样本 m 维)
	 * @param k      聚类簇数
	 * @return 返回初始化好的 k 个质心，每个质心为 m 维向量
	 * @throws std::invalid_argument 如果 data 为空或 k<=0 或 k>n
	 */
	inline std::vector<std::vector<double>> initializeCentroidsPP(
		const std::vector<std::vector<double>>& data,
		int k
	) {
		size_t n = data.size();
		if (n == 0 || k <= 0 || static_cast<size_t>(k) > n)
		{
			throw std::invalid_argument("initializeCentroidsPP: 参数非法");
		}
		size_t m = data[0].size();
		std::vector<std::vector<double>> centroids;
		centroids.reserve(k);

		// 随机数生成器
		std::random_device rd;
		std::mt19937 gen(rd());

		// 随机选择
		std::uniform_int_distribution<size_t> unif(0, n - 1);
		centroids.push_back(data[unif(gen)]);

		std::vector<double> dist2(n);
		for (int c = 1; c < k; ++c)
		{
			for (size_t i = 0; i < n; ++i)
			{
				double dmin = std::numeric_limits<double>::infinity();
				for (const auto& cent : centroids)
				{
					double d = euclideanDistance(data[i], cent);
					dmin = std::min(dmin, d);
				}
				dist2[i] = dmin * dmin;
			}
			// 用距离平方作为权重
			std::discrete_distribution<size_t> dist(dist2.begin(), dist2.end());
			centroids.push_back(data[dist(gen)]);
		}

		return centroids;
	}

	/**
	 * @brief 基于 K-means++ 初始化并执行 K-means 聚类
	 * @param data     输入数据，大小 n×m
	 * @param k        聚类簇数
	 * @param max_iter 最大迭代次数，默认 1000
	 * @return labels  长度 n，每个元素为样本所属簇 ID [0..k-1]
	 * @throws std::invalid_argument 如果 data 为空或 k<=0
	 */
	inline std::vector<int> kMeansPP(
		const std::vector<std::vector<double>>& data,
		int k,
		int max_iter = 1000
	) {
		size_t n = data.size();
		if (n == 0 || k <= 0)
		{
			throw std::invalid_argument("kMeansPP: 参数非法");
		}
		size_t m = data[0].size();

		std::vector<std::vector<double>> centroids = initializeCentroidsPP(data, k);
		std::vector<int> labels(n, -1);
		std::vector<int> counts(k, 0);

		for (int iter = 0; iter < max_iter; ++iter)
		{
			bool changed = false;

			// 分配每个点到最近质心
			for (size_t i = 0; i < n; ++i)
			{
				double dmin = std::numeric_limits<double>::infinity();
				int best = -1;
				for (int c = 0; c < k; ++c)
				{
					double d = euclideanDistance(data[i], centroids[c]);
					if (d < dmin)
					{
						dmin = d;
						best = c;
					}
				}
				if (labels[i] != best)
				{
					labels[i] = best;
					changed = true;
				}
			}

			// 如果没有变化则终止
			if (!changed)
			{
				break;
			}

			std::vector<std::vector<double>> sum(k, std::vector<double>(m, 0.0));
			std::fill(counts.begin(), counts.end(), 0);

			// 累加各簇样本
			for (size_t i = 0; i < n; ++i)
			{
				int c = labels[i];
				for (size_t j = 0; j < m; ++j)
				{
					sum[c][j] += data[i][j];
				}
				++counts[c];
			}

			// 更新质心
			for (int c = 0; c < k; ++c)
			{
				if (counts[c] > 0) {
					for (size_t j = 0; j < m; ++j)
					{
						centroids[c][j] = sum[c][j] / counts[c];
					}
				}
				else
				{
					// 若某簇无样本，可随机重置一个质心
					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_int_distribution<size_t> unif(0, n - 1);
					centroids[c] = data[unif(gen)];
				}
			}
		}

		return labels;
	}
}

#endif
