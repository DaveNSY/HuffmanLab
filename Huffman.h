#include <string>
#include <queue>
#include <vector>
#include <fstream>
#include <unordered_map>

#include "Node.h"
#include "Storage/Storage.h"

#ifndef HUFFMAN_H
#define HUFFMAN_H

/**
 * A Huffman class represents the Huffman tree and it's uses to compress or decompress text files, while also using the Storage class for assistance
 */
class Huffman {
public:
    /**
     * Constructor of a Huffman object, doesn't need to do anything
     */
    Huffman();

    /**
     * The compress function takes in an input file to compress
     * Builds a frequency table, a Huffman tree and inserts binary values into the output file
     * @param input The input file which is taken to be compressed
     * @param output The output file which will hold the binary codes
     */
    void compress(std::string input, std::string output);

    /**
     * The decompress function takes in an input file (binary encoded) to decompress
     * Builds a Huffman tree using the header and reads bit by bit while traversing Huffman tree to get letters to put into the output file
     * @param input The input file which is taken to be decompressed (filled with binary bits)
     * @param output The output file which will hold the decompressed letters
     */
    void decompress(std::string input, std::string output);

private:
    // std::vector<Node*> table;
    std::unordered_map<char, Node*> table;      // <datatype of key/nametag translated into int in hash function, datatype of value>

    /**
     * BuildHeader takes in a root of a fully built Huffman tree and inserts a letter and its code by traversing pre-order style
     * @param root The root Node of a fully built Huffman tree
     * @param header The string which will contain information for decoding
     */
    void BuildHeader(Node* root, std::string &header);

    /**
     * UpdateFreqTable is a function that takes in a line from a text file, reads across the different characters
     * And updates our frequency table (unordered map) with either new characters or incrementing weight count
     * @param inputStr A line from the input text file which we will read through
     */
    void UpdateFreqTable(std::string inputStr);

    /**
     * tableBuildHuffTree builds a Huffman tree from a frequency table
     * @return A pointer Node that is the root to the Huffman tree
     */
    Node* tableBuildHuffTree();

    /**
     * codeBuildHuffTree builds a Huffman tree from the code in a header instead
     * @param header The header taken from the getHeader() function beforehand
     * @return A pointer Node that is the root to the Huffman tree
     */
    Node* codeBuildHuffTree(std::string header);

    /**
     * encode traverses a Huffman tree preorder while keeping track of it's 1 and 0 paths until it hits a leaf node
     * Therefore assigning the binary encoding to that specific letter at a leaf node
     * @param root The root of the Huffman tree where traversal starts
     * @param prefix The string that keeps track of the encoding as the function traverses the Huffman tree
     */
    void encode(Node* root, std::string prefix);
};

#endif //HUFFMAN_H
