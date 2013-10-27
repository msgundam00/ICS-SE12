/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"


/*****************DESCRPITION************************

PRG1:Rotate
	--Eliminate function calls
	--Use Unrolling, 16 pixels in a group
	--Increase Write Hit chance

PRG2:Smooth
	--In line 1, row 1, line dim-1, row dim-1, 
	  Store block value while reading, and when
	  processing next dst block, the value can 
	  be used again.
	--In line 2 to dim-1, group elements by 2*2.
	  Scan all the value around this 2*2 block, 
	  and store the value into destination.


****************************************************/











/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "ics5120379066",              /* Team name */

    "ics5120379066",                /* First member Route-Y ID */
    "Liu Che",     /* First member full name */
    "",  /* First member email address */

    "",                   /* Second member Route-Y ID */
    "",                   /* Second member full name (leave blank if none) */
    ""                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */


#define BLOCK 32

char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}


char rotate2_descr[] = "Rotate2 : Unrolling, 4*4";  
void rotate2(int dim, pixel *src, pixel *dst)   
{   
   int i,j,m,n;
    for (m = 0; m < dim; m += 32 )
	for (n = 0; n < dim; n += 32 )
	{
		for(j=m;j<m+32;j+=4)
		{
			for (i=n;i<n+32;i+=4)
			{
				dst[(dim-1-j)*dim+i] = src[i*dim+j];
				dst[(dim-2-j)*dim+i] = src[i*dim+j+1];
				dst[(dim-3-j)*dim+i] = src[i*dim+j+2];
				dst[(dim-4-j)*dim+i] = src[i*dim+j+3];
			}
			for (i=n;i<n+32;i+=4)
			{
				dst[(dim-1-j)*dim+i+1] = src[(i+1)*dim+j];
				dst[(dim-2-j)*dim+i+1] = src[(i+1)*dim+j+1];
				dst[(dim-3-j)*dim+i+1] = src[(i+1)*dim+j+2];
				dst[(dim-4-j)*dim+i+1] = src[(i+1)*dim+j+3];
			}
			for (i=n;i<n+32;i+=4)
			{
				dst[(dim-1-j)*dim+i+2] = src[(i+2)*dim+j];
				dst[(dim-2-j)*dim+i+2] = src[(i+2)*dim+j+1];
				dst[(dim-3-j)*dim+i+2] = src[(i+2)*dim+j+2];
				dst[(dim-4-j)*dim+i+2] = src[(i+2)*dim+j+3];
			}
			for (i=n;i<n+32;i+=4)
			{
				dst[(dim-1-j)*dim+i+3] = src[(i+3)*dim+j];
				dst[(dim-2-j)*dim+i+3] = src[(i+3)*dim+j+1];
				dst[(dim-3-j)*dim+i+3] = src[(i+3)*dim+j+2];
				dst[(dim-4-j)*dim+i+3] = src[(i+3)*dim+j+3];
			}
		}
	}	

}  
/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate8_descr[] = "rotate8: Current working version, 4*8";
void rotate8(int dim, pixel *src, pixel *dst) 
{
    int i, j, m, n;
    for (i = 0; i < dim; i += BLOCK )
	for (j = 0; j < dim; j += BLOCK ){
		for(n = i ; n < BLOCK + i; n+=4){
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m] = src[m * dim + n];
				dst[(dim - 2 - n) * dim + m] = src[m * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m] = src[m * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m] = src[m * dim + n + 3] ;
				
			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 1] = src[(m+1) * dim + n];
				dst[(dim - 2 - n) * dim + m + 1] = src[(m+1) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 1] = src[(m+1) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 1] = src[(m+1) * dim + n + 3] ;
			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 2] = src[(m+2) * dim + n];
				dst[(dim - 2 - n) * dim + m + 2] = src[(m+2) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 2] = src[(m+2) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 2] = src[(m+2) * dim + n + 3] ;
			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 3] = src[(m+3) * dim + n];
				dst[(dim - 2 - n) * dim + m + 3] = src[(m+3) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 3] = src[(m+3) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 3] = src[(m+3) * dim + n + 3] ;
			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 4] = src[(m+4) * dim + n];
				dst[(dim - 2 - n) * dim + m + 4] = src[(m+4) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 4] = src[(m+4) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 4] = src[(m+4) * dim + n + 3] ;

			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 5] = src[(m+5) * dim + n];
				dst[(dim - 2 - n) * dim + m + 5] = src[(m+5) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 5] = src[(m+5) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 5] = src[(m+5) * dim + n + 3] ;
			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 6] = src[(m+6) * dim + n];
				dst[(dim - 2 - n) * dim + m + 6] = src[(m+6) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 6] = src[(m+6) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 6] = src[(m+6) * dim + n + 3] ;
			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 7] = src[(m+7) * dim + n];
				dst[(dim - 2 - n) * dim + m + 7] = src[(m+7) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 7] = src[(m+7) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 7] = src[(m+7) * dim + n + 3] ;
			}
		}
	}
	    
}

