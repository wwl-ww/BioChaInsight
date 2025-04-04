#include "BCmatrix.h"

void BCmatrix::_checkRowEqual(int size) const
{
    if (size != row)
    {
        throw invalid_argument("Row size does not match matrix dimensions.");
    }
}

void BCmatrix::_checkColumnEqual(int size) const
{
    if (size != column)
    {
        throw invalid_argument("Column size does not match matrix dimensions.");
    }
}

void BCmatrix::_checkRowRange(int index) const
{
    if (index < 0 || index >= row)
    {
        throw out_of_range("Row index out of range.");
    }
}

void BCmatrix::_checkColumnRange(int index) const
{
    if (index < 0 || index >= column)
    {
        throw out_of_range("Column index out of range.");
    }
}

BCmatrix::BCmatrix(){}

BCmatrix::BCmatrix(size_t row, size_t column) : row(row), column(column), value(row, BCarray<double>(column, 0)) {}

int BCmatrix::getRowCount() const 
{ 
    return row; 
}

int BCmatrix::getColumnCount() const 
{ 
    return column; 
}

pair<int, int> BCmatrix::getShape() const 
{ 
    return make_pair(row, column); 
}

BCarray<double> BCmatrix::getRow(size_t row) const
{
    _checkRowRange(row);
    return value[row];
}

BCarray<double> BCmatrix::getColumn(size_t column) const
{
    _checkColumnRange(column);
    BCarray<double> col(row);
    for (size_t i = 0; i < row; ++i)
    {
        col[i] = value[i][column];
    }
    return col;
}

double& BCmatrix::iloc(size_t row, size_t column)
{
    _checkRowRange(row);
    _checkColumnRange(column);
    return value[row][column];
}

double BCmatrix::iloc(size_t row, size_t column) const
{
    _checkRowRange(row);
    _checkColumnRange(column);
    return value[row][column];
}

int BCmatrix::findRow(string rowName) const
{
    for (size_t i = 0; i < row; ++i)
    {
        if (row_lst[i] == rowName)
        {
            return i;
        }
    }
    return -1;
}

int BCmatrix::findColumn(string columnName) const
{
    for (size_t i = 0; i < column; ++i)
    {
        if (column_lst[i] == columnName)
        {
            return i;
        }
    }
    return -1;
}

double& BCmatrix::loc(string rowName, string columnName)
{
    int rowIndex = findRow(rowName);
    int columnIndex = findColumn(columnName);
    if (rowIndex == -1 || columnIndex == -1)
    {
        throw out_of_range("Row or column name not found.");
    }
    return value[rowIndex][columnIndex];
}

double BCmatrix::loc(string rowName, string columnName) const
{
    int rowIndex = findRow(rowName);
    int columnIndex = findColumn(columnName);
    if (rowIndex == -1 || columnIndex == -1)
    {
        throw out_of_range("Row or column name not found.");
    }
    return value[rowIndex][columnIndex];
}

double& BCmatrix::operator()(size_t row, size_t column)
{
    return iloc(row, column);
}

double BCmatrix::operator()(size_t row, size_t column) const
{
    return iloc(row, column);
}

double& BCmatrix::operator()(string rowName, string columnName)
{
    return loc(rowName, columnName);
}

double BCmatrix::operator()(string rowName, string columnName) const
{
    return loc(rowName, columnName);
}

BCarray<double> BCmatrix::operator()(size_t index, char mod) const
{
    if (mod == 'r')
    {
        return getRow(index);
    }
    else if (mod == 'c')
    {
        return getColumn(index);
    }
    else
    {
        throw invalid_argument("Invalid mode. Use 'r' for row or 'c' for column.");
    }
}

BCarray<double> BCmatrix::operator()(string name, char mod) const
{
    if (mod == 'r')
    {
        int index = findRow(name);
        if (index == -1)
        {
            throw out_of_range("Row name not found.");
        }
        return getRow(index);
    }
    else if (mod == 'c')
    {
        int index = findColumn(name);
        if (index == -1)
        {
            throw out_of_range("Column name not found.");
        }
        return getColumn(index);
    }
    else
    {
        throw invalid_argument("Invalid mode. Use 'r' for row or 'c' for column.");
    }
}

