#include "Huffman.h"
#include <sstream>

Huffman::Huffman() {

}

void Huffman::UpdateFreqTable(std::string inputStr) {
    // For each character of a single given line, if the table does not have the character, add it to hashtable
    // Else, increment the letter's weight by one
    // Continues until the end of the string
    for (int i = 0; i < inputStr.length(); i++) {
        char currChar = inputStr[i];
        if (table.find(currChar) == table.end()) {
            table[currChar] = new Node(currChar, 1);    // Assign a space in the hashtable to the new Node with the new letter with weight 1, tying key to Node
        }
        else {
            table[currChar]->weight++;
        }
    }
}

Node* Huffman::tableBuildHuffTree() {
    // Make a priority queue based on our fully build hashtable
    // For the number of elements in our hashtable, create an iterator to point to the start of the hashmap and for each element, add it into te min heap
    std::priority_queue<Node*, std::vector<Node*>, compareWeights> addingQueue;
    for (auto iter = table.begin(); iter != table.end(); ++iter) {
        addingQueue.emplace(iter->second);      // iter->second points to the value of the element, which are the Node pointers
    }

    // While the priority queue has more than two elements, pop two elements with the smallest frequencies together
    // Then, add the two frequencies into a parent node to be added back into the priority queue while connecting it to the two leaf nodes
    // When there is only one element left in the priority queue, that will be the "root" pointer
    while (addingQueue.size() > 1) {
        Node* left = addingQueue.top(); addingQueue.pop();
        Node* right = addingQueue.top(); addingQueue.pop();

        int freqSum = left->weight + right->weight;
        Node* parent = new Node('\0', freqSum, left, right);
        addingQueue.emplace(parent);
    }

    // Returns the root of the Huffman tree
    return addingQueue.top();
}

Node *Huffman::codeBuildHuffTree(std::string header) {
    // At the beginning, the first character of the header string is the first character we want to input to the tree
    // currNode will act as an iterator to go through the tree as we are building it up depending on the codes of the header
    char currChar = header[0];
    Node* root = new Node('\0', 0);
    Node* currNode = root;

    for (int i = 1; i < header.length(); i++) {
        // If we encounter a separator, it means we reached the end of the code for our current letter
        // So we first set our current Node to have that letter, and reset where our currNode points to and point it to the root again
        // Then skip the separator by incrementing i by 1
        // Then set our current character to the single character after that separator
        // Increment i by 1 in order to place iterator onto binary values
        if (header[i] == '\x1F') {
            currNode->letter = currChar;
            currNode = root;

            if (i < header.length() - 1) {
                i++; currChar = header[i]; i++;
            }
        }

        // At this point, we assume we are at the binary code section of the header
        // If index is at a 1, move to the one Node of our current Node (create a new Node if it points to nullptr)
        // If index is at a 0, move to the zero Node of our current Node (create a new Node if it points to nullptr)
        if (header[i] == '1') {
            if (currNode->one == nullptr) {
                Node* temp = new Node('\0', 0);
                currNode->one = temp;
            }
            currNode = currNode->one;
        }
        else if (header[i] == '0') {
            if (currNode->zero == nullptr) {
                Node* temp = new Node('\0', 0);
                currNode->zero = temp;
            }
            currNode = currNode->zero;
        }
    }

    return root;
}

void Huffman::encode(Node* currNode, std::string prefix) {
    // Base case: when we reach a leaf node and the letter is not \0, assign the Node's code to be the binary encoding
    // Recursive case: when the Node we are on is not a leaf node, go to the left/right and prep the binary encoding for each left/right turn
    if (currNode->letter != '\0') {
        currNode->code = prefix;
    }
    else {
        encode(currNode->zero, prefix + "0");
        encode(currNode->one, prefix + "1");
    }
}

void Huffman::BuildHeader(Node *root, std::string &header) {
    // Base case: when we reach a leaf node, return
    if (root == nullptr) {
        return;
    }

    // Recursive case: Add onto the string header the letter followed by the binary encoding followed by a text separator to separate different encodings
    if (root->letter != '\0') {
        header = header + root->letter + root->code + '\x1F';
    }
    BuildHeader(root->zero, header);
    BuildHeader(root->one, header);
}

