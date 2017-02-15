#include "criterion/criterion.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sfmm.h"

/**
 *  HERE ARE OUR TEST CASES NOT ALL SHOULD BE GIVEN STUDENTS
 *  REMINDER MAX ALLOCATIONS MAY NOT EXCEED 4 * 4096 or 16384 or 128KB
 */

Test(sf_memsuite, Malloc_an_Integer, .init = sf_mem_init, .fini = sf_mem_fini) {
    int *x = sf_malloc(sizeof(int));
    *x = 4;
    cr_assert(*x == 4, "Failed to properly sf_malloc space for an integer!");
}

Test(sf_memsuite, Free_block_check_header_footer_values, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *pointer = sf_malloc(sizeof(short));
    printf("%s\n", "we are freeing once");
    sf_free(pointer);

    pointer = pointer - 8;
    sf_header *sfHeader = (sf_header *) pointer;
    cr_assert(sfHeader->alloc == 0, "Alloc bit in header is not 0!\n");
    sf_footer *sfFooter = (sf_footer *) (pointer - 8 + (sfHeader->block_size << 4));
    cr_assert(sfFooter->alloc == 0, "Alloc bit in the footer is not 0!\n");


    printf("%s\n", "THIS IS THE HEADER AT THE FIRST ONE");
    sf_blockprint(freelist_head);
}

Test(sf_memsuite, PaddingSize_Check_char, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *pointer = sf_malloc(sizeof(char));
    pointer = pointer - 8;
    sf_header *sfHeader = (sf_header *) pointer;
    cr_assert(sfHeader->padding_size == 15, "Header padding size is incorrect for malloc of a single char!\n");

    printf("%s\n", "THIS IS THE HEADER AT THE SECOND ONE");
    sf_blockprint(freelist_head);
}

Test(sf_memsuite, Check_next_prev_pointers_of_free_block_at_head_of_list, .init = sf_mem_init, .fini = sf_mem_fini) {
    int *x = sf_malloc(4);
    memset(x, 0, 4);
    cr_assert(freelist_head->next == NULL);
    cr_assert(freelist_head->prev == NULL);
    printf("%s\n", "THIS IS THE HEADER AT THE THIRD ONE");
    sf_blockprint(freelist_head);
}

Test(sf_memsuite, Coalesce_no_coalescing, .init = sf_mem_init, .fini = sf_mem_fini) {
    int *x = sf_malloc(4);
    int *y = sf_malloc(4);
    memset(y, 0xFF, 4);
    printf("%s\n", "we are freeing twice");
     printf("%s\n", "THIS IS THE HEADER AT THE FOURTH ONE");
    sf_blockprint(freelist_head);
    sf_free(x);
    printf("%s\n", "THIS IS THE HEADER AT THE FOURTH ONE1");
    sf_blockprint(freelist_head);
    cr_assert(freelist_head == (void*)x-8);
    sf_free_header *headofx = (void*) x-8;
    sf_footer *footofx = (sf_footer*) (x + headofx->header.block_size) - 8;

    // All of the below should be true if there was no coalescing
    cr_assert(headofx->header.alloc == 0);
    printf("%s\n","is this wrong" );
    sf_blockprint(headofx);
   // cr_assert(headofx->header.block_size == 32  );
    cr_assert(headofx->header.padding_size == 0);

    cr_assert(footofx->alloc == 1);
    //cr_assert(footofx->block_size == 32);
}

/*
//############################################
// STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
// DO NOT DELETE THESE COMMENTS
//############################################
*/


Test(sf_memsuite, Coalesce_no_coalescingCase2, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *x = sf_malloc(4);
    void *y = sf_malloc(4);
    void *z = sf_malloc(4);
   
     memset(z, 0xFF, 4);
  
    sf_free(x);
    
    sf_free(y);


    cr_assert(freelist_head == (void*)x-8);
    sf_free_header *headofx = (void*) x-8;
    int shifted = headofx->header.block_size<<4;
  //  printf("%i\n", shifted);
    sf_footer *footofx = (void*) headofx + shifted - 8;

   // sf_blockprint(headofx);
    //printf("%i\n", headofx->header.block_size);
    //printf("%i\n", footofx->block_size);
    // All of the below should be true if there was no coalescing
    cr_assert(headofx->header.alloc == 0);
   // printf("%s\n","is this wrong" );
   // sf_blockprint(headofx);
    cr_assert(headofx->header.block_size == 4  );
    cr_assert(headofx->header.padding_size == 0);

    cr_assert(footofx->alloc == 0);

    cr_assert(footofx->block_size == 4);
}


