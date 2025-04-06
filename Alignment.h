#pragma once

#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include <iostream>
using namespace std;
#include<vector>

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
};

#endif
