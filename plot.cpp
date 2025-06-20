﻿#include "stdafx.h"
#include "plot.h"
#include "ST.h"
#include "FASTA.h"


#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include<Python.h>
//#include <numpy/arrayobject.h>

bool init_python(const string& python_home)
{
	string path = python_home;
	replace(path.begin(), path.end(), '\\', '/');

	wstring wpath(path.begin(), path.end());
	wchar_t* pyhome = _wcsdup(wpath.c_str());

	Py_SetPythonHome(pyhome);
	Py_Initialize();

	if (!Py_IsInitialized())
	{
		cerr << "[ERROR] Python 初始化失败\n";
		return 0;
	}

	string setup =
		"import sys\n"
		"sys.path.insert(0, '" +
		python_home + "/Lib')\n"
		"sys.path.insert(0, '" +
		python_home + "/Lib/site-packages')\n"
		"sys.path.insert(0, '" +
		python_home + "/DLLs')\n"
		"print('[PYTHON] sys.path:')\n"
		"import pprint; pprint.pprint(sys.path)\n"
		"import encodings";

	PyRun_SimpleString(setup.c_str());
	//import_array(); // 非常重要，初始化 PyArray_API 指针
	return 1;
}

void GenePlot::volcano(
	const vector<double>& log2fc,
	const vector<double>& pvals,
	const vector<string>& genes,
	double fc_thresh,
	double pval_thresh)
{
	// GenePlot::init_python();

	vector<double> x_up, y_up, x_down, y_down, x_ns, y_ns;

	for (size_t i = 0; i < log2fc.size(); ++i)
	{
		double fc = log2fc[i];
		double p = pvals[i];
		double neglogp = -log10(p + 1e-10);

		if (p < pval_thresh)
		{
			if (fc > fc_thresh)
			{
				x_up.push_back(fc);
				y_up.push_back(neglogp);
			}
			else if (fc < -fc_thresh)
			{
				x_down.push_back(fc);
				y_down.push_back(neglogp);
			}
			else
			{
				x_ns.push_back(fc);
				y_ns.push_back(neglogp);
			}
		}
		else
		{
			x_ns.push_back(fc), y_ns.push_back(neglogp);
		}
	}

	// 画图
	plt::scatter(x_ns, y_ns, 10.0, { {"color", "grey"}, {"label", "NS"} });
	plt::scatter(x_up, y_up, 10.0, { {"color", "red"}, {"label", "Up"} });
	plt::scatter(x_down, y_down, 10.0, { {"color", "blue"}, {"label", "Down"} });

	// 参考线
	double y = -log10(pval_thresh);
	vector<double> x = { 0.0, 1.0 };
	vector<double> yline = { y, y };
	plt::plot(x, yline, { {"linestyle", "--"} });
	plt::axvline(fc_thresh, 0, 1, { {"linestyle", "--"} });
	plt::axvline(-fc_thresh, 0, 1, { {"linestyle", "--"} });

	plt::xlabel("log2 Fold Change");
	plt::ylabel("-log10(p-value)");
	plt::title("Volcano Plot");
	plt::legend();
	plt::show();
}

void GenePlot::plotPCAWithKMeans(
	const vector<vector<double>>& data,
	int k,
	int pca_components)
{
	vector<vector<double>> pca_result = StatTools::performPCA(data, pca_components);
	vector<int> labels = StatTools::kMeans(pca_result, k);

	vector<vector<double>> x_vals(k), y_vals(k);

	for (size_t i = 0; i < pca_result.size(); ++i)
	{
		int cluster = labels[i];
		x_vals[cluster].push_back(pca_result[i][0]); // x
		y_vals[cluster].push_back(pca_result[i][1]); // y
	}

	vector<string> colors = { "red", "blue", "green", "purple", "orange", "pink", "cyan", "yellow" };

	for (int i = 0; i < k; ++i)
	{
		string color = colors[i % colors.size()]; // 循环使用
		string label = "Cluster " + to_string(i + 1);
		plt::scatter(x_vals[i], y_vals[i], 10.0, { {"color", color}, {"label", label} });
	}

	plt::xlabel("Principal Component 1");
	plt::ylabel("Principal Component 2");
	plt::title("PCA with K-means Clustering");
	plt::legend();
	plt::show();
}

