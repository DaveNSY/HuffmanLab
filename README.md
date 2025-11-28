# HuffmanLab
The Huffman program allows a user to compress a desired file by first reading form the text-file they wish to compress, then encoding each letter in order to produce a file that is a compressed version of the file that was read in.

# Storage Class was coded by my professor, otherwise all code was done by me

In order to use the Huffman program, all you need is to type in a pre-existing text file as an input. There isn't a need to have an empty pre-existing file for the output, as the program can make one on the fly.

This Huffman compress function is implemented by the usage of a hashmap which has characters as keys and Node pointers as its values. By reading through the text file line by line, the program sees which characters (letters) are new and which has been accounted for, and increments its weight in the Node.

The map then acts as a frequency table, and the program can utilize it as a private variable in order to build a Huffman tree by inserting all the elements of the table into a priority queue (as a minimum heap). When there is more than one element in the queue, pop two off, add their weights and create a parent node with that weight while pointing it to the two nodes, and put it back onto the queue.
This way, we effectively create a Huffman tree with the final node in the queue being the root.

With that root, we assign the binary code of each leaf node (the letters) by traversing our Huffman tree while keeping track of the code by adding 1 and 0 depending on which path we take. We traverse the tree to also build our header, which will hold the information necessary to rebuild our Huffman tree once it comes to decompression.

Finally, we read through our text file one more time, and each letter we see, we search for it in our hashtable and insert that letter's code onto our compressed string, which will later to inserted into our binary file using the Storage class functions.

In order to decompress a file, the function first gets the header again, and builds the Huffman tree by reading and considering the characters. If a character is after a unit separator, it is considered a letter of the text file, and the 1s/0s are considered the code that ends at the unit separator.
While it reads the 1s/0s, it will create the tree, Node by Node.

After the Huffman tree is rebuilt, we will then use the Storage function to extract 8 bits from the compressed file and go through the Huffman tree. When it reaches a leaf node, it reads the letter and inserts it back to the output text file.
