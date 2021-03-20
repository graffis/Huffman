//
//  huff.c
//  proj2
//
//  Created by Owen Graffis on 10/13/19.
//  Copyright © 2019 Owen Graffis. All rights reserved.
//

#include "huff.h"
#include "string.h"

void huff(char * Filename)
{
    int * charArr = NULL;
    charArr = fillArray(Filename);
    struct Node * header = NULL;
    header = buildList(charArr);
    free(charArr);
    sortedList(header);
    int count = getCount(header);
    
    printf("%d", count);
    
    struct Node * EOFNode = buildNode(0, '\0'); //freq = 0 so bottom of the tree. lowest priority
    EOFNode -> next = header;
    header = EOFNode;

    count++;
    
    char * chars = fillChars(header, count);
    int * freqs = fillFreqs(header, count);
    struct MinHeapNode* root = buildHuffmanTree(chars, freqs, count);
    struct tableElement * table = createTable(root, count);
    
    writeFile(Filename, chars, freqs, root, count, table);
}

/*--------------------------------------------*/
//  free memory
/*--------------------------------------------*/

void freeTable(struct tableElement * table)
{
    struct tableElement * temp = table;
    struct codeStr * codeTemp = NULL;
    struct codeStr * codeTemp2 = NULL;
    while (table != NULL)
    {
        codeTemp = temp -> path;
        while(codeTemp != NULL)
        {
            codeTemp2 = codeTemp;
            free(codeTemp);
            codeTemp = codeTemp2 -> next;
        }
        temp = table;
        table = table -> next;
        free(temp);
    }
}

void freeList(struct Node * head)
{
    struct Node* tmp;
    
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}


void freeTree(struct MinHeapNode* root)
{
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

/*--------------------------------------------*/
//  create table
/*--------------------------------------------*/

struct tableElement * createTable(struct MinHeapNode * root, int count)
{
    struct tableElement * header = NULL;
    int height = maxDepth(root);
    int top = 0;
    int arr[height];
    header = findCodes(root, arr, top, header);
    return header;
}

struct tableElement * findCodes(struct MinHeapNode* root, int arr[], int top, struct tableElement * header)
{
    
    // Assign 0 to left edge and recur
    if (root->left) {
        arr[top] = 0;
        header = findCodes(root->left, arr, top + 1, header);
    }
    
    // Assign 1 to right edge and recur
    if (root->right) {
        
        arr[top] = 1;
        header = findCodes(root->right, arr, top + 1, header);
    }
    
    // If this is a leaf node, then
    // it contains one of the input
    // characters, return the character
    // and its code from arr[]
    if (isLeaf(root)) {
        struct tableElement * newElement = (struct tableElement *)malloc(sizeof(struct tableElement));
        newElement -> character = root -> data;
        newElement -> next = header;
        newElement -> path = returnCode(arr, top);
        newElement -> codeLen = top;
        header = newElement;
    }
    
    return header;
}

struct codeStr * returnCode(int * arr, int n)
{
    int i;
    struct codeStr * firstNode = NULL;
    for(i = 0; i < n; i++)
    {
        struct codeStr * newCode = (struct codeStr *)malloc(sizeof(struct codeStr));
        newCode -> lr = arr[i];
        newCode -> next = firstNode;
        firstNode = newCode;
    }
    
    firstNode = flipList(firstNode);
    
    return firstNode;
}

struct codeStr * flipList(struct codeStr * header)
{
    struct codeStr* prev = NULL;
    struct codeStr* current = header;
    struct codeStr* next = NULL;
    
    while (current != NULL) {
        // Store next
        next = current->next;
        
        // Reverse current node's pointer
        current->next = prev;
        
        // Move pointers one position ahead.
        prev = current;
        current = next;
    }
    header = prev;
    return header;
}

/*--------------------------------------------*/
//  write to file
/*--------------------------------------------*/


void writeFile(char * Filename,char * chars, int * freqs, struct MinHeapNode * root, int count, struct tableElement * table)
{
    FILE * fptr = fopen(Filename, "r");
    FILE * optr = fopen(strcat(Filename, ".huff"), "wb");
    fprintf(optr, "%d,", count);
    printHeader(optr, chars, freqs, count);
    encode(fptr, root, optr, table);
    fclose(fptr);
    fclose(optr);
}

void printHeader(FILE * optr, char * chars, int * freqs, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        fprintf(optr, "%d,", freqs[i]);
        fprintf(optr, "%c,", chars[i]);
    }
    i = -1;
    fprintf(optr, "%d,", i);
}