void GenePlot::plotClusterScatter(
	const vector<vector<double>>& data,
	const vector<int>& labels)
{
	size_t n = data.size();
	if (labels.size() != n)
		throw invalid_argument("data.size() != labels.size()");

	// 确保每个点是二维
	for (size_t i = 0; i < n; ++i) {
		if (data[i].size() != 2)
			throw invalid_argument("each data[i] must have exactly 2 elements");
	}

	vector<int> uniq = labels;
	sort(uniq.begin(), uniq.end());
	uniq.erase(unique(uniq.begin(), uniq.end()), uniq.end());

	map<int, vector<double>> xs_map, ys_map;
	for (size_t i = 0; i < n; ++i) {
		xs_map[labels[i]].push_back(data[i][0]);
		ys_map[labels[i]].push_back(data[i][1]);
	}

	static const vector<string> colors = {
		"red","blue","green","purple","orange",
		"pink","cyan","yellow","black","brown"
	};

	for (size_t idx = 0; idx < uniq.size(); ++idx) {
		int lbl = uniq[idx];
		auto& xs = xs_map[lbl];
		auto& ys = ys_map[lbl];
		string c = colors[idx % colors.size()];
		plt::scatter(xs, ys, 20.0, {
			{"color", c},
			{"label", to_string(lbl)}
			});
	}

	plt::xlabel("X");
	plt::ylabel("Y");
	plt::title("Clustered Scatter Plot");
	plt::legend();
	plt::show();
}

void GenePlot::plotKDE(
	const vector<double>& samples,
	double bandwidth,
	int grid_n,
	int width,
	int height
) {
	plt::detail::_interpreter::get();

	double xmin = *min_element(samples.begin(), samples.end());
	double xmax = *max_element(samples.begin(), samples.end());
	double margin = 3 * bandwidth;
	xmin -= margin;
	xmax += margin;

	vector<double> grid_x(grid_n);
	for (int i = 0; i < grid_n; ++i) {
		grid_x[i] = xmin + (xmax - xmin) * i / (grid_n - 1);
	}

	auto densities = StatTools::computeKDE(samples, grid_x, bandwidth);

	// Plot
	plt::figure_size(width, height);
	plt::plot(grid_x, densities);
	plt::xlabel("Value");
	plt::ylabel("Density");
	plt::title("Kernel Density Estimation (Gaussian h=" + to_string(bandwidth) + ")");
	plt::grid(true);
	plt::show();
}


void GenePlot::plotHistogramKDE(
	const vector<double>& samples,
	double bandwidth,
	int bins,
	int grid_n,
	int width,
	int height
) {
	static auto& interp = plt::detail::_interpreter::get();

	plt::figure_size(width, height);
	plt::hist(
		samples,         // 样本数据
		bins,            // 柱子数
		"grey",          // 颜色
		0.5              // 透明度
	);

	auto mm = minmax_element(samples.begin(), samples.end());
	double xmin = *mm.first, xmax = *mm.second;
	xmin -= 3 * bandwidth;
	xmax += 3 * bandwidth;

	vector<double> grid_x(grid_n);
	for (int i = 0; i < grid_n; ++i) {
		grid_x[i] = xmin + (xmax - xmin) * i / (grid_n - 1);
	}

	auto densities = StatTools::computeKDE(samples, grid_x, bandwidth);

	plt::plot(grid_x, densities);
	plt::xlabel("Value");
	plt::ylabel("Density");
	plt::title("Histogram with KDE");
	plt::legend();
	plt::grid(true);
	plt::show();
}

void GenePlot::plot_two_lines(
	const vector<double>& y1,
	const vector<double>& y2,
	int width,
	int height
) {
	if (y1.size() != y2.size()) {
		throw runtime_error("两个向量长度不一致！");
	}

	vector<double> x(y1.size());
	for (size_t i = 0; i < x.size(); ++i)
		x[i] = i;

	plt::figure();
	plt::plot(x, y1, "r-");
	plt::plot(x, y2, "b-");

	plt::legend();
	plt::xlabel("Index");
	plt::ylabel("Value");
	plt::title("Two Line Plot");
	plt::grid(true);
	plt::show();  // 或 plt::save("output.png");
}

