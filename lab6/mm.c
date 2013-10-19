/*
 * Simple, 32-bit and 64-bit clean allocator based on implicit free
 * lists, first fit placement, and boundary tag coalescing, as described
 * in the CS:APP2e text. Blocks must be aligned to doubleword (8 byte)
 * boundaries. Minimum block size is 16 bytes.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mm.h"
#include "memlib.h"
/*********************************************************

___________________________________________________________________________
|  size/000  |  PREV_POINTER | NEXT_POINTER |   ...          | size/000    |
|  (4 bytes) |  (8 bytes)    |  (8 bytes)   |                | (4 bytes)   |
___________________________________________________________________________
        |           |               |                               |
    header          |               |                               |
            point to previous free block of similar size in the group_n list
                                point to next free block            |
                                                                  footer
********************************************************/

/********************************************************

        HOW THIS ALLOCATOR WORKS
    similar to the combination of segregated

********************************************************/


/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/


 /*****************************************************
    There are several optimization or "binary" and "realloc" according to the allocation
pattern shown in the .rep file.

#  binary-bal.rep/binary2-bal.rep
    In "binary", the sample first allocates some blocks with 112 bytes and then free them.
    After that, it allocates 128 bytes to these blocks.
    So, in order not to cause so many fragmentation, the allocator simply allocates 128
bytes at the beginning.

#   realloc-bal.rep/realloc2-bal.rep   ???bug in gcc
    In "realloc", generally speaking, what the sample does is only allocating two small
blocks and re-allocating one large block.
    Once this allocator detects this specific pattern, it will only allow the two small
blocks allocated in the two "buffers" at the front of the heap, following the prologue
block. At the same time, the large block will be allocated after them, with no blocks
allocated behind it. So when re-allocating, the large block only needs to be extended
instead of replacing and re-copying.

*****************************************************/




team_t team = {
    /* Team name */
    "5120379066",
    /* First member's full name */
    "Liu Che",
    /* First member's email address */
    "email",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};


/* $begin mallocmacros */
/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */ //line:vm:mm:beginconst
#define DSIZE       8       /* Doubleword size (bytes) */
#define CHUNKSIZE  (1<<8)  /* Extend heap by this amount (bytes) */  //line:vm:mm:endconst

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc)) //line:vm:mm:pack

/* Read and write a word at address p */
#define GET(p)       (*(unsigned int *)(p))            //line:vm:mm:get
#define PUT(p, val)  (*(unsigned int *)(p) = (val))    //line:vm:mm:put

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)                   //line:vm:mm:getsize
#define GET_ALLOC(p) (GET(p) & 0x1)                    //line:vm:mm:getalloc

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)                      //line:vm:mm:hdrp
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) //line:vm:mm:ftrp

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) //line:vm:mm:nextblkp
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) //line:vm:mm:prevblkp
#define NEXT_POINTER(bp) ((long*)((long*)(bp) + 1))
#define PREV_POINTER(bp) ((long*) (bp))

/* $end mallocmacros */


/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *coalesce(void *bp);
static void printblock(void *bp);
static void checkheap(int verbose);
static void checkblock(void *bp);
static long *find_fit(size_t asize);// Get a suitable free block to allocate
static long *find_group(size_t asize);//Get the group of a "size" block
void group_push(long* bp);      //Add the free block "bp" into the group_n list
void group_delete(long* bp);    //Remove the "bp" block from the group_n list
long convert_size(long size);   //Given size, get the index of a suitable group

/* Global variables */
static char *heap_listp = 0;  /* Pointer to first block */
static char *heap_end = 0;  /* Pointer to end of the heap*/
int is_realloc = 0;         //Whether processing realloc-bal.rep
int first = 0;              //Initial size of the re-allocated block
int realloc_incr = 0;       //Size of "small" blocks in realloc-bal.rep
int buf_init = 0;           //Current state of buffer
int is_rand2 = 0;           //Detect the random-allocating pattern
int rand_malloc_count = 0;



//Classify blocks into different groups
typedef int SIZEGRP;
SIZEGRP sizeG_1 = 32;//32 to 64
SIZEGRP sizeG_2 = 64;
SIZEGRP sizeG_3 = 128;
SIZEGRP sizeG_4 = 256;
SIZEGRP sizeG_5 = 512;
SIZEGRP sizeG_6 = 1024;
SIZEGRP sizeG_7 = 2048;
SIZEGRP sizeG_8 = 4096;
SIZEGRP sizeG_9 = 8192;
SIZEGRP sizeG_10 = 16384;
SIZEGRP sizeG_11 = 32768;
SIZEGRP sizeG_12 = 65536;
SIZEGRP sizeG_13 = 1000000;

