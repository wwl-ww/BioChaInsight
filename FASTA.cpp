# include "FASTA.h"

/*
    Sequence
*/
Sequence::Sequence()
{
    this->sequence = "";
    this->header = "";
}

Sequence::Sequence(const string& sequence, const string& header)
{
    this->sequence = sequence;
    this->header = header;
}

Sequence::Sequence(const Sequence& seq)
{
    this->sequence = seq.sequence;
    this->header = seq.header;
}

// 读取的函数
string Sequence::getSequence() const
{
    return this->sequence;
}

string Sequence::getHeader() const
{
    return this->header;
}

int Sequence::getLength() const
{
    return this->sequence.length();
}

// 计算GC含量
int Sequence::getGCSum() const
{
    int count = 0;
    for (char ch : this->sequence)
    {
        if (ch == 'G' || ch == 'C')
        {
            count++;
        }
    }
    return count;
}

double Sequence::getGCContent() const
{
    return static_cast<double>(this->getGCSum()) / this->getLength();
}

// 分子量
double Sequence::calculateMW() const
{
    unordered_map<char, double> mw = {
        {'A', 331.2218},
        {'C', 307.1971},
        {'G', 347.2212},
        {'T', 322.2085}
    };
    double result = 0;
    for (char ch : this->sequence)
    {
        result += mw[ch];
    }
    return result;
}

// 基序频率
unordered_map<char, double> Sequence::calculateBaseFrequency() const
{
    unordered_map<char, double> freq;
    for (char ch : this->sequence)
    {
        if (freq.find(ch) == freq.end())  // 如果找不到ch，即freq中没有ch
        {
            freq[ch] = 1;
        }
        else
        {
            freq[ch]++;
        }
    }
    for (auto& pair : freq)
    {
        pair.second /= this->getLength();
    }
    return freq;
}

// 一些常见功能的运算符重载
Sequence& Sequence::operator+=(const Sequence& seq)
{
    this->sequence += seq.sequence;
    return *this;
}

Sequence& Sequence::operator+=(const string& seq)
{
    this->sequence += seq;
    return *this;
}


char Sequence::operator[](int index) const
{
    if (index < 0 || index >= this->getLength())
    {
        throw out_of_range("Index out of range");
    }
    return this->sequence[index];
}

char& Sequence::operator[](int index)
{
    if (index < 0 || index >= this->getLength())
    {
        throw out_of_range("Index out of range");
    }
    return this->sequence[index];
}

ostream& operator<<(ostream& output, const Sequence& seq)
{
    output << "Header: " << seq.header << endl;
    output << "Sequence: " << seq.sequence;
    return output;
}

Sequence Sequence::operator!() const
{
    unordered_map<char, char> complement = {
        {'A', 'T'},
        {'T', 'A'},
        {'C', 'G'},
        {'G', 'C'}
    };
    Sequence result(this->getSequence(), "the complement of:" + this->getHeader());
    for (char& ch : result.sequence)
    {
        ch = complement[ch];
    }
    return result;
}

Sequence::operator string() const
{
    return this->sequence;
}

void Sequence::printInfo() const
{
    cout << *this << endl;  // Header and sequence
    cout << "Length: " << this->getLength() << endl;
    cout << "GC content: " << this->getGCContent() << endl;
    cout << "Molecular weight: " << this->calculateMW() << endl;
    cout << "Base frequency: " << endl;
    unordered_map<char, double> freq = this->calculateBaseFrequency();
    for (auto& pair : freq)
    {
        cout << pair.first << ": " << pair.second << endl;
    }
    cout << endl;
}

/*
    FASTAReader
*/
void FASTAReader::_parse(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }
    string line;
    while (getline(file, line))
    {
        if (line.empty()) continue; // Skip empty lines
        if (line[0] == '>') // Header line
        {
            this->seqs.push_back(Sequence("", line.substr(1))); // Create a new sequence with the header
        }
        else // Sequence line
        {
            if (this->seqs.empty())
            {
                cerr << "Error: Sequence data found before header in file " << filename << endl;
                return;
            }
            this->seqs.back() += line; // Append to the last sequence
        }
    }
}

FASTAReader::FASTAReader(const string& filename) : filename(filename)
{
    _parse(filename);
}

vector<Sequence> FASTAReader::getSeqs() const
{
    return this->seqs;
}

vector<Sequence> FASTAReader::operator()() const
{
    return this->seqs;
}

Sequence& FASTAReader::operator[](int index)
{
    if (index < 0 || index >= this->seqs.size())
    {
        cerr << "Error: Index out of bounds in FASTAReader::operator[]" << endl;
        throw out_of_range("Index out of bounds");
    }
    return this->seqs[index];
}

const Sequence& FASTAReader::operator[](int index) const
{
    if (index < 0 || index >= this->seqs.size())
    {
        cerr << "Error: Index out of bounds in FASTAReader::operator[]" << endl;
        throw out_of_range("Index out of bounds");
    }
    return this->seqs[index];
}

void FASTAReader::printInfo() const
{
    cout << "FASTA file: " << this->filename << endl;
    cout << "Number of sequences: " << this->seqs.size() << endl;
}
