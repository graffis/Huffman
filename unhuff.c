//
//  unhuff.c
//  proj2
//
//  Created by Owen Graffis on 10/23/19.
//  Copyright © 2019 Owen Graffis. All rights reserved.
//

#include "unhuff.h"

void unhuff(char * Filename)
{
    FILE * fptr = fopen(Filename, "rb");
    FILE * optr = fopen(strcat(Filename, ".unhuff"), "wb");
    struct MinHeapNode * root = getTree(fptr);
    decode(fptr, root, optr);
    
    freeTree(root);
    
}

struct MinHeapNode * getTree(FILE * fptr)
{
    int size, freq;
    int idx = 0;
    char charX;
    fscanf(fptr, "%d,", &size); //read first number in file, which is the quanitiy of chars + 1 (EOF)
    int * freqs = (int *)malloc(sizeof(int) * (size)); //extra space in array for EOF character
    char * chars = (char *)malloc(sizeof(char) * (size));
    while(1)
    {
        fscanf(fptr, "%d,", &freq);
        if (freq == -1)
        {
            break;
        }
        else
        {
            fscanf(fptr, "%c,", &charX);
            freqs[idx] = freq;
            chars[idx] = charX;
            idx ++;
        }
    }

    struct MinHeapNode* root = buildHuffmanTree(chars, freqs, size);
    
    return root;
}


void decode(FILE * fptr, struct MinHeapNode * root, FILE * optr)
{
    int count = 0; //byte being read
    unsigned char byte = 0b0;
    unsigned char maskedByte = 0b0;
    unsigned char mask = 0b10000000;
    struct MinHeapNode * temp = root;
    
    fread(&byte, 1, 1, fptr);
    count = 8;
    
    while(1)
    {
        if (count == 0) //read all of current byte
        {
            fread(&byte, 1, 1, fptr);
            count = 8;
        }
        
        else
        {
            maskedByte = mask & byte; //get most significant bit
            byte <<= 1;
            count--;
            
            if (maskedByte == 0)
            {
                temp = temp -> left;
            }
            else if (maskedByte == 128)
            {
                temp = temp -> right;
            }
            
            if (isLeaf(temp))
            {
                if (temp -> freq == 0)
                {
                    break;
                }
                
                else
                {
                    fwrite(&(temp->data), sizeof(char), 1, optr);
                    temp = root;
                }
            }
            
        }
    }
}
