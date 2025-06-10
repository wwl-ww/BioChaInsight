# K-means++
[TOC]

## 1. 算法介绍

   * **背景与目标**
     k-means++ 是 David Arthur 和 Sergei Vassilvitskii 于2007年提出的改进 k-means 初始化方法，其核心目标是：

     > 在保证聚类质量的前提下，通过更合理地选择初始簇心，减少 k-means 对随机初始化的敏感性，加快收敛并降低陷入次优解的风险。

   * **应用场景**

     * 所有使用 k-means 聚类但对初始化鲁棒性和收敛速度有较高要求的场景
     * 大规模数据聚类预处理，减少多次重启的计算成本

   * **核心思路**

     1. **第一簇心**：随机从样本集中任选一个点；
     2. **后续簇心**：对于每个样本点，计算其与已有簇心的最小距离平方 $D(x)^2$；
     3. **概率采样**：按权重 $D(x)^2$ 对样本进行加权随机抽样，选出下一个簇心；
     4. **重复**：直到选出 $K$ 个簇心，再执行标准 k-means 算法。

---

## 2. 公式及原理

   设已选出的簇心集合为 $\{\mu_1,\dots,\mu_c\}$，对任意样本点 $\mathbf{x}$：

   1. **最小距离平方**

      $$
        D(\mathbf{x})^2 = \min_{1\le i\le c}\|\mathbf{x}-\mu_i\|^2.
      $$

   2. **采样概率**
      对所有样本归一化后，样本 $\mathbf{x}$ 被选为下一个簇心的概率为：

      $$
        P(\mathbf{x}) = \frac{D(\mathbf{x})^2}{\sum_{x'}D(x')^2}.
      $$

   3. **加速变体**
      实际实现中可用“距离二分桶”或近似方法快速更新 $D(x)$，以降低每次选点的计算开销。

---

## 3. 伪代码

```text
# 输入
#   X: 数据矩阵，形状 (n, d)
#   K: 簇的个数
# 输出
#   centroids: 初始簇心列表，长度 K

function kmeans_pp_init(X, K):
    n, d ← shape(X)
    centroids ← empty list

    # 1) 随机选第一个簇心
    idx0 ← random_integer(1, n)
    centroids.append(X[idx0])

    # 2) 依次选取剩余 K-1 个簇心
    for c in 2…K:
        # 2.1) 计算每个样本到最近已有簇心的距离平方
        D_sq ← array of length n
        for i in 1…n:
            D_sq[i] ← min_{μ ∈ centroids} ‖X[i] - μ‖²

        # 2.2) 按 D_sq 权重进行随机抽样
        sum_D ← sum(D_sq)
        probs ← D_sq / sum_D     # 归一化概率
        idx ← sample_index_with_probability(probs)
        centroids.append(X[idx])

    return centroids

# 后续可将 centroids 传入标准 KMeans 进行迭代
```

* **时间复杂度**

  * 每次选新簇心计算距离：$O(n\,c\,d)$，总计选取 K 个簇心约 $O(n\,K\,d)$
  * 相比随机初始化，多了这一步但通常能显著减少后续迭代次数。

* **注意事项**

  * 随机数种子可固定以保证可复现；
  * 对极大规模数据可结合分层抽样或局部敏感哈希加速采样。
