# pragma once

#ifndef FASTA_H
#define FASTA_H

#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>
using namespace std;

class Sequence
{
private:
    string sequence;
    string header;

public:
    // 构造函数
    Sequence();
    Sequence(const string& sequence, const string& header = "");
    Sequence(const Sequence& seq);

    // 读取的函数
    string getSequence() const;
    string getHeader() const;
    int getLength() const;

    // 计算GC含量
    int getGCSum() const;
    double getGCContent() const;

    // 分子量
    double calculateMW() const;

    // 基序频率
    unordered_map<char, double> calculateBaseFrequency() const;

    // 一些常见功能的运算符重载
    char operator[](int index) const;                             // 下标运算符重载
    char& operator[](int index);                                  // 下标运算符重载
    friend ostream& operator<<(ostream& os, const Sequence& seq); // 输出
    Sequence& operator+=(const Sequence& seq);                    // 连接
    Sequence& operator+=(const string& seq);                      // 连接
    Sequence operator!() const;                                   // 互补
    operator string() const;                                      // 转换为字符串

    // 输出信息
    void printInfo() const;
};

class FASTAReader
{
private:
    string filename;
    vector<Sequence> seqs;
    void _parse(const string& filename);

public:
    FASTAReader(const string& filename);

    vector<Sequence> getSeqs() const;
    vector<Sequence> operator()() const;
    Sequence& operator[](int index);
    const Sequence& operator[](int index) const;

    void printInfo() const;
};

#endif // FASTA_H