#pragma once
#ifndef ALIGNMENT_ALGORITHM_H
#define ALIGNMENT_ALGORITHM_H

#include <string>
#include <vector>
#include <iostream>
using namespace std;

/**
 * @class AlignmentAlgorithm
 * @brief 序列比对基类，模板方法模式
 */
class AlignmentAlgorithm {
public:
    /// 比对状态：FAIL=0, GAP=1, MATCH=2
    enum State { FAIL = 0, GAP = 1, MATCH = 2 };

    AlignmentAlgorithm(
        const string& seq1,
        const string& seq2,
        int match_score = +1,
        int mismatch_score = -1,
        int gap_open = -2,
        int gap_extend = 0
    );

    virtual ~AlignmentAlgorithm();

    void align();

    // 结果访问 
    void printColoredAlignment() const;
    vector<pair<int, int>> getAlignmentPath() const;
    vector<vector<double>> getHighlightedMatrix(double highlight = 100) const;

    const string& getAlignedSeq1() const;
    const string& getAlignedSeq2() const;
    const vector<int>& getSeq1State()   const;
    const vector<int>& getSeq2State()   const;
    const vector<vector<int>>& getMatrix() const;

protected:
    // 校验输入是否合法
    void validateInputs() const;
    // 初始化 DP 矩阵边界
    virtual void initMatrix() = 0;
    // 填充 DP 矩阵
    virtual void computeMatrix() = 0;
    // 回溯构建对齐序列
    virtual void traceback() = 0;
    // 根据 aligned_seq1_/aligned_seq2_ 构建状态向量
    void buildStates();

    string seq1_, seq2_;
    int match_score_, mismatch_score_, gap_open_, gap_extend_;
    int m_, n_;  // 分别为 seq1_.length(), seq2_.length()
    vector<vector<int>> M_;    // 主 DP 矩阵 (m_+1)×(n_+1)
    string aligned_seq1_, aligned_seq2_;
    vector<int> seq1_state_, seq2_state_;
};

/** 全局比对：Needleman–Wunsch 算法 */
class NeedlemanWunsch : public AlignmentAlgorithm {
public:
    using AlignmentAlgorithm::AlignmentAlgorithm;
protected:
    void initMatrix()    override;
    void computeMatrix() override;
    void traceback()     override;
};

/** 局部比对：Smith–Waterman 算法 */
class SmithWaterman : public AlignmentAlgorithm {
public:
    using AlignmentAlgorithm::AlignmentAlgorithm;
protected:
    void initMatrix()    override;
    void computeMatrix() override;
    void traceback()     override;
};

/** 仿射缺口罚分：Gotoh 算法 */
class Gotoh : public AlignmentAlgorithm {
public:
    using AlignmentAlgorithm::AlignmentAlgorithm;
protected:
    void initMatrix()    override;
    void computeMatrix() override;
    void traceback()     override;
private:
    vector<vector<int>> Ix_, Iy_;  // 分别记录在 seq1/seq2 上的 gap 分支
};

/** 线性空间：Hirschberg 算法 */
class Hirschberg : public AlignmentAlgorithm {
public:
    using AlignmentAlgorithm::AlignmentAlgorithm;
protected:
    void initMatrix()    override;
    void computeMatrix() override;
    void traceback()     override;
};

#endif // ALIGNMENT_ALGORITHM_H
