// Jong Hoon Kim
// CS32 - Project 4

#include "provided.h"
#include "Trie.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
using namespace std;

class GenomeMatcherImpl
{
public:
    // Constructor
    //
    // Pre-condition: minimum search length to be passed
    // Post-condition: set the private data member
    GenomeMatcherImpl(int minSearchLength);
    
    // Mutator Function
    //
    // Pre-condition: a Genome object to add
    // Post-condition: Add the Genome to the vector and insert the sequence into the trie
    //                 also add the genome name and the position as a string into last node
    void addGenome(const Genome& genome);
    
    // Accessor Function
    //
    // Pre-condition: N/A
    // Post-condition: returns the minimum search length
    int minimumSearchLength() const;
    //
    // Pre-condition: sequence fragment, minimum length of match, exact match condition boolean,
    //                and DNAMatch vector
    // Post-condition: store satisfied DNAMatch objects into vector and returns true if any
    //                 DNAMatch is found
    bool findGenomesWithThisDNA(const string& fragment, int minimumLength,
                                bool exactMatchOnly, vector<DNAMatch>& matches) const;
    //
    // Pre-condition: comparing Genome, fragment match length, exact match condition, percent
    //                matching threshold, and a vector to store results
    // Post-condition: store all GenomeMatch object into results if there is any indexed genome
    //                 that matches with query Genome. and returns true if found any matching one
    bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly,
                            double matchPercentThreshold, vector<GenomeMatch>& results) const;
    
private:
    int m_minSearchLength;
    vector<Genome> m_genomes;
    Trie<string> m_DNAs;
    
    // Helper Function
    //
    // Pre-condition: string of fragment, exact match condition, DNAMatch object
    // Post-condition: index characters of sequence starting from Genome and position
    //                 specified by DNAMatch. Then count the number of matching characters
    //                 with fragment and store it to DNAMatch object.
    void findMatching(const string& fragment, bool exactMatchOnly, DNAMatch& match) const;
};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
                  :m_minSearchLength(minSearchLength) {}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
    // Try to extract first fragment of the sequence up to minimum search length and if
    // it succeeds, add genome into vector and insert every subset fragment (length of
    // minimum search length) of sequence into the trie.
    string temp;
    int i = 0;
    if (!genome.extract(i, m_minSearchLength, temp))
        return;
    m_genomes.push_back(genome);
    m_DNAs.insert(temp, genome.name() + ", position " + to_string(i));
    while (genome.extract(++i, m_minSearchLength, temp)) {
        m_DNAs.insert(temp, genome.name() + ", position " + to_string(i));
    }
}

int GenomeMatcherImpl::minimumSearchLength() const
{ return m_minSearchLength; }

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment,
                                               int minimumLength,
                                               bool exactMatchOnly,
                                               vector<DNAMatch>& matches) const
{
    // If fragment is shorter than minimum length or minimum length smaller than
    // minimum search length, return false
    if (fragment.size() < minimumLength)   return false;
    if (minimumLength < m_minSearchLength) return false;

    // use trie's find function to get all genomes that has matching up to minimum search length
    // then create DNAMatch object with each one of the result and use findMatching function to
    // define the actual matching length at that position. then store the result into hash table
    // to calculate maximum length for each genome
    unordered_map<string, DNAMatch> umap;
    vector<string> match = m_DNAs.find(fragment.substr(0, m_minSearchLength), exactMatchOnly);
    for (auto it = match.begin(); it != match.end(); ++it) {
        string name = (*it).substr(0, (*it).find(", position"));
        int position = stoi((*it).substr((*it).find(", position") + 11));
        DNAMatch newMatch;
        newMatch.genomeName = name;
        newMatch.length     = 0;
        newMatch.position   = position;
        findMatching(fragment, exactMatchOnly, newMatch);
        auto umapItr = umap.find(newMatch.genomeName);
        if (umapItr == umap.end() || umapItr->second.length < newMatch.length)
            umap[newMatch.genomeName] = newMatch;
    }
    
    // if result has matching length bigger or equal to minimum length, store it to vector
    matches.clear();
    for (auto it = umap.begin(); it != umap.end(); ++it)
        if (it->second.length >= minimumLength)
            matches.push_back(it->second);
    return !(matches.empty());  // returns if found a genome that satisfies
}

void GenomeMatcherImpl::findMatching(const string& fragment,
                                     bool exactMatchOnly,
                                     DNAMatch& match) const
{
    // find the genome in the vector that matches the name
    auto it = m_genomes.begin();
    for (; it != m_genomes.end(); ++it)
        if (it->name() == match.genomeName) break;
    
    if (it != m_genomes.end()) {
        // allow 1 mismatch (SNiP) if exact match is set to false
        const int numAllowedMismatch = exactMatchOnly ? 0 : 1;
        int misCount = 0;
        string ch;
        
        // starting from the position specified in DNAMatch, iterate each character and
        // increase length if they are matching with the fragment. stops if mismatch
        // counter is no
        for (int i = match.position; i < it->length() && i < fragment.size() + match.position; ++i) {
            if (it->extract(i, 1, ch)) {
                if (ch == fragment.substr(i - match.position, 1)) {
                    match.length++;
                }
                else if (misCount < numAllowedMismatch) {
                    misCount++;
                    match.length++;
                }
                else break;
            }
        }
    }
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength,
                                           bool exactMatchOnly, double matchPercentThreshold,
                                           vector<GenomeMatch>& results) const
{
    // if fragment piece length is smaller than minimum search length, return false
    if (fragmentMatchLength < m_minSearchLength) return false;
    
    // initialize variables
    const int division = query.length() / fragmentMatchLength;
    vector<DNAMatch> matches;
    unordered_map<string, int> umap;
    string tempFrag;
    
    // iterate division time (query length divided by piece length) and find the matching genomes
    // and store the each genome's match into the hash table to keep track of number of matches
    // through out the loops
    for (int i = 0; i < division; ++i) {
        query.extract(i * fragmentMatchLength, fragmentMatchLength, tempFrag);
        findGenomesWithThisDNA(tempFrag, fragmentMatchLength, exactMatchOnly, matches);
        for (auto it = matches.begin(); it != matches.end(); ++it) {
            auto umapItr = umap.find(it->genomeName);
            if (umapItr == umap.end())
                umap[it->genomeName] = 1;
            else umapItr->second++;
        }
    }
    
    // calculate the match percentage of each genome in the hash table and push to result vector
    // if the percentage is higher than threshold
    results.clear();
    for (auto it = umap.begin(); it != umap.end(); ++it) {
        if ((double)(it->second) / division * 100 >= matchPercentThreshold) {
            GenomeMatch newGM;
            newGM.genomeName = it->first;
            newGM.percentMatch = (double)(it->second) / division * 100;
            results.push_back(newGM);
        }
    }
    
    // returns true only if there's at least one genome being pushed into vector
    return !(results.empty());
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
    m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
    delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
    m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
    return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