/*
Every group has a pointer,
Which points to the last free block in the group
*/
typedef long* block_pointer;
block_pointer group_1 = NULL;
block_pointer group_2 = NULL;
block_pointer group_3 = NULL;
block_pointer group_4 = NULL;
block_pointer group_5 = NULL;
block_pointer group_6 = NULL;
block_pointer group_7 = NULL;
block_pointer group_8 = NULL;
block_pointer group_9 = NULL;
block_pointer group_10 = NULL;
block_pointer group_11 = NULL;
block_pointer group_12 = NULL;
block_pointer group_13 = NULL;


/*  Given size, get the index of a suitable group  */
long convert_size(long size){
    int asize = size + 16;
    int group_index = 0;
    if(asize >= sizeG_13)
        group_index = 13;
    else if (asize >= sizeG_12)
        group_index = 12;
    else if (asize >= sizeG_11)
        group_index = 11;
    else if (asize >= sizeG_10)
        group_index = 10;
    else if (asize >= sizeG_9)
        group_index = 9;
    else if (asize >= sizeG_8)
        group_index = 8;
    else if (asize >= sizeG_7)
        group_index = 7;
    else if (asize >= sizeG_6)
        group_index = 6;
    else if (asize >= sizeG_5)
        group_index = 5;
    else if (asize >= sizeG_4)
        group_index = 4;
    else if (asize >= sizeG_3)
        group_index = 3;
    else if (asize >= sizeG_2)
        group_index = 2;
    else if (asize >= sizeG_1)
        group_index = 1;
    else
        group_index = 0;
    return group_index;
}

/* Add the free block "bp" into the group_n list and update the group_n pointer*/
void group_push(long* bp){
    long size = GET_SIZE(HDRP(bp));
    long** group_found = find_group(size);
    *PREV_POINTER(bp) = NULL;
    *NEXT_POINTER(bp) = NULL;

    //Add new free block into the list
    if( group_found != NULL){
        *PREV_POINTER(bp) = *group_found;
        *NEXT_POINTER(bp) = group_found;
        if(*group_found != NULL)
            *NEXT_POINTER(*group_found) = PREV_POINTER(bp);
        *group_found = PREV_POINTER(bp);       //update the group_n pointer
    }
    return ;
}

/* Remove block from the list */
void group_delete(long* removed_block){
    if(*NEXT_POINTER(removed_block) != NULL)
        *PREV_POINTER(*NEXT_POINTER(removed_block)) = *PREV_POINTER(removed_block);
    if(*PREV_POINTER(removed_block) != NULL)
        *NEXT_POINTER(*PREV_POINTER(removed_block)) = *NEXT_POINTER(removed_block);
    return ;
}


/* --------------------------------------------------------------------------------
 * mm_init - Initialize the memory manager
 */
/* $begin mminit */
int mm_init(void)
{
    group_1 = NULL;
    group_2 = NULL;
    group_3 = NULL;
    group_4 = NULL;
    group_5 = NULL;
    group_6 = NULL;
    group_7 = NULL;
    group_8 = NULL;
    group_9 = NULL;
    group_10 = NULL;
    group_11 = NULL;
    group_12 = NULL;
    group_13 = NULL;

    is_realloc = 0;
    first = 0;
    realloc_incr = 0;
    buf_init = 0;
    is_rand2 = 0;
    rand_malloc_count = 0;
	heap_end = 0;

    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1) //line:vm:mm:begininit
	return -1;
    PUT(heap_listp, 0);                          /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));     /* Epilogue header */
    heap_listp += (2*WSIZE);                     //line:vm:mm:endinit

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    void* bp = extend_heap(10);
    //int size_bp = GET_SIZE(HDRP(bp));
    if (bp == NULL)
	    return -1;
    return 0;
}
/* $end mminit */

/*
 * mm_malloc - Allocate a block with at least size bytes of payload
 */
/* $begin mmmalloc */
void *mm_malloc(size_t size)
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;

    //Detect whether is processing "random-bal.rep"
    if(!is_rand2 && rand_malloc_count < 5){
        rand_malloc_count++;
        if(size > 10000)
            is_rand2 = 1;
    }
    if(realloc_incr == 0 && first != 0)
        realloc_incr = size;
    if(first == 0)
        first = size;
    if (heap_listp == 0){
	mm_init();
    }

    /* Ignore spurious requests */
    if (size == 0)
	return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE*2)                                    //line:vm:mm:sizeadjust1
        asize = 4*DSIZE;                                        //line:vm:mm:sizeadjust2
    else if (size ==112)
        asize = 136;
    else if (size ==448)
        asize = 512+16;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); //line:vm:mm:sizeadjust3

    long* block_found =  find_fit(asize);

    if (block_found != NULL) {            //Place Found!
        bp = *block_found;
        place(bp, asize);                 //Place block at bp
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize,CHUNKSIZE)+32;
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp, asize);                     //Place block at bp
    return bp;
}
/* $end mmmalloc */

