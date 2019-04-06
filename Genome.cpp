// Jong Hoon Kim
// CS32 - Project 4

#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <istream>
using namespace std;

class GenomeImpl
{
public:
    // Constructor
    //
    // Pre-condition: name and sequence strings
    // Post-condition: set corresponding private data member
    GenomeImpl(const string& nm, const string& sequence);
    
    // Static Function
    //
    // Pre-condition: istream object and a vector of Genome to store the result
    // Post-condition: parse the input using istream, create and store Genome into the vector
    static bool load(istream& genomeSource, vector<Genome>& genomes);
    
    // Accessor Function
    //
    // Pre-condition: N/A
    // Post-condition: returns the size of the sequence
    int length() const;
    //
    // Pre-condition: N/A
    // Post-condition: returns the name of the Genome
    string name() const;
    //
    // Pre-condition: position, length, and a string to store the result
    // Post-condition: parse the fragment of sequence with given info and store it to string
    bool extract(int position, int length, string& fragment) const;
private:
    string m_name;
    string m_sequence;
    int m_length;
};

GenomeImpl::GenomeImpl(const string& nm, const string& sequence)
           :m_name(nm), m_sequence(sequence), m_length(static_cast<int>(sequence.size())) {}

bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes) 
{
    // initialize variables
    genomes.clear();
    string newName;
    string newSeq;
    string temp;
    int i;
    char tempChar;
    bool parsingSeq = false;
    
    // while its possible to read in data from input, parse and store Genome data
    // corresponding to structure of Genome data file
    while (getline(genomeSource, temp)) {
        if (temp[0] == '>') {
            if (parsingSeq && newSeq == "") {
                genomes.clear();
                return false;
            }
            else {
                if (parsingSeq) genomes.push_back(Genome(newName, newSeq));
                newName = temp.substr(1);
                if (newName == "") {
                    genomes.clear();
                    return false;
                }
                newSeq = "";
                parsingSeq = true;
            }
        }
        else {
            if (!parsingSeq) return false;
            for (i = 0; i < temp.size(); ++i) {
                tempChar = toupper(temp[i]);
                if (tempChar == 'A' || tempChar == 'C' ||
                    tempChar == 'T' || tempChar == 'G' ||
                    tempChar == 'N') newSeq += tempChar;
                else {
                    genomes.clear();
                    return false;
                }
            }
        }
    }
    if (newSeq != "") genomes.push_back(Genome(newName, newSeq));
    
    // return true is Genome was successfully stored
    return genomes.size() != 0;
}

int GenomeImpl::length() const
{ return m_length; }

string GenomeImpl::name() const
{ return m_name; }

bool GenomeImpl::extract(int position, int length, string& fragment) const
{
    // return false for invalid input
    if  (position < 0 || length < 0 ||
        (position + length) > m_length) return false;
    
    // otherwise parse the sequence and return true.
    fragment = m_sequence.substr(position, length);
    return true;
}

//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const string& nm, const string& sequence)
{
    m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
    delete m_impl;
}

Genome::Genome(const Genome& other)
{
    m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
    GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
    delete m_impl;
    m_impl = newImpl;
    return *this;
}

bool Genome::load(istream& genomeSource, vector<Genome>& genomes) 
{
    return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
    return m_impl->length();
}

string Genome::name() const
{
    return m_impl->name();
}

bool Genome::extract(int position, int length, string& fragment) const
{
    return m_impl->extract(position, length, fragment);
}
