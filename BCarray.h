#pragma once

#ifndef BCARRAY_H
#define BCARRAY_H

#include <iostream>
using namespace std;
#include <vector>
#include <functional>
#include <algorithm>
#include "ST.h"

// 继承vector，再写一个类，实现一些新的功能
// 在vector的基础上，加入大量的统计功能
// 以及重载一些运算符，还要进行广播操作
template <typename T>
class BCarray : public vector<T>
{
private:
    bool is_row_vector = true; // 是否是行向量

public:
    // 构造函数
    BCarray() : vector<T>(), is_row_vector(true) {}

    BCarray(const vector<T> &vec, bool is_row_vector = true) : vector<T>(vec), is_row_vector(is_row_vector) {}

    BCarray(size_t count, const T &value = T(), bool is_row_vector = true) : vector<T>(count, value), is_row_vector(is_row_vector) {}

    BCarray(T array[], size_t size, bool is_row_vector = true)
    {
        this->assign(array, array + size);
        this->is_row_vector = is_row_vector;
    }

    bool isRowVector() const;

    /*统计函数*/
    // 检查是否有重复值
    bool hasDuplicates() const;
    // 计算和
    T sum() const;
    // 计算最大值
    T max() const;
    // 计算最小值
    T min() const;
    // 计算均值
    double mean() const;
    // 计算范数
    double norm(int ord = 2) const;
    // 计算中位数
    T median() const;
    // 总体方差
    double populationVar() const;
    // 样本方差
    double sampleVar() const;
    // 总体方差 或 样本方差
    double var(bool isSample = false) const;
    // 总体标准差
    double populationStd() const;
    // 样本标准差
    double sampleStd() const;
    // 总体标准差 或 样本标准差
    double std(bool isSample = false) const;
    // 偏度
    double skewness() const;
    // 峰度
    double kurtosis() const;
    // 计算百分位数
    double percentile(double p) const;
    // 众数
    T mode() const;
    // 计算协方差
    double covariance(const BCarray<T> &vec, bool isSample = false) const;
    // 计算相关系数
    double correlation(const BCarray<T> &vec) const;
    // 打印统计信息
    void info() const;

    /*一系列重载*/
    friend ostream &operator<< <T>(ostream &os, const BCarray<T> &vec);
    BCarray operator+(const BCarray<T> &vec2) const;
    BCarray operator+(const T &scalar) const;
    BCarray operator-(const BCarray<T> &vec2) const;
    BCarray operator-(const T &scalar) const;
    BCarray operator-() const;
    BCarray operator*(const BCarray<T> &vec2) const;
    BCarray operator*(const T &scalar) const;
    BCarray operator/(const BCarray<T> &vec2) const;
    BCarray operator/(const T &scalar) const;
    BCarray operator&&(const BCarray<T> &vec2) const;
    BCarray operator&&(const bool &scalar) const;
    BCarray operator||(const BCarray<T> &vec2) const;
    BCarray operator||(const bool &scalar) const;
    BCarray operator!() const;

    /*一些数学函数*/
    // 进行任意的自定义函数
    template <typename U>
    BCarray<U> apply(function<U(const T &)>);
    // 排序
    BCarray<T> sort(bool ascending = true);
    // BCarray<size_t> argsort(ascending = true);
    void sort_(bool ascending = true);
    // 内积
    T dot(const BCarray<T> &vec);
    // 转置。一般都是行向量，显示的.T()就会是列向量
    BCarray<T> t() const;

    // 各种标准化
    BCarray<T> L2Normalize() const;
    BCarray<T> minMaxNormalize() const;
    BCarray<T> zScoreNormalize() const;
    BCarray<T> centralize() const;
    BCarray<T> normalize(string method = "L2") const;

    void L2Normalize_();
    void minMaxNormalize_();
    void zScoreNormalize_();
    void centralize_();
    void normalize_(string method = "L2");

    // 拆分实验组和对照组
    pair<BCarray<T>, BCarray<T>> split(vector<int> &group);
};

// 实现
template <typename T>
bool BCarray<T>::isRowVector() const
{
    return is_row_vector;
}

template <typename T>
bool BCarray<T>::hasDuplicates() const
{
    return StatTools::hasDuplicates(*this);
}

template <typename T>
T BCarray<T>::sum() const
{
    return StatTools::sum(*this);
}

template <typename T>
T BCarray<T>::max() const
{
    return StatTools::max(*this);
}

template <typename T>
T BCarray<T>::min() const
{
    return StatTools::min(*this);
}

template <typename T>
double BCarray<T>::mean() const
{
    return StatTools::mean(*this);
}

template <typename T>
double BCarray<T>::norm(int ord) const
{
    return StatTools::norm(*this, ord);
}

template <typename T>
T BCarray<T>::median() const
{
    return StatTools::median(*this);
}

template <typename T>
double BCarray<T>::populationVar() const
{
    return StatTools::populationVar(*this);
}

template <typename T>
double BCarray<T>::sampleVar() const
{
    return StatTools::sampleVar(*this);
}