void encode(FILE * fptr, struct MinHeapNode* root, FILE * optr, struct tableElement * table)
{
    unsigned char byte = 0;
    int count = 0;
    char currentChar;
    struct tableElement * temp;
    struct codeStr * code;
    
    while(!feof(fptr))
    {
        currentChar = fgetc(fptr);
        //each code for each currentChar
        //while loop to find the character in table
        //while loop until null to shift into byte. if byte is full, write the byte, then set byte back to zero
        temp = table;
        while(temp != NULL)
        {
            if (currentChar == temp -> character)
            {
                //found the character. need to shift in code
                for (code = temp -> path; code != NULL; code = code -> next)
                {
                    byte |= (code -> lr);
                    count++;
                    if (count == 8)
                    {
                        fwrite(&byte, 1, 1, optr);
                        byte = 0;
                        count = 0;
                    }
                    else
                    {
                        byte <<= 1;
                    }
                }
                
                break;
            }
            temp = temp -> next;
        }
        
        //print EOF character
        if(feof(fptr))
        {
            temp = table;
            while(temp != NULL)
            {
                if ('\0' == temp -> character)
                {
                    //found the eof. need to shift in code
                    for (code = temp -> path; code != NULL; code = code -> next)
                    {
                        byte |= (code -> lr);
                        count++;
                        if (count == 8)
                        {
                            fwrite(&byte, 1, 1, optr);
                            byte = 0;
                            count = 0;
                        }
                        else
                        {
                            byte <<= 1;
                        }
                    }

                    break;
                }
                temp = temp -> next;
            }

            if (count != 0)
            {
                byte <<= (8 - count) - 1;
                fwrite(&byte, 1, 1, optr);
            }
        }

    }
    
}

/*--------------------------------------------*/
//  Frequency count
/*--------------------------------------------*/

int * fillArray(char * Filename)
{
    int * temp = (int *) malloc(sizeof(int) * 255);
    FILE * fptr = fopen(Filename, "r");
    
    if (fptr == NULL)
    {
        printf("\nCannot read file\n");
        return NULL;
    }
    
    //initialize all members in temp to zero
    int i;
    for (i = 0; i < 255; i++) {
        temp[i] = 0;
    }
    
    int c;
    while ((c = fgetc(fptr)) != EOF) {
        temp[c]++;
    }
    
    fclose(fptr);
    //  (char)p = character
    //  temp[p] = frequency
    
    return temp;
}

/*--------------------------------------------*/
//  Linked List
/*--------------------------------------------*/

struct Node * buildList(int * charArray)
{
    struct Node * header = NULL;
    struct Node * newNode = NULL;
    
    int idx;
    for (idx = 0; idx < 255; idx++)
    {
        if (charArray[idx] > 0) //frequency is larger than one
        {
            newNode = buildNode(charArray[idx], (char)idx);
            newNode -> next = header;
            header = newNode;
        }
    }
    return newNode;
}

struct Node * buildNode(int freq, char character)
{
    struct Node * newNode = (struct Node *) malloc(sizeof(struct Node));
    newNode -> freq = freq;
    newNode -> character = character;
    return newNode;
}

