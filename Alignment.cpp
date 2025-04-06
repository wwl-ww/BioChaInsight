#include "Alignment.h"

int Alignment::match_score = 0;     // ƥ��÷�
int Alignment::mismatch_score = 0; // ��ƥ��÷�
int Alignment::gap_score = 0;      // ��λ�÷�

int Alignment::score(char a, char b)
{
	return (a == b) ? match_score : mismatch_score;
}

pair<string, string> Alignment::needlemanWunsch(const string& seq1, const string& seq2)
{
	// �����㷨�趨�ĵ÷ֹ���
    match_score = 1;
	mismatch_score = -1;
	gap_score = -2;

    int m = seq1.length();
    int n = seq2.length();

    vector<vector<int>> H(m + 1, vector<int>(n + 1, 0));

    // ��ʼ������ĵ�һ�к͵�һ�У����Ǳ����϶��ǿ�λ����Ϊֻ��һ����������������һ����û�ж�
    for (int i = 1; i <= m; ++i)
        H[i][0] = i * gap_score;
    for (int j = 1; j <= n; ++j)
        H[0][j] = j * gap_score;

    // ������
    for (int i = 1; i <= m; ++i)
    {
        for (int j = 1; j <= n; ++j) {
            int match = H[i - 1][j - 1] + score(seq1[i - 1], seq2[j - 1]); // ƥ���ƥ��
            int deleteOp = H[i - 1][j] + gap_score; // ɾ��������������2�в����λ��
            int insertOp = H[i][j - 1] + gap_score; // ���������������1�в����λ��
            H[i][j] = max({ match, deleteOp, insertOp }); // ѡ�����ֵ
        }
    }

    // �����ҵ����űȶ�·��
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
            // �Խ��߷���ƥ���ƥ�䣩
            aligned_seq1 = seq1[i - 1] + aligned_seq1;
            aligned_seq2 = seq2[j - 1] + aligned_seq2;
            i--;
            j--;
        }
        else if (current_score == score_left + gap_score)
        {
            // ������������2�в����λ��
            aligned_seq1 = seq1[i - 1] + aligned_seq1;
            aligned_seq2 = '-' + aligned_seq2;
            i--;
        }
        else
        {
            // ���Ϸ���������1�в����λ��
            aligned_seq1 = '-' + aligned_seq1;
            aligned_seq2 = seq2[j - 1] + aligned_seq2;
            j--;
        }
    }

    // ����ʣ�ಿ��
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
	// �����㷨�趨�ĵ÷ֹ���
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
            H[i][j] = max({ 0, match, deleteOp, insertOp }); // ��Needleman-Wunsch��ͬ������ȡ0���������������ֵ
        }
    }

    // �ҵ������е����ֵ����λ��
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

    // �����ҵ����űȶ�·��
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

