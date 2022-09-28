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
    - Neither the name of The University of Texas nor the names of its
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

#include <unistd.h>
#include "blis.h"
#include "test_utils.h"

//#define PRINT

static const char* LOCAL_OPNAME_STR = "herk";
static const char* LOCAL_PC_STR     = "ln";

int main( int argc, char** argv )
{
	obj_t    a, c;
	obj_t    c_save;
	obj_t    alpha, beta;
	dim_t    m, k;
	dim_t    p;
	dim_t    p_begin, p_max, p_inc;
	int      m_input, k_input;
	ind_t    ind;
	num_t    dt, dt_real;
	char     dt_ch;
	int      r, n_repeats;
	uplo_t   uploc;
	trans_t  transa;
	f77_char f77_uploc;
	f77_char f77_transa;

	double   dtime;
	double   dtime_save;
	double   gflops;

	params_t params;

	// Supress compiler warnings about unused variable 'ind'.
	( void )ind;


	//bli_init();

	//bli_error_checking_level_set( BLIS_NO_ERROR_CHECKING );

	// Parse the command line options into strings, integers, enums,
	// and doubles, as appropriate.
	parse_cl_params( argc, argv, init_def_params, &params );

	dt        = params.dt;
	dt_real   = bli_dt_proj_to_real( dt );

	ind       = params.im;

	p_begin   = params.sta;
	p_max     = params.end;
	p_inc     = params.inc;

	m_input   = params.m;
	k_input   = params.k;

	n_repeats = params.nr;


	// Map the datatype to its corresponding char.
	bli_param_map_blis_to_char_dt( dt, &dt_ch );

	// Map the parameter chars to their corresponding BLIS enum type values.
	bli_param_map_char_to_blis_uplo( params.pc_str[0], &uploc );
	bli_param_map_char_to_blis_trans( params.pc_str[1], &transa );

	// Map the BLIS enum type values to their corresponding BLAS chars.
	bli_param_map_blis_to_netlib_uplo( uploc, &f77_uploc );
	bli_param_map_blis_to_netlib_trans( transa, &f77_transa );

	// Begin with initializing the last entry to zero so that
	// matlab allocates space for the entire array once up-front.
	for ( p = p_begin; p + p_inc <= p_max; p += p_inc ) ;

	printf( "data_%s_%cherk_%s", THR_STR, dt_ch, IMPL_STR );
	printf( "( %4lu, 1:3 ) = [ %5lu %5lu %8.2f ];\n",
	        ( unsigned long )(p - p_begin)/p_inc + 1,
	        ( unsigned long )0,
	        ( unsigned long )0, 0.0 );


	//for ( p = p_begin; p <= p_max; p += p_inc )
	for ( p = p_max; p_begin <= p; p -= p_inc )
	{

		if ( m_input < 0 ) m = p / ( dim_t )abs(m_input);
		else               m =     ( dim_t )    m_input;
		if ( k_input < 0 ) k = p / ( dim_t )abs(k_input);
		else               k =     ( dim_t )    k_input;

		bli_obj_create( dt_real, 1, 1, 0, 0, &alpha );
		bli_obj_create( dt,      1, 1, 0, 0, &beta );

		// Choose the storage of each matrix based on the corresponding
		// char in the params_t struct. Note that the expected order of
		// storage specifers in sc_str is CA (not AC).
		if ( params.sc_str[1] == 'c' )
		{
			if ( bli_does_trans( transa ) ) bli_obj_create( dt, k, m, 0, 0, &a );
			else                            bli_obj_create( dt, m, k, 0, 0, &a );
		}
		else // if ( params.sc_str[1] == 'r' )
		{
			if ( bli_does_trans( transa ) ) bli_obj_create( dt, k, m, m, 1, &a );
			else                            bli_obj_create( dt, m, k, k, 1, &a );
		}

		if ( params.sc_str[0] == 'c' ) bli_obj_create( dt, m, m, 0, 0, &c );
		else                           bli_obj_create( dt, m, m, m, 1, &c );

		if ( params.sc_str[0] == 'c' ) bli_obj_create( dt, m, m, 0, 0, &c_save );
		else                           bli_obj_create( dt, m, m, m, 1, &c_save );

		bli_randm( &a );
		bli_randm( &c );

		bli_obj_set_struc( BLIS_HERMITIAN, &c );
		bli_obj_set_uplo( uploc, &c );

		// Make C densely Hermitian, and zero the unstored triangle to
		// ensure the implementation reads only from the stored region.
		bli_mkherm( &c );
		bli_mktrim( &c );

		bli_obj_set_conjtrans( transa, &a );

		//bli_setsc(  (2.0/1.0), 0.0, &alpha );
		//bli_setsc(  (1.0/1.0), 0.0, &beta );
		bli_setsc( params.alpha, 0.0, &alpha );
		bli_setsc( params.beta,  0.0, &beta );

		bli_copym( &c, &c_save );

#ifdef BLIS
		// Switch to the induced method specified by ind.
		bli_ind_disable_all_dt( dt );
		bli_ind_enable_dt( ind, dt );
#endif

		dtime_save = DBL_MAX;

		for ( r = 0; r < n_repeats; ++r )
		{
			bli_copym( &c_save, &c );

			dtime = bli_clock();

#ifdef PRINT
			bli_printm( "a", &a, "%4.1f", "" );
			bli_printm( "c", &c, "%4.1f", "" );
#endif

#ifdef BLIS

			bli_herk( &alpha,
			          &a,
			          &beta,
			          &c );

#else

			if ( bli_is_float( dt ) )
			{
				f77_int   mm     = bli_obj_length( &c );
				f77_int   kk     = bli_obj_width_after_trans( &a );
				f77_int   lda    = bli_obj_col_stride( &a );
				f77_int   ldc    = bli_obj_col_stride( &c );
				float*    alphap = ( float* )bli_obj_buffer( &alpha );
				float*    ap     = ( float* )bli_obj_buffer( &a );
				float*    betap  = ( float* )bli_obj_buffer( &beta );
				float*    cp     = ( float* )bli_obj_buffer( &c );

				ssyrk_( &f77_uploc,
				        &f77_transa,
				        &mm,
				        &kk,
				        alphap,
				        ap, &lda,
				        betap,
				        cp, &ldc );
			}
			else if ( bli_is_double( dt ) )
			{
				f77_int   mm     = bli_obj_length( &c );
				f77_int   kk     = bli_obj_width_after_trans( &a );
				f77_int   lda    = bli_obj_col_stride( &a );
				f77_int   ldc    = bli_obj_col_stride( &c );
				double*   alphap = ( double* )bli_obj_buffer( &alpha );
				double*   ap     = ( double* )bli_obj_buffer( &a );
				double*   betap  = ( double* )bli_obj_buffer( &beta );
				double*   cp     = ( double* )bli_obj_buffer( &c );

				dsyrk_( &f77_uploc,
				        &f77_transa,
				        &mm,
				        &kk,
				        alphap,
				        ap, &lda,
				        betap,
				        cp, &ldc );
			}
			else if ( bli_is_scomplex( dt ) )
			{
				f77_int   mm     = bli_obj_length( &c );
				f77_int   kk     = bli_obj_width_after_trans( &a );
				f77_int   lda    = bli_obj_col_stride( &a );
				f77_int   ldc    = bli_obj_col_stride( &c );
#ifdef EIGEN
				float*    alphap = ( float*    )bli_obj_buffer( &alpha );
				float*    ap     = ( float*    )bli_obj_buffer( &a );
				float*    betap  = ( float*    )bli_obj_buffer( &beta );
				float*    cp     = ( float*    )bli_obj_buffer( &c );
#else
				float*    alphap = ( float*    )bli_obj_buffer( &alpha );
				scomplex* ap     = ( scomplex* )bli_obj_buffer( &a );
				float*    betap  = ( float*    )bli_obj_buffer( &beta );
				scomplex* cp     = ( scomplex* )bli_obj_buffer( &c );
#endif

				cherk_( &f77_uploc,
				        &f77_transa,
				        &mm,
				        &kk,
				        alphap,
				        ap, &lda,
				        betap,
				        cp, &ldc );
			}
			else if ( bli_is_dcomplex( dt ) )
			{
				f77_int   mm     = bli_obj_length( &c );
				f77_int   kk     = bli_obj_width_after_trans( &a );
				f77_int   lda    = bli_obj_col_stride( &a );
				f77_int   ldc    = bli_obj_col_stride( &c );
#ifdef EIGEN
				double*   alphap = ( double*   )bli_obj_buffer( &alpha );
				double*   ap     = ( double*   )bli_obj_buffer( &a );
				double*   betap  = ( double*   )bli_obj_buffer( &beta );
				double*   cp     = ( double*   )bli_obj_buffer( &c );
#else
				double*   alphap = ( double*   )bli_obj_buffer( &alpha );
				dcomplex* ap     = ( dcomplex* )bli_obj_buffer( &a );
				double*   betap  = ( double*   )bli_obj_buffer( &beta );
				dcomplex* cp     = ( dcomplex* )bli_obj_buffer( &c );
#endif

				zherk_( &f77_uploc,
				        &f77_transa,
				        &mm,
				        &kk,
				        alphap,
				        ap, &lda,
				        betap,
				        cp, &ldc );
			}
#endif

#ifdef PRINT
			bli_printm( "c after", &c, "%4.1f", "" );
			exit(1);
#endif

			dtime_save = bli_clock_min_diff( dtime_save, dtime );
		}

		gflops = ( 1.0 * m * k * m ) / ( dtime_save * 1.0e9 );

		if ( bli_is_complex( dt ) ) gflops *= 4.0;

		printf( "data_%s_%cherk_%s", THR_STR, dt_ch, IMPL_STR );
		printf( "( %4lu, 1:3 ) = [ %5lu %5lu %8.2f ];\n",
		        ( unsigned long )(p - p_begin)/p_inc + 1,
		        ( unsigned long )m,
		        ( unsigned long )k, gflops );
		fflush( stdout );

		fflush(stdout);

		bli_obj_free( &alpha );
		bli_obj_free( &beta );

		bli_obj_free( &a );
		bli_obj_free( &c );
		bli_obj_free( &c_save );
	}

	//bli_finalize();

	return 0;
}

void init_def_params( params_t* params )
{
	params->opname    = LOCAL_OPNAME_STR;
	params->impl      = IMPL_STR;

	params->pc_str    = LOCAL_PC_STR;
	params->dt_str    = GLOB_DEF_DT_STR;
	params->sc_str    = GLOB_DEF_SC_STR;

	params->im_str    = GLOB_DEF_IM_STR;

	params->ps_str    = GLOB_DEF_PS_STR;
	params->m_str     = GLOB_DEF_M_STR;
	params->k_str     = GLOB_DEF_K_STR;

	params->nr_str    = GLOB_DEF_NR_STR;

	params->alpha_str = GLOB_DEF_ALPHA_STR;
	params->beta_str  = GLOB_DEF_BETA_STR;
}

