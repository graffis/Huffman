//
//  main.c
//  proj2
//
//  Created by Owen Graffis on 10/12/19.
//  Copyright © 2019 Owen Graffis. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "huff.h"
#include "unhuff.h"

int main(int argc, const char * argv[]) {
    
    
    char filename[100];
    printf("Enter input filename:\n");
    scanf("%s", filename);
    huff(filename); //output is Output.txt
    unhuff(filename);
    printf("Done\n");
    
    return 0;
}
