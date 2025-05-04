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

void GenePlot::plotKDE(
    const std::vector<double>& samples,
    double bandwidth,
    int grid_n,
    int width,
    int height 
) {
    // Initialize Python interpreter and matplotlib backend
    plt::detail::_interpreter::get();

    // Compute grid range
    double xmin = *std::min_element(samples.begin(), samples.end());
    double xmax = *std::max_element(samples.begin(), samples.end());
    double margin = 3 * bandwidth;
    xmin -= margin;
    xmax += margin;

    // Build grid
    std::vector<double> grid_x(grid_n);
    for (int i = 0; i < grid_n; ++i) {
        grid_x[i] = xmin + (xmax - xmin) * i / (grid_n - 1);
    }

    // Compute densities
    auto densities = StatTools::computeKDE(samples, grid_x, bandwidth);

    // Plot
    plt::figure_size(width, height);
    plt::plot(grid_x, densities);
    plt::xlabel("Value");
    plt::ylabel("Density");
    plt::title("Kernel Density Estimation (Gaussian h=" + std::to_string(bandwidth) + ")");
    plt::grid(true);
    plt::show();
}


void GenePlot::plotHistogramKDE(
    const std::vector<double>& samples,
    double bandwidth,
    int bins,
    int grid_n,
    int width,
    int height
) {
    // 1) 初始化解释器（只需调用一次，放到 main() 里也行）
    static auto& interp = plt::detail::_interpreter::get();

    // 2) 画归一化直方图：named_hist 会自动归一化（area=1）并带标签
    plt::figure_size(width, height);
    plt::hist(
        samples,         // 样本数据
        bins,            // 柱子数
        "grey",          // 颜色
        0.5              // 透明度
    );

    // 3) 计算样本范围 + 三倍带宽的扩展区间
    auto mm = std::minmax_element(samples.begin(), samples.end());
    double xmin = *mm.first, xmax = *mm.second;
    xmin -= 3 * bandwidth;
    xmax += 3 * bandwidth;

    // 4) 构建 grid_x
    std::vector<double> grid_x(grid_n);
    for (int i = 0; i < grid_n; ++i) {
        grid_x[i] = xmin + (xmax - xmin) * i / (grid_n - 1);
    }

    // 5) 计算 KDE 密度
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
    const std::vector<double>& y1, 
    const std::vector<double>& y2,
    int width,
    int height
) {
    if (y1.size() != y2.size()) {
        throw std::runtime_error("两个向量长度不一致！");
    }

    std::vector<double> x(y1.size());
    for (size_t i = 0; i < x.size(); ++i)
        x[i] = i;  // 生成 x 坐标

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
    const std::vector<double>& y1,
    const std::vector<double>& y2,
    int width,
    int height
) {
    if (y1.size() != y2.size()) {
        throw std::runtime_error("x 和 y 向量长度不一致！");
    }

    plt::figure_size(width, height);

    plt::scatter(y1, y2, 10.0);

    // 找到所有点中的 min 和 max，用于对角线
    double min_x, max_x, min_y, max_y;
    min_x = StatTools::min(y1);
    min_y = StatTools::min(y2);
    max_x = StatTools::max(y1);
    max_y = StatTools::max(y2);

    double diag_min = std::min(min_x, min_y);
    double diag_max = std::max(max_x, max_y);

    std::vector<double> diag_x = { diag_min, diag_max };
    std::vector<double> diag_y = { diag_min, diag_max };
    plt::plot(diag_x, diag_y, { {"label", "y = x"}, {"color", "red"}, {"linestyle", "--"} });

    // 装饰
    plt::xlabel("X");
    plt::ylabel("Y");
    plt::title("Scatter Plot with y = x");
    plt::legend();
    plt::grid(true);
    plt::show();
}

void GenePlot::plot_two_boxplot(
    const std::vector<double>& y1,
    const std::vector<double>& y2,
    int width,
    int height
) {
    plt::figure_size(width, height);

    // 将两个数据列打包成 vector<vector<double>>
    std::vector<std::vector<double>> all_data = { y1, y2 };

    // 画箱线图
    plt::boxplot(all_data);

    // 添加标签
    plt::xticks(std::vector<double>{1, 2}, std::vector<std::string>{"Group 1", "Group 2"});
    plt::title("Boxplot of Two Groups");
    plt::grid(true);
    plt::show();  // 或 plt::save("boxplot.png");
}

void GenePlot::plot_heatmap(
    const std::vector<std::vector<double>>& matrix,
    bool show_colorbar ,
    int width ,
    int height 
) {
    // 初始化 Python（只调用一次）
    static auto& interp = plt::detail::_interpreter::get();

    if (matrix.empty() || matrix[0].empty()) {
        throw std::invalid_argument("Input matrix is empty.");
    }

    int rows = matrix.size();
    int cols = matrix[0].size();

    // 检查所有行的长度一致
    for (const auto& row : matrix) {
        if (row.size() != cols) {
            throw std::invalid_argument("All rows in the matrix must have the same number of columns.");
        }
    }

    // 拉平为一维数组（按行优先）
    std::vector<float> flat_data;
    flat_data.reserve(rows * cols);
    for (const auto& row : matrix) {
        flat_data.insert(flat_data.end(), row.begin(), row.end());
    }

    // 画图：注意这里 colors=1 表示灰度图
    PyObject* im = nullptr;
    plt::imshow(flat_data.data(), rows, cols, 1, { {"cmap", "PiYG"} }, &im);

    // 可选加 colorbar
    if (show_colorbar && im != nullptr) {
        plt::colorbar(im);
    }

    plt::title("Heatmap");
    plt::show();
}
