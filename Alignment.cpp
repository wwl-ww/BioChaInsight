// AlignmentAlgorithm.cpp
#include "stdafx.h"
#include "Alignment.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>

AlignmentAlgorithm::AlignmentAlgorithm(
	const string& seq1,
	const string& seq2,
	int match_score,
	int mismatch_score,
	int gap_open,
	int gap_extend
)
	: seq1_(seq1)
	, seq2_(seq2)
	, match_score_(match_score)
	, mismatch_score_(mismatch_score)
	, gap_open_(gap_open)
	, gap_extend_(gap_extend)
	, m_(static_cast<int>(seq1.size()))
	, n_(static_cast<int>(seq2.size()))
	, M_(m_ + 1, vector<int>(n_ + 1, 0))
{
}

AlignmentAlgorithm::~AlignmentAlgorithm() {}

void AlignmentAlgorithm::align() {
	validateInputs();
	initMatrix();
	computeMatrix();
	traceback();
	buildStates();
}

void AlignmentAlgorithm::printColoredAlignment() const {
	const string& s1 = aligned_seq1_;
	const string& s2 = aligned_seq2_;
	size_t L = s1.size();
	if (s2.size() != L) return;  // 安全检查

	// ANSI 转义码
	const char* RESET = "\033[0m";
	const char* FG_GREEN = "\033[32m";
	const char* FG_RED = "\033[31m";
	const char* BG_GRAY = "\033[100m";

	// 打印 第一条序列
	cout << "Seq1: ";
	for (size_t i = 0; i < L; i++) {
		char c = s1[i];
		switch (seq1_state_[i]) {
		case MATCH:
			cout << FG_GREEN << c << RESET;
			break;
		case FAIL:
			cout << FG_RED << c << RESET;
			break;
		case GAP:
			cout << BG_GRAY << c << RESET;
			break;
		}
	}
	cout << "\n";

	// 打印 第二条序列
	cout << "Seq2: ";
	for (size_t i = 0; i < L; i++) {
		char c = s2[i];
		switch (seq2_state_[i]) {
		case MATCH:
			cout << FG_GREEN << c << RESET;
			break;
		case FAIL:
			cout << FG_RED << c << RESET;
			break;
		case GAP:
			cout << BG_GRAY << c << RESET;
			break;
		}
	}
	cout << endl;
}

vector<pair<int, int>> AlignmentAlgorithm::getAlignmentPath() const {
	vector<pair<int, int>> path;
	int i = 0, j = 0;
	size_t L = aligned_seq1_.size();
	path.reserve(L + 1);
	path.emplace_back(i, j);

	for (size_t k = 0; k < L; ++k) {
		char c1 = aligned_seq1_[k];
		char c2 = aligned_seq2_[k];
		if (c1 != '-' && c2 != '-') {
			// 对角前进一步
			i++;
			j++;
		}
		else if (c1 != '-' && c2 == '-') {
			// seq2 插入空位 seq1 前进一步
			i++;
		}
		else if (c1 == '-' && c2 != '-') {
			// seq1 插入空位 seq2 前进一步
			j++;
		}
		else {
			// 同时都是 '-' 不应该出现，但若出现我们就都不动
		}
		path.emplace_back(i, j);
	}
	return path;
}

vector<vector<double>> AlignmentAlgorithm::getHighlightedMatrix(double highlight) const {
	const auto& matInt = getMatrix();
	size_t rows = matInt.size();
	size_t cols = rows ? matInt[0].size() : 0;

	vector<vector<double>> matD(rows, vector<double>(cols, 0.0));
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			matD[i][j] = static_cast<double>(matInt[i][j]);
		}
	}

	auto path = getAlignmentPath();
	for (auto& p : path) {
		int i = p.first;
		int j = p.second;
		if (i >= 0 && i < static_cast<int>(rows) &&
			j >= 0 && j < static_cast<int>(cols)) {
			matD[i][j] += highlight;
		}
	}

	return matD;
}

const string& AlignmentAlgorithm::getAlignedSeq1() const {
	return aligned_seq1_;
}

const string& AlignmentAlgorithm::getAlignedSeq2() const {
	return aligned_seq2_;
}

const vector<int>& AlignmentAlgorithm::getSeq1State() const {
	return seq1_state_;
}

const vector<int>& AlignmentAlgorithm::getSeq2State() const {
	return seq2_state_;
}

const vector<vector<int>>& AlignmentAlgorithm::getMatrix() const {
	return M_;
}

