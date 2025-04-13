#pragma once

#ifndef BCARRAY_H
#define BCARRAY_H

#include <iostream>
using namespace std;
#include <vector>
#include <functional>
#include <algorithm>
#include "ST.h"

// �̳�vector����дһ���࣬ʵ��һЩ�µĹ���
// ��vector�Ļ����ϣ����������ͳ�ƹ���
// �Լ�����һЩ���������Ҫ���й㲥����
template <typename T>
class BCarray : public vector<T>
{
private:
    bool is_row_vector = true; // �Ƿ���������

public:
    // ���캯��
    BCarray() : vector<T>(), is_row_vector(true) {}

    BCarray(const vector<T> &vec, bool is_row_vector = true) : vector<T>(vec), is_row_vector(is_row_vector) {}

    BCarray(size_t count, const T &value = T(), bool is_row_vector = true) : vector<T>(count, value), is_row_vector(is_row_vector) {}

    BCarray(T array[], size_t size, bool is_row_vector = true)
    {
        this->assign(array, array + size);
        this->is_row_vector = is_row_vector;
    }

    bool isRowVector() const;

    /*ͳ�ƺ���*/
    // ����Ƿ����ظ�ֵ
    bool hasDuplicates() const;
    // �����
    T sum() const;
    // �������ֵ
    T max() const;
    // ������Сֵ
    T min() const;
    // �����ֵ
    double mean() const;
    // ���㷶��
    double norm(int ord = 2) const;
    // ������λ��
    T median() const;
    // ���巽��
    double populationVar() const;
    // ��������
    double sampleVar() const;
    // ���巽�� �� ��������
    double var(bool isSample = false) const;
    // �����׼��
    double populationStd() const;
    // ������׼��
    double sampleStd() const;
    // �����׼�� �� ������׼��
    double std(bool isSample = false) const;
    // ƫ��
    double skewness() const;
    // ���
    double kurtosis() const;
    // ����ٷ�λ��
    double percentile(double p) const;
    // ����
    T mode() const;
    // ����Э����
    double covariance(const BCarray<T> &vec, bool isSample = false) const;
    // �������ϵ��
    double correlation(const BCarray<T> &vec) const;
    // ��ӡͳ����Ϣ
    void info() const;

    /*һϵ������*/
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

    /*һЩ��ѧ����*/
    // ����������Զ��庯��
    template <typename U>
    BCarray<U> apply(function<U(const T &)>);
    // ����
    BCarray<T> sort(bool ascending = true);
    // BCarray<size_t> argsort(ascending = true);
    void sort_(bool ascending = true);
    // �ڻ�
    T dot(const BCarray<T> &vec);
    // ת�á�һ�㶼������������ʾ��.T()�ͻ���������
    BCarray<T> t() const;

    // ���ֱ�׼��
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

    // ���ʵ����Ͷ�����
    pair<BCarray<T>, BCarray<T>> split(vector<int> &group);
};

// ʵ��
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
    // ��ӡ����ͳ����Ϣ
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
    // �����������
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
// ʹ��ʵ��
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
