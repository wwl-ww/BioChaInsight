# PCA
[TOC]

## 1. 算法介绍

   * **背景与目标**
     主成分分析（PCA）最早由 Karl Pearson 在1901年提出，是一种无监督的线性降维方法。它的核心目标是：

     > 在保留数据总方差（信息量）尽可能多的前提下，将高维数据投影到低维空间。

   * **应用场景**

     * 降维与可视化：将高维样本映射到2D/3D以便可视化
     * 去噪：去除小方差方向上的噪声
     * 特征压缩：在训练机器学习模型前减少输入维度，提升效率
     * 数据预处理：常用作后续分类、聚类等算法的输入

   * **核心思路**

     1. **数据中心化**（Zero-mean）——去除均值后，保证后续方差统计不受偏移影响；
     2. **协方差矩阵**或**SVD**分解——捕捉变量之间的线性相关结构；
     3. **特征向量**（主成分方向）对应最大方差方向；
     4. **投影**——将原始数据投影到前 $k$ 个主成分，完成降维。

---

## 2. 公式及原理

   **2.1 方差最大化视角**

   * 寻找一个单位向量 $\mathbf{w}$，使得投影后的一维变量 $y_i = \mathbf{w}^\top \mathbf{x}_i$ 具有最大方差：

     $$
       \max_{\|\mathbf{w}\|=1} \quad \mathrm{Var}(y)
       = \max_{\|\mathbf{w}\|=1} \frac{1}{n}\sum_{i=1}^n (\mathbf{w}^\top \mathbf{x}_i - \overline{y})^2
       = \max_{\|\mathbf{w}\|=1} \mathbf{w}^\top \mathbf{C}\,\mathbf{w}
     $$

     其中 $\mathbf{C}$ 是样本协方差矩阵。

   * 引入拉格朗日乘子 $\lambda$，解

     $$
       \mathcal{L}(\mathbf{w},\lambda)
       = \mathbf{w}^\top \mathbf{C}\,\mathbf{w} - \lambda(\mathbf{w}^\top \mathbf{w}-1),
     $$

     对 $\mathbf{w}$ 求导并令梯度为零，得到

     $$
       \mathbf{C}\,\mathbf{w} = \lambda\,\mathbf{w},
     $$

     即标准的**特征值问题**。最大方差对应最大特征值 $\lambda_1$ 及其特征向量 $\mathbf{v}_1$。

   **2.2 协方差矩阵构造**

   1. 原始数据矩阵 $\mathbf{X}$ 为 $n\times d$，每行一个样本。
   2. 样本均值：

      $$
        \boldsymbol{\mu} = \frac{1}{n}\sum_{i=1}^n \mathbf{x}_i \quad (d\text{-维向量})
      $$
   3. 中心化数据：

      $$
        \mathbf{X}_{\mathrm{c}} = \mathbf{X} - \mathbf{1}\,\boldsymbol{\mu}^\top
      $$
   4. 协方差矩阵：

      $$
        \mathbf{C}
        = \frac{1}{n-1}\,\mathbf{X}_{\mathrm{c}}^\top\,\mathbf{X}_{\mathrm{c}}
        \quad (d\times d)
      $$

   **2.3 特征分解与降维**

   * 对 $\mathbf{C}$ 做特征值分解：

     $$
       \mathbf{C} = \mathbf{V}\,\boldsymbol{\Lambda}\,\mathbf{V}^\top,
       \quad
       \boldsymbol{\Lambda} = \mathrm{diag}(\lambda_1,\dots,\lambda_d),
       \quad
       \lambda_1 \ge \lambda_2 \ge \dots \ge \lambda_d.
     $$
   * 取前 $k$ 个特征向量组成矩阵
     $\mathbf{V}_k = [\mathbf{v}_1,\dots,\mathbf{v}_k]$（维度 $d\times k$）。
   * 投影得到低维表示：

     $$
       \mathbf{Z} = \mathbf{X}_{\mathrm{c}}\,\mathbf{V}_k
       \quad (n\times k)
     $$

   **2.4 SVD 视角（可选）**

   * 对中心化矩阵 $\mathbf{X}_{\mathrm{c}}$ 做奇异值分解：

     $$
       \mathbf{X}_{\mathrm{c}} = \mathbf{U}\,\boldsymbol{\Sigma}\,\mathbf{V}^\top,
     $$

     则 $\mathbf{C} = \frac{1}{n-1}\,\mathbf{V}\,\boldsymbol{\Sigma}^2\,\mathbf{V}^\top$，奇异值平方与特征值对应，$\mathbf{V}$ 的前 $k$ 列即主成分方向。

---

## 3. 伪代码

```text
# 输入
#   X: 原始数据矩阵，形状 (n, d)
#   k: 降维后目标维度（k ≤ d）
# 输出
#   Z: 降维后数据，形状 (n, k)
#   V_k: 主成分方向矩阵，形状 (d, k)

function PCA(X, k):
    # 1) 计算样本均值
    μ ← (1/n) * sum_rows(X)             # μ: (d,)

    # 2) 数据中心化
    X_c ← X − repeat_row_vector(μ, n)  # X_c: (n, d)

    # 3) 计算协方差矩阵
    C ← (1/(n−1)) * X_cᵀ × X_c         # C: (d, d)

    # 4) 特征值分解
    (Λ, V) ← eigendecompose(C)         # Λ: (d,), V: (d, d)
    #    Λ 按从大到小排序，V 列向量对应特征向量

    # 5) 取前 k 个特征向量
    V_k ← V[:, 0:k]                    # V_k: (d, k)

    # 6) 数据投影
    Z ← X_c × V_k                      # Z: (n, k)

    return Z, V_k
```

* **时间复杂度**

  * 协方差矩阵构造： $O(nd^2)$
  * 特征分解： $O(d^3)$（当 $d$ 很大时可用 SVD + 随机投影等近似方法加速）

* **内存注意**

  * 若 $d\gg n$，更倾向于直接对 $X_c$ 做 SVD，以节省构造 $d\times d$ 矩阵的内存。