char rotate6_descr[] = "rotate: Current working version, 8*8";
void rotate6(int dim, pixel *src, pixel *dst) 
{
    int i, j, m, n;
    for (i = 0; i < dim; i += BLOCK )
	for (j = 0; j < dim; j += BLOCK ){
		for(n = i ; n < BLOCK + i; n+=8){
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m] = src[m * dim + n];
				dst[(dim - 2 - n) * dim + m] = src[m * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m] = src[m * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m] = src[m * dim + n + 3] ;
				dst[(dim - 5 - n) * dim + m] = src[m * dim + n + 4];
				dst[(dim - 6 - n) * dim + m] = src[m * dim + n + 5] ;
				dst[(dim - 7 - n) * dim + m] = src[m * dim + n + 6] ;
				dst[(dim - 8 - n) * dim + m] = src[m * dim + n + 7] ;
				
			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 1] = src[(m+1) * dim + n];
				dst[(dim - 2 - n) * dim + m + 1] = src[(m+1) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 1] = src[(m+1) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 1] = src[(m+1) * dim + n + 3] ;
				dst[(dim - 5 - n) * dim + m + 1] = src[(m+1) * dim + n + 4] ;
				dst[(dim - 6 - n) * dim + m + 1] = src[(m+1) * dim + n + 5] ;
				dst[(dim - 7 - n) * dim + m + 1] = src[(m+1) * dim + n + 6] ;
				dst[(dim - 8 - n) * dim + m + 1] = src[(m+1) * dim + n + 7] ;
			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 2] = src[(m+2) * dim + n];
				dst[(dim - 2 - n) * dim + m + 2] = src[(m+2) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 2] = src[(m+2) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 2] = src[(m+2) * dim + n + 3] ;
				dst[(dim - 5 - n) * dim + m + 2] = src[(m+2) * dim + n + 4] ;
				dst[(dim - 6 - n) * dim + m + 2] = src[(m+2) * dim + n + 5] ;
				dst[(dim - 7 - n) * dim + m + 2] = src[(m+2) * dim + n + 6] ;
				dst[(dim - 8 - n) * dim + m + 2] = src[(m+2) * dim + n + 7] ;
			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 3] = src[(m+3) * dim + n];
				dst[(dim - 2 - n) * dim + m + 3] = src[(m+3) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 3] = src[(m+3) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 3] = src[(m+3) * dim + n + 3] ;
				dst[(dim - 5 - n) * dim + m + 3] = src[(m+3) * dim + n + 4] ;
				dst[(dim - 6 - n) * dim + m + 3] = src[(m+3) * dim + n + 5] ;
				dst[(dim - 7 - n) * dim + m + 3] = src[(m+3) * dim + n + 6] ;
				dst[(dim - 8 - n) * dim + m + 3] = src[(m+3) * dim + n + 7] ;
			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 4] = src[(m+4) * dim + n];
				dst[(dim - 2 - n) * dim + m + 4] = src[(m+4) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 4] = src[(m+4) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 4] = src[(m+4) * dim + n + 3] ;
				dst[(dim - 5 - n) * dim + m + 4] = src[(m+4) * dim + n + 4] ;
				dst[(dim - 6 - n) * dim + m + 4] = src[(m+4) * dim + n + 5] ;
				dst[(dim - 7 - n) * dim + m + 4] = src[(m+4) * dim + n + 6] ;
				dst[(dim - 8 - n) * dim + m + 4] = src[(m+4) * dim + n + 7] ;

			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 5] = src[(m+5) * dim + n];
				dst[(dim - 2 - n) * dim + m + 5] = src[(m+5) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 5] = src[(m+5) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 5] = src[(m+5) * dim + n + 3] ;
				dst[(dim - 5 - n) * dim + m + 5] = src[(m+5) * dim + n + 4] ;
				dst[(dim - 6 - n) * dim + m + 5] = src[(m+5) * dim + n + 5] ;
				dst[(dim - 7 - n) * dim + m + 5] = src[(m+5) * dim + n + 6] ;
				dst[(dim - 8 - n) * dim + m + 5] = src[(m+5) * dim + n + 7] ;
			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 6] = src[(m+6) * dim + n];
				dst[(dim - 2 - n) * dim + m + 6] = src[(m+6) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 6] = src[(m+6) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 6] = src[(m+6) * dim + n + 3] ;
				dst[(dim - 5 - n) * dim + m + 6] = src[(m+6) * dim + n + 4] ;
				dst[(dim - 6 - n) * dim + m + 6] = src[(m+6) * dim + n + 5] ;
				dst[(dim - 7 - n) * dim + m + 6] = src[(m+6) * dim + n + 6] ;
				dst[(dim - 8 - n) * dim + m + 6] = src[(m+6) * dim + n + 7] ;
			}
			for(m = j ; m < BLOCK + j; m+=8){
				dst[(dim - 1 - n) * dim + m + 7] = src[(m+7) * dim + n];
				dst[(dim - 2 - n) * dim + m + 7] = src[(m+7) * dim + n + 1] ;
				dst[(dim - 3 - n) * dim + m + 7] = src[(m+7) * dim + n + 2] ;
				dst[(dim - 4 - n) * dim + m + 7] = src[(m+7) * dim + n + 3] ;
				dst[(dim - 5 - n) * dim + m + 7] = src[(m+7) * dim + n + 4] ;
				dst[(dim - 6 - n) * dim + m + 7] = src[(m+7) * dim + n + 5] ;
				dst[(dim - 7 - n) * dim + m + 7] = src[(m+7) * dim + n + 6] ;
				dst[(dim - 8 - n) * dim + m + 7] = src[(m+7) * dim + n + 7] ;
			}
		}
	}
	    
}

