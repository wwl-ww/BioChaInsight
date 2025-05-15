#pragma once
#ifndef ALIGNMENT_ALGORITHM_H
#define ALIGNMENT_ALGORITHM_H

#include <string>
#include <vector>
#include <iostream>
using namespace std;

/**
 * @class AlignmentAlgorithm
 * @brief ���бȶԻ��࣬ģ�巽��ģʽ
 */
class AlignmentAlgorithm {
public:
    /// �ȶ�״̬��FAIL=0, GAP=1, MATCH=2
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

    // ������� 
    void printColoredAlignment() const;
    vector<pair<int, int>> getAlignmentPath() const;
    vector<vector<double>> getHighlightedMatrix(double highlight = 100) const;

    const string& getAlignedSeq1() const;
    const string& getAlignedSeq2() const;
    const vector<int>& getSeq1State()   const;
    const vector<int>& getSeq2State()   const;
    const vector<vector<int>>& getMatrix() const;

protected:
    // У�������Ƿ�Ϸ�
    void validateInputs() const;
    // ��ʼ�� DP ����߽�
    virtual void initMatrix() = 0;
    // ��� DP ����
    virtual void computeMatrix() = 0;
    // ���ݹ�����������
    virtual void traceback() = 0;
    // ���� aligned_seq1_/aligned_seq2_ ����״̬����
    void buildStates();

    string seq1_, seq2_;
    int match_score_, mismatch_score_, gap_open_, gap_extend_;
    int m_, n_;  // �ֱ�Ϊ seq1_.length(), seq2_.length()
    vector<vector<int>> M_;    // �� DP ���� (m_+1)��(n_+1)
    string aligned_seq1_, aligned_seq2_;
    vector<int> seq1_state_, seq2_state_;
};

/** ȫ�ֱȶԣ�Needleman�CWunsch �㷨 */
class NeedlemanWunsch : public AlignmentAlgorithm {
public:
    using AlignmentAlgorithm::AlignmentAlgorithm;
protected:
    void initMatrix()    override;
    void computeMatrix() override;
    void traceback()     override;
};

/** �ֲ��ȶԣ�Smith�CWaterman �㷨 */
class SmithWaterman : public AlignmentAlgorithm {
public:
    using AlignmentAlgorithm::AlignmentAlgorithm;
protected:
    void initMatrix()    override;
    void computeMatrix() override;
    void traceback()     override;
};

/** ����ȱ�ڷ��֣�Gotoh �㷨 */
class Gotoh : public AlignmentAlgorithm {
public:
    using AlignmentAlgorithm::AlignmentAlgorithm;
protected:
    void initMatrix()    override;
    void computeMatrix() override;
    void traceback()     override;
private:
    vector<vector<int>> Ix_, Iy_;  // �ֱ��¼�� seq1/seq2 �ϵ� gap ��֧
};

/** ���Կռ䣺Hirschberg �㷨 */
class Hirschberg : public AlignmentAlgorithm {
public:
    using AlignmentAlgorithm::AlignmentAlgorithm;
protected:
    void initMatrix()    override;
    void computeMatrix() override;
    void traceback()     override;
};

#endif // ALIGNMENT_ALGORITHM_H