void GenePlot::plot_two_xy(
	const vector<double>& y1,
	const vector<double>& y2,
	int width,
	int height
) {
	if (y1.size() != y2.size()) {
		throw runtime_error("x 和 y 向量长度不一致！");
	}

	plt::figure_size(width, height);

	plt::scatter(y1, y2, 10.0);

	// 找到所有点中的 min 和 max，用于对角线
	double min_x, max_x, min_y, max_y;
	min_x = StatTools::min(y1);
	min_y = StatTools::min(y2);
	max_x = StatTools::max(y1);
	max_y = StatTools::max(y2);

	double diag_min = min(min_x, min_y);
	double diag_max = max(max_x, max_y);

	vector<double> diag_x = { diag_min, diag_max };
	vector<double> diag_y = { diag_min, diag_max };
	plt::plot(diag_x, diag_y, { {"label", "y = x"}, {"color", "red"}, {"linestyle", "--"} });

	plt::xlabel("X");
	plt::ylabel("Y");
	plt::title("Scatter Plot with y = x");
	plt::legend();
	plt::grid(true);
	plt::show();
}

void GenePlot::plot_two_boxplot(
	const vector<double>& y1,
	const vector<double>& y2,
	int width,
	int height
) {
	plt::figure_size(width, height);

	vector<vector<double>> all_data = { y1, y2 };

	plt::boxplot(all_data);

	plt::xticks(vector<double>{1, 2}, vector<string>{"Group 1", "Group 2"});
	plt::title("Boxplot of Two Groups");
	plt::grid(true);
	plt::show();  // 或 plt::save("boxplot.png");
}

