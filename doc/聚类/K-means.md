# K-means
[TOC]

## 1. 算法介绍

   * **背景与目标**
     K-means 是最经典、最常用的原型聚类（prototype-based clustering）算法之一，由 Stuart Lloyd 于1957年提出，1982年被广泛推广。其核心目标是：

     > 将给定的 $n$ 个数据点划分为 $K$ 个簇，使得簇内样本的相似度（通常用欧氏距离）最大化、簇间差异最大化。

   * **应用场景**

     * 客户分群、市场细分
     * 图像分割（基于像素特征）
     * 文档聚类（基于文本向量）
     * 低维数据的快速可视化与预处理

   * **核心思路**

     1. **初始化**：在数据空间中随机选取 $K$ 个点作为初始簇心（centroid），或使用 k-means++ 改进；
     2. **迭代优化**：

        * **分配（Assignment）**：将每个样本指派给最近的簇心；
        * **更新（Update）**：根据新的簇内样本重新计算簇心；
     3. **收敛判定**：当簇心位置不再发生显著变化，或达到最大迭代次数时停止。

---

## 2. 公式及原理

   **2.1 目标函数**
   K-means 旨在最小化簇内平方误差之和（Within-Cluster Sum of Squares, WCSS）：

   $$
     J = \sum_{j=1}^K \sum_{\mathbf{x}_i \in C_j} \bigl\|\mathbf{x}_i - \boldsymbol{\mu}_j\bigr\|^2,
   $$

   其中

   * $\mathbf{x}_i$ 为第 $i$ 个样本向量；
   * $C_j$ 为第 $j$ 个簇的样本集合；
   * $\boldsymbol{\mu}_j = \frac{1}{|C_j|}\sum_{\mathbf{x}_i\in C_j}\mathbf{x}_i$ 为第 $j$ 簇的中心。

   **2.2 分配步骤**
   对每个样本 $\mathbf{x}_i$，计算其到各簇心的欧氏距离，并指派给最近的簇：

   $$
     c_i = \arg\min_{1 \le j \le K} \|\mathbf{x}_i - \boldsymbol{\mu}_j\|^2.
   $$

   **2.3 更新步骤**
   对每个簇 $j$，根据当前指派的样本重新计算簇心：

   $$
     \boldsymbol{\mu}_j = \frac{1}{|C_j|}\sum_{\{\mathbf{x}_i \mid c_i = j\}}\mathbf{x}_i.
   $$

   **2.4 收敛条件**

   * 簇心位置前后迭代变化小于阈值：$\|\boldsymbol{\mu}_j^{(t)} - \boldsymbol{\mu}_j^{(t-1)}\| < \varepsilon$；
   * 达到预设最大迭代次数；
   * 目标函数 $J$ 的下降量低于阈值。

   **2.5 初始化选择**

   * **随机初始化**：随机选取 $K$ 个样本作为初始簇心，可能导致收敛到次优解；
   * **k-means++**：首个簇心随机，后续簇心按与已有簇心距离平方加权随机选取，能显著提高聚类质量和收敛速度。

---

## 3. 伪代码

```text
# 输入
#   X: 原始数据矩阵，形状 (n, d)
#   K: 簇的个数
#   max_iter: 最大迭代次数
#   ε: 收敛阈值（可选）
# 输出
#   c: 样本簇标号数组，长度 n，c[i] ∈ {1,…,K}
#   μ: 簇心矩阵，形状 (K, d)

function KMeans(X, K, max_iter, ε):
    n, d ← shape(X)

    # 1) 初始化簇心 μ (可选 k-means++ 或 随机选样本)
    μ ← initialize_centroids(X, K)

    for t in 1…max_iter:
        # 2) 分配步骤：指派每个样本到最近簇心
        for i in 1…n:
            distances ← [‖X[i] − μ[j]‖² for j in 1…K]
            c[i] ← argmin(distances)

        # 3) 更新步骤：重新计算每个簇的中心
        μ_new ← zero_matrix(K, d)
        counts ← zero_vector(K)
        for i in 1…n:
            μ_new[c[i]] ← μ_new[c[i]] + X[i]
            counts[c[i]] ← counts[c[i]] + 1
        for j in 1…K:
            if counts[j] > 0:
                μ_new[j] ← μ_new[j] / counts[j]
            else:
                μ_new[j] ← μ[j]  # 若簇空，可保持原中心或重新初始化

        # 4) 收敛判定
        if max_j ‖μ_new[j] − μ[j]‖ < ε:
            break

        μ ← μ_new

    return c, μ
```

* **时间复杂度**

  * 每次迭代分配： $O(n K d)$
  * 更新簇心： $O(n d)$
  * 总计： \(\displaystyle O(\text{max\_iter}\times n K d)\)

* **注意事项**

  * 结果对初始化敏感，建议多次运行取最优；
  * 当数据分布复杂或簇数 $K$ 未知时，可结合肘部法则（Elbow Method）、轮廓系数（Silhouette）等方法选取 $K$。