void Huffman::compress(std::string input, std::string output) {
    std::ifstream input_file;
    std::string currLine = "";
    Node* root = nullptr;

    // First, open the file and when successfully opening the file, build our frequency table with the first cycle through our input text file
    // In our current Huffman object (with its own unordered map that starts out empty), update it with each current line we are on
    // After the first cycle, we will have a fully completed frequency table correctly accounting all letters of the entire text file
    // We then assign a root node pointer to the root of our Huffman tree, by building the tree with the object's build in frequency table
    // Next, use that Huffman tree to traverse through it while building up encodings for each letter of the frequency table
    input_file.open(input);
    if (!input_file.fail()) {
        while (std::getline(input_file, currLine)) {
            UpdateFreqTable(currLine);
            UpdateFreqTable("\n");
        }
        root = tableBuildHuffTree();
        encode(root, "");
    }

    // This new file is a Storage object which is set to the output file, and uses Storage functions to open, close and set headers
    Storage* file = new Storage();
    file->open(output, "write");

    // Building the header by first creating an empty string, running our hashtable and string together to build the header
    // Then setting that header string with setHeader() to the output file
    std::string header = "";
    BuildHeader(root, header);

    // Calling getline() already means the cursor is at bottom, so we first call clear() and seekg()
    // This brings cursor up to beginning of text file again for the second cycle of reading
    // "content" string will be all the binary bits that will be inserted into the output file
    input_file.clear();
    input_file.seekg(0);
    std::string content = "";
    while (std::getline(input_file, currLine)) {
        // For each character of our current line, the hash map lets us find the current character's code to be inserted into the output file
        for (int i = 0; i < currLine.length(); i++) {
            // Searches in the hashtable the current character's letter, then add the code to the content
            content += table[currLine[i]]->code;
        }
        // Adds newline character which is erased when doing getline()
        content += table['\n']->code;
    }

    // This adds the number of the contentLength at the front of our header with all the codes
    // Sets the header first, and then insert all the binary bits into our output file
    // Closes the file to enable binary code makes it to text file
    header = std::to_string(content.length()) + '\x1F' + header;
    file->setHeader(header);
    file->insert(content);
    file->close();
}

void Huffman::decompress(std::string input, std::string output) {
    // "lengthPrefix" will be the string representation of the number of bits in the compressed file
    // "header" will be the string representation of the header from the compressed file
    // "currBinary" will be the current 8 sets of bit we have reading through when going through our Huffman tree
    // "decoded" will be a decoded letter we get after traversing and reaching a leaf node in our Huffman tree
    std::ofstream output_file;
    std::string lengthPrefix;
    std::string header = "";
    std::string currBinary = "";
    std::string decoded = "";

    // Open the input file with binary coding
    Storage* binary_file = new Storage();
    binary_file->open(input, "read");

    header = binary_file->getHeader();
    // Use stringstream as a shortcut to extract the amount of bits in the content from the header
    // ss is a form of the header which we can use getline() to pinpoint which section to insert into contentLength variable
    // The 2nd getline() gets the remaining codes from the header which is used as an argument for codeBuildHuffTree()
    std::stringstream ss(header);
    getline(ss, lengthPrefix, '\x1F');
    int contentLength = std::stoi(lengthPrefix);
    getline(ss, header, '\0');

    // Create a root node that will be the root of our newly created Huffman tree
    Node* root = codeBuildHuffTree(header);

    // currNode will be like an iterator to go through our Hufftree
    // count will increment when reading a single binary from the file content
    Node* currNode = root;
    int count = 0;
    output_file.open(output);
    while (binary_file->extract(currBinary)) {
        for (int i = 0; i < currBinary.length(); i++) {
            if (count > contentLength) break;

            if (currBinary[i] == '1') {
                currNode = currNode->one;
            }
            else if (currBinary[i] == '0') {
                currNode = currNode->zero;
            }

            if (currNode->letter != '\0') {
                decoded = currNode->letter;
                output_file << decoded;
                currNode = root;
            }
            count++;
        }
    }

    binary_file->close();
    output_file.close();
}








