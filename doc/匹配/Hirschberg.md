# Hirschberg
[TOC]

## 1. 算法介绍

   * **背景与目标**
     Hirschberg 算法由 Dan Hirschberg 于1975年提出，是对 Needleman–Wunsch 全局比对的内存优化，通过分治策略将空间复杂度从 $O(mn)$ 降到 $O(\min(m,n))$，但仍保持 $O(mn)$ 的时间复杂度。

     > 核心目标：在内存受限的情况下，仍能获得两条序列的最优全局比对结果。

   * **应用场景**

     * 在大规模或内存受限环境（如嵌入式设备）中做序列全局比对
     * 对长序列（数万甚至数十万长度）进行比对而不爆内存
     * 作为组合更复杂比对算法的空间优化子例程

   * **核心思路**

     1. **线性空间动态规划**——用两行滚动数组计算 Needleman–Wunsch 得分向量；
     2. **分治分割**——将序列 $A$ 在中点处分成左右两段，分别对左半段与 $B$ 做“正向” DP，对右半段与 $B$ 做“反向” DP；
     3. **寻找最优分割点**——在 $B$ 上找到使前后两段得分和最大的分割索引；
     4. **递归对齐**——分治地对两对子序列递归调用 Hirschberg，还原最终全局对齐。

---

## 2. 公式及原理

   **2.1 线性空间 DP**
   对长度为 $m$ 的序列 $A$ 和长度为 $n$ 的序列 $B$，标准 NW 填表需要维护 $(m+1)\times(n+1)$ 矩阵。Hirschberg 只保留当前行和上一行：

   $$
score_{curr}[j]
     = \max \begin{cases}
       score_{prev}[j-1] + s(a_i, b_j),\\
       score_{prev}[j]   - d,\\
       score_{curr}[j-1] - d.
     \end{cases}
$$

   这样每次只用 $O(n)$ 空间计算出一行得分向量。

   **2.2 分治分割**
   将 $A$ 分为两半 $A_{\text{L}}=A[1..\lfloor m/2\rfloor]$ 和 $A_{\text{R}}=A[\lfloor m/2\rfloor+1..m]$。

   * **正向得分**：对 $A_{\text{L}}$ 和 $B$ 用线性空间 DP，得到得分向量 $F[j]$（代表对齐到 $B[1..j]$ 的最优得分）。
   * **反向得分**：对反转后的 $A_{\text{R}}$ 和反转后的 $B$ 做同样 DP，得到向量 $G[j]$（对应对齐到尾部 $B[j+1..n]$）。
   * **合并**：在所有划分点 $j\in[0..n]$ 上计算 $F[j]+G[n-j]$，取最大者 $j^*$ 作为最佳分割。

   **2.3 递归重构**
   按分割索引将问题分为两对子序列 $(A_{\text{L}}, B[1..j^*])$ 和 $(A_{\text{R}}, B[j^*+1..n])$，递归调用 Hirschberg；当子问题中某条序列长度为 0 或 1 时，退化回标准 Needleman–Wunsch 或简单对齐，完成回溯。

---

## 3. 伪代码

```text
# 输入
#   A[1..m], B[1..n]
#   s(a,b): 匹配得分函数
#   d: 线性 gap penalty
# 输出
#   aligned_A, aligned_B

function Hirschberg(A, B):
    m ← length(A); n ← length(B)

    # 基本情况：若一条序列长度 ≤ 1，用标准 NW 直接对齐
    if m == 0:
        return repeat('-', n), B
    if n == 0:
        return A, repeat('-', m)
    if m == 1 or n == 1:
        return NeedlemanWunsch(A, B, s, d)

    # 1) 分割 A
    mid ← ⌊m/2⌋
    A_L ← A[1..mid];   A_R ← A[mid+1..m]

    # 2) 正向 DP：A_L vs B → F[0..n]
    F ← NW_ScoreVector(A_L, B, s, d)

    # 3) 反向 DP：reverse(A_R) vs reverse(B) → G[0..n]
    G_rev ← NW_ScoreVector(reverse(A_R), reverse(B), s, d)
    G ← reverse(G_rev)

    # 4) 找到最佳分割点 j*
    best_j, _ ← argmax_{j=0..n} (F[j] + G[j])

    # 5) 递归对齐左右两段
    (A1, B1) ← Hirschberg(A_L,   B[1..best_j])
    (A2, B2) ← Hirschberg(A_R,   B[best_j+1..n])

    return (A1 ∥ A2), (B1 ∥ B2)

# 辅助：计算 DP 得分向量（只返回最后一行）
function NW_ScoreVector(X, Y, s, d):
    n ← length(Y)
    prev[0..n] ← [0, -d, -2d, …, -n·d]
    for i in 1..length(X):
        curr[0] ← -i·d
        for j in 1..n:
            match  ← prev[j-1] + s(X[i], Y[j])
            delete ← prev[j]   - d
            insert ← curr[j-1] - d
            curr[j] ← max(match, delete, insert)
        prev ← curr
    return prev
```

* **时间复杂度**：$O(m \times n)$
* **空间复杂度**：$O(n)$（额外递归栈 $O(\log m)$，总 $O(n + \log m)$）
