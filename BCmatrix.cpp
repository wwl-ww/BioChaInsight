#include "stdafx.h"
#include "BCmatrix.h"
#include "ST.h"

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

BCmatrix::BCmatrix() {}

BCmatrix::BCmatrix(size_t row, size_t column) : row(row), column(column), value(row, BCarray<double>(column, 0)) {}

vector<BCarray<double>> BCmatrix::getValue() const
{
	return value;
}

vector<vector<double>> BCmatrix::getPureValue() const
{
	vector<vector<double>> res(row, vector<double>(column));
	for (size_t i = 0; i < row; i++)
	{
		for (size_t j = 0; j < column; j++)
		{
			res[i][j] = value[i][j];
		}
	}
	return res;
}

vector<int> BCmatrix::getGroup() const
{
	return group;
}

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

vector<string> BCmatrix::getRowName() const
{
	return row_lst;
}

vector<string> BCmatrix::getColumnName() const
{
	return column_lst;
}

BCarray<double> BCmatrix::getRow(size_t row) const
{
	_checkRowRange(row);
	return value[row];
}

BCmatrix BCmatrix::sliceRows(size_t start_index, size_t end_index) const {
	if (start_index > end_index)
		throw invalid_argument("sliceRows: start_index 必须 <= end_index");
	if (end_index >= row)
		throw out_of_range("sliceRows: end_index 越界");

	size_t new_row_count = end_index - start_index + 1;
	BCmatrix result(new_row_count, column);

	result.column_lst = this->column_lst;

	result.row_lst.clear();
	for (size_t i = start_index; i <= end_index; i++) {
		result.row_lst.push_back(this->row_lst[i]);
	}

	result.group.clear();
	if (!this->group.empty())
		result.set_group(this->getGroup());

	result.value.clear();
	result.value.reserve(new_row_count);
	for (size_t i = start_index; i <= end_index; i++)
		result.value.push_back(this->value[i]);

	return result;
}

BCarray<double> BCmatrix::getColumn(size_t column) const
{
	_checkColumnRange(column);
	BCarray<double> col(row);
	for (size_t i = 0; i < row; i++)
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
	for (size_t i = 0; i < row; i++)
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
	for (size_t i = 0; i < column; i++)
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
	for (size_t i = 0; i < row; i++)
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
	for (size_t i = 0; i < row; i++)
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
	for (size_t i = 0; i < row; i++)
		for (size_t j = 0; j < column; j++)
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
	for (size_t i = 0; i < row; i++)
		for (size_t j = 0; j < column; j++)
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
		for (size_t i = 0; i < row; i++)
			for (size_t j = 0; j < column; j++)
				result(i, j) += vec[j];
		return result;
	}
	else if (!vec.isRowVector() && vec.size() == row)
	{
		BCmatrix result(*this);
		for (size_t i = 0; i < row; i++)
			for (size_t j = 0; j < column; j++)
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
		for (size_t i = 0; i < row; i++)
			for (size_t j = 0; j < column; j++)
				result(i, j) *= vec[j];
		return result;
	}
	else if (!vec.isRowVector() && vec.size() == row)
	{
		BCmatrix result(*this);
		for (size_t i = 0; i < row; i++)
			for (size_t j = 0; j < column; j++)
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
		for (size_t i = 0; i < row; i++)
			for (size_t j = 0; j < column; j++)
				result(i, j) /= vec[j];
		return result;
	}
	else if (!vec.isRowVector() && vec.size() == row)
	{
		BCmatrix result(*this);
		for (size_t i = 0; i < row; i++)
			for (size_t j = 0; j < column; j++)
				result(i, j) /= vec[i];
		return result;
	}
	else
	{
		throw invalid_argument("Vector size does not match matrix dimensions.");
	}
}

void BCmatrix::clear()
{
	value.clear();
	row_lst.clear();
	column_lst.clear();
	row = 0;
	column = 0;
}

void BCmatrix::load_data(const string& filename)
{
	ifstream file(filename);
	if (!file.is_open())
	{
		throw runtime_error("Failed to open file.");
	}

	this->clear();
	string line;

	// 读取表头
	if (getline(file, line))
	{
		stringstream ss(line);
		string token;
		getline(ss, token, ','); // 跳过 "Gene ID"
		while (getline(ss, token, ','))
		{
			column_lst.push_back(token);
		}
		column = column_lst.size();
	}

	// 读取每一行基因数据
	while (getline(file, line))
	{
		stringstream ss(line);
		string geneID;
		getline(ss, geneID, ',');
		row_lst.push_back(geneID);

		BCarray<double> rowValues(column);
		string token;
		size_t colIdx = 0;
		while (getline(ss, token, ','))
		{
			rowValues[colIdx++] = stod(token);
		}
		this->value.push_back(rowValues);
	}

	row = row_lst.size();
	file.close();
}