void GenePlot::plot_heatmap(
	const std::vector<std::vector<double>>& matrix,
	bool show_colorbar,
	int width,
	int height
) {
	if (matrix.empty() || matrix[0].empty()) {
		throw std::invalid_argument("Input matrix is empty.");
	}
	int rows = static_cast<int>(matrix.size());
	int cols = static_cast<int>(matrix[0].size());

	// 检查每行长度是否一致
	for (const auto& row : matrix) {
		if (static_cast<int>(row.size()) != cols) {
			throw std::invalid_argument("All rows in the matrix must have the same number of columns.");
		}
	}

	// 1. 初始化 Python 解释器（如果还没初始化）
	if (!Py_IsInitialized()) {
		Py_Initialize();
	}

	// 2. 构造一个 Python 嵌套列表 pyMatrix，等下用 numpy.array(pyMatrix)
	PyObject* pyMatrix = PyList_New(rows);
	if (!pyMatrix) {
		PyErr_Print();
		throw std::runtime_error("Failed to create Python list for matrix.");
	}
	for (int i = 0; i < rows; ++i) {
		PyObject* pyRow = PyList_New(cols);
		if (!pyRow) {
			PyErr_Print();
			Py_DECREF(pyMatrix);
			throw std::runtime_error("Failed to create Python list for a row.");
		}
		for (int j = 0; j < cols; ++j) {
			PyObject* pyVal = PyFloat_FromDouble(matrix[i][j]);
			if (!pyVal) {
				PyErr_Print();
				Py_DECREF(pyRow);
				Py_DECREF(pyMatrix);
				throw std::runtime_error("Failed to convert matrix element to Python float.");
			}
			// PyList_SetItem 会把 pyVal 的引用计数收走
			PyList_SetItem(pyRow, j, pyVal);
		}
		PyList_SetItem(pyMatrix, i, pyRow);  // 收走 pyRow
	}

	// 3. 导入 numpy 和 matplotlib.pyplot
	PyObject* numpyMod = PyImport_ImportModule("numpy");
	if (!numpyMod) {
		PyErr_Print();
		Py_DECREF(pyMatrix);
		throw std::runtime_error("Failed to import numpy.");
	}
	PyObject* pltMod = PyImport_ImportModule("matplotlib.pyplot");
	if (!pltMod) {
		PyErr_Print();
		Py_DECREF(numpyMod);
		Py_DECREF(pyMatrix);
		throw std::runtime_error("Failed to import matplotlib.pyplot.");
	}

	// 4. 把 Python 嵌套列表转换成 numpy.ndarray：arr = numpy.array(pyMatrix)
	PyObject* arrayFunc = PyObject_GetAttrString(numpyMod, "array");
	if (!arrayFunc) {
		PyErr_Print();
		Py_DECREF(pltMod);
		Py_DECREF(numpyMod);
		Py_DECREF(pyMatrix);
		throw std::runtime_error("Failed to get numpy.array.");
	}
	PyObject* argsArray = PyTuple_Pack(1, pyMatrix);
	PyObject* npArray = PyObject_CallObject(arrayFunc, argsArray);
	Py_DECREF(argsArray);
	Py_DECREF(arrayFunc);
	Py_DECREF(pyMatrix);
	if (!npArray) {
		PyErr_Print();
		Py_DECREF(pltMod);
		Py_DECREF(numpyMod);
		throw std::runtime_error("Failed to call numpy.array.");
	}

	// 5. 设定图形大小：plt.figure(figsize=(width/100.0, height/100.0))
	PyObject* figureFunc = PyObject_GetAttrString(pltMod, "figure");
	if (!figureFunc) {
		PyErr_Print();
		Py_DECREF(npArray);
		Py_DECREF(pltMod);
		Py_DECREF(numpyMod);
		throw std::runtime_error("Failed to get matplotlib.pyplot.figure.");
	}
	double inchW = width / 100.0;
	double inchH = height / 100.0;
	PyObject* pyInchW = PyFloat_FromDouble(inchW);
	PyObject* pyInchH = PyFloat_FromDouble(inchH);
	PyObject* sizeTuple = PyTuple_Pack(2, pyInchW, pyInchH);
	Py_DECREF(pyInchW);
	Py_DECREF(pyInchH);
	// 调用 figure(figsize=(inchW, inchH))
	PyObject* kwFig = PyDict_New();
	PyDict_SetItemString(kwFig, "figsize", sizeTuple);
	Py_DECREF(sizeTuple);
	PyObject* figRes = PyObject_Call(figureFunc, PyTuple_New(0), kwFig);
	Py_DECREF(kwFig);
	Py_DECREF(figureFunc);
	if (!figRes) {
		PyErr_Print();
		Py_DECREF(npArray);
		Py_DECREF(pltMod);
		Py_DECREF(numpyMod);
		throw std::runtime_error("Failed to call plt.figure.");
	}
	Py_DECREF(figRes);

	// 6. 调用 im = plt.imshow(npArray, cmap='PiYG', aspect='auto')
	PyObject* imshowFunc = PyObject_GetAttrString(pltMod, "imshow");
	if (!imshowFunc) {
		PyErr_Print();
		Py_DECREF(npArray);
		Py_DECREF(pltMod);
		Py_DECREF(numpyMod);
		throw std::runtime_error("Failed to get matplotlib.pyplot.imshow.");
	}
	// 准备关键字参数：{'cmap':'PiYG', 'aspect':'auto'}
	PyObject* kwIm = PyDict_New();
	PyDict_SetItemString(kwIm, "cmap", PyUnicode_FromString("PiYG"));
	PyDict_SetItemString(kwIm, "aspect", PyUnicode_FromString("auto"));
	// 调用 imshow
	PyObject* argsIm = PyTuple_Pack(1, npArray);
	PyObject* imRes = PyObject_Call(imshowFunc, argsIm, kwIm);
	Py_DECREF(argsIm);
	Py_DECREF(kwIm);
	Py_DECREF(imshowFunc);
	Py_DECREF(npArray);
	if (!imRes) {
		PyErr_Print();
		Py_DECREF(pltMod);
		Py_DECREF(numpyMod);
		throw std::runtime_error("plt.imshow call failed.");
	}

	// 7. 如果需要 colorbar，则 plt.colorbar(imRes)
	if (show_colorbar) {
		PyObject* colorbarFunc = PyObject_GetAttrString(pltMod, "colorbar");
		if (!colorbarFunc) {
			PyErr_Print();
			Py_DECREF(imRes);
			Py_DECREF(pltMod);
			Py_DECREF(numpyMod);
			throw std::runtime_error("Failed to get matplotlib.pyplot.colorbar.");
		}
		PyObject* argsCb = PyTuple_Pack(1, imRes);
		PyObject* cbRes = PyObject_CallObject(colorbarFunc, argsCb);
		Py_DECREF(argsCb);
		Py_DECREF(colorbarFunc);
		if (!cbRes) {
			PyErr_Print();
			Py_DECREF(imRes);
			Py_DECREF(pltMod);
			Py_DECREF(numpyMod);
			throw std::runtime_error("plt.colorbar call failed.");
		}
		Py_DECREF(cbRes);
	}
	Py_DECREF(imRes);

	// 8. plt.title("Heatmap")
	PyObject* titleFunc = PyObject_GetAttrString(pltMod, "title");
	if (!titleFunc) {
		PyErr_Print();
		Py_DECREF(pltMod);
		Py_DECREF(numpyMod);
		throw std::runtime_error("Failed to get matplotlib.pyplot.title.");
	}
	PyObject* titleArg = PyUnicode_FromString("Heatmap");
	PyObject* argsTitle = PyTuple_Pack(1, titleArg);
	PyObject* titleRes = PyObject_CallObject(titleFunc, argsTitle);
	Py_DECREF(argsTitle);
	Py_DECREF(titleArg);
	Py_DECREF(titleFunc);
	if (!titleRes) {
		PyErr_Print();
		Py_DECREF(pltMod);
		Py_DECREF(numpyMod);
		throw std::runtime_error("plt.title call failed.");
	}
	Py_DECREF(titleRes);

	// 9. plt.show()
	PyObject* showFunc = PyObject_GetAttrString(pltMod, "show");
	if (!showFunc) {
		PyErr_Print();
		Py_DECREF(pltMod);
		Py_DECREF(numpyMod);
		throw std::runtime_error("Failed to get matplotlib.pyplot.show.");
	}
	PyObject* showRes = PyObject_CallObject(showFunc, nullptr);
	Py_DECREF(showFunc);
	if (!showRes) {
		PyErr_Print();
		Py_DECREF(pltMod);
		Py_DECREF(numpyMod);
		throw std::runtime_error("plt.show call failed.");
	}
	Py_DECREF(showRes);

	// 10. 清理
	Py_DECREF(pltMod);
	Py_DECREF(numpyMod);
}