char rotate3_descr[] = "rotate: Current working version , 4*4, dst preferred";
void rotate3(int dim, pixel *src, pixel *dst) 
{
   /* if(dim % 64 == 0)  
	BLOCK = 64;
*/
    int i, j, m, n,m_max,n_max;
    for (i = 0; i < dim; i += BLOCK )
	for (j = 0; j < dim; j += BLOCK ){
		m_max = BLOCK + i;
		n_max = BLOCK + j;
		for(m = i ; m < m_max ; m+=4){
			for(n = j ; n < n_max ; n+=4){
				dst[RIDX(m, n , dim)] = src[RIDX(n, dim - 1 - m, dim)];
				dst[RIDX(m, n+1 , dim)] = src[RIDX(n+1, dim - 1 - m, dim)];
				dst[RIDX(m, n+2 , dim)] = src[RIDX(n+2, dim - 1 - m, dim)];
				dst[RIDX(m, n+3 , dim)] = src[RIDX(n+3, dim - 1 - m, dim)];
			}
			for(n = j ; n < n_max ; n+=4){
				dst[RIDX(m+1, n , dim)] = src[RIDX(n, dim - 2 - m, dim)];
				dst[RIDX(m+1, n+1 , dim)] = src[RIDX(n+1, dim - 2 - m, dim)];
				dst[RIDX(m+1, n+2 , dim)] = src[RIDX(n+2, dim - 2 - m, dim)];
				dst[RIDX(m+1, n+3 , dim)] = src[RIDX(n+3, dim - 2 - m, dim)];
			}
			for(n = j ; n < n_max ; n+=4){
				dst[RIDX(m+2, n , dim)] = src[RIDX(n, dim - 3 - m, dim)];
				dst[RIDX(m+2, n+1 , dim)] = src[RIDX(n+1, dim - 3 - m, dim)];
				dst[RIDX(m+2, n+2 , dim)] = src[RIDX(n+2, dim - 3 - m, dim)];
				dst[RIDX(m+2, n+3 , dim)] = src[RIDX(n+3, dim - 3 - m, dim)];
			}
			for(n = j ; n < n_max ; n+=4){
				dst[RIDX(m+3, n , dim)] = src[RIDX(n, dim - 4 - m, dim)];
				dst[RIDX(m+3, n+1 , dim)] = src[RIDX(n+1, dim - 4 - m, dim)];
				dst[RIDX(m+3, n+2 , dim)] = src[RIDX(n+2, dim - 4 - m, dim)];
				dst[RIDX(m+3, n+3 , dim)] = src[RIDX(n+3, dim - 4 - m, dim)];
			}
		}
	}
	    
}