void BCmatrix::load_group(const string& filename)
{
	ifstream file(filename);
	if (!file.is_open())
	{
		throw runtime_error("Failed to open file.");
	}

	this->group.clear();
	string line;

	getline(file, line); // 跳过表头id
	while (getline(file, line))
	{
		group.push_back(stoi(line));
	}

	// 检查group数是否与列数一致
	if (group.size() != column)
	{
		cout << "group size:  " << group.size() << endl;
		cout << "column size: " << column << endl;
		throw runtime_error("group size does not match column size.\n group size:  " + to_string(group.size()) + "\ncolumn size: " + to_string(column));
	}
	file.close();
}

void BCmatrix::set_group(const vector<int> gp) {
	this->group = gp;
}

void BCmatrix::to_csv(const string& filename) const
{
	ofstream out(filename);
	if (!out.is_open())
		throw runtime_error("Unable to open file for writing: " + filename);

	out << "Gene";
	for (const auto& sample_name : column_lst)
		out << "," << sample_name;
	out << "\n";

	for (size_t i = 0; i < row; i++)
	{
		out << row_lst[i];
		for (size_t j = 0; j < column; j++)
			out << "," << setprecision(6) << value[i][j];
		out << "\n";
	}

	out.close();
}

void BCmatrix::normalize(const string& method, const string& axis)
{
	if (axis == "column")
	{
		for (size_t j = 0; j < column; j++)
		{
			BCarray<double> col = this->getColumn(j);
			BCarray<double> normCol = col.normalize(method);
			for (size_t i = 0; i < row; i++)
			{
				value[i][j] = normCol[i];
			}
		}
	}
	else if (axis == "row")
	{
		for (size_t i = 0; i < row; i++)
		{
			BCarray<double> rowVec = this->getRow(i);
			BCarray<double> normRow = rowVec.normalize(method);
			for (size_t j = 0; j < column; j++)
			{
				value[i][j] = normRow[j];
			}
		}
	}
	else if (axis == "all")
	{
		// 把所有元素按行优先"拍瘪"到一个 BCarray
		BCarray<double> flat;
		flat.reserve(row * column);
		for (size_t i = 0; i < row; i++)
			for (size_t j = 0; j < column; j++)
				flat.push_back(value[i][j]);

		// 整体归一化
		BCarray<double> normFlat = flat.normalize(method);

		size_t idx = 0;
		for (size_t i = 0; i < row; i++)
			for (size_t j = 0; j < column; j++)
				value[i][j] = normFlat[idx++];
	}
	else
	{
		throw invalid_argument("normalize(): 'axis' 参数必须为 'row', 'column' 或 'all'");
	}
}

BCmatrix BCmatrix::t_test()
{
	BCmatrix result;
	result.column = 3;
	result.column_lst = { "log2_fc", "t", "p_value" };
	result.group = group;

	// 分实验组和对照组，进行t检验
	for (size_t i = 0; i < row; i++)
	{
		BCarray<double> rowData = this->getRow(i);
		pair<BCarray<double>, BCarray<double>> splitData = rowData.split(group);
		StatTools::t_testResult t_testResult = StatTools::t_test(splitData.first, splitData.second);

		vector<double> res = { t_testResult.log2_fc, t_testResult.t, t_testResult.p_value };
		BCarray<double> rowResult(res);
		result.addRow(rowResult, row_lst[i]);
	}

	// FDR调整
	BCarray<double> fdr_p_values = StatTools::adjust_fdr(result.getColumn(2));
	result.addColumn(fdr_p_values, "fdr_p_value");

	return result;
}

BCmatrix BCmatrix::deseq2()
{
	BCmatrix result;
	// 3 列：log2FC, pvalue, padj
	result.column = 3;
	result.column_lst = { "log2FC", "pvalue", "padj" };
	result.group = this->group;

	// 将 BCmatrix 中的 counts 提取为 G*N 的 vector  
	size_t G = this->row;
	size_t N = this->column;
	vector<vector<double>> counts(G, vector<double>(N));
	for (size_t i = 0; i < G; i++) {
		BCarray<double> rowData = this->getRow(i);
		for (size_t j = 0; j < N; j++) {
			counts[i][j] = rowData[j];
		}
	}

	vector<StatTools::DESeq2Result> deRes = StatTools::performDESeq2(counts, this->group);

	// 将结果写入新的 BCmatrix
	for (size_t i = 0; i < G; i++) {
		vector<double> vals = {
			deRes[i].log2FC,
			deRes[i].pvalue,
			deRes[i].padj
		};
		BCarray<double> rowArr(vals);
		result.addRow(rowArr, this->row_lst[i]);
	}

	return result;
}

