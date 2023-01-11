/*

   BLIS
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2014, The University of Texas at Austin
   Copyright (C) 2018 - 2019, Advanced Micro Devices, Inc.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    - Neither the name(s) of the copyright holder(s) nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "blis.h"

void bli_cntl_init_node
     (
       bszid_t bszid,
       void_fp var_func,
       cntl_t* sub_node,
       cntl_t* cntl
     )
{
	bli_cntl_set_bszid( bszid, cntl );
	bli_cntl_set_var_func( var_func, cntl );
	bli_cntl_set_sub_prenode( NULL, cntl );
	bli_cntl_set_sub_node( sub_node, cntl );
}

#if 0

cntl_t* bli_cntl_create_node
     (
       pool_t* pool,
       bszid_t bszid,
       void_fp var_func,
       void*   params,
       cntl_t* sub_node
     )
{
	cntl_t* cntl;

	#ifdef BLIS_ENABLE_MEM_TRACING
	printf( "bli_cntl_create_node(): " );
	#endif

	// Allocate the cntl_t struct.
	cntl = bli_sba_acquire( pool, sizeof( cntl_t ) );

	bli_cntl_set_bszid( bszid, cntl );
	bli_cntl_set_var_func( var_func, cntl );
	bli_cntl_set_params( params, cntl );
	bli_cntl_set_sub_prenode( NULL, cntl );
	bli_cntl_set_sub_node( sub_node, cntl );

	return cntl;
}

void bli_cntl_free_node
     (
       pool_t* pool,
       cntl_t* cntl
     )
{
	#ifdef BLIS_ENABLE_MEM_TRACING
	printf( "bli_cntl_free_node(): " );
	#endif

	bli_sba_release( pool, cntl );
}

void bli_cntl_free
     (
       pool_t* pool,
       cntl_t* cntl
     )
{
	// Base case: simply return when asked to free NULL nodes.
	if ( cntl == NULL ) return;

	cntl_t* cntl_sub_prenode = bli_cntl_sub_prenode( cntl );
	cntl_t* cntl_sub_node    = bli_cntl_sub_node( cntl );
	void*   cntl_params      = bli_cntl_params( cntl );

	// Only recurse into prenode branch if it exists.
	if ( cntl_sub_prenode != NULL )
	{
		// Recursively free all memory associated with the sub-prenode and its
		// children.
		bli_cntl_free( pool, cntl_sub_prenode );
	}

	// Only recurse into the child node if it exists.
	if ( cntl_sub_node != NULL )
	{
		// Recursively free all memory associated with the sub-node and its
		// children.
		bli_cntl_free( pool, cntl_sub_node );
	}

	// Free the current node's params field, if it is non-NULL.
	if ( cntl_params != NULL )
	{
		#ifdef BLIS_ENABLE_MEM_TRACING
		printf( "bli_cntl_free_w_thrinfo(): " );
		#endif

		bli_sba_release( pool, cntl_params );
	}

	// Free the current node.
	bli_cntl_free_node( pool, cntl );
}

#endif

void bli_cntl_clear_node
     (
       cntl_t* cntl
     )
{
	// Clear various fields in the control tree. Clearing these fields
	// actually is not needed, but we do it for debugging/completeness.
	bli_cntl_set_var_func( NULL, cntl );
	bli_cntl_set_sub_prenode( NULL, cntl );
	bli_cntl_set_sub_node( NULL, cntl );
}