char rotate_descr[] = "rotate4: Pointers, Unrolling 16";
void rotate(int dim, pixel *src, pixel *dst) 
{


	int i,j;
	int maxSteps = dim >> 4;
	src += dim - 1;
	for(i = 0 ; i < maxSteps ; i++,src += dim * (16 + 1),dst -= dim * dim - 16 ){
		for(j = 0 ; j < dim ; j++,src -= dim * 16 + 1,dst += dim - 16 ){

				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
		} 
		
	} 

    
}



char rotate4_descr[] = "rotate4: Pointers";
void rotate4(int dim, pixel *src, pixel *dst) 
{
   /* if(dim % 64 == 0)  
	BLOCK = 64;
*/



	int i,j;
	int maxSteps = dim >> 5;
	src += dim - 1;
	for(i = 0 ; i < maxSteps ; i++ ){
		for(j = 0 ; j < dim ; j++,src -= dim * BLOCK + 1,dst += dim - BLOCK ){

				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
		} 
		src += dim * (BLOCK + 1);
		dst -= dim * dim - BLOCK;
	} 

    
}

char rotate7_descr[] = "rotate7: Pointers with 32-64 division";
void rotate7(int dim, pixel *src, pixel *dst) 
{



	int i,j;
	int maxSteps;
	if(dim % 64 != 0){
	maxSteps = dim >> 5;
	src += dim - 1;
	for(i = 0 ; i < maxSteps ; i++ ){
		for(j = 0 ; j < dim ; j++){
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
			src -= dim * 32 + 1;
			dst += dim - 32;
		} 
		src += dim * (32 + 1);
		dst -= dim * dim - 32;
	} 
	return ;
	}
	maxSteps = dim >> 4;
    	src += dim - 1;
	for(i = 0 ; i < maxSteps ; i++ ){
		for(j = 0 ; j < dim ; j++){
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
				*dst++ = *src;
				src += dim;
			src -= dim * 16 + 1;
			dst += dim - 16;
		} 
		src += dim * (16 + 1);
		dst -= dim * dim - 16;
	} 
}
char rotate5_descr[] = "rotate5:Pointers, src preferred, unrolling 16";
void rotate5(int dim, pixel *src, pixel *dst) 
{
   

	int i,j;
	int maxSteps = dim >> 4;
	dst += (dim-1) * dim;
	for(i = 0 ; i < maxSteps ; i++,src -= dim * (dim - 1),dst -= dim - 1 ){
		for(j = 0 ; j < dim ; j++,src += dim - 16 , dst += dim * 16 + 1 ){
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
				*dst = *src++;
				dst -= dim;
		} 
		
	} 

    
	    
}
/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate, rotate_descr);  
    add_rotate_function(&rotate6, rotate6_descr);  
    add_rotate_function(&rotate2, rotate2_descr);   
    add_rotate_function(&rotate3, rotate3_descr);   
    add_rotate_function(&rotate4, rotate4_descr);   
    add_rotate_function(&rotate7, rotate7_descr);   
    add_rotate_function(&rotate8, rotate8_descr);  

    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
#define min(a,b) (a < b ? a : b)
#define max(a,b) (a > b ? a : b)
/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    for( j = 0 ; j < dim-1 ; j++)
    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */

char smooth4_descr[] = "smooth: Line Span";

