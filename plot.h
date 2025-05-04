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
     * ����ɽͼ��Volcano Plot��
     * @param log2fc        ÿ������� log2 Fold Change�����������챶����
     * @param pvals         ÿ�������Ӧ�� p ֵ
     * @param genes         ���л�������֣���ѡ�����Ҫ��ע�ض�����
     * @param fc_thresh     Fold Change ��ֵ������ 1.0 ��ʾ 2 �����죬Ĭ����1.0��
     * @param pval_thresh   p ֵ��ֵ������ 0.05 ��ʾ 5% ������ˮƽ��Ĭ����0.05��
     */
    void volcano(
        const vector<double>& log2fc,
        const vector<double>& pvals,
        const vector<string>& genes = {}, // ���֮����ʵ��
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
     * �Ը������ݽ������ɷַ�����PCA���������ݽ�ά��ָ����ά�ȣ�
     * Ȼ��Խ�ά�������Ӧ�� K-means ���࣬�����ƾ�������
     *
     * �ú������ȶ��������ݽ��� PCA ��ά����ά��ָ�����������ɷ֣�Ĭ��Ϊ 2 ά���Ա���ж�ά���ӻ�����
     * ��ά��ɺ�Ӧ�� K-means �����㷨�����ݻ���Ϊ `k` ���ء�Ȼ�󣬺��������ݵ���ӻ���
     * ÿ����ʹ�ò�ͬ����ɫ��ʾ��
     *
     * ���ӻ���ɢ��ͼ�У�
     * - X ���ʾ��һ���ɷ֡�
     * - Y ���ʾ�ڶ����ɷ֣���� `pca_components` > 2�����ʾ�������ɷ֣���
     * - ÿ����ʹ�ò�ͬ��ɫ��ʾ��
     *
     * @param data һ����ά������ÿһ����һ�����ݵ㣬ÿһ����һ��������
     *             ������������ݣ�����ͨ�� PCA ��ά����ʹ�� K-means ���ࡣ
     * @param k K-means ����Ĵ��������������ݻ���Ϊ `k` ���ء�
     * @param pca_components PCA ��ά���������ɷ�������Ĭ��ֵΪ 2�������ڶ�ά���ӻ���
     *
     * @throws invalid_argument ������� `k` С�� 1��������������Ϊ�ա�
     *
     * @note �ú����ٶ����������ʺϽ��� PCA������������ֵ���Ҹ�ʽ��ȷ����
     */
    void plotPCAWithKMeans(
        const vector<vector<double>>& data,
        int k,
        int pca_components = 2);

    /**
     * @brief ���Ƹ�˹���ܶȹ��ƣ�KDE������
     * @param samples �����һά������������
     * @param bandwidth ��˹�˴���h��������ƽ���̶�
     * @param grid_n դ��������������ں��������ϲ����ĵ���
     * @param width ͼ�񻭲���ȣ����أ�
     * @param height ͼ�񻭲��߶ȣ����أ�
     */
    void plotKDE(
        const vector<double>& samples,
        double bandwidth,
        int grid_n = 200,
        int width = 800,
        int height = 600
    );

    /**
     * @brief ��ͬһ��ͼ�л�������ֱ��ͼ�͸�˹���ܶȹ��ƣ�KDE������
     * @param samples �����һά������������
     * @param bandwidth ��˹�˴���h���������ܶ����ߵ�ƽ���̶�
     * @param bins ֱ��ͼ����������Ĭ�� 30��
     * @param grid_n KDE ���߲���������Ĭ�� 200��
     * @param width ͼ�񻭲���ȣ����أ�Ĭ�� 800��
     * @param height ͼ�񻭲��߶ȣ����أ�Ĭ�� 600��
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
     * @brief ������������������������
     */
    void plot_two_lines(
        const vector<double>& y1, 
        const vector<double>& y2,
        int width = 800,
        int height = 600
    );

    /**
     * @brief ��������������һ����Ϊx��һ����Ϊy��ͬʱ����y=x
     */
    void plot_two_xy(
        const vector<double>& y1,
        const vector<double>& y2,
        int width = 800,
        int height = 600
    );

    /**
     * @brief ����������������ͬһ��ͼ�ϻ���������ͼ
     */
    void plot_two_boxplot(
        const vector<double>& y1,
        const vector<double>& y2,
        int width = 800,
        int height = 600
    );

    /**
    * @brief ����һ����ά���󣬻�������ͼ
    */
    void plot_heatmap(
        const vector<vector<double>>& matrix,
        bool show_colorbar = true,
        int width = 800,
        int height = 600
    );


} // namespace GenePlot

#endif //