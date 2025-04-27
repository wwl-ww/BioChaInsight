#pragma once
#ifndef PLOT_H
#define PLOT_H

#include "matplotlibcpp.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

namespace plt = matplotlibcpp;

bool init_python(const string &python_home = "D:/anaconda");

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
        const vector<double> &log2fc,
        const vector<double> &pvals,
        const vector<string> &genes = {}, // ���֮����ʵ��
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
     * @throws std::invalid_argument ������� `k` С�� 1��������������Ϊ�ա�
     *
     * @note �ú����ٶ����������ʺϽ��� PCA������������ֵ���Ҹ�ʽ��ȷ����
     */
    void plotPCAWithKMeans(
        const std::vector<std::vector<double>> &data,
        int k,
        int pca_components = 2);

} // namespace GenePlot

#endif //