void BCmatrix::setRowName(size_t index, string name)
{
    _checkRowRange(index);
    row_lst[index] = name;
}

void BCmatrix::setColumnName(size_t index, string name)
{
    _checkColumnRange(index);
    column_lst[index] = name;
}

void BCmatrix::deleteRow(size_t index)
{
    _checkRowRange(index);
    value.erase(value.begin() + index);
    row_lst.erase(row_lst.begin() + index);
    --row;
}

void BCmatrix::deleteColumn(size_t index)
{
    _checkColumnRange(index);
    for (size_t i = 0; i < row; ++i)
    {
        value[i].erase(value[i].begin() + index);
    }
    column_lst.erase(column_lst.begin() + index);
    --column;
}

void BCmatrix::addRow(const BCarray<double>& newRow, string name)
{
    _checkColumnEqual(newRow.size());
    value.push_back(newRow);
    row_lst.push_back(name);
    ++row;
}

void BCmatrix::addColumn(const BCarray<double>& newColumn, string name)
{
    _checkRowEqual(newColumn.size());
    for (size_t i = 0; i < row; ++i)
    {
        value[i].push_back(newColumn[i]);
    }
    column_lst.push_back(name);
    ++column;
}

// 实现和标量的四则运算
BCmatrix BCmatrix::operator+(const double& scalar)
{
    BCmatrix result(*this);
    for (size_t i = 0; i < row; ++i)
        for (size_t j = 0; j < column; ++j)
            result(i, j) += scalar;
    return result;
}

BCmatrix BCmatrix::operator-(const double& scalar)
{
    return operator+(-scalar);
}

BCmatrix BCmatrix::operator*(const double& scalar)
{
    BCmatrix result(*this);
    for (size_t i = 0; i < row; ++i)
        for (size_t j = 0; j < column; ++j)
            result(i, j) *= scalar;
    return result;
}

BCmatrix BCmatrix::operator/(const double& scalar)
{
    return operator*(1.0 / scalar);
}

// 实现和向量的四则运算（注意行向量和列向量）
BCmatrix BCmatrix::operator+(const BCarray<double>& vec)
{
    if (vec.isRowVector() && vec.size() == column)
    {
        BCmatrix result(*this);
        for (size_t i = 0; i < row; ++i)
            for (size_t j = 0; j < column; ++j)
                result(i, j) += vec[j];
        return result;
    }
    else if (!vec.isRowVector() && vec.size() == row)
    {
        BCmatrix result(*this);
        for (size_t i = 0; i < row; ++i)
            for (size_t j = 0; j < column; ++j)
                result(i, j) += vec[i];
        return result;
    }
    else
    {
        throw invalid_argument("Vector size does not match matrix dimensions.");
    }
}

BCmatrix BCmatrix::operator-(const BCarray<double>& vec)
{
    return operator+(vec * -1);
}

BCmatrix BCmatrix::operator*(const BCarray<double>& vec)
{
    if (vec.isRowVector() && vec.size() == column)
    {
        BCmatrix result(*this);
        for (size_t i = 0; i < row; ++i)
            for (size_t j = 0; j < column; ++j)
                result(i, j) *= vec[j];
        return result;
    }
    else if (!vec.isRowVector() && vec.size() == row)
    {
        BCmatrix result(*this);
        for (size_t i = 0; i < row; ++i)
            for (size_t j = 0; j < column; ++j)
                result(i, j) *= vec[i];
        return result;
    }
    else
    {
        throw invalid_argument("Vector size does not match matrix dimensions.");
    }
}

BCmatrix BCmatrix::operator/(const BCarray<double>& vec)
{
    if (vec.isRowVector() && vec.size() == column)
    {
        BCmatrix result(*this);
        for (size_t i = 0; i < row; ++i)
            for (size_t j = 0; j < column; ++j)
                result(i, j) /= vec[j];
        return result;
    }
    else if (!vec.isRowVector() && vec.size() == row)
    {
        BCmatrix result(*this);
        for (size_t i = 0; i < row; ++i)
            for (size_t j = 0; j < column; ++j)
                result(i, j) /= vec[i];
        return result;
    }
    else
    {
        throw invalid_argument("Vector size does not match matrix dimensions.");
    }
}
