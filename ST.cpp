#include "ST.h"
#include <type_traits>

namespace StatTools
{
    template <typename T>
    void checkVector(const std::vector<T>& vec) {
        if (vec.empty())
        {
            throw std::invalid_argument("Vector is empty");
        }
        static_assert(std::is_arithmetic<T>::value, "Vector elements must be numeric");
    }

    template <typename T>
    bool hasDuplicates(const std::vector<T>& vec)
    {
        return vec.size() != std::unordered_set<T>(vec.begin(), vec.end()).size();
    }

    template <typename T>
    T sum(const std::vector<T>& vec)
    {
        checkVector(vec);
        return std::accumulate(vec.begin(), vec.end(), T{});
    }

    template <typename T>
    T max(const std::vector<T>& vec)
    {
        checkVector(vec);
        return *std::max_element(vec.begin(), vec.end());
    }

    template <typename T>
    T min(const std::vector<T>& vec)
    {
        checkVector(vec);
        return *std::min_element(vec.begin(), vec.end());
    }

    template <typename T>
    double mean(const std::vector<T>& vec)
    {
        checkVector(vec);
        return static_cast<double>(sum<T>(vec)) / vec.size();
    }

    template <typename T>
    double norm(const std::vector<T>& vec)
    {
        checkVector(vec);
        double sumOfSquares = 0.0;
        for (const auto& x : vec)
        {
            sumOfSquares += x * x;
        }
        return std::sqrt(sumOfSquares);
    }

    template <typename T>
    double norm(const std::vector<T>& vec, const int& ord)
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

    template <typename T>
    T median(const std::vector<T>& vec)
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

    template <typename T>
    double populationVar(const std::vector<T>& vec)
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

    template <typename T>
    double sampleVar(const std::vector<T>& vec)
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

    template <typename T>
    double var(const std::vector<T>& vec, bool isSample)
    {
        return isSample ? sampleVar(vec) : populationVar(vec);
    }

    template <typename T>
    double populationStd(const std::vector<T>& vec)
    {
        return std::sqrt(populationVar(vec));
    }

    template <typename T>
    double sampleStd(const std::vector<T>& vec)
    {
        return std::sqrt(sampleVar(vec));
    }

    template <typename T>
    double std(const std::vector<T>& vec, bool isSample)
    {
        return std::sqrt(var(vec, isSample));
    }

    template <typename T>
    double skewness(const std::vector<T>& vec)
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

    template <typename T>
    double kurtosis(const std::vector<T>& vec)
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

    template <typename T>
    double percentile(const std::vector<T>& vec, double p)
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

    template <typename T>
    T mode(const std::vector<T>& vec)
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

    template <typename T>
    double covariance(const std::vector<T>& vec1, const std::vector<T>& vec2, bool isSample) 
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

    template <typename T>
    double correlation(const std::vector<T>& vec1, const std::vector<T>& vec2)
    {
        return covariance(vec1, vec2) / (std(vec1) * std(vec2));
    }
}