void AlignmentAlgorithm::validateInputs() const {
	if (m_ == 0 || n_ == 0) {
		throw runtime_error("输入序列长度不能为空");
	}
}

void AlignmentAlgorithm::buildStates() {
	size_t L = aligned_seq1_.size();
	seq1_state_.assign(L, FAIL);
	seq2_state_.assign(L, FAIL);
	for (size_t i = 0; i < L; i++) {
		char c1 = aligned_seq1_[i], c2 = aligned_seq2_[i];
		// seq1 状态
		if (c1 == '-')
			seq1_state_[i] = GAP;
		else if (c1 == c2)
			seq1_state_[i] = MATCH;
		else
			seq1_state_[i] = FAIL;
		// seq2 状态
		if (c2 == '-')
			seq2_state_[i] = GAP;
		else if (c2 == c1)
			seq2_state_[i] = MATCH;
		else
			seq2_state_[i] = FAIL;
	}
}

void NeedlemanWunsch::initMatrix() {
	for (int i = 1; i <= m_; i++) M_[i][0] = i * gap_open_;
	for (int j = 1; j <= n_; j++) M_[0][j] = j * gap_open_;
}

void NeedlemanWunsch::computeMatrix() {
	for (int i = 1; i <= m_; i++) {
		for (int j = 1; j <= n_; j++) {
			int sc = (seq1_[i - 1] == seq2_[j - 1] ? match_score_ : mismatch_score_);
			int diag = M_[i - 1][j - 1] + sc;
			int up = M_[i - 1][j] + gap_open_;
			int left = M_[i][j - 1] + gap_open_;
			M_[i][j] = max(max(diag, up), left);
		}
	}
}

void NeedlemanWunsch::traceback() {
	aligned_seq1_.clear();
	aligned_seq2_.clear();
	int i = m_, j = n_;
	while (i > 0 && j > 0) {
		int sc = (seq1_[i - 1] == seq2_[j - 1] ? match_score_ : mismatch_score_);
		if (M_[i][j] == M_[i - 1][j - 1] + sc) {
			aligned_seq1_ = seq1_[i - 1] + aligned_seq1_;
			aligned_seq2_ = seq2_[j - 1] + aligned_seq2_;
			--i; --j;
		}
		else if (M_[i][j] == M_[i - 1][j] + gap_open_) {
			aligned_seq1_ = seq1_[i - 1] + aligned_seq1_;
			aligned_seq2_ = '-' + aligned_seq2_;
			--i;
		}
		else {
			aligned_seq1_ = '-' + aligned_seq1_;
			aligned_seq2_ = seq2_[j - 1] + aligned_seq2_;
			--j;
		}
	}
	while (i > 0) {
		aligned_seq1_ = seq1_[i - 1] + aligned_seq1_;
		aligned_seq2_ = '-' + aligned_seq2_;
		--i;
	}
	while (j > 0) {
		aligned_seq1_ = '-' + aligned_seq1_;
		aligned_seq2_ = seq2_[j - 1] + aligned_seq2_;
		--j;
	}
}

void SmithWaterman::initMatrix() {
	// 首行首列设为0
	for (int i = 0; i <= m_; i++) M_[i][0] = 0;
	for (int j = 0; j <= n_; j++) M_[0][j] = 0;
}

void SmithWaterman::computeMatrix() {
	for (int i = 1; i <= m_; i++) {
		for (int j = 1; j <= n_; j++) {
			int sc = (seq1_[i - 1] == seq2_[j - 1] ? match_score_ : mismatch_score_);
			int diag = M_[i - 1][j - 1] + sc;
			int up = M_[i - 1][j] + gap_open_;
			int left = M_[i][j - 1] + gap_open_;
			M_[i][j] = max(0, max(max(diag, up), left));
		}
	}
}

void SmithWaterman::traceback() {
	// 找到最大值位置
	int best_i = 0, best_j = 0, best = 0;
	for (int i = 1; i <= m_; i++)
		for (int j = 1; j <= n_; j++)
			if (M_[i][j] > best) {
				best = M_[i][j];
				best_i = i; best_j = j;
			}
	aligned_seq1_.clear();
	aligned_seq2_.clear();
	int i = best_i, j = best_j;
	while (i > 0 && j > 0 && M_[i][j] > 0) {
		int sc = (seq1_[i - 1] == seq2_[j - 1] ? match_score_ : mismatch_score_);
		if (M_[i][j] == M_[i - 1][j - 1] + sc) {
			aligned_seq1_ = seq1_[i - 1] + aligned_seq1_;
			aligned_seq2_ = seq2_[j - 1] + aligned_seq2_;
			--i; --j;
		}
		else if (M_[i][j] == M_[i - 1][j] + gap_open_) {
			aligned_seq1_ = seq1_[i - 1] + aligned_seq1_;
			aligned_seq2_ = '-' + aligned_seq2_;
			--i;
		}
		else {
			aligned_seq1_ = '-' + aligned_seq1_;
			aligned_seq2_ = seq2_[j - 1] + aligned_seq2_;
			--j;
		}
	}
}

