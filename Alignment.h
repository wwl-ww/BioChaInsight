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
    static int match_score;     // ƥ��÷�
    static int mismatch_score; // ��ƥ��÷�
    static int gap_score;      // ��λ�÷�

public:

    int score(char a, char b);

	// Needleman-Wunsch�㷨
    pair<string, string> needlemanWunsch(const string& seq1, const string& seq2);

	// Smith-Waterman�㷨
    pair<string, string> smithWaterman(const string& seq1, const string& seq2);

    // Gotoh�㷨
    pair<string, string> gotoh(const string& seq1, const string& seq2);

    // Hirschberg�㷨
    pair<string, string> hirschberg(const string& seq1, const string& seq2);
    pair<string, string> hirschbergRecursive(const string& seq1, const string& seq2);
    vector<int> nwScore(const string& seq1, const string& seq2);
    string reverse(const string& str);
};

#endif
