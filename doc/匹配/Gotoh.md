# Gotoh
[TOC]

## 1. 算法介绍

   * **背景与目标**
     Gotoh 算法由 O. Gotoh 于1982年提出，是 Needleman–Wunsch 全局比对算法的一个改进，支持 **仿射缺口惩罚**（affine gap penalty）：

     $$
gap\_score(k) = -\alpha - (k-1) \beta,
$$

     其中 $\alpha>0$ 是缺口打开惩罚（gap opening），$\beta>0$ 是缺口延伸惩罚（gap extension）。

     > 核心目标：在允许对开新缺口和延续已有缺口的不同惩罚权重下，找到两条序列的最优全局比对。

   * **应用场景**

     * 需要对插入/删除连续区域区分开惩罚力度的序列比对
     * 蛋白质结构域比对、进化分析、模板识别等

   * **核心思路**

     1. 用三个矩阵分别处理“对齐”“在 A 中插入 gap”“在 B 中插入 gap”三种状态；
     2. 通过递推公式同时维护这三种状态下的最优得分；
     3. 最后取三个矩阵在 $(m,n)$ 处的最大值，并回溯还原比对。

---

## 2. 公式及原理
   令序列
   $\mathbf{A}=a_1\cdots a_m,\quad \mathbf{B}=b_1\cdots b_n$，匹配得分函数 $s(a_i,b_j)$，仿射缺口参数 $\alpha$（打开）和 $\beta$（延伸）。

   **2.1 定义三个矩阵**

   * $H[i,j]$：以 $a_i$ 对齐 $b_j$ 为结尾的最优得分
   * $E[i,j]$：以在 B（列方向）插入 gap 为结尾的最优得分
   * $F[i,j]$：以在 A（行方向）插入 gap 为结尾的最优得分

   **2.2 递推公式**

   $$
\begin{aligned}
     E[i,j] &= \max\bigl\{ E[i, j-1]-\beta, H[i, j-1] -(\alpha+\beta)\bigr\},\\
     F[i,j] &= \max\bigl\{ F[i-1, j]-\beta, H[i-1, j] -(\alpha+\beta)\bigr\},\\
     H[i,j] &= \max\bigl\{ H[i-1, j-1] + s(a_i,b_j), E[i,j], F[i,j]\bigr\}.
   \end{aligned}
$$

   **2.3 初始化**

   $$
\begin{aligned}
     H[0,0] &= 0, & E[0,0] = F[0,0] &= -INF,\\
     \forall j\ge1: &
       H[0,j] = -{\alpha+(j-1)\beta},\quad
       E[0,j] = -{\alpha+(j-1)\beta},\quad
       F[0,j] = -INF,\\
     \forall i\ge1: &
       H[i,0] = -{\alpha+(i-1)\beta},\quad
       F[i,0] = -{\alpha+(i-1)\beta},\quad
       E[i,0] = -INF.
   \end{aligned}
$$

   其中 “$-\infty$” 表示该状态不合法。

---

## 3. 伪代码

```text
# 输入
#   A[1..m], B[1..n]: 待比对序列
#   s(a,b): 匹配得分函数
#   α: 缺口打开惩罚
#   β: 缺口延伸惩罚
# 输出
#   aligned_A, aligned_B: 全局比对结果

function Gotoh(A, B, s, α, β):
    m ← length(A); n ← length(B)
    # 1) 初始化矩阵 H, E, F 大小 (m+1)x(n+1)
    for i in 0..m:
        for j in 0..n:
            H[i,j], E[i,j], F[i,j] ← -∞
    H[0,0] ← 0
    for j in 1..n:
        H[0,j] ← - (α + (j-1)*β)
        E[0,j] ← - (α + (j-1)*β)
    for i in 1..m:
        H[i,0] ← - (α + (i-1)*β)
        F[i,0] ← - (α + (i-1)*β)

    # 2) 递推填表
    for i in 1..m:
        for j in 1..n:
            E[i,j] ← max(E[i, j-1] - β,
                          H[i, j-1] - (α + β))
            F[i,j] ← max(F[i-1, j] - β,
                          H[i-1, j] - (α + β))
            H[i,j] ← max(H[i-1, j-1] + s(A[i], B[j]),
                         E[i,j],
                         F[i,j])

    # 3) 回溯还原对齐
    aligned_A, aligned_B ← empty strings
    i, j ← m, n
    # 从矩阵 H、E、F 的最大值状态出发
    state ← argmax_over({H[i,j], E[i,j], F[i,j]})
    while i>0 or j>0:
        if state == H:
            if H[i,j] == H[i-1,j-1] + s(A[i],B[j]):
                aligned_A.prepend(A[i]); aligned_B.prepend(B[j])
                i, j ← i-1, j-1
                state ← argmax_over({H[i,j], E[i,j], F[i,j]})
            else if H[i,j] == E[i,j]:
                state ← E
            else:
                state ← F
        else if state == E:
            aligned_A.prepend('-'); aligned_B.prepend(B[j])
            j ← j-1
            # E[i,j] 来源于 E (延伸) or H (新开)
            state ← (E[i,j] + β == E[i, j-1]) ? E : H
        else:  # state == F
            aligned_A.prepend(A[i]); aligned_B.prepend('-')
            i ← i-1
            state ← (F[i,j] + β == F[i-1, j]) ? F : H

    return aligned_A, aligned_B
```

* **时间复杂度**：$O(m\times n)$
* **空间复杂度**：$O(m\times n)$ （可用带回溯链的分块或带压缩状态的优化降至线性）