vector<vector<double>> BCmatrix::values() const
{
	vector<vector<double>> matrix(row);

	for (size_t i = 0; i < row; i++)
	{
		matrix[i].resize(column);
		for (size_t j = 0; j < column; j++) {
			matrix[i][j] = value[i][j];
		}
	}

	return matrix;
}

BCmatrix BCmatrix::performPCA(int num_components) const
{
	auto data = this->getPureValue();
	auto transformed = StatTools::performPCA(data, num_components);

	BCmatrix result(this->getRowCount(), static_cast<size_t>(num_components));

	result.row_lst = this->row_lst;
	result.column_lst.clear();
	for (int j = 0; j < num_components; j++) {
		result.column_lst.push_back("component_" + to_string(j + 1));
	}

	result.value.clear();
	result.value.reserve(transformed.size());
	for (size_t i = 0; i < transformed.size(); i++) {
		result.value.emplace_back(transformed[i], true);
	}
	return result;
}

BCmatrix BCmatrix::performLLE(int n_neighbors, int dim, double reg) const {
	auto data = this->getPureValue();
	auto transformed = StatTools::performLLE(data, n_neighbors, dim, reg);

	BCmatrix result(this->getRowCount(), static_cast<size_t>(dim));

	result.row_lst = this->row_lst;
	result.column_lst.clear();
	for (int j = 0; j < dim; j++) {
		result.column_lst.push_back("lle_" + to_string(j + 1));
	}

	result.value.clear();
	result.value.reserve(transformed.size());
	for (size_t i = 0; i < transformed.size(); i++) {
		result.value.emplace_back(transformed[i], true);
	}
	return result;
}

BCmatrix BCmatrix::performTSNE(int dim, double perplexity, int max_iter, double lr) const
{
	auto data = this->getPureValue();
	auto transformed = StatTools::performTSNE(data, dim, perplexity, max_iter, lr);

	BCmatrix result(this->getRowCount(), static_cast<size_t>(dim));

	result.row_lst = this->row_lst;
	result.column_lst.clear();
	for (int j = 0; j < dim; j++) {
		result.column_lst.push_back("tsne_" + to_string(j + 1));
	}

	result.value.clear();
	result.value.reserve(transformed.size());
	for (size_t i = 0; i < transformed.size(); i++) {
		result.value.emplace_back(transformed[i], true);
	}
	return result;
}

BCmatrix BCmatrix::describe(const string& axis) const
{
	static const vector<string> metrics = {
		"count", "mean", "std", "min", "25%", "50%", "75%", "max",
		"skewness", "kurtosis", "mode"
	};
	const size_t M = metrics.size();

	// 统计方向
	size_t N;
	bool byColumn;
	if (axis == "column") {
		N = this->column;
		byColumn = true;
	}
	else if (axis == "row") {
		N = this->row;
		byColumn = false;
	}
	else {
		throw invalid_argument("describe: axis 必须是 'row' 或 'column'");
	}

	BCmatrix result(M, N);
	result.row_lst = metrics;
	result.column_lst = byColumn ? this->column_lst : this->row_lst;

	for (size_t j = 0; j < N; j++) {
		// 取出这一列或这一行的数据
		BCarray<double> arr = byColumn ? this->getColumn(j) : this->getRow(j);
		vector<double> vec = static_cast<vector<double>>(arr);

		for (size_t i = 0; i < M; i++) {
			double v = 0.0;
			switch (i) {
			case 0:  // count
				v = static_cast<double>(vec.size());
				break;
			case 1:  // mean
				v = StatTools::mean(vec);
				break;
			case 2:  // std
				v = StatTools::sampleStd(vec);
				break;
			case 3:  // min
				v = StatTools::min(vec);
				break;
			case 4:  // 25%
				v = StatTools::percentile(vec, 0.25);
				break;
			case 5:  // 50%
				v = StatTools::percentile(vec, 0.50);
				break;
			case 6:  // 75%
				v = StatTools::percentile(vec, 0.75);
				break;
			case 7:  // max
				v = StatTools::max(vec);
				break;
			case 8:  // skewness
				v = StatTools::skewness(vec);
				break;
			case 9:  // kurtosis
				v = StatTools::kurtosis(vec);
				break;
			case 10: // mode
				v = StatTools::mode(vec);
				break;
			}
			result.value[i][j] = v;
		}
	}

	return result;
}


