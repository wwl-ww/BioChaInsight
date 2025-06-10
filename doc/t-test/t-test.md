# t-test
[TOC]

## 1. 算法介绍

   * **背景与目标**
     在基因表达差异分析中，常用两组（实验组 vs. 对照组）的独立样本 t 检验（two-sample t-test）来判断某一基因在两种处理下表达水平的均值是否存在显著差异。

     > 核心目标：检验基因在实验组和对照组的平均表达值 $\mu_1$ 与 $\mu_2$ 是否相等，即
     >
     > $$
     > H_0:\;\mu_1 = \mu_2,\quad H_A:\;\mu_1 \neq \mu_2.
     > $$

   * **应用场景**

     * RNA-seq、微阵列等高通量数据的单基因差异表达初步筛选
     * 简单实验设计下的小样本差异检测
     * 与多重检验校正方法（如 FDR）联合使用

   * **核心思路**

     1. 分别计算两组样本的均值和方差；
     2. 构造 t 统计量，反映两组均值差异相对于组内变异的大小；
     3. 根据 t 分布计算双侧 p 值，判断差异显著性。

---

## 2. 公式及原理

   **2.1 样本统计量**
   对照组样本表达值 $\{x_{1i}\}_{i=1}^{n_1}$，实验组样本表达值 $\{x_{2j}\}_{j=1}^{n_2}$：

   $$
     \bar{x}_1 = \frac{1}{n_1}\sum_{i=1}^{n_1}x_{1i},\quad
     \bar{x}_2 = \frac{1}{n_2}\sum_{j=1}^{n_2}x_{2j};
   $$

   $$
     s_1^2 = \frac{1}{n_1-1}\sum_{i=1}^{n_1}(x_{1i}-\bar{x}_1)^2,\quad
     s_2^2 = \frac{1}{n_2-1}\sum_{j=1}^{n_2}(x_{2j}-\bar{x}_2)^2.
   $$

   **2.2 等方差 t 检验（pooled）**

   1. **合并方差**

      $$
        s_p^2
        = \frac{(n_1-1)\,s_1^2 + (n_2-1)\,s_2^2}{n_1 + n_2 - 2}.
      $$
   2. **t 统计量**

      $$
        t = \frac{\bar{x}_1 - \bar{x}_2}
                 {s_p\,\sqrt{\frac{1}{n_1} + \frac{1}{n_2}}}.
      $$
   3. **自由度**

      $$
        \mathrm{df} = n_1 + n_2 - 2.
      $$

   **2.3 Welch’s t 检验（不等方差）**
   当两组方差明显不等时，使用 Welch 校正：

   $$
     t = \frac{\bar{x}_1 - \bar{x}_2}
              {\sqrt{\frac{s_1^2}{n_1} + \frac{s_2^2}{n_2}}},\quad
     \mathrm{df}
     = \frac{\bigl(\tfrac{s_1^2}{n_1}+\tfrac{s_2^2}{n_2}\bigr)^2}
            {\tfrac{(s_1^2/n_1)^2}{n_1-1} + \tfrac{(s_2^2/n_2)^2}{n_2-1}}.
   $$

   **2.4 p 值计算**
   计算双侧 p 值：

   $$
     p = 2 \,\bigl[1 - F_{t}(|t|;\,\mathrm{df})\bigr],
   $$

   其中 $F_{t}(\cdot;\,\mathrm{df})$ 为自由度为 df 的 t 分布累计函数。

---

## 3. 伪代码

```text
# 输入
#   X1: 对照组表达值数组，长度 n1
#   X2: 实验组表达值数组，长度 n2
#   equal_var: 布尔，True→等方差检验, False→Welch 校正
# 输出
#   t_stat: t 统计量
#   df: 自由度
#   p_value: 双侧 p 值

function two_sample_t_test(X1, X2, equal_var):
    n1 ← length(X1);   n2 ← length(X2)
    μ1 ← mean(X1);     μ2 ← mean(X2)
    s1_sq ← variance(X1)  # 分母 n1−1
    s2_sq ← variance(X2)

    if equal_var:
        # pooled variance
        sp_sq ← ((n1−1)*s1_sq + (n2−1)*s2_sq) / (n1 + n2 − 2)
        t_stat ← (μ1 − μ2) / (sqrt(sp_sq) * sqrt(1/n1 + 1/n2))
        df ← n1 + n2 − 2
    else:
        # Welch’s t
        t_stat ← (μ1 − μ2) / sqrt(s1_sq/n1 + s2_sq/n2)
        numerator ← (s1_sq/n1 + s2_sq/n2)^2
        denominator ← (s1_sq/n1)^2/(n1−1) + (s2_sq/n2)^2/(n2−1)
        df ← numerator / denominator

    # 双侧 p 值
    p_value ← 2 * (1 − t_cdf(abs(t_stat), df))

    return t_stat, df, p_value
```

* **时间复杂度**：$O(n_1 + n_2)$
* **注意事项**：

  * 样本量过小时，正态性假设更关键；
  * 基因组范围内多重检验时需配合 FDR 或 Bonferroni 校正；
  * 对极低表达或零膨胀数据，建议先做滤除或转换（如 log2(x+1)）。