void Gotoh::initMatrix() {
	// 分配额外矩阵
	Ix_.assign(m_ + 1, vector<int>(n_ + 1, numeric_limits<int>::min() / 2));
	Iy_.assign(m_ + 1, vector<int>(n_ + 1, numeric_limits<int>::min() / 2));
	// 起点
	M_[0][0] = 0;
	Ix_[0][0] = Iy_[0][0] = numeric_limits<int>::min() / 2;
	// i=0 行
	for (int j = 1; j <= n_; j++) {
		Iy_[0][j] = gap_open_ + (j - 1) * gap_extend_;
		M_[0][j] = Iy_[0][j];
	}
	// j=0 列
	for (int i = 1; i <= m_; i++) {
		Ix_[i][0] = gap_open_ + (i - 1) * gap_extend_;
		M_[i][0] = Ix_[i][0];
	}
}

void Gotoh::computeMatrix() {
	for (int i = 1; i <= m_; i++) {
		for (int j = 1; j <= n_; j++) {
			// 计算插入/删除（仿射）
			Ix_[i][j] = max(
				M_[i][j - 1] + gap_open_,
				Ix_[i][j - 1] + gap_extend_
			);
			Iy_[i][j] = max(
				M_[i - 1][j] + gap_open_,
				Iy_[i - 1][j] + gap_extend_
			);
			// 计算匹配/不匹配
			int sc = (seq1_[i - 1] == seq2_[j - 1] ? match_score_ : mismatch_score_);
			int mm = max(
				max(M_[i - 1][j - 1], Ix_[i - 1][j - 1]),
				Iy_[i - 1][j - 1]
			) + sc;
			M_[i][j] = mm;
		}
	}
}

void Gotoh::traceback() {
	aligned_seq1_.clear();
	aligned_seq2_.clear();
	// 先找终点：三矩阵中分数最高的
	int i = m_, j = n_;
	int scoreM = M_[i][j],
		scoreX = Ix_[i][j],
		scoreY = Iy_[i][j];
	enum { IN_M, IN_X, IN_Y } state;
	if (scoreX > scoreM && scoreX > scoreY) state = IN_X;
	else if (scoreY > scoreM && scoreY > scoreX) state = IN_Y;
	else state = IN_M;

	while (i > 0 || j > 0) {
		if (state == IN_M) {
			int sc = (i > 0 && j > 0 && seq1_[i - 1] == seq2_[j - 1]) ? match_score_ : mismatch_score_;
			// 来自对角
			if (i > 0 && j > 0 && M_[i][j] == M_[i - 1][j - 1] + sc) {
				aligned_seq1_ = seq1_[i - 1] + aligned_seq1_;
				aligned_seq2_ = seq2_[j - 1] + aligned_seq2_;
				--i; --j;
				state = IN_M;
			}
			// 来自 X 矩阵
			else if (i > 0 && j > 0 && M_[i][j] == Ix_[i - 1][j - 1] + sc) {
				aligned_seq1_ = seq1_[i - 1] + aligned_seq1_;
				aligned_seq2_ = seq2_[j - 1] + aligned_seq2_;
				--i; --j;
				state = IN_X;
			}
			// 来自 Y 矩阵
			else if (i > 0 && j > 0 && M_[i][j] == Iy_[i - 1][j - 1] + sc) {
				aligned_seq1_ = seq1_[i - 1] + aligned_seq1_;
				aligned_seq2_ = seq2_[j - 1] + aligned_seq2_;
				--i; --j;
				state = IN_Y;
			}
		}
		else if (state == IN_X) {
			// X 矩阵产生的是 seq1 插空
			if (j > 0) {
				aligned_seq1_ = '-' + aligned_seq1_;
				aligned_seq2_ = seq2_[j - 1] + aligned_seq2_;
				// 决定下个状态
				if (Ix_[i][j] == M_[i][j - 1] + gap_open_) state = IN_M;
				else state = IN_X;
				--j;
			}
			else {
				// 退回 M
				state = IN_M;
			}
		}
		else { // IN_Y
			// Y 矩阵产生的是 seq2 插空 (gap in seq2)
			if (i > 0) {
				aligned_seq1_ = seq1_[i - 1] + aligned_seq1_;
				aligned_seq2_ = '-' + aligned_seq2_;
				if (Iy_[i][j] == M_[i - 1][j] + gap_open_) state = IN_M;
				else state = IN_Y;
				--i;
			}
			else {
				state = IN_M;
			}
		}
	}
}