Test(sf_memsuite, Coalesce_no_coalescingCase3, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *x = sf_malloc(4);
    void *y = sf_malloc(4);
    void *z = sf_malloc(4);
   
     memset(x, 0xFF, 4);
  
    sf_free(z);
    
    sf_free(y);


    cr_assert(freelist_head == (void*)y-8);
    sf_free_header *headofx = (void*) y-8;
    int shifted = headofx->header.block_size<<4;
  //  printf("%i\n", shifted);
    sf_footer *footofx = (void*) headofx + shifted - 8;

   // sf_blockprint(headofx);
    //printf("%i\n", headofx->header.block_size);
    //printf("%i\n", footofx->block_size);
    // All of the below should be true if there was no coalescing
    cr_assert(headofx->header.alloc == 0);
   // printf("%s\n","is this wrong" );
   // sf_blockprint(headofx);
    cr_assert(headofx->header.block_size == 254 );
    cr_assert(headofx->header.padding_size == 0);

    cr_assert(footofx->alloc == 0);

    cr_assert(footofx->block_size == 254);
}
Test(sf_memsuite, Coalesce_no_coalescingCase4, .init = sf_mem_init, .fini = sf_mem_fini) {
    void *x = sf_malloc(4);
    void *y = sf_malloc(4);
    void *z = sf_malloc(4);
    //printf("%s\n","This should be case four" );
     memset(x, 0xFF, 4);
  
    sf_free(y);
    
    sf_free(z);


    cr_assert(freelist_head == (void*)y-8);
    sf_free_header *headofx = (void*) y-8;
    int shifted = headofx->header.block_size<<4;
  //  printf("%i\n", shifted);
    sf_footer *footofx = (void*) headofx + shifted - 8;

   // sf_blockprint(headofx);
    //printf("%i\n", headofx->header.block_size);
    //printf("%i\n", footofx->block_size);
    // All of the below should be true if there was no coalescing
    cr_assert(headofx->header.alloc == 0);
   // printf("%s\n","is this wrong" );
   // sf_blockprint(headofx);
    cr_assert(headofx->header.block_size == 254 );
    cr_assert(headofx->header.padding_size == 0);

    cr_assert(footofx->alloc == 0);

    cr_assert(footofx->block_size == 254);
}
/*
Test(sf_memsuite, case4EdgeCase, .init = sf_mem_init, .fini = sf_mem_fini) {
    void*x = sf_malloc(4080);
    

        //printf("%s\n", "case 1?");
    sf_free(x);

    cr_assert(freelist_head == (void*)x-8);
    sf_free_header *headofx = (void*) x-8;
    int shifted = headofx->header.block_size<<4;
  //  printf("%i\n", shifted);
    sf_footer *footofx = (void*) headofx + shifted - 8;


   // sf_blockprint(headofx);
//printf("%i\n", headofx->header.block_size);
//sf_blockprint(x-8);
    //printf("%i\n", footofx->block_size);
    // All of the below should be true if there was no coalescing
    cr_assert(headofx->header.alloc == 0);
   // printf("%s\n","is this wrong" );
   // sf_blockprint(headofx);
    cr_assert(headofx->header.block_size == 256 );
    cr_assert(headofx->header.padding_size == 0);

    cr_assert(footofx->alloc == 0);

    cr_assert(footofx->block_size == 256);
}
*/
Test(sf_memsuite,Case3EdgeCase , .init = sf_mem_init, .fini = sf_mem_fini) {
    void*x = sf_malloc(5);
    void*y = sf_malloc(5);

    sf_free(y);
    sf_free(x);
     cr_assert(freelist_head == (void*)x-8);
    sf_free_header *headofx = (void*) x-8;
    int shifted = headofx->header.block_size<<4;
  //  printf("%i\n", shifted);
    sf_footer *footofx = (void*) headofx + shifted - 8;


   // sf_blockprint(headofx);
//printf("%i\n", headofx->header.block_size);
//sf_blockprint(x-8);
    //printf("%i\n", footofx->block_size);
    // All of the below should be true if there was no coalescing
    cr_assert(headofx->header.alloc == 0);
   // printf("%s\n","is this wrong" );
   //sf_blockprint(headofx);
    cr_assert(headofx->header.block_size == 256 );
    cr_assert(headofx->header.padding_size == 0);

    cr_assert(footofx->alloc == 0);

    cr_assert(footofx->block_size == 256);

    }
