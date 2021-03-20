//
//  huff.h
//  proj2
//
//  Created by Owen Graffis on 10/13/19.
//  Copyright © 2019 Owen Graffis. All rights reserved.
//

#ifndef huff_h
#define huff_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-------------------------------------*/
//linked list node

struct Node {
    char character;
    int freq;
    struct Node * next;
};



/*-------------------------------------*/
//bst nodes

struct MinHeap {
    unsigned size; // Current size of min heap
    unsigned capacity; // capacity of min heap
    struct MinHeapNode** array; // Array of minheap node pointers
};

struct MinHeapNode {
    char data; // One of the input characters
    unsigned freq; // Frequency of the character //changed from unsigned
    struct MinHeapNode *left, *right; // Left and right child of this node
};

/*-------------------------------------*/
//code table list elements

struct tableElement {
    char character;
    struct tableElement * next;
    struct codeStr * path;
    int codeLen;
};

struct codeStr {
    int lr;
    struct codeStr * next;
};

/*-------------------------------------*/

//create bst
struct MinHeapNode* newNode(char data, unsigned freq);
struct MinHeap* createMinHeap(unsigned capacity);
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size);
struct MinHeap* createAndBuildMinHeap(char data[], int freq[], int size);
void buildMinHeap(struct MinHeap* minHeap);
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode);
struct MinHeapNode* extractMin(struct MinHeap* minHeap);
void minHeapify(struct MinHeap* minHeap, int idx);
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b);
int isSizeOne(struct MinHeap* minHeap);
int isLeaf(struct MinHeapNode* root);
void printArr(int arr[], int n);
int maxDepth(struct MinHeapNode* node);

//fill arrays for bst
int getCount(struct Node * header);
char * fillChars(struct Node * header, int count);
int * fillFreqs(struct Node * header, int count);

//write compressed file
void writeFile(char * Filename,char * chars, int * freqs, struct MinHeapNode * root, int count, struct tableElement * table);
void printHeader(FILE * optr, char * chars, int * freqs, int count);
void encode(FILE * fptr, struct MinHeapNode* root, FILE * optr, struct tableElement * table);

//create character code table. 2d linked list
struct tableElement * createTable(struct MinHeapNode * root, int count);
struct tableElement * findCodes(struct MinHeapNode* root, int arr[], int top, struct tableElement * header);
struct codeStr * returnCode(int * arr, int n);
struct codeStr * flipList(struct codeStr * header);

//dumb stuff I did at the beginning
void huff(char *);
int * fillArray(char *); //takes in filename
struct Node * buildList(int *); //takes in array of chars with frequencies
struct Node * buildNode(int freq, char character);
void sortedList(struct Node *); //return the sorted linked list, takes the initial header node
void swap(struct Node *a, struct Node *b);

void freeTree(struct MinHeapNode* root);
void freeList(struct Node * head);
void freeTable(struct tableElement * table);

#endif /* huff_h */