void sortedList(struct Node * origHeader)
{
    int swapped;
    struct Node * ptr1;
    struct Node * lptr = NULL;
    
    if (origHeader == NULL) {
        return; //empty list case
    }
    do
    {
        swapped = 0;
        ptr1 = origHeader;
        while (ptr1 -> next != lptr)
        {
            if ((ptr1 -> freq) > (ptr1 -> next -> freq)) {
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1 -> next;
        }
        lptr = ptr1;
    } while(swapped);
    
}

void swap(struct Node * a, struct Node * b)
{
    int temp = a -> freq;
    char tempC = a -> character;
    a -> freq = b -> freq;
    a -> character = b -> character;
    b -> freq = temp;
    b -> character = tempC;
}

char * fillChars(struct Node * header, int count)
{

    
    char * chars = (char *)malloc(sizeof(char) * count);
    struct Node * temp = header;
    temp = header;
    int idx = 0;
    
    while (temp != NULL)
    {
        chars[idx] = temp -> character;
        temp = temp -> next;
        idx++;
    }
    
    return chars;
    
}

int * fillFreqs(struct Node * header, int count)
{
    
    int * freqs = (int *)malloc(sizeof(int) * count);
    struct Node * temp = header;
    int idx = 0;
    while (temp != NULL)
    {
        freqs[idx] = temp -> freq;
        temp = temp -> next;
        idx++;
    }
    
    return freqs;
}

int getCount(struct Node * header)
{
    struct Node * temp = header;
    int count = 0;
    
    while (temp != NULL) {
        count++;
        temp = temp -> next;
    }
    return count;
}
/*--------------------------------------------*/
//  BST
/*--------------------------------------------*/

struct MinHeapNode* newNode(char data, unsigned freq)
{
    struct MinHeapNode* temp= (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    
    return temp;
}

struct MinHeap* createMinHeap(unsigned capacity)
{
    struct MinHeap* minHeap
    = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    
    // current size is 0
    minHeap->size = 0;
    
    minHeap->capacity = capacity;
    
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
} 

struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size)
{
    struct MinHeapNode *left, *right, *top;
    
    // Step 1: Create a min heap of capacity
    // equal to size.  Initially, there are
    // modes equal to size.
    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);
    
    // Iterate while size of heap doesn't become 1
    while (!isSizeOne(minHeap)) {

        left = extractMin(minHeap);
        right = extractMin(minHeap);
        
        top = newNode('$', left->freq + right->freq);
        
        top->left = left;
        top->right = right;
        
        insertMinHeap(minHeap, top);
    }
    
    // Step 4: The remaining node is the
    // root node and the tree is complete.
    return extractMin(minHeap);
}

void buildMinHeap(struct MinHeap* minHeap)
{
    
    int n = minHeap->size - 1;
    int i;
    
    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
} 

struct MinHeap* createAndBuildMinHeap(char data[], int freq[], int size)
{
    
    struct MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
    {
        minHeap->array[i] = newNode(data[i], freq[i]);
    }
    minHeap->size = size;
    buildMinHeap(minHeap);
    
    return minHeap;
} 

void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode)
{
    
    ++minHeap->size;
    int i = minHeap->size - 1;
    
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    
    minHeap->array[i] = minHeapNode;
}

struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
    
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void minHeapify(struct MinHeap* minHeap, int idx)
{
    
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    
    if (left < minHeap->size && minHeap->array[left]->
        freq < minHeap->array[smallest]->freq)
        smallest = left;
    
    if (right < minHeap->size && minHeap->array[right]->
        freq < minHeap->array[smallest]->freq)
        smallest = right;
    
    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest],
                        &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

int isSizeOne(struct MinHeap* minHeap)
{
    
    return (minHeap->size == 1);
}

int isLeaf(struct MinHeapNode* root)
{
    return !(root->left) && !(root->right);
}

void printArr(int arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
    {
        printf("%d", arr[i]);
    }
    printf("\n");
}

int maxDepth(struct MinHeapNode* node)
{
    if (node==NULL)
        
        return 0;
    
    else
    {
        /* compute the depth of each subtree */
        int lDepth = maxDepth(node->left);
        int rDepth = maxDepth(node->right);
        
        /* use the larger one */
        if (lDepth > rDepth)
        {
            return(lDepth+1);
        }
        else
        {
            return(rDepth+1);
        }
    }
}
