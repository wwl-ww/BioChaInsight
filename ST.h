#pragma once

#ifndef ST_H
#define ST_H

#define _USE_MATH_DEFINES
#include <iostream>
#include <numeric>
#include <vector>
#include <numeric>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <algorithm>
#include <unordered_set>
#include <map>
#include <unordered_map>
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
    inline void checkVector(const std::vector<T> &vec)
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
    inline bool hasDuplicates(const std::vector<T> &vec)
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
    inline T sum(const std::vector<T> &vec)
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
    inline T max(const std::vector<T> &vec)
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
    inline T min(const std::vector<T> &vec)
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
    inline double mean(const std::vector<T> &vec)
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
    inline double norm(const std::vector<T> &vec)
    {
        checkVector(vec);
        double sumOfSquares = 0.0;
        for (const auto &x : vec)
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
    inline double norm(const std::vector<T> &vec, int ord)
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
            for (const auto &x : vec)
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
            for (const auto &x : vec)
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
    inline T median(const std::vector<T> &vec)
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
    inline double populationVar(const std::vector<T> &vec)
    {
        checkVector(vec);
        double meanVal = mean(vec);
        double variance = 0.0;
        for (const auto &x : vec)
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
    inline double sampleVar(const std::vector<T> &vec)
    {
        checkVector(vec);
        double meanVal = mean(vec);
        double variance = 0.0;
        for (const auto &x : vec)
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
    inline double var(const std::vector<T> &vec, bool isSample = false)
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
    inline double populationStd(const std::vector<T> &vec)
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
    inline double sampleStd(const std::vector<T> &vec)
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
    inline double std(const std::vector<T> &vec, bool isSample = false)
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
    inline double skewness(const std::vector<T> &vec)
    {
        checkVector(vec);
        if (vec.size() < 3)
        {
            throw std::invalid_argument("Skewness is undefined for vector of size less than 3");
        }
        double meanVal = mean(vec);
        double stdVal = populationStd(vec);
        double skew = 0.0;
        for (const auto &x : vec)
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
    inline double kurtosis(const std::vector<T> &vec)
    {
        checkVector(vec);
        if (vec.size() < 4)
        {
            throw std::invalid_argument("Kurtosis is undefined for vector of size less than 4");
        }
        double meanVal = mean(vec);
        double stdVal = populationStd(vec);
        double kurt = 0.0;
        for (const auto &x : vec)
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
    inline double percentile(const std::vector<T> &vec, double p)
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
    inline T mode(const std::vector<T> &vec)
    {
        checkVector(vec);
        std::unordered_map<T, int> freqMap;
        for (const auto &x : vec)
        {
            freqMap[x]++;
        }
        T modeValue = vec[0];
        int maxFreq = 0;
        for (const auto &pair : freqMap)
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
    inline double covariance(const std::vector<T> &vec1, const std::vector<T> &vec2, bool isSample = false)
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
    inline double correlation(const std::vector<T> &vec1, const std::vector<T> &vec2)
    {
        return covariance(vec1, vec2) / (std(vec1) * std(vec2));
    }

    struct t_testResult
    {
        double t;
        double p_value;
        double log2_fc;
    };

    /*
    第二次扩展
    t-test
    */

    /**
     * Performs a two-sample t-test on two vectors.
     *
     * @tparam T The type of elements in the vectors.
     * @param vec1 The first vector.
     * @param vec2 The second vector.
     * @return A t_testResult object containing the t-statistic, p-value, and log2 fold change.
     */
    template <typename T>
    inline t_testResult t_test(const std::vector<T> &vec1, const std::vector<T> &vec2, bool log2_data = true)
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

        return {t, p, log2_fc};
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
            pv_with_index.push_back({p_values[i], i});
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
    inline std::vector<std::vector<double>> multiplyMatrix(const std::vector<std::vector<double>> &A, const std::vector<std::vector<double>> &B)
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
    inline std::pair<int, int> findMaxOffDiagonal(const std::vector<std::vector<double>> &mat)
    {
        int n = mat.size();
        double max_val = 0.0;
        std::pair<int, int> max_pos = {0, 1};
        for (int i = 0; i < n; ++i)
        {
            for (int j = i + 1; j < n; ++j)
            {
                double abs_val = fabs(mat[i][j]);
                if (abs_val > max_val)
                {
                    max_val = abs_val;
                    max_pos = {i, j};
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

        // Initialize Q as identity matrix
        for (size_t i = 0; i < rows; ++i)
        {
            Q[i][i] = 1.0;
        }

        for (size_t j = 0; j < cols; ++j)
        {
            // Step 1: Compute the R[j, j] element
            double norm = 0.0;
            for (size_t i = 0; i < rows; ++i)
            {
                norm += A[i][j] * A[i][j];
            }
            R[j][j] = sqrt(norm);

            // Step 2: Compute the Q column
            std::vector<double> q_col(rows);
            for (size_t i = 0; i < rows; ++i)
            {
                q_col[i] = A[i][j] / R[j][j];
            }

            // Step 3: Compute the Q matrix and update the remaining columns
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

            // Step 4: Update the Q matrix
            for (size_t i = 0; i < rows; ++i)
            {
                Q[i][j] = q_col[i];
            }
        }

        return {Q, R};
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
    inline std::pair<std::vector<double>, std::vector<std::vector<double>>> eigenvalues_and_eigenvectors(std::vector<std::vector<double>> &A, int max_iter = 1000, double tol = 1e-6)
    {
        size_t n = A.size();
        std::vector<double> eigenvals(n);
        std::vector<std::vector<double>> eigenvectors(n, std::vector<double>(n, 0.0));

        // Initialize eigenvectors as identity matrix
        for (size_t i = 0; i < n; ++i)
        {
            eigenvectors[i][i] = 1.0;
        }

        for (int iter = 0; iter < max_iter; ++iter)
        {
            // Step 1: Perform QR decomposition
            std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>> result = qrDecomposition(A);
            std::vector<std::vector<double>> Q = result.first;
            std::vector<std::vector<double>> R = result.second;

            // Step 2: Update matrix A = R * Q
            A = multiplyMatrix(R, Q);

            // Step 3: Extract eigenvalues (diagonal elements)
            bool converged = true;
            for (size_t i = 0; i < n; ++i)
            {
                eigenvals[i] = A[i][i];
                if (fabs(A[i][i] - eigenvals[i]) > tol)
                {
                    converged = false;
                }
            }

            // Step 4: Update eigenvectors
            eigenvectors = multiplyMatrix(Q, eigenvectors);

            // Step 5: Check for convergence
            if (converged)
                break;
        }

        return {eigenvals, eigenvectors};
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

        // 1. 数据标准化
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

        // 2. 计算协方差矩阵
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

        // 3. 计算特征值和特征向量
        auto result = eigenvalues_and_eigenvectors(cov_matrix);

        // 4. 选择前num_components个主成分
        std::vector<size_t> indices(col);
        std::iota(indices.begin(), indices.end(), 0);
        sort(indices.begin(), indices.end(), [&result](size_t i1, size_t i2)
             { return result.first[i1] > result.first[i2]; });

        // 5. 投影数据到主成分
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
    inline double euclideanDistance(const std::vector<double> &a, const std::vector<double> &b)
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
    inline std::vector<int> kMeans(const std::vector<std::vector<double>> &data, int k, int max_iter = 1000)
    {
        size_t n = data.size();
        size_t m = data[0].size();

        // 初始化簇中心（随机选择K个数据点作为初始簇中心）
        std::vector<std::vector<double>> centroids(k, std::vector<double>(m));
        std::vector<int> labels(n, -1); // 存储每个数据点的簇标签

        // 随机选择初始簇中心
        srand(time(0));
        std::vector<int> indices(n);
        std::iota(indices.begin(), indices.end(), 0);
        random_shuffle(indices.begin(), indices.end());

        for (int i = 0; i < k; ++i)
        {
            centroids[i] = data[indices[i]]; // 将随机选择的数据点作为簇中心
        }

        // 迭代进行 K-means 聚类
        for (int iter = 0; iter < max_iter; ++iter)
        {
            bool changed = false;

            // 步骤 1：分配每个点到最近的簇
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

                // 如果数据点的簇标签发生变化，标记为改变
                if (labels[i] != closest_centroid)
                {
                    labels[i] = closest_centroid;
                    changed = true;
                }
            }

            // 步骤 2：更新簇中心为簇中所有点的均值
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
                    new_centroids[j][i] /= count[j]; // 更新簇中心
                }
            }

            centroids = new_centroids;

            // 如果没有簇中心变化，停止迭代
            if (!changed)
            {
                break;
            }
        }

        return labels; // 返回每个点的簇标签
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
     * @param data N×D matrix
     * @return N×N symmetric matrix of distances
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

            // 合并 a, b → merged
            auto merged = clusters[a];
            merged.insert(merged.end(),
                clusters[b].begin(),
                clusters[b].end());

            // 记录 linkage
            Z.push_back({ a, b, best, (int)merged.size() });

            // **先删掉旧簇 `a` 和 `b`**
            clusters.erase(a);
            clusters.erase(b);

            // 再插入新簇 `nextId`→merged
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
    

}

#endif