void GenePlot::showTwoBaseCompositionPieDialog(const Sequence& seq1,
	const Sequence& seq2,
	QWidget* parent)
{
	QFont jbMono("JetBrains Mono", 10);
	static const QVector<QColor> pastel = {
		QColor(0xFF, 0xB3, 0xBA),  // 粉
		QColor(0xBA, 0xD2, 0xFF),  // 蓝
		QColor(0xBA, 0xFF, 0xD2),  // 绿
		QColor(0xFF, 0xE0, 0xBA)   // 橙
	};

	auto makeChartView = [&](const Sequence& seq) -> QChartView* {
		auto freq = seq.calculateBaseFrequency();
		int len = seq.getLength();
		const QString bases = "ACGT";

		QPieSeries* series = new QPieSeries();

		QVector<int> counts(4);
		QVector<double> pcts(4);

		for (int i = 0; i < 4; ++i) {
			char base = bases[i].toLatin1();
			int count = static_cast<int>(freq.at(base) * len + 0.5);
			double pct = count * 100.0 / len;
			counts[i] = count;
			pcts[i] = pct;
			series->append(QString("%1: %2%").arg(bases[i]).arg(QString::number(pct, 'f', 1)), count);
		}

		const auto slices = series->slices();
		for (int i = 0; i < slices.size(); ++i) {
			slices[i]->setLabel(QString("%1: %2%")
				.arg(bases[i])
				.arg(QString::number(pcts[i], 'f', 1)));
			slices[i]->setBrush(pastel[i]);
			slices[i]->setLabelFont(jbMono);
			slices[i]->setLabelVisible(true);
		}

		// 图表
		QChart* chart = new QChart();
		chart->addSeries(series);
		chart->setTitle(QString("Length=%1    MW=%2").arg(len).arg(seq.calculateMW()));
		chart->setTitleFont(jbMono);

		// 图例
		QLegend* legend = chart->legend();
		legend->setFont(jbMono);
		legend->setAlignment(Qt::AlignRight);
		legend->setMaximumWidth(200);

		const auto markers = legend->markers(series);
		for (int i = 0; i < markers.size(); ++i) {
			markers[i]->setLabel(QString("%1: %2")
				.arg(bases[i])
				.arg(counts[i]));
		}

		QChartView* view = new QChartView(chart);
		view->setRenderHint(QPainter::Antialiasing);
		view->setMinimumSize(400, 400);
		return view;
		};

	// 弹出对话框
	QDialog dlg(parent);
	dlg.setWindowTitle("Base Composition Comparison");
	dlg.resize(1500, 450);

	QHBoxLayout* layout = new QHBoxLayout(&dlg);
	layout->setContentsMargins(5, 5, 5, 5);
	layout->addWidget(makeChartView(seq1));
	layout->addWidget(makeChartView(seq2));

	dlg.exec();
}