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

pair<string, string> Alignment::gotoh(const string& seq1, const string& seq2)
{
    // �����㷨�趨�ĵ÷ֹ���
    match_score = 1;
    mismatch_score = -1;
    gap_score = -2;
    int gap_open_penalty = -5; // Gap�򿪵ĳͷ�

    int m = seq1.length();
    int n = seq2.length();

    // ��ʼ����������
    vector<vector<int>> H(m + 1, vector<int>(n + 1, 0)); // ����ȫ�ֱȶ�
    vector<vector<int>> E(m + 1, vector<int>(n + 1, 0)); // ����gap��չ
    vector<vector<int>> F(m + 1, vector<int>(n + 1, 0)); // ����gap��չ

    // ��ʼ������ĵ�һ�к͵�һ��
    for (int i = 1; i <= m; ++i)
        H[i][0] = i * gap_score;
    for (int j = 1; j <= n; ++j)
        H[0][j] = j * gap_score;

    // ���H, E, F ����
    for (int i = 1; i <= m; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            // H �������
            int match = H[i - 1][j - 1] + score(seq1[i - 1], seq2[j - 1]);
            int deleteOp = E[i - 1][j] + gap_open_penalty;
            int insertOp = F[i][j - 1] + gap_open_penalty;
            H[i][j] = max({ match, deleteOp, insertOp });

            // E ������䣨gap��չ��
            E[i][j] = max({ H[i - 1][j] + gap_score, E[i - 1][j] + gap_score });

            // F ������䣨gap��չ��
            F[i][j] = max({ H[i][j - 1] + gap_score, F[i][j - 1] + gap_score });
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

pair<string, string> Alignment::hirschberg(const string& seq1, const string& seq2) {
    // �趨��ֵ��������г��Ⱥ�С��ֱ��ʹ�ø��򵥵��㷨����Needleman-Wunsch��
    if (seq1.size() <= 2 || seq2.size() <= 2) {
        return needlemanWunsch(seq1, seq2);  // �����г���С����ֵʱ��ʹ�ô�ͳ�㷨
    }

    // ���õݹ麯��
    return hirschbergRecursive(seq1, seq2);
}

pair<string, string> Alignment::hirschbergRecursive(const string& seq1, const string& seq2) {
    if (seq1.empty()) {
        return {string(seq2.size(), '-'), seq2};
    }
    if (seq2.empty()) {
        return {seq1, string(seq1.size(), '-')};
    }

    // �������е��м��к��м���
    int mid = seq1.size() / 2;

    // ���м��еļ���ֳ���������
    vector<int> score1 = nwScore(seq1.substr(0, mid), seq2);          // ǰ�벿�ֵıȶԷ���
    vector<int> score2 = nwScore(reverse(seq1.substr(mid)), reverse(seq2)); // ��벿�ֵıȶԷ���

    // ������ѷָ�λ��
    int max_index = 0;
    int max_score = score1[0] + score2[0];
    for (int i = 1; i < score1.size(); ++i) {
        int score = score1[i] + score2[i];
        if (score > max_score) {
            max_score = score;
            max_index = i;
        }
    }

    // �ݹ����벿�ֺ��Ұ벿�ֽ��бȶ�
    pair<string, string> left = hirschbergRecursive(seq1.substr(0, mid), seq2.substr(0, max_index));
    pair<string, string> right = hirschbergRecursive(seq1.substr(mid), seq2.substr(max_index));

    // �ϲ����
    return {left.first + right.first, left.second + right.second};
}

vector<int> Alignment::nwScore(const string& seq1, const string& seq2) {
    vector<int> prev(seq2.size() + 1);
    vector<int> curr(seq2.size() + 1);

    for (int j = 0; j <= seq2.size(); ++j) {
        prev[j] = j * gap_score;
    }

    for (int i = 1; i <= seq1.size(); ++i) {
        curr[0] = i * gap_score;
        for (int j = 1; j <= seq2.size(); ++j) {
            curr[j] = max({prev[j] + gap_score, curr[j - 1] + gap_score, prev[j - 1] + score(seq1[i - 1], seq2[j - 1])});
        }
        prev = curr;
    }

    return curr;
}

string Alignment::reverse(const string& str) {
    return string(str.rbegin(), str.rend());
}
