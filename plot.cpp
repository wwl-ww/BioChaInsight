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
