#pragma once
#include "stdafx.h"
#ifndef HISVG_H
#define HISVG_H

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <string>
#include "ST.h"
using namespace StatTools;

// 用来构建层次聚类树
struct Node {
	Node* left = nullptr;
	Node* right = nullptr;
	double height = 0.0;
	double x = 0.0, y = 0.0;
};

/**
 * @brief 构建聚类树（dendrogram）
 * @param Z linkage 记录
 * @param N 叶节点数量
 * @param maxH 输出最大合并高度（用于布局）
 * @return 聚类树根节点指针
 */
Node* buildDendrogram(const std::vector<Linkage>& Z, int N, double& maxH);

// 为节点分配 x 坐标：叶子等距，内部节点取子节点中点
void assignX(Node* n, double spacing, double& cur);

// 为节点分配 y 坐标：底部留白 20px
void assignY(Node* n, double maxH, double H);

// 绘制树状线段
void draw(Node* n, std::ofstream& os);

/**
 * @brief 将聚类结果输出为 SVG 文件，并在叶节点下方显示自定义标签
 * @param root 聚类树根节点
 * @param N 叶节点数量
 * @param W SVG 画布宽度
 * @param H SVG 画布高度
 * @param file 输出文件路径
 * @param labels 叶节点标签列表，长度应为 N
 */
void writeSVG(
	Node* root, int N, double W, double H,
	const std::string& file,
	const std::vector<std::string>& labels
);

// 递归释放树节点内存
void cleanup(Node* n);

#endif // HISVG_H
