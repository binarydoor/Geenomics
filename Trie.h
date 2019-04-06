// Jong Hoon Kim
// CS32 - Project 4

#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>


template<typename ValueType>
class Trie
{
public:
    // Constructor
    //
    // Pre-condition: Only default constructor can be called
    // Post-condition: Create a new trieNode and assign address to root
    Trie();
    
    // Destructor
    //
    // Pre-condition: N/A
    // Post-condition: Use deleteChildren function to deallocate all trieNodes in tree
    //                 and deallocate root trieNode
    ~Trie();
    
    // Mutator Functions
    //
    // Pre-condition: N/A
    // Post-condition: Use deleteChildren function to deallocate all trieNodes in tree,
    //                 deallocate root trieNode, and create a new root trieNode
    void reset();
    //
    // Pre-condition: a string and a value to be stored in trie
    // Post-condition: branch the trie with characters in string and store the value
    //                 at the end of the node. (use helper function)
    void insert(const std::string& key, const ValueType& value);
    
    // Accessor Functions
    //
    // Pre-condition: a string to search in trie and boolean for exact match condition
    // Post-condition: returns the value stored at the end of the node if the string is
    //                 branched in the trie. (use helper function)
    std::vector<ValueType> find(const std::string& key, bool exactMatchOnly) const;

      // C++11 syntax for preventing copying and assignment
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
private:
    struct trieNode {
        std::vector<ValueType> values;
        struct triePointer {
            std::vector<char> label;
            std::vector<trieNode*> trieNodePtr;
        } children;
    } *root;
    
    // helper functions
    //
    // Pre-condition: needs trieNode pointer
    // Post-condition: recursively call each children pointers and delete them
    void deleteChildren(trieNode* parent);
    //
    // Pre-condition: key string to search, value to store, and trieNode pointer
    // Post-condition: recursively branch/traverse trie with each character in the key and
    //                 insert the value at the node once key becomes empty
    void insertChildren(const std::string& key, const ValueType& value, trieNode* current);
    //
    // Pre-condition: key string to search, condition for exact match, pointer to trieNode,
    //                and vector to store values
    // Post-condition: recursively traverse trie with each character in the key and insert
    //                 all values found in the end node to the vector
    void searchChildren(const std::string& key, bool exactMatchOnly, trieNode* current,
                        std::vector<ValueType> &matchedValues) const;
};


template<typename ValueType>
Trie<ValueType>::Trie()
     : root(new trieNode) { }


template<typename ValueType>
Trie<ValueType>::~Trie()
{
    deleteChildren(root);
    delete root;
}


template<typename ValueType>
void Trie<ValueType>::reset()
{
    deleteChildren(root);
    delete root;
    root = new trieNode;
}


template<typename ValueType>
void Trie<ValueType>::deleteChildren(trieNode* parent)
{
    // iterate through each children pointers and recursively call with children
    // to delete all children pointers in the child. then delete the child
    auto it = parent->children.trieNodePtr.begin();
    for (; it != parent->children.trieNodePtr.end(); ++it) {
        deleteChildren(*it);
        delete (*it);
    }
}


template<typename ValueType>
void Trie<ValueType>::insert(const std::string& key,
                             const ValueType& value)
{ insertChildren(key, value, root); }


template<typename ValueType>
void Trie<ValueType>::insertChildren(const std::string& key,
                                     const ValueType& value,
                                     trieNode* current)
{
    // base case: if key is empty, insert the value at the current node
    if (key == "") current->values.push_back(value);
    else {
        // find the index of child that has matching label as first character in key
        int i = 0;
        while (i < current->children.label.size()) {
            if (current->children.label[i] == key[0]) break;
            ++i;
        }
        
        // if child of matching label is found, set it to next ndoe
        // otherwise, create a new trieNode with first character of the key
        trieNode *nextNode;
        if (i < current->children.label.size())
            nextNode = current->children.trieNodePtr[i];
        else {
            current->children.label.push_back(key[0]);
            nextNode = new trieNode;
            current->children.trieNodePtr.push_back(nextNode);
        }
        // recursive call with the key excluding first character and the nextNode
        insertChildren(key.substr(1), value, nextNode);
    }
}


template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::find(const std::string& key,
                                             bool exactMatchOnly) const
{
    // create a vector to return
    std::vector<ValueType> newVector;
    
    // check if the first character of the key is within the root's children label
    // if the child with matching label exists, use searchChildren function
    auto it = root->children.label.begin();
    for (; it != root->children.label.end(); ++it) {
        if (*it == key[0]) {
            searchChildren(key, exactMatchOnly, root, newVector);
            break;
        }
    }
    // return the result vector. will be empty if first character of key didn't match
    return newVector;
}


template<typename ValueType>
void Trie<ValueType>::searchChildren(const std::string& key,
                                     bool exactMatchOnly,
                                     trieNode* current,
                                     std::vector<ValueType> &matchedValues) const
{
    // base case: if key is empty, insert all values stored at current node to the vector
    if (key == "")
        for (auto it = current->values.begin(); it != current->values.end(); ++it)
            matchedValues.push_back(*it);
    else {
        // iterate through each child to check if label matches with first character in key
        // case 1 (matched):
        // recursive call with key excluding first character and child pointer
        // case 2 (unmatched and exactMatchOnly is false):
        // recursive call with key excluding first character, true for exact match condition,
        // and child pointer
        // case 3 (unmatched and exactMatchOnly is true):
        // do nothing
        for (int index = 0; index < current->children.label.size(); ++index) {
            if (key[0] == current->children.label[index]) {
                searchChildren(key.substr(1),
                               exactMatchOnly,
                               current->children.trieNodePtr[index],
                               matchedValues);
            }
            else if (!exactMatchOnly) {
                searchChildren(key.substr(1),
                               true,
                               current->children.trieNodePtr[index],
                               matchedValues);
            }
        }
    }
}

#endif // TRIE_INCLUDED