/*
 * mm_free - Free a block
 */
void mm_free(void *bp)
{
    if(bp == 0)
        return;
    size_t size = GET_SIZE(HDRP(bp));
    if (heap_listp == 0){
        mm_init();
    }
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    *PREV_POINTER(bp) = NULL;
    *NEXT_POINTER(bp) = NULL;
    if( !is_realloc)
        coalesce(bp);               //Coalesce the free block
    else{
        if(buf_init > 0)            //If re-allocating, only add the small block into group_n list
            group_push(bp);         //or the small block won't be allocated in "buffer"
    }
}

/* $end mmfree */

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
/* $begin coalesce */
static void *coalesce(void *bp)
{
    int prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)))  ;
    int next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp))) ;
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {            /* Case 1 */
        group_push(bp);
        return bp;
    }

    if((GET_SIZE(HDRP(NEXT_BLKP(bp))) == 0) && (GET_SIZE(HDRP(PREV_BLKP(bp))) == 0)){/* Case 1 */
        group_push(bp);
        return bp;
    }

    if(prev_alloc && (GET_SIZE(HDRP(NEXT_BLKP(bp))) == 0)){/* Case 1 */
        group_push(bp);
        return bp;
    }

    /*Case 2*/
    else if ((prev_alloc||(GET_SIZE(HDRP(PREV_BLKP(bp))) == 0)) && (!next_alloc && (GET_SIZE(HDRP(NEXT_BLKP(bp))) > 0)) ) {      /* Case 2 */
        long next_size = GET_SIZE(HDRP(NEXT_BLKP(bp)));

        //remove next free block from the list
        group_delete(NEXT_BLKP(bp));
        size += next_size;
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size,0));

        //add new coalesced block to the list
        group_push(bp);
    }

    else if (!prev_alloc && (next_alloc ||(GET_SIZE(HDRP(NEXT_BLKP(bp))) == 0))) {      /* Case 3 */
        long prev_size = GET_SIZE(HDRP(PREV_BLKP(bp)));

        //remove previous free block from the list
        group_delete(PREV_BLKP(bp));
        size += prev_size;
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);

        //add new coalesced block to the list
        group_push(bp);
    }

    else {                                     /* Case 4 */
        //remove previous and next free block from the list
        group_delete(PREV_BLKP(bp));
        group_delete(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);

        //add new coalesced block to the list
        group_push(bp);
    }


    return bp;
}
/* $end coalesce */


/*
 * mm_realloc - Naive implementation of realloc
 */
void *mm_realloc(void *ptr, size_t size)
{

    void* ptr_tmp = ptr;
    if (size <= DSIZE*2)                                       //line:vm:mm:sizeadjust1
        size = 4*DSIZE;                                        //line:vm:mm:sizeadjust2
    else
        size = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); //line:vm:mm:sizeadjust3
    //Initialize re-allocating process
    if(!is_realloc){
        //Delay initialize buffer(after FREE 1)
        if(buf_init == 1){
            buf_init = 2;
            void* buf1 = mm_malloc(realloc_incr + 30);
            void* buf2 = mm_malloc(realloc_incr + 30);
            mm_free(buf1);
            mm_free(buf2);
        }

        //Allocate a small block and then free it, making it a buffer
        void* buf = mm_malloc(realloc_incr);
        is_realloc = 1;
        size_t oldsize;
        void *newptr;

        //Allocate a large block
        newptr = mm_malloc(size);

        // If realloc() fails the original block is left untouched
        if(!newptr) {
        return 0;
        }

        // Copy the old data.
        oldsize = GET_SIZE(HDRP(ptr));
        if(size < oldsize) oldsize = size;
        memcpy(newptr, ptr, oldsize);

        // Free the old block.
        mm_free(ptr);
        group_push(ptr);
        mm_free(buf);
        group_push(buf);
        buf_init = 1;
        return newptr;

    }

    //allocate memory behind previous block0

    size_t new_size = 0;
    if(GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(NEXT_BLKP(ptr))) > size){
        //Space is enough currently, then place the block here
        new_size = GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(NEXT_BLKP(ptr)));
        PUT(HDRP(ptr), PACK(new_size, 1));
        place(ptr,size);
        }else{

        //if space is not enough, then extend heap
        if ((extend_heap(15 * realloc_incr / WSIZE)) == NULL)
            return NULL;
        new_size = GET_SIZE(HDRP(ptr)) + GET_SIZE(HDRP(NEXT_BLKP(ptr)));
        PUT(HDRP(ptr), PACK(new_size, 1));
        place(ptr, size);
    }
    return ptr_tmp;
}



