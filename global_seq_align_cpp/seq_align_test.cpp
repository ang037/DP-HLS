#include <stdio.h>
#include "seq_align.h"
#include "params.h"
#include <cstdlib>
#include <iostream>
#include "ap_int.h"
#include "ap_fixed.h"

using namespace std;

int main ()
{
	//initializing query sequence
	//FIXME - this can also be randomized
    char reference_string[ref_length] = {'A', 'G', 'A', 'A', 'C', 'A', 'A', 'G', 'G', 'C', 'G', 'T'};

    char query_string[query_length] =  {'A', 'C', 'A', 'A', 'G', 'A', 'C', 'A', 'G', 'C', 'G', 'T'};

    //generating random reference sequence of given length specified in params.h
   /* srand(time(NULL));

    char reference_string[ref_length];

    char alphabet[4] = { 'A', 'C', 'G','T'};
 
    for (int i = 0; i < ref_length; i++){
        reference_string[i] = alphabet[rand() % 4];
    }*/

    ap_fixed<M,N> dummy;

    //calling sequence alignment function
    seq_align(query_string, reference_string, &dummy);

    return 0;

}
