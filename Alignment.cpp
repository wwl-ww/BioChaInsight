#include "Alignment.h"

int Alignment::match_score = 0;     // 匹配得分
int Alignment::mismatch_score = 0; // 不匹配得分
int Alignment::gap_score = 0;      // 空位得分

int Alignment::score(char a, char b)
{
	return (a == b) ? match_score : mismatch_score;
}

pair<string, string> Alignment::needlemanWunsch(const string& seq1, const string& seq2)
{
	// 根据算法设定的得分规则
    match_score = 1;
	mismatch_score = -1;
	gap_score = -2;

    int m = seq1.length();
    int n = seq2.length();

    vector<vector<int>> H(m + 1, vector<int>(n + 1, 0));

    // 初始化矩阵的第一行和第一列，它们本质上都是空位。因为只有一挑链在向后遍历，另一条链没有动
    for (int i = 1; i <= m; ++i)
        H[i][0] = i * gap_score;
    for (int j = 1; j <= n; ++j)
        H[0][j] = j * gap_score;

    // 填充矩阵
    for (int i = 1; i <= m; ++i)
    {
        for (int j = 1; j <= n; ++j) {
            int match = H[i - 1][j - 1] + score(seq1[i - 1], seq2[j - 1]); // 匹配或不匹配
            int deleteOp = H[i - 1][j] + gap_score; // 删除操作（在序列2中插入空位）
            int insertOp = H[i][j - 1] + gap_score; // 插入操作（在序列1中插入空位）
            H[i][j] = max({ match, deleteOp, insertOp }); // 选择最大值
        }
    }

    // 回溯找到最优比对路径
    string aligned_seq1, aligned_seq2;
    int i = m, j = n;
    while (i > 0 && j > 0)
    {
        int current_score = H[i][j];
        int score_diag = H[i - 1][j - 1];
        int score_up = H[i][j - 1];
        int score_left = H[i - 1][j];

        if (current_score == score_diag + score(seq1[i - 1], seq2[j - 1]))
        {
            // 对角线方向（匹配或不匹配）
            aligned_seq1 = seq1[i - 1] + aligned_seq1;
            aligned_seq2 = seq2[j - 1] + aligned_seq2;
            i--;
            j--;
        }
        else if (current_score == score_left + gap_score)
        {
            // 向左方向（在序列2中插入空位）
            aligned_seq1 = seq1[i - 1] + aligned_seq1;
            aligned_seq2 = '-' + aligned_seq2;
            i--;
        }
        else
        {
            // 向上方向（在序列1中插入空位）
            aligned_seq1 = '-' + aligned_seq1;
            aligned_seq2 = seq2[j - 1] + aligned_seq2;
            j--;
        }
    }

    // 处理剩余部分
    while (i > 0)
    {
        aligned_seq1 = seq1[i - 1] + aligned_seq1;
        aligned_seq2 = '-' + aligned_seq2;
        i--;
    }
    while (j > 0)
    {
        aligned_seq1 = '-' + aligned_seq1;
        aligned_seq2 = seq2[j - 1] + aligned_seq2;
        j--;
    }

    return { aligned_seq1, aligned_seq2 };
}

pair<string, string> Alignment::smithWaterman(const string& seq1, const string& seq2)
{
	// 根据算法设定的得分规则
	match_score = 1;
	mismatch_score = -1;
	gap_score = -2;

    int m = seq1.length();
    int n = seq2.length();

    vector<vector<int>> H(m + 1, vector<int>(n + 1, 0));

    for (int i = 1; i <= m; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            int match = H[i - 1][j - 1] + score(seq1[i - 1], seq2[j - 1]);
            int deleteOp = H[i - 1][j] + gap_score;
            int insertOp = H[i][j - 1] + gap_score;
            H[i][j] = max({ 0, match, deleteOp, insertOp }); // 与Needleman-Wunsch不同，这里取0和三个操作的最大值
        }
    }

    // 找到矩阵中的最大值及其位置
    int max_i = 0, max_j = 0, max_score = 0;
    for (int i = 1; i <= m; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (H[i][j] > max_score)
            {
                max_score = H[i][j];
                max_i = i;
                max_j = j;
            }
        }
    }

    // 回溯找到最优比对路径
    string aligned_seq1, aligned_seq2;
    int i = max_i, j = max_j;
    while (i > 0 && j > 0 && H[i][j] > 0)
    {
        int current_score = H[i][j];
        int score_diag = H[i - 1][j - 1];
        int score_up = H[i][j - 1];
        int score_left = H[i - 1][j];

        if (current_score == score_diag + score(seq1[i - 1], seq2[j - 1]))
        {
            aligned_seq1 = seq1[i - 1] + aligned_seq1;
            aligned_seq2 = seq2[j - 1] + aligned_seq2;
            i--;
            j--;
        }
        else if (current_score == score_left + gap_score)
        {
            aligned_seq1 = seq1[i - 1] + aligned_seq1;
            aligned_seq2 = '-' + aligned_seq2;
            i--;
        }
        else
        {
            aligned_seq1 = '-' + aligned_seq1;
            aligned_seq2 = seq2[j - 1] + aligned_seq2;
            j--;
        }
    }

    return { aligned_seq1, aligned_seq2 };
}

