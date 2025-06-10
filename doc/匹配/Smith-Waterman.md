# Smith-Waterman
[TOC]

## 1. 算法介绍

   * **背景与目标**
     Smith–Waterman 算法由 Temple F. Smith 和 Michael S. Waterman 于1981年提出，是用于生物序列（如蛋白质或 DNA）的**局部比对**（local alignment）经典动态规划方法。其核心目标是：

     > 在允许插入、缺失（gap）和错配的情况下，找到两条序列中得分最高的局部片段比对。

   * **应用场景**

     * 在长序列中发现高度相似的局部区域（例如功能域、保守motif）
     * 数据库搜索时比对查询序列与目标序列的最佳匹配子串
     * 模式识别与结构功能预测中局部相似性分析

   * **核心思路**

     1. **矩阵初始化**——大小为 $(m+1)\times(n+1)$ 的得分矩阵 $H$，首行和首列均置 0；
     2. **递推填表**——对每个位置 $(i,j)$ 计算：

        * 对齐 $a_i$ 与 $b_j$（match/mismatch）
        * 在 $A$ 中插入 gap
        * 在 $B$ 中插入 gap
        * 或者不延续比对（得分置 0，终止局部比对）
     3. **寻找最优**——记录矩阵中的最大值及其坐标 $(i^*,j^*)$；
     4. **回溯**——从 $(i^*,j^*)$ 开始，根据递推来源回溯，直到遇到得分 0，获得局部最优对齐片段。

---

## 2. 公式及原理

   **2.1 符号与评分**

   * 序列 $\mathbf{A}=a_1a_2\cdots a_m$，$\mathbf{B}=b_1b_2\cdots b_n$。
   * 匹配／错配得分函数 $s(a_i,b_j)$，常见如 BLOSUM 或简单的：

     $$
s(a_i,b_j)=
       \begin{cases}
         +\alpha, & a_i=b_j,\\
         -\beta,  & a_i\neq b_j.
       \end{cases}
$$
   * 线性 gap penalty $d>0$。

   **2.2 初始化**

   $$
H[i,0]=0,\quad H[0,j]=0,\quad \forall 0\le i\le m, 0\le j\le n.
$$

   **2.3 递推公式**
   对任意 $1\le i\le m$, $1\le j\le n$：

   $$
H[i,j]
     = \max \begin{cases}
       0,\\
       H[i-1,j-1] + s(a_i,b_j),\\
       H[i-1,j]   - d,\\
       H[i,j-1]   - d.
     \end{cases}
$$

   * 其中，$\max\{0,\dots\}$ 保证局部比对在得分为负时重置为 0，从而支持局部对齐。
   * 最大值 $\max_{i,j}H[i,j]$ 对应最优局部对齐结束位置。

   **2.4 回溯（Traceback）**
   从得分最高的 $(i^*,j^*)$ 出发，按下列优先顺序回溯：

   1. 如果 $H[i^*,j^*] = H[i^*-1,j^*-1] + s(a_{i^*},b_{j^*})$，对齐 $a_{i^*}$ 与 $b_{j^*}$；
   2. 否则如果 $H[i^*,j^*] = H[i^*-1,j^*] - d$，对齐 $a_{i^*}$ 与 gap；
   3. 否则对齐 gap 与 $b_{j^*}$；
      重复直到遇到 $H[i,j]=0$，此点即局部比对起点。

---

## 3. 伪代码

```text
# 输入
#   A[1..m], B[1..n]: 待比对序列
#   s(a,b): 匹配得分函数
#   d: 线性 gap penalty
# 输出
#   aligned_A, aligned_B: 局部比对结果

function SmithWaterman(A, B, s, d):
    m ← length(A); n ← length(B)

    # 1) 初始化矩阵 H (m+1)x(n+1)，并记录最大得分位置
    for i in 0..m:
        H[i,0] ← 0
    for j in 0..n:
        H[0,j] ← 0
    max_score ← 0
    (end_i, end_j) ← (0, 0)

    # 2) 填表并追踪最大值
    for i in 1..m:
        for j in 1..n:
            match ← H[i-1,j-1] + s(A[i], B[j])
            delete ← H[i-1,j]   - d
            insert ← H[i,j-1]   - d
            H[i,j] ← max(0, match, delete, insert)
            if H[i,j] > max_score:
                max_score ← H[i,j]
                (end_i, end_j) ← (i, j)

    # 3) 回溯还原局部比对
    aligned_A, aligned_B ← empty strings
    i, j ← end_i, end_j
    while i>0 and j>0 and H[i,j] > 0:
        if H[i,j] == H[i-1,j-1] + s(A[i], B[j]):
            aligned_A.prepend(A[i])
            aligned_B.prepend(B[j])
            i ← i-1; j ← j-1
        else if H[i,j] == H[i-1,j] - d:
            aligned_A.prepend(A[i])
            aligned_B.prepend('-')
            i ← i-1
        else:
            aligned_A.prepend('-')
            aligned_B.prepend(B[j])
            j ← j-1

    return aligned_A, aligned_B
```

* **时间复杂度**：$O(m \times n)$
* **空间复杂度**：$O(m \times n)$（可用带回溯链的优化或分块策略略减内存）
