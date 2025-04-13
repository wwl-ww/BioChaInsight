# pragma once

#ifndef ST_H
#define ST_H

#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <unordered_set>
#include <map>
#include<unordered_map>
#include <boost/math/distributions/students_t.hpp>

namespace StatTools
{
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
    inline double covariance(const std::vector<T>& vec1, const std::vector<T>& vec2, bool isSample = false) {
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
        double s1 = var(vec1, true)/vec1.size();
        double s2 = var(vec2, true)/vec2.size();

        // 计算log2 fold change
        double log2_fc = 0;
        if(log2_data)
            // 数据已经经过了log2处理，所以直接相减即可
            log2_fc = m1 - m2;
        else
            log2_fc = std::log2(m1/m2);
        
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
    
}

#endif 