static vector<int> nwScoreRow(
	const string& A,
	const string& B,
	int match_score,
	int mismatch_score,
	int gap_open
) {
	// 计算标准 NW 的最后一行，用于 Hirschberg 切分
	int n = static_cast<int>(B.size());
	vector<int> prev(n + 1, 0), curr(n + 1, 0);
	for (int j = 1; j <= n; j++) prev[j] = prev[j - 1] + gap_open;
	for (size_t i = 1; i <= A.size(); i++) {
		curr[0] = prev[0] + gap_open;
		for (int j = 1; j <= n; j++) {
			int sc = (A[i - 1] == B[j - 1] ? match_score : mismatch_score);
			int diag = prev[j - 1] + sc;
			int up = prev[j] + gap_open;
			int left = curr[j - 1] + gap_open;
			curr[j] = max(max(diag, up), left);
		}
		prev.swap(curr);
	}
	return prev;
}

// 递归 Hirschberg 对齐
static void hirschbergRec(
	const string& A,
	const string& B,
	int match_score,
	int mismatch_score,
	int gap_open,
	string& Aalign,
	string& Balign
) {
	int m = static_cast<int>(A.size()), n = static_cast<int>(B.size());
	if (m == 0) {
		Aalign.assign(n, '-');
		Balign = B;
		return;
	}
	if (n == 0) {
		Aalign = A;
		Balign.assign(m, '-');
		return;
	}
	if (m == 1 || n == 1) {
		// 小尺寸时退化为 Needleman-Wunsch
		NeedlemanWunsch nw(A, B, match_score, mismatch_score, gap_open);
		nw.align();
		Aalign = nw.getAlignedSeq1();
		Balign = nw.getAlignedSeq2();
		return;
	}
	int mid = m / 2;
	// 左半段
	vector<int> scoreL = nwScoreRow(
		A.substr(0, mid), B, match_score, mismatch_score, gap_open
	);
	// 右半段（反序计算）
	string Ar = A.substr(mid);
	reverse(Ar.begin(), Ar.end());
	string Br = B;
	reverse(Br.begin(), Br.end());
	vector<int> scoreR = nwScoreRow(
		Ar, Br, match_score, mismatch_score, gap_open
	);
	// 找到最优切分点 k
	int kBest = 0, best = numeric_limits<int>::min();
	for (int j = 0; j <= n; j++) {
		int val = scoreL[j] + scoreR[n - j];
		if (val > best) {
			best = val;
			kBest = j;
		}
	}
	// 递归对齐左右两段
	string A1, B1, A2, B2;
	hirschbergRec(A.substr(0, mid), B.substr(0, kBest),
		match_score, mismatch_score, gap_open,
		A1, B1);
	hirschbergRec(A.substr(mid), B.substr(kBest),
		match_score, mismatch_score, gap_open,
		A2, B2);
	Aalign = A1 + A2;
	Balign = B1 + B2;
}

void Hirschberg::initMatrix() {
	for (int i = 1; i <= m_; i++) M_[i][0] = i * gap_open_;
	for (int j = 1; j <= n_; j++) M_[0][j] = j * gap_open_;
	for (int i = 1; i <= m_; i++) {
		for (int j = 1; j <= n_; j++) {
			int sc = (seq1_[i - 1] == seq2_[j - 1] ? match_score_ : mismatch_score_);
			int diag = M_[i - 1][j - 1] + sc;
			int up = M_[i - 1][j] + gap_open_;
			int left = M_[i][j - 1] + gap_open_;
			M_[i][j] = max(max(diag, up), left);
		}
	}
}
void Hirschberg::computeMatrix() {
	// 不做额外操作，全在 initMatrix 中完成
}
void Hirschberg::traceback() {
	aligned_seq1_.clear();
	aligned_seq2_.clear();
	hirschbergRec(seq1_, seq2_,
		match_score_, mismatch_score_, gap_open_,
		aligned_seq1_, aligned_seq2_);
}
