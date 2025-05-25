#pragma once

#include "stdafx.h"
#ifndef BCMATRIX_H
#define BCMATRIX_H

#include <iostream>
using namespace std;
#include <vector>
#include "BCarray.h"
#include <fstream>
#include <sstream>

class BCmatrix
{
private:
	size_t row;
	size_t column;
	vector<string> row_lst;
	vector<string> column_lst;
	vector<BCarray<double>> value;
	// 用来划分实验组和对照组
	vector<int> group;

	// 检查行数/列数是否相等
	void _checkRowEqual(int size) const;
	void _checkColumnEqual(int size) const;

	// 检查行/列索引是否越界
	void _checkRowRange(int index) const;
	void _checkColumnRange(int index) const;

public:
	BCmatrix();
	BCmatrix(size_t row, size_t column);

	// 得到value和group
	vector<BCarray<double>> getValue() const;
	vector<vector<double>> getPureValue() const;
	vector<int> getGroup() const;

	// 获取行数和列数
	int getRowCount() const;
	int getColumnCount() const;
	pair<int, int> getShape() const;

	// 获取行名和列名
	vector<string> getRowName() const;
	vector<string> getColumnName() const;

	// 获取行和列
	BCarray<double> getRow(size_t row) const;
	BCmatrix sliceRows(size_t start_index, size_t end_index) const; // 用来取特定的行数
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

	// 用于清空和加载新的数据
	void clear();
	void load_data(const string& filename);
	void load_group(const string& filename);
	void set_group(const vector<int> gp);
	// 导出数据
	void to_csv(const string& filename) const;
	vector<vector<double>> values() const;

	// 数据归一化
	void normalize(const string& method = "zscore", const string& axis = "column");

	// 进行t-test
	BCmatrix t_test();
	BCmatrix deseq2();

	// 降维
	BCmatrix performPCA(int num_components = 2) const;
	BCmatrix performLLE(int n_neighbors, int dim, double reg = 1e-3) const;
	BCmatrix performTSNE(int dim = 2, double perplexity = 30.0, int max_iter = 1000, double lr = 200.0) const;

	// 描述
	BCmatrix describe(const string& axis = "column") const;

};

#endif
