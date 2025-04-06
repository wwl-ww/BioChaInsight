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
    // ���캯��
    Sequence();
    Sequence(const string& sequence, const string& header = "");
    Sequence(const Sequence& seq);

    // ��ȡ�ĺ���
    string getSequence() const;
    string getHeader() const;
    int getLength() const;

    // ����GC����
    int getGCSum() const;
    double getGCContent() const;

    // ������
    double calculateMW() const;

    // ����Ƶ��
    unordered_map<char, double> calculateBaseFrequency() const;

    // һЩ�������ܵ����������
    char operator[](int index) const;                             // �±����������
    char& operator[](int index);                                  // �±����������
    friend ostream& operator<<(ostream& os, const Sequence& seq); // ���
    Sequence& operator+=(const Sequence& seq);                    // ����
    Sequence& operator+=(const string& seq);                      // ����
    Sequence operator!() const;                                   // ����
    operator string() const;                                      // ת��Ϊ�ַ���

    // �����Ϣ
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