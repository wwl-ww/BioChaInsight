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
        cerr << "[ERROR] Python ��ʼ��ʧ��\n";
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
    import_array(); // �ǳ���Ҫ����ʼ�� PyArray_API ָ��
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

    // ��ͼ
    plt::scatter(x_ns, y_ns, 10.0, {{"color", "grey"}, {"label", "NS"}});
    plt::scatter(x_up, y_up, 10.0, {{"color", "red"}, {"label", "Up"}});
    plt::scatter(x_down, y_down, 10.0, {{"color", "blue"}, {"label", "Down"}});

    // �ο���
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
    // 1. ִ�� PCA �����ݽ�ά��ָ��ά��
    std::vector<std::vector<double>> pca_result = StatTools::performPCA(data, pca_components);

    // 2. ʹ�� K-means �����㷨�����ݽ��о���
    std::vector<int> labels = StatTools::kMeans(pca_result, k);

    // 3. ��̬Ϊÿ���ط��䲻ͬ����ɫ
    std::vector<std::vector<double>> x_vals(k), y_vals(k); // �洢ÿ���ص�x��y����

    // ����ά������ݵ㰴�ر�ǩ����
    for (size_t i = 0; i < pca_result.size(); ++i)
    {
        int cluster = labels[i];
        x_vals[cluster].push_back(pca_result[i][0]); // ��һ���ɷ֣�x���꣩
        y_vals[cluster].push_back(pca_result[i][1]); // �ڶ����ɷ֣�y���꣩
    }

    // 4. ��ͼ��Ϊÿ����ʹ�ò�ͬ����ɫ
    std::vector<std::string> colors = {"red", "blue", "green", "purple", "orange", "pink", "cyan", "yellow"};

    for (int i = 0; i < k; ++i)
    {
        std::string color = colors[i % colors.size()]; // ��ɫѭ��ʹ��
        std::string label = "Cluster " + std::to_string(i + 1);
        plt::scatter(x_vals[i], y_vals[i], 10.0, {{"color", color}, {"label", label}});
    }

    // 5. ���òο��ߺ�ͼ�α�ǩ
    plt::xlabel("Principal Component 1");
    plt::ylabel("Principal Component 2");
    plt::title("PCA with K-means Clustering");
    plt::legend(); // ��ʾͼ��
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
    // 1) ��ʼ����������ֻ�����һ�Σ��ŵ� main() ��Ҳ�У�
    static auto& interp = plt::detail::_interpreter::get();

    // 2) ����һ��ֱ��ͼ��named_hist ���Զ���һ����area=1��������ǩ
    plt::figure_size(width, height);
    plt::hist(
        samples,         // ��������
        bins,            // ������
        "grey",          // ��ɫ
        0.5              // ͸����
    );

    // 3) ����������Χ + �����������չ����
    auto mm = std::minmax_element(samples.begin(), samples.end());
    double xmin = *mm.first, xmax = *mm.second;
    xmin -= 3 * bandwidth;
    xmax += 3 * bandwidth;

    // 4) ���� grid_x
    std::vector<double> grid_x(grid_n);
    for (int i = 0; i < grid_n; ++i) {
        grid_x[i] = xmin + (xmax - xmin) * i / (grid_n - 1);
    }

    // 5) ���� KDE �ܶ�
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
        throw std::runtime_error("�����������Ȳ�һ�£�");
    }

    std::vector<double> x(y1.size());
    for (size_t i = 0; i < x.size(); ++i)
        x[i] = i;  // ���� x ����

    plt::figure();
    plt::plot(x, y1, "r-");
    plt::plot(x, y2, "b-");

    plt::legend();
    plt::xlabel("Index");
    plt::ylabel("Value");
    plt::title("Two Line Plot");
    plt::grid(true);
    plt::show();  // �� plt::save("output.png");
}

void GenePlot::plot_two_xy(
    const std::vector<double>& y1,
    const std::vector<double>& y2,
    int width,
    int height
) {
    if (y1.size() != y2.size()) {
        throw std::runtime_error("x �� y �������Ȳ�һ�£�");
    }

    plt::figure_size(width, height);

    plt::scatter(y1, y2, 10.0);

    // �ҵ����е��е� min �� max�����ڶԽ���
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

    // װ��
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

    // �����������д���� vector<vector<double>>
    std::vector<std::vector<double>> all_data = { y1, y2 };

    // ������ͼ
    plt::boxplot(all_data);

    // ��ӱ�ǩ
    plt::xticks(std::vector<double>{1, 2}, std::vector<std::string>{"Group 1", "Group 2"});
    plt::title("Boxplot of Two Groups");
    plt::grid(true);
    plt::show();  // �� plt::save("boxplot.png");
}

void GenePlot::plot_heatmap(
    const std::vector<std::vector<double>>& matrix,
    bool show_colorbar ,
    int width ,
    int height 
) {
    // ��ʼ�� Python��ֻ����һ�Σ�
    static auto& interp = plt::detail::_interpreter::get();

    if (matrix.empty() || matrix[0].empty()) {
        throw std::invalid_argument("Input matrix is empty.");
    }

    int rows = matrix.size();
    int cols = matrix[0].size();

    // ��������еĳ���һ��
    for (const auto& row : matrix) {
        if (row.size() != cols) {
            throw std::invalid_argument("All rows in the matrix must have the same number of columns.");
        }
    }

    // ��ƽΪһά���飨�������ȣ�
    std::vector<float> flat_data;
    flat_data.reserve(rows * cols);
    for (const auto& row : matrix) {
        flat_data.insert(flat_data.end(), row.begin(), row.end());
    }

    // ��ͼ��ע������ colors=1 ��ʾ�Ҷ�ͼ
    PyObject* im = nullptr;
    plt::imshow(flat_data.data(), rows, cols, 1, { {"cmap", "PiYG"} }, &im);

    // ��ѡ�� colorbar
    if (show_colorbar && im != nullptr) {
        plt::colorbar(im);
    }

    plt::title("Heatmap");
    plt::show();
}