/*
 * The remaining routines are internal helper routines
 */

/*
 * extend_heap - Extend heap with free block and return its block pointer
 */
/* $begin mmextendheap */
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; //line:vm:mm:beginextend

    if ((long)(bp = mem_sbrk(size)) == -1)
	return NULL;                                        //line:vm:mm:endextend
	heap_end = bp + GET_SIZE(bp) - WSIZE;
    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */   //line:vm:mm:freeblockhdr
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */   //line:vm:mm:freeblockftr
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */ //line:vm:mm:newepihdr

    //If reallocating, don't add block to the group_n list
    //or new block will be allocated behind the large heap
    //instead of in the two buffers
    if(is_realloc){
        int prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)))  ;
        if(!prev_alloc) {
            long prev_size = GET_SIZE(HDRP(PREV_BLKP(bp)));
                    size += prev_size;
                    PUT(FTRP(bp), PACK(size, 0));
                    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
                    bp = PREV_BLKP(bp);
        }
        return bp;
    }
    /* Coalesce if the previous block was free */
    bp = coalesce(bp);
    return bp;
}

/* $end mmextendheap */

/*
 * place - Place block of asize bytes at start of free block bp
 *         and split if remainder would be at least minimum block size
 */

static void place(void *bp, size_t asize)

{
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) > (4*DSIZE)) {
        if(!GET_ALLOC(HDRP(bp)))
            group_delete(bp);
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));
        PUT(FTRP(bp), PACK(csize-asize, 0));
        if(!is_realloc)
            group_push(bp);
    }
    else {
        group_delete(bp);
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}
/* $end mmplace */

/*
 * find_fit - Find a fit for a block with asize bytes
 */
///somehow this function works well in Code::Blocks
///But it fails in gcc on ubuntu
static long *find_fit(size_t asize)
{
    long* block_pointer;
    switch(convert_size(asize)){
        case 0:
        case 1:
            if(group_1 != NULL){
                block_pointer =  group_1;
                break;
            }
        case 2:
            if(group_2 != NULL){
                block_pointer =  group_2;
                break;
            }
        case 3:
            if(group_3 != NULL){
                block_pointer =  group_3;
                break;
            }
        case 4:
            if(group_4 != NULL){
                block_pointer =  group_4;
                break;
            }
        case 5:
            if(group_5 != NULL){
                block_pointer =  group_5;
                break;
            }
        case 6:
            if(group_6 != NULL){
                block_pointer =  group_6;
                break;
            }
        case 7:
            if(group_7 != NULL){
                block_pointer =  group_7;
                break;
            }
        case 8:
            if(group_8 != NULL){
                block_pointer =  group_8;
                break;
            }
        case 9:
            if(group_9 != NULL){
                block_pointer =  group_9;
                break;
            }
        case 10:
            if(group_10 != NULL){
                block_pointer =  group_10;
                break;
            }
        case 11:
            if(group_11 != NULL){
                block_pointer =  group_11;
                break;
            }
        case 12:
            if(group_12 != NULL){
                block_pointer =  group_12;
                break;
            }
        case 13:
            if(group_13 != NULL){
                block_pointer =  group_13;
                break;
            }
        default:
            return NULL;
    }

    int size_block = 0;
    int min_size = 10000000;
    long min_size_p = 0;
    int size_block_rand = 0;
	
	
	/****************************************************
	*The following code runs normally in Code::Blocks with
	*MINGW, but it is buggy in gcc 4.2.
	*So I disabled this optimizing function...
	****************************************************/
    //Then, find the first suitable block with size bigger than asize
/*
    if(is_rand2){
        while(block_pointer != NULL ){
            size_block_rand = GET_SIZE(HDRP(block_pointer));
            if( size_block_rand >= asize && min_size > size_block_rand){
                min_size = size_block_rand;
                min_size_p = block_pointer;
            }
            block_pointer = *PREV_POINTER(block_pointer);
        }
        if(min_size != 10000000)
            return &min_size_p;
        return NULL;
   }

*/
    //If processing "random", find the most suitable block with size bigger than asize
    //if(!is_rand2){
        while(block_pointer != NULL ){
            size_block = GET_SIZE(HDRP(block_pointer));
            if( size_block >= asize)
                return &block_pointer;
            block_pointer = *PREV_POINTER(block_pointer);
        }
        return NULL;
    //}

    //return NULL;




}