template <typename T>
double BCarray<T>::var(bool isSample) const
{
    return StatTools::var(*this, isSample);
}

template <typename T>
double BCarray<T>::populationStd() const
{
    return StatTools::populationStd(*this);
}

template <typename T>
double BCarray<T>::sampleStd() const
{
    return StatTools::sampleStd(*this);
}

template <typename T>
double BCarray<T>::std(bool isSample) const
{
    return StatTools::std(*this, isSample);
}

template <typename T>
double BCarray<T>::skewness() const
{
    return StatTools::skewness(*this);
}

template <typename T>
double BCarray<T>::kurtosis() const
{
    return StatTools::kurtosis(*this);
}

template <typename T>
double BCarray<T>::percentile(double p) const
{
    return StatTools::percentile(*this, p);
}

template <typename T>
T BCarray<T>::mode() const
{
    return StatTools::mode(*this);
}

template <typename T>
double BCarray<T>::covariance(const BCarray<T> &vec, bool isSample) const
{
    return StatTools::covariance(*this, vec, isSample);
}

template <typename T>
double BCarray<T>::correlation(const BCarray<T> &vec) const
{
    return StatTools::correlation(*this, vec);
}

template <typename T>
void BCarray<T>::info() const
{
    // 打印各种统计信息
    cout << "size:\t" << this->size() << endl;
    cout << "mean:\t" << mean() << endl;
    cout << "mode:\t" << mode() << endl;

    cout << "min:\t" << min() << endl;
    cout << "25th percentile:\t" << percentile(0.25) << endl;
    cout << "median:\t" << median() << endl;
    cout << "75th percentile:\t" << percentile(0.75) << endl;
    cout << "max:\t" << max() << endl;

    cout << "variance:\t" << var() << endl;
    cout << "standard deviation:\t" << std() << endl;
    cout << "skewness:\t" << skewness() << endl;
    cout << "kurtosis:\t" << kurtosis() << endl;
    cout << endl;
}

template <typename T>
ostream &operator<<(ostream &os, const BCarray<T> &vec)
{
    os << "BCarray:\t";
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        os << vec[i];
        if (i != vec.size() - 1)
        {
            os << ", ";
        }
    }
    os << "]";
    // 输出数据类型
    os << "\ttype: " << typeid(T).name();
    return os;
}

