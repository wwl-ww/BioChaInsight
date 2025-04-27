#pragma once

#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include <iostream>
using namespace std;
#include<vector>
#include<math.h>

class Alignment
{
private:
    static int match_score;     // 匹配得分
    static int mismatch_score; // 不匹配得分
    static int gap_score;      // 空位得分

public:

    int score(char a, char b);

	// Needleman-Wunsch算法
    pair<string, string> needlemanWunsch(const string& seq1, const string& seq2);

	// Smith-Waterman算法
    pair<string, string> smithWaterman(const string& seq1, const string& seq2);

    // Gotoh算法
    pair<string, string> gotoh(const string& seq1, const string& seq2);

    // Hirschberg算法
    pair<string, string> hirschberg(const string& seq1, const string& seq2);
    pair<string, string> hirschbergRecursive(const string& seq1, const string& seq2);
    vector<int> nwScore(const string& seq1, const string& seq2);
    string reverse(const string& str);
};

#endif