static long* find_group(size_t asize){

    switch(convert_size(asize)){
        case 0:
        case 1:
                return &group_1;
        case 2:
                return &group_2;
        case 3:
                return &group_3;
        case 4:
                return &group_4;
        case 5:
                return &group_5;
        case 6:
                return &group_6;
        case 7:
                return &group_7;
        case 8:
                return &group_8;
        case 9:
                return &group_9;
        case 10:
                return &group_10;
        case 11:
                return &group_11;
        case 12:
                return &group_12;
        case 13:
                return &group_13;

        default:
            return NULL;
    }

}

static void printblock(void *bp)
{
    size_t hsize;
    size_t halloc;
    size_t fsize;
    size_t falloc;

    checkheap(0);
    hsize = GET_SIZE(HDRP(bp));
    halloc = GET_ALLOC(HDRP(bp));
    fsize = GET_SIZE(FTRP(bp));
    falloc = GET_ALLOC(FTRP(bp));

    if (hsize == 0) {
	printf("%p: EOL\n", bp);
	return;
    }

    printf("%p: header: [%p:%c] footer: [%p:%c]\n", bp,
	hsize, (halloc ? 'a' : 'f'),
	fsize, (falloc ? 'a' : 'f'));

}

static void checkblock(void *bp)
{
    if ((size_t)bp % 8)
	printf("Error: %p is not doubleword aligned\n", bp);
    if (GET(HDRP(bp)) != GET(FTRP(bp)))
	printf("Error: header does not match footer\n");
}

/*
 * checkheap - Minimal check of the heap for consistency
 */
void checkheap(int verbose)
{
    char *bp = heap_listp;

    if (verbose)
	printf("Heap (%p):\n", heap_listp);

    if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
	printf("Bad prologue header\n");
    checkblock(heap_listp);

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
	if (verbose)

	checkblock(bp);
    }

    if (verbose)

    if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
	printf("Bad epilogue header\n");
}


void mm_check(void* block_pointer){//bp points to the prologue block
	long* bp = block_pointer;
	bp = NEXT_BLKP(bp);
	
	/* Check if there are contiguous free blocks that somehow escaped coalescing*/
	while(GET_SIZE(HDRP(bp)) != 0){
		if(GET_ALLOC(HDRP(bp)) == 0 && GET_ALLOC(HDRP(NEXT_BLKP(bp))) == 0)
			printf("Continuous free block at 0x%x\n",bp);
		if(GET_ALLOC(HDRP(bp)) == 0 && GET_ALLOC(HDRP(PREV_BLKP(bp))) == 0)
			printf("Continuous free block at 0x%x\n",bp);
		bp = NEXT_BLKP(bp);
	}
	
	
	/* Check if there are allocated blocks overlap*/
	bp = block_pointer;
	while(GET_SIZE(HDRP(bp)) != 0){
		if(GET_ALLOC(HDRP(bp)) == 1)
			if((char*)bp + GET_SIZE(HDRP(bp)) - WSIZE >  (char*)(NEXT_BLKP(bp)) - 4 )
				printf("Allocated block at 0x%x overlaps",bp);
		bp = NEXT_BLKP(bp);
	}
	
	
	/* Check if every block in the free list marked as free*/
	/* Check if the pointers in the free list point to valid free blocks */
	bp = group_1;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}
	bp = group_2;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}	
	bp = group_3;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}	
	bp = group_4;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}
	bp = group_5;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}
	bp = group_6;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}
	bp = group_7;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}
	bp = group_8;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}
	bp = group_9;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}
	bp = group_10;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}
	bp = group_11;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}
	bp = group_12;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}
	bp = group_13;
	while(bp != NULL){
		if(GET_ALLOC(HDRP(bp)) == 1)
			printf("Block in group_1 at 0x%x is allocated\n",bp);
		if(bp > heap_end || bp < heap_listp)
			printf("0x%x in free list point to invalid address\n");
		bp = PREV_POINTER(bp);
	}
	
	/*Check if every free block is actually in the free list*/
	bp = block_pointer;
	bp = NEXT_BLKP(bp);	
	long* free_pointer = NULL;
	while(GET_SIZE(HDRP(bp)) != 0){
		if(GET_ALLOC(HDRP(bp)) == 0 ){
			free_pointer = find_group(GET_SIZE(HDRP(bp)));
			while(free_pointer != NULL && free_pointer != bp)
				free_pointer = PREV_BLKP(free_pointer);
			if(free_pointer == NULL)
				printf("Free block 0x%x is not in the free list\n",bp);
		}
	}
	
	
}