template <typename T>
BCarray<T> BCarray<T>::operator+(const BCarray<T> &vec2) const
{
    BCarray<T> res(this->size());
    for (size_t i = 0; i < this->size(); ++i)
    {
        res[i] = (*this)[i] + vec2[i];
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator+(const T &scalar) const
{
    BCarray<T> res(*this);
    for (size_t i = 0; i < res.size(); ++i)
    {
        res[i] += scalar;
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator-(const BCarray<T> &vec2) const
{
    BCarray<T> res(this->size());
    for (size_t i = 0; i < this->size(); ++i)
    {
        res[i] = (*this)[i] - vec2[i];
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator-(const T &scalar) const
{
    BCarray<T> res(*this);
    for (size_t i = 0; i < res.size(); ++i)
    {
        res[i] -= scalar;
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator-() const
{
    BCarray<T> res(this->size());
    for (size_t i = 0; i < this->size(); ++i)
    {
        res[i] = -(*this)[i];
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator*(const BCarray<T> &vec2) const
{
    BCarray<T> res(this->size());
    for (size_t i = 0; i < this->size(); ++i)
    {
        res[i] = (*this)[i] * vec2[i];
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator*(const T &scalar) const
{
    BCarray<T> res(*this);
    for (size_t i = 0; i < res.size(); ++i)
    {
        res[i] *= scalar;
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator/(const BCarray<T> &vec2) const
{
    BCarray<T> res(this->size());
    for (size_t i = 0; i < this->size(); ++i)
    {
        res[i] = (*this)[i] / vec2[i];
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator/(const T &scalar) const
{
    BCarray<T> res(*this);
    for (size_t i = 0; i < res.size(); ++i)
    {
        res[i] /= scalar;
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator&&(const BCarray<T> &vec2) const
{
    BCarray<T> res(this->size());
    for (size_t i = 0; i < this->size(); ++i)
    {
        res[i] = (*this)[i] && vec2[i];
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator&&(const bool &scalar) const
{
    BCarray<T> res(this->size());
    for (size_t i = 0; i < this->size(); ++i)
    {
        res[i] = (*this)[i] && scalar;
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator||(const BCarray<T> &vec2) const
{
    BCarray<T> res(this->size());
    for (size_t i = 0; i < this->size(); ++i)
    {
        res[i] = (*this)[i] || vec2[i];
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator||(const bool &scalar) const
{
    BCarray<T> res(this->size());
    for (size_t i = 0; i < this->size(); ++i)
    {
        res[i] = (*this)[i] || scalar;
    }
    return res;
}

template <typename T>
BCarray<T> BCarray<T>::operator!() const
{
    BCarray<T> res(this->size());
    for (size_t i = 0; i < this->size(); ++i)
    {
        res[i] = !(*this)[i];
    }
    return res;
}

template <typename T>
template <typename U>
BCarray<U> BCarray<T>::apply(function<U(const T &)>)
{
    BCarray<U> result(this->size());
    for (int i = 0; i < this->size(); i++)
        result[i] = func(this->at(i));
    return result;
}
// 使用实例
/*
BCarray<double> a = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
BCarray<double> b = a.apply([](double x) {return x * x; });
BCarray<string> c = a.apply([](double x) {return to_string(x); });
*/

template <typename T>
BCarray<T> BCarray<T>::sort(bool ascending)
{
    BCarray<T> result(*this);
    sort(result.begin(), result.end(), [ascending](const T &a, const T &b)
         { return ascending ? a < b : b < a; });
    return result;
}

// template <typename T>
// BCarray<size_t> argsort(ascending = true)
// {

// }

template <typename T>
void BCarray<T>::sort_(bool ascending)
{
    sort(this->begin(), this->end(), [ascending](const T &a, const T &b)
         { return ascending ? a < b : b < a; });
}

template <typename T>
T BCarray<T>::dot(const BCarray<T> &vec)
{
    return (*this * vec).sum();
}

template <typename T>
BCarray<T> BCarray<T>::t() const
{
    BCarray<T> result(*this);
    result.is_row_vector = !this->is_row_vector;
    return result;
}

template <typename T>
BCarray<T> BCarray<T>::L2Normalize() const
{
    double normValue = this->norm(2);
    if (normValue == 0)
    {
        throw std::runtime_error("Cannot normalize a zero vector.");
    }
    BCarray<T> result(*this);
    result = result / normValue;
    return result;
}

template <typename T>
BCarray<T> BCarray<T>::minMaxNormalize() const
{
    T minValue = this->min();
    T maxValue = this->max();
    if (maxValue == minValue)
    {
        throw std::runtime_error("Cannot normalize a vector with all identical values.");
    }
    BCarray<T> result(*this);
    result = (result - minValue) / (maxValue - minValue);
    return result;
}

template <typename T>
BCarray<T> BCarray<T>::zScoreNormalize() const
{
    double meanValue = this->mean();
    double stdValue = this->std();
    if (stdValue == 0)
    {
        throw std::runtime_error("Cannot normalize a vector with zero standard deviation.");
    }
    BCarray<T> result(*this);
    result = (result - meanValue) / stdValue;
    return result;
}

template <typename T>
BCarray<T> BCarray<T>::centralize() const
{
    double meanValue = this->mean();
    BCarray<T> result(*this);
    result = result - meanValue;
    return result;
}

template <typename T>
BCarray<T> BCarray<T>::normalize(string method) const
{
    if (method == "L2")
    {
        return this->L2Normalize();
    }
    else if (method == "minmax")
    {
        return this->minMaxNormalize();
    }
    else if (method == "zscore")
    {
        return this->zScoreNormalize();
    }
    else if (method == "centralize")
    {
        return this->centralize();
    }
    else
    {
        throw std::invalid_argument("only L2, minmax, and zscore are supported.");
    }
}

template <typename T>
void BCarray<T>::L2Normalize_()
{
    double normValue = this->norm(2);
    if (normValue == 0)
    {
        throw std::runtime_error("Cannot normalize a zero vector.");
    }
    *this = *this / normValue;
}

template <typename T>
void BCarray<T>::minMaxNormalize_()
{
    T minValue = this->min();
    T maxValue = this->max();
    if (maxValue == minValue)
    {
        throw std::runtime_error("Cannot normalize a vector with all identical values.");
    }
    *this = (*this - minValue) / (maxValue - minValue);
}

template <typename T>
void BCarray<T>::zScoreNormalize_()
{
    double meanValue = this->mean();
    double stdValue = this->std();
    if (stdValue == 0)
    {
        throw std::runtime_error("Cannot normalize a vector with zero standard deviation.");
    }
    *this = (*this - meanValue) / stdValue;
}

template <typename T>
void BCarray<T>::centralize_()
{
    double meanValue = this->mean();
    *this = *this - meanValue;
}

template <typename T>
void BCarray<T>::normalize_(string method)
{
    if (method == "L2")
    {
        this->L2Normalize_();
    }
    else if (method == "minmax")
    {
        this->minMaxNormalize_();
    }
    else if (method == "zscore")
    {
        this->zScoreNormalize_();
    }
    else if (method == "centralize")
    {
        this->centralize_();
    }
    else
    {
        throw std::invalid_argument("only L2, minmax, and zscore are supported.");
    }
}

template <typename T>
pair<BCarray<T>, BCarray<T>> BCarray<T>::split(vector<int> &group)
{
    BCarray<T> group1, group2;
    for (int i = 0; i < group.size(); i++)
    {
        if (group[i] == 0)
        {
            group1.push_back(this->at(i));
        }
        else if (group[i] == 1)
        {
            group2.push_back(this->at(i));
        }
        else
        {
            throw std::invalid_argument("groupmust be 0 or 1.");
        }
    }
    return make_pair(group1, group2);
}

#endif
