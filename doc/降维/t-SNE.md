# t-SNE
[TOC]

# 1. 算法介绍

   * **背景与目标**
     t-SNE（t-distributed Stochastic Neighbor Embedding）由 Laurens van der Maaten 和 Geoffrey Hinton 于2008年提出，是一种非线性降维与可视化技术。其核心目标是：

     > 在低维空间中尽可能保留高维数据的局部结构（相似度），使得“相似”的样本点在低维中也距离较近，“不相似”的样本点距离较远，从而便于可视化高维数据的聚类与分布特征。

   * **应用场景**

     * 高维特征的二维或三维可视化（如图像、文本、基因表达等）
     * 对聚类结构的直观展示
     * 探索数据的局部拓扑关系

   * **核心思路**

     1. **高维相似度建模**——将高维点对 $\mathbf{x}_i,\mathbf{x}_j$ 的距离转换成条件概率 $p_{j|i}$（用高斯分布），以衡量“在点 $\mathbf{x}_i$ 邻域中看到 $\mathbf{x}_j$ 的概率”；
     2. **对称化概率**——构造对称相似度 $p_{ij} = \frac{p_{j|i}+p_{i|j}}{2n}$；
     3. **低维相似度建模**——在低维空间用 Student t 分布（自由度为1）的重尾性质，定义 $q_{ij}$；
     4. **最小化 KL 散度**——通过梯度下降，使低维分布 $q_{ij}$ 与高维分布 $p_{ij}$ 在全局上最接近。

---

# 2. 公式及原理

   **2.1 高维条件概率**
   对于每个数据点 $\mathbf{x}_i$，令其对 $\mathbf{x}_j$ 的条件相似度为：

   $$
     p_{j|i} = \frac{\exp\bigl(-\|\mathbf{x}_i-\mathbf{x}_j\|^2/2\sigma_i^2\bigr)}{\sum_{k\neq i}\exp\bigl(-\|\mathbf{x}_i-\mathbf{x}_k\|^2/2\sigma_i^2\bigr)},
   $$

   其中 $\sigma_i$ 根据预设的**困惑度**（perplexity）通过二分搜索确定，使得

   $$
     \mathrm{Perp}(P_i) = 2^{-\sum_j p_{j|i}\log_2 p_{j|i}}
     \approx \text{预设值}.
   $$

   **2.2 对称化相似度**

   $$
     p_{ij} = \frac{p_{j|i}+p_{i|j}}{2n},\quad p_{ii}=0.
   $$

   **2.3 低维 Student t 分布**
   对于低维映射点 $\mathbf{y}_i,\mathbf{y}_j$，定义

   $$
     q_{ij}
     = \frac{\bigl(1 + \|\mathbf{y}_i-\mathbf{y}_j\|^2\bigr)^{-1}}
            {\sum_{k\neq \ell}\bigl(1 + \|\mathbf{y}_k-\mathbf{y}_\ell\|^2\bigr)^{-1}},
     \quad q_{ii}=0.
   $$

   **2.4 目标函数：KL 散度**
   最小化高维与低维分布之间的 Kullback–Leibler 散度：

   $$
     C = \mathrm{KL}(P\|Q)
       = \sum_{i\neq j} p_{ij}\,\log\frac{p_{ij}}{q_{ij}}.
   $$

   对 $\mathbf{Y}=\{\mathbf{y}_i\}$ 求梯度，并用带动量的梯度下降更新：

   $$
     \frac{\partial C}{\partial \mathbf{y}_i}
     = 4 \sum_j (p_{ij}-q_{ij})
       \,( \mathbf{y}_i-\mathbf{y}_j)\,(1+\|\mathbf{y}_i-\mathbf{y}_j\|^2)^{-1}.
   $$

---

# 3. 伪代码

```text
# 输入
#   X: 原始数据矩阵，形状 (n, d)
#   perplexity: 困惑度超参数
#   dim: 目标低维维度（通常为2或3）
#   max_iter: 最大迭代次数
# 输出
#   Y: 降维后数据，形状 (n, dim)

function tSNE(X, perplexity, dim, max_iter):
    n ← number_of_rows(X)

    # 1) 计算高维相似度矩阵 P
    for i in 1…n:
        # 根据困惑度二分搜索 σ_i，得到 p_{j|i}
        σ_i ← binary_search_sigma(X[i], X, perplexity)
        for j in 1…n, j≠i:
            p_{j|i} ← exp(-||X[i]-X[j]||²/(2σ_i²))
        normalize p_{·|i} so sum_j p_{j|i}=1
    construct p_{ij} ← (p_{j|i}+p_{i|j})/(2n)

    # 2) 初始化 Y（小的随机扰动）
    Y ← random_matrix(n, dim) * 0.0001

    # 3) 梯度下降：带学习率、动量和早期夸张
    momentum ← 0.5
    gains ← ones(n, dim)
    Y_inc ← zeros(n, dim)
    P ← P * 4           # 早期夸张 (optional)
    
    for t in 1…max_iter:
        # 3.1) 计算低维相似度 Q
        for i,j in 1…n:
            num_{ij} ← (1 + ||Y[i]-Y[j]||²)^(-1)
        Q ← normalize num s.t. sum_{i≠j} Q_{ij}=1

        # 3.2) 计算梯度
        for i in 1…n:
            grad[i] ← 4 * Σ_j (P_{ij}-Q_{ij}) * (Y[i]-Y[j]) * num_{ij}

        # 3.3) 自适应学习率与动量更新
        for i in 1…n, d in 1…dim:
            gains[i,d] ← (gains[i,d]+0.2) if sign(grad[i,d])≠sign(Y_inc[i,d])
                          else gains[i,d]*0.8
            gains[i,d] ← max(gains[i,d], 0.01)
            Y_inc[i,d] ← momentum * Y_inc[i,d]
                          - learning_rate * gains[i,d] * grad[i,d]
            Y[i,d]   ← Y[i,d] + Y_inc[i,d]

        # 3.4) 调整动量与早期夸张
        if t == 250:
            momentum ← 0.8
        if t == 100:
            P ← P / 4      # 结束早期夸张

    return Y
```

* **复杂度**

  * 高维相似度计算： $O(n^2 d)$（可用近似算法如 Barnes–Hut 或 FFT 加速到 $O(n\log n)$）
  * 每次迭代梯度计算： $O(n^2)$（同样可借助近似方法加速）