void smooth4(int dim, pixel *src, pixel *dst) 
{
	typedef struct pixel_sum{
		int r;
		int g;
		int b;
	}Psum;
	Psum preVal;
	Psum del;
       	int i, j;
	int red=0, green=0, blue=0;
	unsigned short *color = &src[0].red;
	unsigned short *color2 = &src[0].red + 3 * dim;
	unsigned short *color3 = &src[0].red + 6 * dim;
	/*****************************LINE  1************************************/
	/*Up-left corner*/
	red = *color++;
	preVal.r = red;
	del.r = red;	
	green = *color++;
	preVal.g = green;
	del.g = green;
	blue = *color++;
	preVal.b = blue;
	del.b = blue;

	preVal.r += *color++;	
	preVal.g += *color++;
	preVal.b += *color++;

	red = *color2++;
	preVal.r += red;
	del.r += red;
	green = *color2++;
	preVal.g += green;
	del.g += green;
	blue = *color2++;
	preVal.b += blue;
	del.b += blue;

	preVal.r += *color2++;
	preVal.g += *color2++;
	preVal.b += *color2++;

	dst[0].red = (unsigned short) (preVal.r >> 2);
	dst[0].green = (unsigned short) (preVal.g >> 2);
	dst[0].blue = (unsigned short) (preVal.b >> 2);

	
	/*Line 1*/
	for( j=1,red=0, green=0, blue=0 ; j < dim-1 ; j++){
		red = *color++;
		green = *color++;
		blue = *color++;

		red += *color2++;
		green += *color2++;
		blue += *color2++;
	
		preVal.r += red;
		preVal.g += green;
		preVal.b += blue;
		dst[j].red = (unsigned short) (preVal.r/6);
		dst[j].green = (unsigned short) (preVal.g/6);
		dst[j].blue = (unsigned short) (preVal.b/6);
		
		preVal.r -= del.r;
		preVal.g -= del.g;
		preVal.b -= del.b;


		del.r = preVal.r - red;
		del.g = preVal.g - green;
		del.b = preVal.b - blue;
	}
	/*Column dim-1*/

	dst[dim-1].red = (unsigned short) (preVal.r>>2);
	dst[dim-1].green = (unsigned short) (preVal.g>>2);
	dst[dim-1].blue = (unsigned short) (preVal.b>>2);

	/*****************************END OF LINE1************************************/

	/*****************************LINE  2 to dim-1************************************/
	for (i = 1; i < dim - 1 ; i++){

		color = &src[(i-1) * dim].red;
		color2 = &src[i * dim].red;
		color3 = &src[(i+1) * dim].red;
		/*Column 0*/
		red = *color++;
		preVal.r = red;
		del.r = red;	
		green = *color++;
		preVal.g = green;
		del.g = green;
		blue = *color++;
		preVal.b = blue;
		del.b = blue;

		preVal.r += *color++;	
		preVal.g += *color++;
		preVal.b += *color++;

		red = *color2++;
		preVal.r += red;
		del.r += red;
		green = *color2++;
		preVal.g += green;
		del.g += green;
		blue = *color2++;
		preVal.b += blue;
		del.b += blue;

		preVal.r += *color2++;
		preVal.g += *color2++;
		preVal.b += *color2++;

		red = *color3++;
		preVal.r += red;
		del.r += red;
		green = *color3++;
		preVal.g += green;
		del.g += green;
		blue = *color3++;
		preVal.b += blue;
		del.b += blue;

		preVal.r += *color3++;
		preVal.g += *color3++;
		preVal.b += *color3++;
		dst[i * dim].red = (unsigned short) (preVal.r / 6);
		dst[i * dim].green = (unsigned short) (preVal.g / 6);
		dst[i * dim].blue = (unsigned short) (preVal.b / 6);


		/*Column 1 to n-1*/
		for( j=1,red=0, green=0, blue=0 ; j < dim-1 ; j++){
			red = *color++;
			green = *color++;
			blue = *color++;

			red += *color2++;
			green += *color2++;
			blue += *color2++;
	
			red += *color3++;
			green += *color3++;
			blue += *color3++;

			preVal.r += red;
			preVal.g += green;
			preVal.b += blue;
			dst[i * dim + j].red = (unsigned short) (preVal.r/9);
			dst[i * dim + j].green = (unsigned short) (preVal.g/9);
			dst[i * dim + j].blue = (unsigned short) (preVal.b/9);
		
			preVal.r -= del.r;
			preVal.g -= del.g;
			preVal.b -= del.b;


			del.r = preVal.r - red;
			del.g = preVal.g - green;
			del.b = preVal.b - blue;
		}

		/*Column dim-1*/

		dst[i * dim + dim-1].red = (unsigned short) (preVal.r/6);
		dst[i * dim + dim-1].green = (unsigned short) (preVal.g/6);
		dst[i * dim + dim-1].blue = (unsigned short) (preVal.b/6);
		

	}
		
	/*****************************END OF LINE dim-1 ************************************/


	
	/*****************************LINE dim************************************/
	color = &src[dim * (dim - 2)].red;
	color2 =&src[dim * (dim - 1)].red;
	/*Column 1*/
	red = *color++;
	preVal.r = red;
	del.r = red;	
	green = *color++;
	preVal.g = green;
	del.g = green;
	blue = *color++;
	preVal.b = blue;
	del.b = blue;

	preVal.r += *color++;	
	preVal.g += *color++;
	preVal.b += *color++;

	red = *color2++;
	preVal.r += red;
	del.r += red;
	green = *color2++;
	preVal.g += green;
	del.g += green;
	blue = *color2++;
	preVal.b += blue;
	del.b += blue;

	preVal.r += *color2++;
	preVal.g += *color2++;
	preVal.b += *color2++;

	dst[dim * (dim-1)].red = (unsigned short) (preVal.r >> 2);
	dst[dim * (dim-1)].green = (unsigned short) (preVal.g >> 2);
	dst[dim * (dim-1)].blue = (unsigned short) (preVal.b >> 2);

	
	/*Line 1*/
	for( j=1,red=0, green=0, blue=0 ; j < dim-1 ; j++){
		red = *color++;
		green = *color++;
		blue = *color++;

		red += *color2++;
		green += *color2++;
		blue += *color2++;
	
		preVal.r += red;
		preVal.g += green;
		preVal.b += blue;
		dst[dim * (dim-1) + j].red = (unsigned short) (preVal.r/6);
		dst[dim * (dim-1) + j].green = (unsigned short) (preVal.g/6);
		dst[dim * (dim-1) + j].blue = (unsigned short) (preVal.b/6);
		
		preVal.r -= del.r;
		preVal.g -= del.g;
		preVal.b -= del.b;


		del.r = preVal.r - red;
		del.g = preVal.g - green;
		del.b = preVal.b - blue;
	}
	/*Column dim-1*/

	dst[dim * dim-1].red = (unsigned short) (preVal.r>>2);
	dst[dim * dim-1].green = (unsigned short) (preVal.g>>2);
	dst[dim * dim-1].blue = (unsigned short) (preVal.b>>2);
	/*****************************END OF LINE dim************************************/
}


