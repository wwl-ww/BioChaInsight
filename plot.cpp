#include "plot.h"
#include "ST.h"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

bool init_python(const string &python_home)
{
    string path = python_home;
    replace(path.begin(), path.end(), '\\', '/');

    wstring wpath(path.begin(), path.end());
    wchar_t *pyhome = _wcsdup(wpath.c_str());

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
    import_array(); // 非常重要，初始化 PyArray_API 指针
    return 1;
}

void GenePlot::volcano(
    const vector<double> &log2fc,
    const vector<double> &pvals,
    const vector<string> &genes,
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
    plt::scatter(x_ns, y_ns, 10.0, {{"color", "grey"}, {"label", "NS"}});
    plt::scatter(x_up, y_up, 10.0, {{"color", "red"}, {"label", "Up"}});
    plt::scatter(x_down, y_down, 10.0, {{"color", "blue"}, {"label", "Down"}});

    // 参考线
    double y = -log10(pval_thresh);
    vector<double> x = {0.0, 1.0};
    vector<double> yline = {y, y};
    plt::plot(x, yline, {{"linestyle", "--"}});
    plt::axvline(fc_thresh, 0, 1, {{"linestyle", "--"}});
    plt::axvline(-fc_thresh, 0, 1, {{"linestyle", "--"}});

    plt::xlabel("log2 Fold Change");
    plt::ylabel("-log10(p-value)");
    plt::title("Volcano Plot");
    plt::legend();
}

void GenePlot::plotPCAWithKMeans(
    const std::vector<std::vector<double>> &data,
    int k,
    int pca_components)
{
    // 1. 执行 PCA 将数据降维到指定维度
    std::vector<std::vector<double>> pca_result = StatTools::performPCA(data, pca_components);

    // 2. 使用 K-means 聚类算法对数据进行聚类
    std::vector<int> labels = StatTools::kMeans(pca_result, k);

    // 3. 动态为每个簇分配不同的颜色
    std::vector<std::vector<double>> x_vals(k), y_vals(k); // 存储每个簇的x和y坐标

    // 将降维后的数据点按簇标签分配
    for (size_t i = 0; i < pca_result.size(); ++i)
    {
        int cluster = labels[i];
        x_vals[cluster].push_back(pca_result[i][0]); // 第一主成分（x坐标）
        y_vals[cluster].push_back(pca_result[i][1]); // 第二主成分（y坐标）
    }

    // 4. 绘图：为每个簇使用不同的颜色
    std::vector<std::string> colors = {"red", "blue", "green", "purple", "orange", "pink", "cyan", "yellow"};

    for (int i = 0; i < k; ++i)
    {
        std::string color = colors[i % colors.size()]; // 颜色循环使用
        std::string label = "Cluster " + std::to_string(i + 1);
        plt::scatter(x_vals[i], y_vals[i], 10.0, {{"color", color}, {"label", label}});
    }

    // 5. 设置参考线和图形标签
    plt::xlabel("Principal Component 1");
    plt::ylabel("Principal Component 2");
    plt::title("PCA with K-means Clustering");
    plt::legend(); // 显示图例
}
