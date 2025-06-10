# Needleman-Wunsch
[TOC]

## 1. 算法介绍

   * **背景与目标**
     Needleman–Wunsch 算法由 Saul B. Needleman 和 Christian D. Wunsch 于1970年提出，是用于生物序列（如蛋白质或 DNA）**全局比对**（global alignment）的经典动态规划方法。其核心目标是：

     > 在允许插入、缺失（gap）和错配的情况下，找到两条序列从头到尾的最优比对，使得总体得分最大。

   * **应用场景**

     * 两条全长蛋白质序列或 DNA 序列的全局比对
     * 构建进化距离矩阵、聚类与系统发生学分析
     * 作为后续更复杂比对（如多序列比对、局部比对）的基础

   * **核心思路**

     1. 构建一个大小为 $(m+1)\times(n+1)$ 的积分得分矩阵 $F$，其中 $m,n$ 分别为两序列长度；
     2. 以线性或 affine gap penalty 设定缺口代价；
     3. 通过动态规划递推填表，计算从起点到任意 $(i,j)$ 的最优比对得分；
     4. 从右下角回溯（traceback），还原最佳全局比对路径。

---

## 2. 公式及原理

   **2.1 符号与评分函数**

   * 序列 $\mathbf{A}=a_1a_2\cdots a_m$，$\mathbf{B}=b_1b_2\cdots b_n$。
   * 设定匹配／错配得分函数：

     $$
s(a_i,b_j) =
       \begin{cases}
         +\alpha, & a_i = b_j \quad(match)\\
         -\beta,  & a_i \neq b_j \quad(mismatch)
       \end{cases}
$$
   * 线性缺口惩罚：对于连续插入或删除长度为 $k$，惩罚为 $-k\cdot d$。

   **2.2 初始化**

   $$
F[0,0] = 0,\quad
     F[i,0] = -i\cdot d\quad (i=1,\dots,m),\quad
     F[0,j] = -j\cdot d\quad (j=1,\dots,n).
$$

   **2.3 递推公式**
   对任意 $1\le i\le m$, $1\le j\le n$，

   $$
F[i,j] = \max\begin{cases}
       F[i-1, j-1] + s(a_i,b_j),\\
       F[i-1, j] - d,\\
       F[i, j-1] - d.
     \end{cases}
$$

   **2.4 回溯（Traceback）**
   从 $(i,j)=(m,n)$ 开始：

   * 如果 $F[i,j] = F[i-1,j-1] + s(a_i,b_j)$，则对齐 $a_i$ 与 $b_j$，移动 $(i,j)\to(i-1,j-1)$；
   * 否则若 $F[i,j] = F[i-1,j] - d$，则对齐 $a_i$ 与 gap，移动 $(i,j)\to(i-1,j)$；
   * 否则对齐 gap 与 $b_j$，移动 $(i,j)\to(i,j-1)$。
     直到回到 $(0,0)$。

---

## 3. 伪代码

```text
# 输入
#   A[1..m], B[1..n]: 待比对序列
#   s(a,b): 匹配得分函数
#   d: 线性 gap penalty
# 输出
#   aligned_A, aligned_B: 两个同长的对齐序列

function NeedlemanWunsch(A, B, s, d):
    m ← length(A); n ← length(B)

    # 1) 初始化矩阵 F 大小 (m+1)x(n+1)
    for i in 0..m:
        F[i,0] ← -i * d
    for j in 0..n:
        F[0,j] ← -j * d

    # 2) 填表
    for i in 1..m:
        for j in 1..n:
            match ← F[i-1,j-1] + s(A[i], B[j])
            delete ← F[i-1,j]   - d
            insert ← F[i,  j-1] - d
            F[i,j] ← max(match, delete, insert)

    # 3) 回溯还原比对
    i ← m; j ← n
    aligned_A, aligned_B ← empty strings
    while i>0 or j>0:
        if i>0 and j>0 and F[i,j] == F[i-1,j-1] + s(A[i],B[j]):
            aligned_A.prepend(A[i])
            aligned_B.prepend(B[j])
            i ← i-1; j ← j-1
        else if i>0 and F[i,j] == F[i-1,j] - d:
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
* **空间复杂度**：$O(m \times n)$（可用带回溯链的 Hirschberg 算法降到 $O(m+n)$）