char smooth_descr[] = "smooth4: 2*2";
void smooth(int dim, pixel *src, pixel *dst) 
{
	typedef struct pixel_sum{
		int r;
		int g;
		int b;
	}Psum;
	Psum preVal;
	Psum del;
       	int i, j;
	int red=0, green=0, blue=0;
	unsigned short *color = &src[0].red;
	unsigned short *color2 = &src[0].red + 3 * dim;
	unsigned short *color3 = &src[0].red + 6 * dim;

	unsigned short *line1;	
	unsigned short *line2;
	unsigned short *line3;
	unsigned short *line4;
	unsigned short *dest;
	/*****************************LINE  1************************************/
	/*Up-left corner*/
	red = *color++;
	preVal.r = red;
	del.r = red;	
	green = *color++;
	preVal.g = green;
	del.g = green;
	blue = *color++;
	preVal.b = blue;
	del.b = blue;

	preVal.r += *color++;	
	preVal.g += *color++;
	preVal.b += *color++;

	red = *color2++;
	preVal.r += red;
	del.r += red;
	green = *color2++;
	preVal.g += green;
	del.g += green;
	blue = *color2++;
	preVal.b += blue;
	del.b += blue;

	preVal.r += *color2++;
	preVal.g += *color2++;
	preVal.b += *color2++;

	dst[0].red = (unsigned short) (preVal.r >> 2);
	dst[0].green = (unsigned short) (preVal.g >> 2);
	dst[0].blue = (unsigned short) (preVal.b >> 2);

	
	/*Line 1*/
	for( j=1,red=0, green=0, blue=0 ; j < dim-1 ; j++){
		red = *color++;
		green = *color++;
		blue = *color++;

		red += *color2++;
		green += *color2++;
		blue += *color2++;
	
		preVal.r += red;
		preVal.g += green;
		preVal.b += blue;
		dst[j].red = (unsigned short) (preVal.r/6);
		dst[j].green = (unsigned short) (preVal.g/6);
		dst[j].blue = (unsigned short) (preVal.b/6);
		
		preVal.r -= del.r;
		preVal.g -= del.g;
		preVal.b -= del.b;


		del.r = preVal.r - red;
		del.g = preVal.g - green;
		del.b = preVal.b - blue;
	}
	/*Column dim-1*/

	dst[dim-1].red = (unsigned short) (preVal.r>>2);
	dst[dim-1].green = (unsigned short) (preVal.g>>2);
	dst[dim-1].blue = (unsigned short) (preVal.b>>2);

	/*****************************END OF LINE1********************************/

	/*****************************ROW 1********************************/
	color = &src[dim * 2].red;
	preVal.r = src[0].red + src[1].red;
	del.r = preVal.r;
	preVal.g = src[0].green + src[1].green;
	del.g = preVal.g;
	preVal.b = src[0].blue + src[1].blue;
	del.b = preVal.b;
	preVal.r += src[dim].red + src[dim +1].red;
	preVal.g += src[dim].green + src[dim + 1].green;
	preVal.b += src[dim].blue + src[dim + 1].blue;
	for( j=1; j < dim-1 ; j++){
		red = *color++;
		green = *color++;
		blue = *color++;

		red += *color++;
		green += *color++;
		blue += *color++;
		color += (dim-2) * 3;
		preVal.r += red;
		preVal.g += green;
		preVal.b += blue;
		dst[j * dim].red = (unsigned short) (preVal.r/6);
		dst[j * dim].green = (unsigned short) (preVal.g/6);
		dst[j * dim].blue = (unsigned short) (preVal.b/6);
		
		preVal.r -= del.r;
		preVal.g -= del.g;
		preVal.b -= del.b;


		del.r = preVal.r - red;
		del.g = preVal.g - green;
		del.b = preVal.b - blue;

	}
	/*****************************END OF ROW1********************************/



	/*****************************HERE GOES THE MIDDLE EARTH****************************************************************/
	for(i = 1 ; i < dim - 1 ; i+=2){
		for(j = 1 ; j < dim - 1 ; j +=2){
			Psum block00,block01,block10,block11;
			dest = &dst[i * dim + j].red;
			line1 = &src[(i-1) * dim + j - 1].red;
			line2 = line1 + dim * 3 +3;
			line3 = line1 + dim * 6 +3;
			line4 = line1 + dim * 9;

			block01.r = *line2++;
			block01.g = *line2++;
			block01.b = *line2++;
			block01.r += *line2++;
			block01.g += *line2++;
			block01.b += *line2++;
			block01.r += *line3++;
			block01.g += *line3++;
			block01.b += *line3++;
			block01.r += *line3++;
			block01.g += *line3++;
			block01.b += *line3++;
			block10 = block01;
			block00 = block01;


			block01.r += *line2++;
			block01.g += *line2++;
			block01.b += *line2++;
			block01.r += *line3++;
			block01.g += *line3++;
			block01.b += *line3++;
			block11 = block01;

			line2 -= 12;
			line3 -= 12;
			block00.r += *line2++;
			block00.g += *line2++;
			block00.b += *line2++;
			block00.r += *line3++;
			block00.g += *line3++;
			block00.b += *line3++;
			block10 = block00;
			
			block00.r += *line1++;
			block00.g += *line1++;
			block00.b += *line1++;		

			
			red = *line1++;
			green = *line1++;
			blue = *line1++;
			red += *line1++;
			green += *line1++;
			blue += *line1++;
			block00.r += red;
			block00.g += green;
			block00.b += blue;
			block01.r += red;
			block01.g += green;
			block01.b += blue;

			block01.r += *line1++;
			block01.g += *line1++;
			block01.b += *line1++;


			block10.r += *line4++;
			block10.g += *line4++;
			block10.b += *line4++;		

			
			red = *line4++;
			green = *line4++;
			blue = *line4++;
			red += *line4++;
			green += *line4++;
			blue += *line4++;
			block10.r += red;
			block10.g += green;
			block10.b += blue;
			block11.r += red;
			block11.g += green;
			block11.b += blue;

			block11.r += *line4++;
			block11.g += *line4++;
			block11.b += *line4++;

			*dest++ = (unsigned short) (block00.r/9);
			*dest++ = (unsigned short) (block00.g/9);
			*dest++ = (unsigned short) (block00.b/9);
			*dest++ = (unsigned short) (block01.r/9);
			*dest++ = (unsigned short) (block01.g/9);
			*dest++ = (unsigned short) (block01.b/9);
			dest += (dim*3)-6;
			*dest++ = (unsigned short) (block10.r/9);
			*dest++ = (unsigned short) (block10.g/9);
			*dest++ = (unsigned short) (block10.b/9);
			*dest++ = (unsigned short) (block11.r/9);
			*dest++ = (unsigned short) (block11.g/9);
			*dest++ = (unsigned short) (block11.b/9);


			
		}
	}

	
	/*****************************END OF THE MIDDLE EARTH********************************/


	/*****************************ROW n**************************************/
	color = &src[dim-2 + dim*2].red;
	preVal.r = src[dim-2].red + src[dim - 1].red;
	del.r = preVal.r;
	preVal.g = src[dim-2].green + src[dim - 1].green;
	del.g = preVal.g;
	preVal.b = src[dim-2].blue + src[dim - 1].blue;
	del.b = preVal.b;
	preVal.r += src[dim + dim-2].red + src[dim + dim - 1].red;
	preVal.g += src[dim + dim-2].green + src[dim + dim - 1].green;
	preVal.b += src[dim + dim-2].blue + src[dim + dim - 1].blue;

	for( j=1,red=0, green=0, blue=0 ; j < dim-1 ; j++){
		red = *color++;
		green = *color++;
		blue = *color++;

		red += *color++;
		green += *color++;
		blue += *color++;
		color += (dim-2) * 3;

		preVal.r += red;
		preVal.g += green;
		preVal.b += blue;
		dst[(j+1) * dim - 1].red = (unsigned short) (preVal.r/6);
		dst[(j+1) * dim - 1].green = (unsigned short) (preVal.g/6);
		dst[(j+1) * dim - 1].blue = (unsigned short) (preVal.b/6);
		
		preVal.r -= del.r;
		preVal.g -= del.g;
		preVal.b -= del.b;


		del.r = preVal.r - red;
		del.g = preVal.g - green;
		del.b = preVal.b - blue;

	}
	/*****************************END OF ROW n*********************************/

	/*****************************LINE dim************************************/
	color = &src[dim * (dim - 2)].red;
	color2 =&src[dim * (dim - 1)].red;
	/*Column 1*/
	red = *color++;
	preVal.r = red;
	del.r = red;	
	green = *color++;
	preVal.g = green;
	del.g = green;
	blue = *color++;
	preVal.b = blue;
	del.b = blue;

	preVal.r += *color++;	
	preVal.g += *color++;
	preVal.b += *color++;

	red = *color2++;
	preVal.r += red;
	del.r += red;
	green = *color2++;
	preVal.g += green;
	del.g += green;
	blue = *color2++;
	preVal.b += blue;
	del.b += blue;

	preVal.r += *color2++;
	preVal.g += *color2++;
	preVal.b += *color2++;

	dst[dim * (dim-1)].red = (unsigned short) (preVal.r >> 2);
	dst[dim * (dim-1)].green = (unsigned short) (preVal.g >> 2);
	dst[dim * (dim-1)].blue = (unsigned short) (preVal.b >> 2);

	
	/*Line 1*/
	for( j=1,red=0, green=0, blue=0 ; j < dim-1 ; j++){
		red = *color++;
		green = *color++;
		blue = *color++;

		red += *color2++;
		green += *color2++;
		blue += *color2++;
	
		preVal.r += red;
		preVal.g += green;
		preVal.b += blue;
		dst[dim * (dim-1) + j].red = (unsigned short) (preVal.r/6);
		dst[dim * (dim-1) + j].green = (unsigned short) (preVal.g/6);
		dst[dim * (dim-1) + j].blue = (unsigned short) (preVal.b/6);
		
		preVal.r -= del.r;
		preVal.g -= del.g;
		preVal.b -= del.b;


		del.r = preVal.r - red;
		del.g = preVal.g - green;
		del.b = preVal.b - blue;
	}
	/*Column dim-1*/

	dst[dim * dim-1].red = (unsigned short) (preVal.r>>2);
	dst[dim * dim-1].green = (unsigned short) (preVal.g>>2);
	dst[dim * dim-1].blue = (unsigned short) (preVal.b>>2);
	/*****************************END OF LINE dim************************************/


}






/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&smooth4, smooth4_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);

    /* ... Register additional test functions here */
}
