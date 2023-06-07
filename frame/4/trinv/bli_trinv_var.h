/*

   BLIS
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2022, The University of Texas at Austin
   Copyright (C) 2022, Oracle Labs, Oracle Corporation

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

#ifndef BLIS_TRINV_VAR_H
#define BLIS_TRINV_VAR_H

//
// Prototype object-based interfaces.
//

#undef  GENPROT
#define GENPROT( opname ) \
\
err_t PASTEMAC0(opname) \
     ( \
       const obj_t*  a, \
       const cntx_t* cntx, \
             rntm_t* rntm, \
             cntl_t* cntl  \
     );

GENPROT(trinv_l_unb_var1)
GENPROT(trinv_l_unb_var2)
GENPROT(trinv_l_unb_var3)
GENPROT(trinv_l_unb_var4)

GENPROT(trinv_l_blk_var1)
GENPROT(trinv_l_blk_var2)
GENPROT(trinv_l_blk_var3)
GENPROT(trinv_l_blk_var4)

GENPROT(trinv_l_opt_var1)
GENPROT(trinv_l_opt_var2)
GENPROT(trinv_l_opt_var3)
GENPROT(trinv_l_opt_var4)

GENPROT(trinv_u_unb_var1)
GENPROT(trinv_u_unb_var2)
GENPROT(trinv_u_unb_var3)
GENPROT(trinv_u_unb_var4)

GENPROT(trinv_u_blk_var1)
GENPROT(trinv_u_blk_var2)
GENPROT(trinv_u_blk_var3)
GENPROT(trinv_u_blk_var4)

GENPROT(trinv_u_opt_var1)
GENPROT(trinv_u_opt_var2)
GENPROT(trinv_u_opt_var3)
GENPROT(trinv_u_opt_var4)

//
// Prototype BLAS-like interfaces with void pointer operands.
//

#undef  GENTPROT
#define GENTPROT( ctype, ch, varname ) \
\
err_t PASTEMAC(ch,varname) \
     ( \
             uplo_t  uploa, \
             diag_t  diaga, \
             dim_t   m, \
             ctype*  a, inc_t rs_a, inc_t cs_a, \
       const cntx_t* cntx, \
             rntm_t* rntm  \
     );

INSERT_GENTPROT_BASIC0(trinv_l_opt_var1)
INSERT_GENTPROT_BASIC0(trinv_l_opt_var2)
INSERT_GENTPROT_BASIC0(trinv_l_opt_var3)
INSERT_GENTPROT_BASIC0(trinv_l_opt_var4)

INSERT_GENTPROT_BASIC0(trinv_u_opt_var1)
INSERT_GENTPROT_BASIC0(trinv_u_opt_var2)
INSERT_GENTPROT_BASIC0(trinv_u_opt_var3)
INSERT_GENTPROT_BASIC0(trinv_u_opt_var4)

#endif