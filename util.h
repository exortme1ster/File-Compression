//
// STARTER CODE: util.h
//
// util.h file
// Author: Nikita Mashchenko, UIC 2021
//
#include "hashmap.h"
#include "bitstream.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <vector>
#include <string>
using namespace std;
#pragma once

typedef hashmap hashmapF;
typedef unordered_map <int, string> hashmapE;

struct HuffmanNode {
    int character;
    int count;
    int order;
    HuffmanNode* zero;
    HuffmanNode* one;
    //     HuffmanNode(int charac, int cnt)
    // {
    //     zero = one = NULL;
    //     this->character = charac;
    //     this->count = cnt;
    // }
};
class prioritize {
 public:
    bool operator()(HuffmanNode* p1, HuffmanNode* p2) {
            if (p1->count == p2->count) {
                return p1->order > p2->order;
            } else {
                return p1->count > p2->count;
            }
        }
};
// helper free function
void clearTr(HuffmanNode *nd) {
    if (nd == nullptr) {
        return;
    }
        clearTr(nd->zero);
        clearTr(nd->one);
        delete nd;
}
//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode* node) {
        clearTr(node);
}
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
void buildFrequencyMap(string filename, bool isFile, hashmapF &map) {
    if (isFile) {
        ifstream file(filename);
        if (!file.good()) {
    cout << endl;
    cout << "**Error: unable to open input file '" << filename << "'." << endl;
    cout << endl;
    return;
  }
    char temp;
    while (file.get(temp)) {
                int pass = (int)(temp);
                        if (!map.containsKey(pass)) {
                            map.put(pass, 1);
                        } else {
                            map.put(pass, map.get(pass)+1);
                        }
    }
     map.put(PSEUDO_EOF, 1);
    } else {
        for (auto temp : filename) {
                            if (!map.containsKey(temp)) {
                                map.put(temp, 1);
                            } else {
                                map.put(temp, map.get(temp)+1);
                            }
        }
         map.put(PSEUDO_EOF, 1);
    }
}
// helper function for generating tree
HuffmanNode* generateTree(priority_queue<HuffmanNode*,
                              vector<HuffmanNode*>,
                                             prioritize> &pq_, int &ord) {
    HuffmanNode* top;
    while (pq_.size() != 1) {
        HuffmanNode* left = pq_.top();
        pq_.pop();
        HuffmanNode* right = pq_.top();
        pq_.pop();
        top = new HuffmanNode();
        top->character = NOT_A_CHAR;
        top->order = ord;
        top->count = left->count + right->count;
        top->zero = left;
        top->one = right;
        pq_.push(top);
        ord++;
    }
    return top;
}
//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmapF &map) {
    vector<int> keyss;
    keyss = map.keys();
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, prioritize> pq;
    int order_ = 1;
        for (int key : map.keys()) {
            HuffmanNode* node = new HuffmanNode;
            node->character = key;
            node->count = map.get(node->character);
            node->zero = nullptr;
            node->one = nullptr;
            node->order = order_;
            order_++;
            pq.push(node);
    }
    HuffmanNode* returned = generateTree(pq, order_);
    return returned;  // TO DO: update this return
}
//
// *Recursive helper function for building the encoding map.
//
void _buildEncodingMap(HuffmanNode* node, hashmapE &encodingMap, string str,
                       HuffmanNode* prev) {
    if (node == nullptr) {
        return;
    }
    // if both nodes are nullptr
    if (node->character != NOT_A_CHAR) {
        encodingMap[node->character] = str;
        return;
    }
    _buildEncodingMap(node->zero, encodingMap, str + '0', node);
    _buildEncodingMap(node->one, encodingMap, str + '1', node);
}
//
// *This function builds the encoding map from an encoding tree.
//
hashmapE buildEncodingMap(HuffmanNode* tree) {
    hashmapE encodingMap;
    _buildEncodingMap(tree, encodingMap, "", nullptr);
    return encodingMap;  // TO DO: update this return
}
//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, hashmapE &encodingMap, ofbitstream& output,
              int &size, bool makeFile) {
    string encoded;
    char temp;
    while (input.get(temp)) {
    encoded.append(encodingMap.at(temp));
    }
    if (makeFile) {
        for (size_t i = 0; i < encoded.length(); i++) {
             if (encoded[i] == '1') {
                output.writeBit(1);
                size++;
            }
             if (encoded[i] == '0') {
                output.writeBit(0);
                size++;
            }
        }
    }
    encoded.append(encodingMap.at(PSEUDO_EOF));
    return encoded;
}
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    HuffmanNode* curr = encodingTree;
    string decoder = "";
    while (!input.eof()) {
        if (curr->character == PSEUDO_EOF) {
            break;
        }
    int number = input.readBit();
        if (curr->character != NOT_A_CHAR) {  // is leaf node
        output.put(curr->character);
        decoder += curr->character;
        curr = encodingTree;
        }
        if (number == 0) {
            curr = curr->zero;
        } else if (number == 1) {
            curr = curr->one;
        }
    }
    return decoder;
}
//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
    hashmapF map1;
    ifstream temp(filename);

    ofbitstream temp1(filename + ".huf");
    buildFrequencyMap(filename, true, map1);

    HuffmanNode* root = buildEncodingTree(map1);
    hashmapE encodingMap1 = buildEncodingMap(root);

    int size1 = 0;
    temp1 << map1;

    string compressf = encode(temp, encodingMap1, temp1, size1, true);

    freeTree(root);
    return compressf;
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {

size_t pos = filename.find(".huf");
        if ((int)pos >= 0) {
            filename = filename.substr(0, pos);
        }
        pos = filename.find(".");
        string ext = filename.substr(pos, filename.length() - pos);
        filename = filename.substr(0, pos);
        ifbitstream input(filename + ext + ".huf");
        ofstream output(filename + "_unc" + ext);

        hashmapF dump;
        input >> dump;  // get rid of frequency map at top of file
        HuffmanNode* root = buildEncodingTree(dump);

        string decodeStr  = decode(input, root, output);
        freeTree(root);
    return decodeStr;
}
