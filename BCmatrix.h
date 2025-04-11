#pragma once

# ifndef BCMATRIX_H
# define BCMATRIX_H

# include<iostream>
using namespace std;
# include<vector>
# include"BCarray.h"
# include <fstream>
# include <sstream>

class BCmatrix
{
private:
    size_t row;
    size_t column;
    vector<string> row_lst;
    vector<string> column_lst;
    vector<BCarray<double>> value;

	// �������/�����Ƿ����
    void _checkRowEqual(int size) const;
    void _checkColumnEqual(int size) const;

	// �����/�������Ƿ�Խ��
    void _checkRowRange(int index) const;
    void _checkColumnRange(int index) const;

public:
    BCmatrix();
    BCmatrix(size_t row, size_t column);

	// ��ȡ����������
    int getRowCount() const;
    int getColumnCount() const;
    pair<int, int> getShape() const;

	// ��ȡ�к���
    BCarray<double> getRow(size_t row) const;
    BCarray<double> getColumn(size_t column) const;

	// �����к���������ȡԪ��
    double& iloc(size_t row, size_t column);
    double iloc(size_t row, size_t column) const;

	// ����������������ȡ����
    int findRow(string rowName) const;
    int findColumn(string columnName) const;

	// ����������������ȡԪ��
    double& loc(string rowName, string columnName);
    double loc(string rowName, string columnName) const;

    // ����()��������������/�� ���Ԫ��
    double& operator()(size_t row, size_t column);
    double operator()(size_t row, size_t column) const;
    double& operator()(string rowName, string columnName);
    double operator()(string rowName, string columnName) const;

	// ����()��������������/�� �����/��
	// mod = 'r' ��ʾ��ȡ�У�mod = 'c' ��ʾ��ȡ��
    BCarray<double> operator()(size_t index, char mod = 'r') const;
    BCarray<double> operator()(string name, char mod = 'r') const;

	// ��������������
    void setRowName(size_t index, string name);
    void setColumnName(size_t index, string name);

	// ɾ���к���
    void deleteRow(size_t index);
    void deleteColumn(size_t index);

	// ����к���
    void addRow(const BCarray<double>& newRow, string name = "");
    void addColumn(const BCarray<double>& newColumn, string name = "");

    // ʵ�ֺͱ�������������
    BCmatrix operator+(const double& scalar);
    BCmatrix operator-(const double& scalar);
    BCmatrix operator*(const double& scalar);
    BCmatrix operator/(const double& scalar);

    // ʵ�ֺ��������������㣨ע������������������
    BCmatrix operator+(const BCarray<double>& vec);
    BCmatrix operator-(const BCarray<double>& vec);
    BCmatrix operator*(const BCarray<double>& vec);
    BCmatrix operator/(const BCarray<double>& vec);

    void clear();
    void read_csv(const string& filename);

    void normalize(const string& method="zscore", const string& axis="column");
};

# endif 
