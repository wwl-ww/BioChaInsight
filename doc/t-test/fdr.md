# FDR 校正
[TOC]

## 1. 算法介绍

   * **背景与目标**
     在基因表达差异分析中，我们通常对成千上万个基因分别进行统计检验（如 t-test、Wald 检验等），得到大量原始 p 值。直接以显著性水平 $\alpha$ 筛选会导致大量假阳性（Type I error）。

     > adjust\_fdr（FDR 校正）旨在在多重假设检验场景下，控制**假发现率**（False Discovery Rate），即期望的假阳性结果占所有阳性结果的比例不超过给定阈值。

   * **应用场景**

     * RNA-seq、微阵列的全基因差异表达分析
     * 高通量蛋白质组、甲基化等组学数据中的多重检验校正
     * 任何成千上万并行假设检验的场景

   * **核心思路**

     1. **排序原始 p 值**——将 $m$ 个 p 值从小到大排序；
     2. **计算调整后阈值**——根据排序位置和总检验数，为每个 p 值分配一个对应的 FDR 阈值；
     3. **反向修正**——保证调整后 p 值单调不增；
     4. **选择显著基因**——以调整后 p 值 $\le$ 预设 FDR（如 0.05）为显著。

---

## 2. 公式及原理

   设对 $m$ 个基因检测得到原始 p 值集合 $\{p_1,\dots,p_m\}$。

   1. **排序**
      将 p 值按升序排列，记排序后第 $i$ 小的 p 值为 $p_{(i)}$。

   2. **Benjamini–Hochberg 校正**
      对每个排序后位置 $i$，计算

      $$
        q_{(i)} = \min\!\Biggl(1,\;\frac{m}{i}\,p_{(i)}\Biggr).
      $$

   3. **单调性修正**
      为保证调整后 p 值随着 $i$ 增加不降低，反向遍历：

      $$
        \tilde{q}_{(i)} = \min\!\bigl(q_{(i)},\,\tilde{q}_{(i+1)}\bigr),
        \quad \tilde{q}_{(m)} = q_{(m)}.
      $$

   4. **映射回原序**
      将 $\tilde{q}_{(i)}$ 对应回每个基因的原始索引，得到每个基因的 FDR 调整后 p 值。

   * **假设条件**
     Benjamini–Hochberg 方法在检验独立或正相关时能严格控制 FDR；若依赖结构未知，可视为保守估计。

---

## 3. 伪代码

```text
# 输入
#   P: 原始 p 值列表，长度 m
# 输出
#   Q: 调整后（FDR 控制）p 值列表，长度 m

function adjust_fdr(P):
    m ← length(P)
    # 1) 排序并记录原始索引
    order ← argsort(P)            # 返回排序后索引列表，p[order[0]] 最小
    P_sorted ← P[order]

    # 2) 初步校正 q 值
    Q_sorted ← array_of_length(m)
    for i in 1..m:
        Q_sorted[i] ← min(1, (m / i) * P_sorted[i])

    # 3) 单调性修正（反向遍历）
    for i in m−1 down to 1:
        Q_sorted[i] ← min(Q_sorted[i], Q_sorted[i+1])

    # 4) 映射回原序
    Q ← array_of_length(m)
    for k in 1..m:
        original_idx ← order[k]
        Q[original_idx] ← Q_sorted[k]

    return Q
```

* **时间复杂度**：主要在排序 $O(m\log m)$
* **注意事项**：

  * 若要更严格控制（依赖结构复杂），可使用 Benjamini–Yekutieli 校正，调整 $\frac{m}{i}$ 部分；
  * 输出 Q 中，通常以 Q$\le$0.05 判定差异表达基因；
  * 可配合其它多重检验方法（e.g. Bonferroni）做补充对比，或结合先验信息做基因集富集分析。
