# LLE
[TOC]

## 1. 算法介绍

   * **背景与目标**
     局部线性嵌入（LLE, Locally Linear Embedding）由 Roweis 和 Saul 于2000年提出，是一种基于流形学习的非线性降维方法。其核心目标是：

     > 在低维空间中保留高维数据在局部邻域内的线性重构关系，从而揭示数据的内在低维流形结构。

   * **应用场景**

     * 高维流形数据的可视化
     * 保留局部几何特征的降维预处理
     * 图像、文本、传感器时序等数据的非线性嵌入

   * **核心思路**

     1. **局部邻域选择**——对每个样本点 $\mathbf{x}_i$，找到其在原空间中最接近的 $k$ 个邻居；
     2. **线性重构权重**——在高维空间内，用邻居点的线性组合尽可能重构 $\mathbf{x}_i$，求解最优权重矩阵 $\{W_{ij}\}$；
     3. **全局嵌入**——在低维空间中，保持相同的权重关系，求解所有低维嵌入点 $\{\mathbf{y}_i\}$，使局部重构误差最小。

---

## 2. 公式及原理

   **2.1 邻域与重构权重**
   对于每个高维样本 $\mathbf{x}_i\in\mathbb{R}^d$：

   1. 找到其 $k$ 个最近邻索引集合 $N(i)$。
   2. 求解线性重构权重 $\{W_{ij}\}_{j\in N(i)}$，使下式最小：

      $$
        \min_{W_{ij}}\;\bigl\|\mathbf{x}_i - \sum_{j\in N(i)} W_{ij}\,\mathbf{x}_j\bigr\|^2
        \quad\text{s.t.}\;\sum_{j\in N(i)} W_{ij} = 1.
      $$

   这是一组带和为1约束的最小二乘问题。常用拉格朗日乘子或直接对局部协方差矩阵求解：

   $$
     C^{(i)}_{lm} = (\mathbf{x}_i - \mathbf{x}_l)^\top(\mathbf{x}_i - \mathbf{x}_m),
     \quad l,m\in N(i),
   $$

   $$
     W_{i,N(i)} = \frac{C^{(i)^{-1}}\mathbf{1}}{\mathbf{1}^\top C^{(i)^{-1}}\mathbf{1}}.
   $$

   **2.2 低维全局嵌入**
   利用高维空间得到的同样权重，在低维 $\mathbf{y}_i\in\mathbb{R}^m$ 中重构误差：

   $$
     \Phi(\mathbf{Y})
     = \sum_{i=1}^n \Bigl\|\mathbf{y}_i - \sum_{j\in N(i)} W_{ij}\,\mathbf{y}_j\Bigr\|^2.
   $$

   展开可得二次型：

   $$
     \Phi(\mathbf{Y})
     = \mathrm{Tr}\bigl(\mathbf{Y}^\top \mathbf{M}\,\mathbf{Y}\bigr),
     \quad
     \mathbf{M} = (\mathbf{I}-\mathbf{W})^\top(\mathbf{I}-\mathbf{W}),
   $$

   其中 $\mathbf{W}$ 为 $n\times n$ 权重矩阵（非零项仅在邻域内）。最小化 $\Phi$ 等价于取矩阵 $\mathbf{M}$ 的最小特征值对应的特征向量。由于 $\mathbf{M}\mathbf{1}=0$，其最小特征值为0，对应特征向量 $\mathbf{1}$（平移不变性），舍去后取接下来的 $m$ 个最小特征向量，组成低维坐标矩阵 $\mathbf{Y}\in\mathbb{R}^{n\times m}$。

---

## 3. 伪代码

```text
# 输入
#   X: 原始数据矩阵，形状 (n, d)
#   k: 每点邻居数
#   m: 目标低维维度（m < d）
# 输出
#   Y: 降维后数据，形状 (n, m)

function LLE(X, k, m):
    n, d ← shape(X)

    # 1) 找到每个点的 k 近邻（欧氏距离或其他度量）
    for i in 1…n:
        N(i) ← indices_of_k_nearest_neighbors(X[i], X, k)

    # 2) 计算重构权重 W
    W ← zero_matrix(n, n)
    for i in 1…n:
        # 构造局部协方差矩阵 C^(i)
        C_i ← zero_matrix(k, k)
        for a in 1…k, b in 1…k:
            C_i[a,b] ← (X[i] - X[N(i)[a]]) · (X[i] - X[N(i)[b]])
        # 加入小正则项以确保可逆
        C_i ← C_i + ε * trace(C_i) * I_k
        # 计算局部权重
        w_i ← solve(C_i, ones(k))
        w_i ← w_i / sum(w_i)
        for idx in 1…k:
            W[i, N(i)[idx]] ← w_i[idx]

    # 3) 构造矩阵 M = (I - W)'(I - W)
    I ← identity_matrix(n)
    M ← (I - W)ᵀ × (I - W)  # M: (n, n)

    # 4) 特征分解：取 M 的最小 m+1 个特征向量
    (eigvals, eigvecs) ← eigendecompose(M)
    # 按特征值从小到大排序
    sort_indices ← argsort(eigvals)
    # 舍去第一个（对应特征值0的常数向量）
    selected ← sort_indices[2 : m+1]
    Y ← eigvecs[:, selected]  # Y: (n, m)

    return Y
```

* **时间与内存复杂度**

  * 最近邻搜索：$O(n^2 d)$（可用 KD-tree、球树等加速）
  * 局部最小二乘：$n$ 次求解 $k\times k$ 线性系统，总 $O(n k^3)$
  * 全局特征分解：对 $n\times n$ 矩阵，复杂度 $O(n^3)$（可用 Lanczos 等稀疏方法加速）

* **注意事项**

  * 邻居数 $k$ 不宜过大或过小，经验上 $k\in[5,50]$；
  * 正则项 $\epsilon$ 可防止协方差矩阵奇异；
  * 对大规模数据可采用近似算法或分批处理。