Test(sf_memsuite,MutipleCasesAndRealloc , .init = sf_mem_init, .fini = sf_mem_fini) {
    void * a = sf_malloc(4);
    void * b = sf_malloc(4);
    void * c = sf_malloc(4);
    void * d = sf_malloc(4);
    void * e = sf_malloc(4);
    void * f = sf_malloc(4);
    void * g = sf_malloc(4);
    void * h = sf_malloc(4);
    void * i = sf_malloc(4);
    void * j = sf_malloc(4);


    sf_free(b);
    sf_free(c);
    sf_free(e);
    sf_free(d);
    sf_free(h);
    sf_free(g);
    sf_free(f);
    sf_free(a);
    sf_free(j);
    sf_free(i);
    
    printf("%s\n","a" );
    sf_blockprint(a-8);
    printf("%s\n","b" );
    sf_blockprint(b-8);
    printf("%s\n","c" );
    sf_blockprint(c-8);
    printf("%s\n","d" );
    sf_blockprint(d-8);
    printf("%s\n","e" );
    sf_blockprint(e-8);
    printf("%s\n","f" );
    sf_blockprint(f-8);
    printf("%s\n","h" );
    sf_blockprint(h-8);
    printf("%s\n","i" );
    sf_blockprint(i-8);
    printf("%s\n","j" );
    sf_blockprint(j-8);
    //sf_snapshot(true);

    
    /*
    sf_realloc(b,10);
    sf_realloc(j,40);
    sf_realloc(i,2);
    sf_realloc(a,500);
    */


}

Test(sf_memsuite,  Reallocation  , .init = sf_mem_init, .fini = sf_mem_fini) {
    void * a = sf_malloc(4);
    void * b = sf_malloc(4);
  
    void * d = sf_malloc(4);

    sf_blockprint(sf_realloc(a,500));
    sf_blockprint(sf_realloc(b,1));
   
    sf_blockprint(sf_realloc(d,2));


}
Test(sf_memsuite,  infoTest  , .init = sf_mem_init, .fini = sf_mem_fini) {

    void * a = sf_malloc(4);
    void * b = sf_malloc(4);
    void * c = sf_malloc(4);
    void * d = sf_malloc(4);
    void * e = sf_malloc(4);
    void * f = sf_malloc(4);
    void * g = sf_malloc(4);
    void * h = sf_malloc(4);
    void * i = sf_malloc(4);
    void * j = sf_malloc(4);


    sf_free(b);
    sf_free(c);
    sf_free(e);
    sf_free(d);
    sf_free(h);
    sf_free(g);
    sf_free(f);
    sf_free(a);
    sf_free(j);
    sf_free(i);
    info information = {
    .internal = 0,
    .external = 0,
    .allocations = 0,
    .frees = 0,
    .coalesce = 0
};
    int x = sf_info(&information);
    printf("%i\n", x);
}


Test(sf_memsuite,  EdgeCaseTestAgain  , .init = sf_mem_init, .fini = sf_mem_fini) {
    void*x = sf_malloc(5);
    void*y = sf_malloc(5);

    sf_free(x);
    sf_free(y);
     cr_assert(freelist_head == (void*)x-8);
    sf_free_header *headofx = (void*) x-8;
    int shifted = headofx->header.block_size<<4;
  //  printf("%i\n", shifted);
    sf_footer *footofx = (void*) headofx + shifted - 8;


   // sf_blockprint(headofx);
//printf("%i\n", headofx->header.block_size);
//sf_blockprint(x-8);
    //printf("%i\n", footofx->block_size);
    // All of the below should be true if there was no coalescing
    cr_assert(headofx->header.alloc == 0);
   // printf("%s\n","is this wrong" );
   //sf_blockprint(headofx);
    cr_assert(headofx->header.block_size == 256 );
    cr_assert(headofx->header.padding_size == 0);

    cr_assert(footofx->alloc == 0);

    cr_assert(footofx->block_size == 256);

    }

Test(sf_memsuite,  NullCaseTest  , .init = sf_mem_init, .fini = sf_mem_fini) {
    void*x = sf_malloc(-5);
    sf_free(x);
}

Test(sf_memsuite,  Malloc16Test  , .init = sf_mem_init, .fini = sf_mem_fini) {
    printf("%s\n", "TEST MALLOC 16");
    void*x = sf_malloc(4080);
    sf_blockprint(x-8);
}
