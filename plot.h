#pragma once
#ifndef PLOT_H
#define PLOT_H

#include "matplotlibcpp.h"
#include <cmath>
#include<vector>
#include <algorithm>
#include<string>
using namespace std;

namespace plt = matplotlibcpp;

bool init_python(const string& python_home = "D:/anaconda");

namespace GenePlot {

    /**
     * 画火山图（Volcano Plot）
     * @param log2fc        每个基因的 log2 Fold Change（对数表达差异倍数）
     * @param pvals         每个基因对应的 p 值
     * @param genes         所有基因的名字（可选，如果要标注特定基因）
     * @param fc_thresh     Fold Change 阈值（例如 1.0 表示 2 倍差异，默认是1.0）
     * @param pval_thresh   p 值阈值（例如 0.05 表示 5% 显著性水平，默认是0.05）
     */
    void volcano(
        const vector<double>& log2fc,
        const vector<double>& pvals,
        const vector<string>& genes = {},  // 这个之后再实现
        double fc_thresh = 1.0,
        double pval_thresh = 0.05
    );

    /*void heatmap(const vector<vector<double>>& matrix,
        const vector<string>& gene_labels,
        const vector<string>& sample_labels) {

        plt::figure();
        plt::imshow(matrix);
        plt::colorbar();
        plt::title("Gene Expression Heatmap");
    }*/

} // namespace GenePlot

#endif // 

