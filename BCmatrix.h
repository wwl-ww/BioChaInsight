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

	// 检查行数/列数是否相等
    void _checkRowEqual(int size) const;
    void _checkColumnEqual(int size) const;

	// 检查行/列索引是否越界
    void _checkRowRange(int index) const;
    void _checkColumnRange(int index) const;

public:
    BCmatrix();
    BCmatrix(size_t row, size_t column);

	// 获取行数和列数
    int getRowCount() const;
    int getColumnCount() const;
    pair<int, int> getShape() const;

	// 获取行和列
    BCarray<double> getRow(size_t row) const;
    BCarray<double> getColumn(size_t column) const;

	// 根据行和列索引获取元素
    double& iloc(size_t row, size_t column);
    double iloc(size_t row, size_t column) const;

	// 根据行名和列名获取索引
    int findRow(string rowName) const;
    int findColumn(string columnName) const;

	// 根据行名和列名获取元素
    double& loc(string rowName, string columnName);
    double loc(string rowName, string columnName) const;

    // 重载()，根据行列索引/名 获得元素
    double& operator()(size_t row, size_t column);
    double operator()(size_t row, size_t column) const;
    double& operator()(string rowName, string columnName);
    double operator()(string rowName, string columnName) const;

	// 重载()，根据行列索引/名 获得行/列
	// mod = 'r' 表示获取行，mod = 'c' 表示获取列
    BCarray<double> operator()(size_t index, char mod = 'r') const;
    BCarray<double> operator()(string name, char mod = 'r') const;

	// 设置行名和列名
    void setRowName(size_t index, string name);
    void setColumnName(size_t index, string name);

	// 删除行和列
    void deleteRow(size_t index);
    void deleteColumn(size_t index);

	// 添加行和列
    void addRow(const BCarray<double>& newRow, string name = "");
    void addColumn(const BCarray<double>& newColumn, string name = "");

    // 实现和标量的四则运算
    BCmatrix operator+(const double& scalar);
    BCmatrix operator-(const double& scalar);
    BCmatrix operator*(const double& scalar);
    BCmatrix operator/(const double& scalar);

    // 实现和向量的四则运算（注意行向量和列向量）
    BCmatrix operator+(const BCarray<double>& vec);
    BCmatrix operator-(const BCarray<double>& vec);
    BCmatrix operator*(const BCarray<double>& vec);
    BCmatrix operator/(const BCarray<double>& vec);

    void clear();
    void read_csv(const string& filename);

    void normalize(const string& method="zscore", const string& axis="column");
};

# endif 
