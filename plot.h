#pragma once
#ifndef PLOT_H
#define PLOT_H

#include "ST.h"
#include "matplotlibcpp.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

namespace plt = matplotlibcpp;

bool init_python(const string& python_home = "D:/anaconda");

namespace GenePlot
{

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
        const vector<string>& genes = {}, // 这个之后再实现
        double fc_thresh = 1.0,
        double pval_thresh = 0.05);

    /*void heatmap(const vector<vector<double>>& matrix,
        const vector<string>& gene_labels,
        const vector<string>& sample_labels) {

        plt::figure();
        plt::imshow(matrix);
        plt::colorbar();
        plt::title("Gene Expression Heatmap");
    }*/

    /**
     * 对给定数据进行主成分分析（PCA），将数据降维到指定的维度，
     * 然后对降维后的数据应用 K-means 聚类，并绘制聚类结果。
     *
     * 该函数首先对输入数据进行 PCA 降维，降维到指定数量的主成分（默认为 2 维，以便进行二维可视化）。
     * 降维完成后，应用 K-means 聚类算法将数据划分为 `k` 个簇。然后，函数将数据点可视化，
     * 每个簇使用不同的颜色表示。
     *
     * 可视化的散点图中：
     * - X 轴表示第一主成分。
     * - Y 轴表示第二主成分（如果 `pca_components` > 2，则表示其他主成分）。
     * - 每个簇使用不同颜色表示。
     *
     * @param data 一个二维向量，每一行是一个数据点，每一列是一个特征。
     *             这是输入的数据，将会通过 PCA 降维，并使用 K-means 聚类。
     * @param k K-means 聚类的簇数，函数将数据划分为 `k` 个簇。
     * @param pca_components PCA 降维后保留的主成分数量，默认值为 2，适用于二维可视化。
     *
     * @throws invalid_argument 如果簇数 `k` 小于 1，或者输入数据为空。
     *
     * @note 该函数假定输入数据适合进行 PCA（即数据是数值型且格式正确）。
     */
    void plotPCAWithKMeans(
        const vector<vector<double>>& data,
        int k,
        int pca_components = 2);

    /**
     * @brief 绘制高斯核密度估计（KDE）曲线
     * @param samples 输入的一维数据样本向量
     * @param bandwidth 高斯核带宽（h），控制平滑程度
     * @param grid_n 栅格点数量，决定在横轴区间上采样的点数
     * @param width 图像画布宽度（像素）
     * @param height 图像画布高度（像素）
     */
    void plotKDE(
        const vector<double>& samples,
        double bandwidth,
        int grid_n = 200,
        int width = 800,
        int height = 600
    );

    /**
     * @brief 在同一张图中绘制样本直方图和高斯核密度估计（KDE）曲线
     * @param samples 输入的一维数据样本向量
     * @param bandwidth 高斯核带宽（h），控制密度曲线的平滑程度
     * @param bins 直方图柱子数量（默认 30）
     * @param grid_n KDE 曲线采样点数（默认 200）
     * @param width 图像画布宽度（像素，默认 800）
     * @param height 图像画布高度（像素，默认 600）
     */
    void plotHistogramKDE(
        const vector<double>& samples,
        double bandwidth,
        int bins = 30,
        int grid_n = 200,
        int width = 800,
        int height = 600
    );

    /**
     * @brief 传入两个向量，画出两条线
     */
    void plot_two_lines(
        const vector<double>& y1, 
        const vector<double>& y2,
        int width = 800,
        int height = 600
    );

    /**
     * @brief 传入两个向量，一个作为x，一个作为y。同时画出y=x
     */
    void plot_two_xy(
        const vector<double>& y1,
        const vector<double>& y2,
        int width = 800,
        int height = 600
    );

    /**
     * @brief 传入两个向量，在同一张图上画两个箱线图
     */
    void plot_two_boxplot(
        const vector<double>& y1,
        const vector<double>& y2,
        int width = 800,
        int height = 600
    );

    /**
    * @brief 接收一个二维矩阵，绘制热力图
    */
    void plot_heatmap(
        const vector<vector<double>>& matrix,
        bool show_colorbar = true,
        int width = 800,
        int height = 600
    );


} // namespace GenePlot

#endif //