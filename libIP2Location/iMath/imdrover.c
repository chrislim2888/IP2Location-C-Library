/*
  Name:     imdrover.c
  Purpose:  Keeper of the hordes of testing code.
  Author:   M. J. Fromberger <http://www.dartmouth.edu/~sting/>
  Info:     $Id: imdrover.c 761 2007-01-11 17:43:18Z sting $

  Copyright (C) 2002 Michael J. Fromberger, All Rights Reserved.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "imdrover.h"
#include "imath.h"
#include "imrat.h"

/* Globals visible from outside this file */
mp_result imath_errno;
char     *imath_errmsg;

#define CHECK(X) \
do{if((res=(X))!=MP_OK)return(imath_errno=res);}while(0)
#define FAIL(E) return (imath_errno = (E));

#define OUTPUT_LIMIT 2048
#define NUM_REGS     16
#define OTHER_ERROR  -1024

static char g_output[OUTPUT_LIMIT];
static mpz_t g_zreg[NUM_REGS];
static mpq_t g_qreg[NUM_REGS];
static unsigned char g_bin1[OUTPUT_LIMIT];
static unsigned char g_bin2[OUTPUT_LIMIT];

extern void trim_line(char *line); /* borrowed from imtest.c */

/* Read in a string with radix tags */
static mp_result read_int_value(mp_int z, char *str);
static mp_result read_rat_value(mp_rat q, char *str);

/* Read in a string with radix tags, as a long (not an mp_int) */
static int       read_long(long *z, char *str);

/* Parse the input and output values and fill in pointers to the
   registers containing them.  Returns true if all is well, false
   in case of error.  Caller allocates in/out to correct sizes. */
static int       parse_int_values(testspec_t *t, mp_int *in, mp_int *out,
				  mp_result *rval);
static int       parse_rat_values(testspec_t *t, mp_rat *in, mp_rat *out,
				  mp_result *rval);

/* Parse a result code name and return the corresponding result
   code */
static int       parse_result_code(char *str, mp_result *code);

/* Read in a dot-delimited binary sequence to the given buffer, and
   return the number of bytes read.  Returns < 0 in case of a syntax
   error.  Records no more than limit bytes. */
static int       parse_binary(char *str, unsigned char *buf, int limit);

/* Clean up registers (called from atexit()) */
static void      done_testing(void);

/*------------------------------------------------------------------------*/
/* Utility subroutines for writing tests (explained above)                */

/* {{{ read_int_value(z, str) */

static mp_result read_int_value(mp_int z, char *str)
{
  int radix = 10;

  if(*str == '#') {
    ++str;
    switch(*str) {
    case 'x': case 'X':
      radix = 16;
      break;
    case 'd': case 'D':
      radix = 10;
      break;
    case 'o': case 'O':
      radix = 8;
      break;
    case 'b': case 'B':
      radix = 2;
      break;
    default:
      return MP_RANGE;
    }
    ++str;
  }

  return mp_int_read_string(z, radix, str);
}

/* }}} */

/* {{{ read_rat_value(q, str) */

static mp_result read_rat_value(mp_rat q, char *str)
{
  int radix = 10;

  if(*str == '#') {
    ++str;
    switch(*str) {
    case 'x': case 'X':
      radix = 16;
      break;
    case 'd': case 'D':
      radix = 10;
      break;
    case 'o': case 'O':
      radix = 8;
      break;
    case 'b': case 'B':
      radix = 2;
      break;
    default:
      return MP_RANGE;
    }
    ++str;
  }

  if(*str == '@') 
    return mp_rat_read_decimal(q, radix, str + 1);
  else
    return mp_rat_read_string(q, radix, str);
}

/* }}} */

/* {{{ read_long(z, str) */

static int       read_long(long *z, char *str)
{
  char *end;
  int radix = 10;

  if(*str == '#') {
    ++str;
    switch(*str) {
    case 'x': case 'X':
      radix = 16;
      break;
    case 'd': case 'D':
      radix = 10;
      break;
    case 'o': case 'O':
      radix = 8;
      break;
    case 'b': case 'B':
      radix = 2;
      break;
    default:
      return 0;
    }
    ++str;
  }

  *z = strtol(str, &end, radix);
  return (end != str && *end == '\0');
}

/* }}} */

/* {{{ parse_int_values(t, in, out) */

static int       parse_int_values(testspec_t *t, mp_int *in, mp_int *out,
				  mp_result *rval)
{
  int i, pos = 0;
  char *str;

  if(rval != NULL)
    *rval = MP_OK;  /* default */

  if(in != NULL) {
    for(i = 0; i < t->num_inputs; ++i) {
      str = t->input[i];

      trim_line(str);

      if(*str == '=') {
	int k = abs(atoi(str + 1)) - 1;
	
	if(k < 0 || k >= i) {
	  fprintf(stderr, "Line %d: Invalid input back-reference [%s]\n",
		  t->line, str);
	  return 0;
	}
      
	in[i] = in[k];
      } else {
	mp_int reg = g_zreg + pos++; /* grab next free register */
	
	if(read_int_value(reg, str) != MP_OK) {
	  fprintf(stderr, "Line %d: Invalid input value [%s]\n",
		  t->line, str);
	  return 0;
	}
	
	in[i] = reg;
      }
    }
  }

  for(i = 0; i < t->num_outputs; ++i) {
    mp_int reg = g_zreg + pos++;

    str = t->output[i];
    
    trim_line(str);

    if(strcmp(str, "?") == 0)
      mp_int_zero(reg);
    else if(*str == '$') {
      mp_result code;

      if(!parse_result_code(str, &code)) {
	fprintf(stderr, "Line %d: Invalid result code [%s]\n", 
		t->line, str);
	return 0;
      }
      else if(rval == NULL) {
	fprintf(stderr, "Line %d: Result code not permitted here [%s]\n",
		t->line, str);
	return 0;
      }
      else
	*rval = code;

      /* Provide a dummy value for the corresponding output */
      mp_int_zero(reg);
    }
    else if(out != NULL && read_int_value(reg, str) != MP_OK) {
      fprintf(stderr, "Line %d: Invalid output value [%s]\n",
	      t->line, str);
      return 0;
    }

    if(out != NULL)
      out[i] = reg;
  }

  return 1;
}

/* }}} */

/* {{{ parse_rat_values(t, in, out) */

static int       parse_rat_values(testspec_t *t, mp_rat *in, mp_rat *out,
				  mp_result *rval)
{
  int i, pos = 0;
  char *str;

  if(rval != NULL)
    *rval = MP_OK;  /* default */

  if(in != NULL) {
    for(i = 0; i < t->num_inputs; ++i) {
      str = t->input[i];

      trim_line(str);

      if(*str == '=') {
	int k = abs(atoi(str + 1)) - 1;
	
	if(k < 0 || k >= i) {
	  fprintf(stderr, "Line %d: Invalid input back-reference [%s]\n",
		  t->line, str);
	  return 0;
	}
      
	in[i] = in[k];
      } else {
	mp_rat reg = g_qreg + pos++; /* grab next free register */
	
	if(read_rat_value(reg, str) != MP_OK) {
	  fprintf(stderr, "Line %d: Invalid input value [%s]\n",
		  t->line, str);
	  return 0;
	}
	
	in[i] = reg;
      }
    }
  }

  for(i = 0; i < t->num_outputs; ++i) {
    mp_rat reg = g_qreg + pos++;

    str = t->output[i];
    
    trim_line(str);

    if(strcmp(str, "?") == 0)
      mp_rat_zero(reg);
    else if(*str == '$') {
      mp_result code;

      if(!parse_result_code(str, &code)) {
	fprintf(stderr, "Line %d: Invalid result code [%s]\n", 
		t->line, str);
	return 0;
      }
      else if(rval == NULL) {
	fprintf(stderr, "Line %d: Result code not permitted here [%s]\n",
		t->line, str);
	return 0;
      }
      else
	*rval = code;

      /* Provide a dummy value for the corresponding output */
      mp_rat_zero(reg);
    }
    else if(out != NULL && read_rat_value(reg, str) != MP_OK) {
      fprintf(stderr, "Line %d: Invalid output value [%s]\n",
	      t->line, str);
      return 0;
    }

    if(out != NULL)
      out[i] = reg;
  }

  return 1;
}

/* }}} */

/* {{{ parse_result_code(str, *code) */

static int       parse_result_code(char *str, mp_result *code)
{
  if(str[0] == '$') {
    if(str[1] == '#') {
      long v;

      if(!read_long(&v, str + 2)) 
	return 0;

      *code = (mp_result) v;
    } 
    else if(strcmp(str + 1, "MP_OK") == 0 ||
	    strcmp(str + 1, "MP_FALSE") == 0) 
      *code = MP_OK;
    else if(strcmp(str + 1, "MP_TRUE") == 0) 
      *code = MP_TRUE;
    else if(strcmp(str + 1, "MP_MEMORY") == 0) 
      *code = MP_MEMORY;
    else if(strcmp(str + 1, "MP_RANGE") == 0)
      *code = MP_RANGE;
    else if(strcmp(str + 1, "MP_UNDEF") == 0)
      *code = MP_UNDEF;
    else if(strcmp(str + 1, "MP_TRUNC") == 0)
      *code = MP_TRUNC;
    else if(strcmp(str + 1, "MP_ROUND_UP") == 0)
      *code = MP_ROUND_UP;
    else if(strcmp(str + 1, "MP_ROUND_DOWN") == 0)
      *code = MP_ROUND_DOWN;
    else if(strcmp(str + 1, "MP_ROUND_HALF_UP") == 0)
      *code = MP_ROUND_HALF_UP;
    else if(strcmp(str + 1, "MP_ROUND_HALF_DOWN") == 0)
      *code = MP_ROUND_HALF_DOWN;
    else
      return 0;
  }

  return 1;
}

/* }}} */

/* {{{ parse_binary(str, buf, limit) */

static int       parse_binary(char *str, unsigned char *buf, int limit)
{
  int    pos = 0;
  char  *tok;

  trim_line(str);

  for(tok = strtok(str, ".");
      tok != NULL && pos < limit;
      tok = strtok(NULL, ".")) {
    long v;

    if(!read_long(&v, tok) || v > UCHAR_MAX || v < 0)
      return -1;
      
    buf[pos++] = (unsigned char)v;
  }

  return pos;
}

/* }}} */

/* {{{ done_testing() */

static void done_testing(void)
{
  int i;

  for(i = 0; i < NUM_REGS; ++i) {
    mp_int_clear(g_zreg + i);
    mp_rat_clear(g_qreg + i);
  }
}

/* }}} */

/*------------------------------------------------------------------------*/
/* Global functions visible to callers outside this file                  */

/* {{{ init_testing() */

void init_testing(void)
{
  static int is_done = 0;
  int i;

  if(is_done)
    return;

  for(i = 0; i < NUM_REGS; ++i) {
    assert(mp_int_init(g_zreg + i) == MP_OK);
    assert(mp_rat_init(g_qreg + i) == MP_OK);
  }
  
  imath_errmsg = g_output;

  assert(atexit(done_testing) == 0);
  is_done = 1;
}

/* }}} */

/* {{{ reset_registers() */

void reset_registers(void)
{
  int i;

  for(i = 0; i < NUM_REGS; ++i) {
    mp_int_zero(g_zreg + i);
    mp_rat_zero(g_qreg + i);
  }
}

/* }}} */

/* {{{ test_neg(t, ofp) */

int test_neg(testspec_t *t, FILE *ofp) 
{ 
  mp_int in[2], out[1];
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_neg(in[0], in[1])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[1], out[0]) != 0) {
    mp_int_to_string(in[1], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_abs(t, ofp) */

int test_abs(testspec_t *t, FILE *ofp)
{
  mp_int in[2], out[1];
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_abs(in[0], in[1])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[1], out[0]) != 0) {
    mp_int_to_string(in[1], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_add(t, ofp) */

int test_add(testspec_t *t, FILE *ofp)
{
  mp_int in[3], out[1];
  int    v;
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if(strcmp(t->code, "addv") == 0) {
    if((res = mp_int_to_int(in[1], &v)) != MP_OK)
      return imath_errno = res, 0;
    if((res = mp_int_add_value(in[0], v, in[2])) != expect)
      return imath_errno = res, 0;
  } else {
    if((res = mp_int_add(in[0], in[1], in[2])) != expect)
      return imath_errno = res, 0;
  }

  if(expect == MP_OK && mp_int_compare(in[2], out[0]) != 0) {
    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_sub(t, ofp) */

int test_sub(testspec_t *t, FILE *ofp)
{
  mp_int in[3], out[1];
  int    v;
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if(strcmp(t->code, "subv") == 0) {
    if((res = mp_int_to_int(in[1], &v)) != MP_OK)
      return imath_errno = res, 0;
    if((res = mp_int_sub_value(in[0], v, in[2])) != expect)
      return imath_errno = res, 0;
  } else {
    if((res = mp_int_sub(in[0], in[1], in[2])) != expect)
      return imath_errno = res, 0;
  }

  if(expect == MP_OK && mp_int_compare(in[2], out[0]) != 0) {
    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_mul(t, ofp) */

int test_mul(testspec_t *t, FILE *ofp)
{
  mp_int in[3], out[1];
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_mul(in[0], in[1], in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[2], out[0]) != 0) {
    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_mulp2(t, ofp) */

int test_mulp2(testspec_t *t, FILE *ofp)
{
  mp_int in[3], out[1];
  mp_result res, expect;
  int p2;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_to_int(in[1], &p2)) != MP_OK) 
    return imath_errno = res, 0;

  if((res = mp_int_mul_pow2(in[0], p2, in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[2], out[0]) != 0) {
    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_mulv(t, ofp) */

int test_mulv(testspec_t *t, FILE *ofp)
{
  mp_int in[3], out[1];
  mp_result res, expect;
  int v;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_to_int(in[1], &v)) != MP_OK) 
    return imath_errno = res, 0;

  if((res = mp_int_mul_value(in[0], v, in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[2], out[0]) != 0) {
    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_sqr(t, ofp) */

int test_sqr(testspec_t *t, FILE *ofp)
{
  mp_int in[2], out[1];
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_sqr(in[0], in[1])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[1], out[0]) != 0) {
    mp_int_to_string(in[1], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_div(t, ofp) */

int test_div(testspec_t *t, FILE *ofp)
{
  mp_int in[4], out[2];
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_div(in[0], in[1], in[2], in[3])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && 
     ((mp_int_compare(in[2], out[0]) != 0) ||
      (mp_int_compare(in[3], out[1]) != 0))) {
    int len;
    char *str;

    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    str = g_output + (len = strlen(g_output));
    *str++ = ',';
    mp_int_to_string(in[3], 10, str, OUTPUT_LIMIT - (len + 1));
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_divp2(t, ofp) */

int test_divp2(testspec_t *t, FILE *ofp)
{
  mp_int in[4], out[2];
  mp_result res, expect;
  int p2;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_to_int(in[1], &p2)) != MP_OK)
    return imath_errno = res, 0;

  if((res = mp_int_div_pow2(in[0], p2, in[2], in[3])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK &&
     ((mp_int_compare(in[2], out[0]) != 0) ||
      (mp_int_compare(in[3], out[1]) != 0))) {
    int len;
    char *str;
    
    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    str = g_output + (len = strlen(g_output));
    *str++ = ',';
    mp_int_to_string(in[3], 10, str, OUTPUT_LIMIT - (len + 1));
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_divv(t, ofp) */

int test_divv(testspec_t *t, FILE *ofp)
{
  mp_int in[3], out[2];
  mp_result res, expect;
  int v, rem, orem;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_to_int(in[1], &v)) != MP_OK)
    return imath_errno = res, 0;
  if((res = mp_int_to_int(out[1], &orem)) != MP_OK)
    return imath_errno = res, 0;

  if((res = mp_int_div_value(in[0], v, in[2], &rem)) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK &&
     ((mp_int_compare(in[2], out[0]) != 0) || (rem != orem))) {
    char *str;
    
    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    str = g_output + strlen(g_output);
    *str++ = ',';
    sprintf(str, "%d", rem);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_expt(t, ofp) */

int test_expt(testspec_t *t, FILE *ofp) 
{
  mp_int in[3], out[1];
  mp_result res, expect;
  int pow;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_to_int(in[1], &pow)) != MP_OK)
    return imath_errno = res, 0;

  if((res = mp_int_expt(in[0], pow, in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[2], out[0]) != 0) {
    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_exptv(t, ofp) */

int test_exptv(testspec_t *t, FILE *ofp) 
{
  mp_int in[3], out[1];
  mp_result res, expect;
  int a, b;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_to_int(in[0], &a)) != MP_OK) 
    return imath_errno = res, 0;
  if((res = mp_int_to_int(in[1], &b)) != MP_OK)
    return imath_errno = res, 0;

  if((res = mp_int_expt_value(a, b, in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[2], out[0]) != 0) {
    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_mod(t, ofp) */

int test_mod(testspec_t *t, FILE *ofp)
{
  mp_int in[3], out[1];
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_mod(in[0], in[1], in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[2], out[0]) != 0) {
    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_gcd(t, ofp) */

int test_gcd(testspec_t *t, FILE *ofp)
{
  mp_int in[3], out[1];
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_gcd(in[0], in[1], in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[2], out[0]) != 0) {
    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_egcd(t, ofp) */

int test_egcd(testspec_t *t, FILE *ofp)
{
  mp_int in[5], out[3], t1 = g_zreg + 8, t2 = g_zreg + 9;
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_egcd(in[0], in[1], in[2], in[3], in[4])) != expect)
    return imath_errno = res, 0;

  /* If we got an error we expected, return success immediately */
  if(expect != MP_OK)
    return 1;

  if((mp_int_compare(in[2], out[0]) != 0) ||
     (mp_int_compare(in[3], out[1]) != 0) ||
     (mp_int_compare(in[4], out[2]) != 0)) {
    int len, len2;
    char *str;

    /* Failure might occur because the tester computed x and y in a different
       way than we did.  Verify that the results are correct before reporting
       an error. */
    mp_int_mul(in[3], in[0], t1);
    mp_int_mul(in[4], in[1], t2);
    mp_int_add(t1, t2, t2);
    if(mp_int_compare(t2, in[2]) == 0) 
      return 1;

    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    str = g_output + (len = strlen(g_output));
    *str++ = ',';
    mp_int_to_string(in[3], 10, str, OUTPUT_LIMIT - (len + 1));
    str = str + (len2 = strlen(str));
    *str++ = ',';
    mp_int_to_string(in[4], 10, str, OUTPUT_LIMIT - (len + len2 + 2));
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_sqrt(t, ofp) */

int test_sqrt(testspec_t *t, FILE *ofp)
{
  mp_int    in[2], out[1];
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_sqrt(in[0], in[1])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[1], out[0]) != 0) {
    mp_int_to_string(in[1], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }
  
  return 1;
}

/* }}} */

/* {{{ test_invmod(t, ofp) */

int test_invmod(testspec_t *t, FILE *ofp)
{
  mp_int in[3], out[1];
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_invmod(in[0], in[1], in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[2], out[0]) != 0) {
    mp_int_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;  
}

/* }}} */

/* {{{ test_exptmod(t, ofp) */

int test_exptmod(testspec_t *t, FILE *ofp)
{
  mp_int in[4], out[1];
  mp_result res, expect;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_exptmod(in[0], in[1], in[2], in[3])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[3], out[0]) != 0) {
    mp_int_to_string(in[3], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_exptmod_ev(t, ofp) */

int test_exptmod_ev(testspec_t *t, FILE *ofp)
{
  mp_int in[4], out[1];
  mp_result res, expect;
  int       v;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_to_int(in[1], &v)) != MP_OK)
    return imath_errno = res, 0;

  if((res = mp_int_exptmod_evalue(in[0], v, in[2], in[3])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[3], out[0]) != 0) {
    mp_int_to_string(in[3], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_exptmod_bv(t, ofp) */

int test_exptmod_bv(testspec_t *t, FILE *ofp)
{
  mp_int in[4], out[1];
  mp_result res, expect;
  int       v;

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_to_int(in[0], &v)) != MP_OK)
    return imath_errno = res, 0;

  if((res = mp_int_exptmod_bvalue(v, in[1], in[2], in[3])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in[3], out[0]) != 0) {
    mp_int_to_string(in[3], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_tostr(t, ofp) */

int test_tostr(testspec_t *t, FILE *ofp)
{
  mp_int    in[2];
  int       radix;
  mp_result res, len;

  if(!parse_int_values(t, in, NULL, NULL))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_to_int(in[1], &radix)) != MP_OK)
    return imath_errno = MP_BADARG, 0;

  if(radix < MP_MIN_RADIX || radix > MP_MAX_RADIX)
    return imath_errno = MP_RANGE, 0;

  trim_line(t->output[0]);

  len = mp_int_string_len(in[0], radix);

  if((res = mp_int_to_string(in[0], radix, g_output, len)) != MP_OK)
    return imath_errno = res, 0;

  if(strcmp(t->output[0], g_output) != 0) 
    return imath_errno = OTHER_ERROR, 0;

  return 1;
}

/* }}} */

/* {{{ test_tobin(t, ofp) */

int test_tobin(testspec_t *t, FILE *ofp)
{
  mp_int in[1];
  int    test_len, out_len;
  mp_result res;

  if(!parse_int_values(t, in, NULL, NULL))
    return imath_errno = MP_BADARG, 0;

  trim_line(t->output[0]);
  if((out_len = parse_binary(t->output[0], g_bin1, sizeof(g_bin1))) < 0)
    return imath_errno = MP_BADARG, 0;
  
  if((test_len = mp_int_binary_len(in[0])) != out_len) {
    sprintf(g_output, "Output lengths do not match (want %d, got %d)",
	    test_len, out_len);
    return imath_errno = OTHER_ERROR, 0;
  }

  if((res = mp_int_to_binary(in[0], g_bin2, sizeof(g_bin2))) != MP_OK)
    return imath_errno = res, 0;

  if(memcmp(g_bin1, g_bin2, test_len) != 0) {
    int pos = 0, i;

    for(i = 0; i < test_len - 1; ++i)
      pos += sprintf(g_output + pos, "%d.", g_bin2[i]);

    sprintf(g_output + pos, "%d", g_bin2[i]);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_read_binary(t, ofp) */

int test_read_binary(testspec_t *t, FILE *ofp)
{
  mp_int out[1], in = g_zreg + 1;
  int in_len;
  mp_result res, expect;

  if(!parse_int_values(t, NULL, out, &expect))
    return imath_errno = MP_BADARG, 0;

  trim_line(t->input[0]);
  if((in_len = parse_binary(t->input[0], g_bin1, sizeof(g_bin1))) < 0)
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_read_binary(in, g_bin1, in_len)) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in, out[0]) != 0) {
    mp_int_to_string(in, 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_to_uns(t, ofp) */

int test_to_uns(testspec_t *t, FILE *ofp)
{
  mp_int in[1];
  int    test_len, out_len;
  mp_result res;

  if(!parse_int_values(t, in, NULL, NULL))
    return imath_errno = MP_BADARG, 0;

  trim_line(t->output[0]);
  if((out_len = parse_binary(t->output[0], g_bin1, sizeof(g_bin1))) < 0)
    return imath_errno = MP_BADARG, 0;
  
  if((test_len = mp_int_unsigned_len(in[0])) != out_len) {
    sprintf(g_output, "Output lengths do not match (want %d, got %d)",
	    test_len, out_len);
    return imath_errno = OTHER_ERROR, 0;
  }

  if((res = mp_int_to_unsigned(in[0], g_bin2, sizeof(g_bin2))) != MP_OK)
    return imath_errno = res, 0;

  if(memcmp(g_bin1, g_bin2, test_len) != 0) {
    int pos = 0, i;

    for(i = 0; i < test_len - 1; ++i)
      pos += sprintf(g_output + pos, "%d.", g_bin2[i]);

    sprintf(g_output + pos, "%d", g_bin2[i]);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_read_uns(t, ofp) */

int test_read_uns(testspec_t *t, FILE *ofp)
{
  mp_int out[1], in = g_zreg + 1;
  int in_len;
  mp_result res, expect;

  if(!parse_int_values(t, NULL, out, &expect))
    return imath_errno = MP_BADARG, 0;

  trim_line(t->input[0]);
  if((in_len = parse_binary(t->input[0], g_bin1, sizeof(g_bin1))) < 0)
    return imath_errno = MP_BADARG, 0;

  if((res = mp_int_read_unsigned(in, g_bin1, in_len)) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_int_compare(in, out[0]) != 0) {
    mp_int_to_string(in, 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_meta(t, ofp) */

int test_meta(testspec_t *t, FILE *ofp)
{
  mp_int *in = NULL, *out = NULL;
  int i, j;
  mp_result expect;

  if(t->num_inputs > 0)
    in = calloc(t->num_inputs, sizeof(mp_int));
  if(t->num_outputs > 0)
    out = calloc(t->num_outputs, sizeof(mp_int));

  if(!parse_int_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  fprintf(ofp, "Test '%s' defined at line %d\n",
	  t->code, t->line);
  fprintf(ofp, "Expected result: %d\n", expect);
  fprintf(ofp, "Input values: %d\n", t->num_inputs);
  for(i = 0; i < t->num_inputs; ++i) {
    mp_int_to_string(in[i], 10, g_output, OUTPUT_LIMIT);

    fprintf(ofp, " %2d.) %s", i + 1, g_output);

    for(j = i - 1; j >= 0; --j)
      if(in[j] == in[i]) {
	fprintf(ofp, " (=> %d)", j + 1);
	break;
      }

    fputc('\n', ofp);
  }
  fprintf(ofp, "Output values: %d\n", t->num_outputs);
  for(i = 0; i < t->num_outputs; ++i) {
    mp_int_to_string(out[i], 10, g_output, OUTPUT_LIMIT);

    fprintf(ofp, " %2d.) %s\n", i + 1, g_output);
  }
  return 1;
}

/* }}} */

/* {{{ test_qneg(t, ofp) */

int test_qneg(testspec_t *t, FILE *ofp)
{
  mp_rat in[2], out[1];
  mp_result res, expect;

  if(!parse_rat_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_rat_neg(in[0], in[1])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_rat_compare(in[1], out[0]) != 0) {
    mp_rat_to_string(in[1], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_qrecip(t, ofp) */

int test_qrecip(testspec_t *t, FILE *ofp)
{
  mp_rat in[2], out[1];
  mp_result res, expect;

  if(!parse_rat_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_rat_recip(in[0], in[1])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK  && mp_rat_compare(in[1], out[0]) != 0) {
    mp_rat_to_string(in[1], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_qabs(t, ofp) */

int test_qabs(testspec_t *t, FILE *ofp)
{
  mp_rat in[2], out[1];
  mp_result res, expect;

  if(!parse_rat_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_rat_abs(in[0], in[1])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_rat_compare(in[1], out[0]) != 0) {
    mp_rat_to_string(in[1], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_qadd(t, ofp) */

int test_qadd(testspec_t *t, FILE *ofp)
{
  mp_rat in[3], out[1];
  mp_result res, expect;

  if(!parse_rat_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_rat_add(in[0], in[1], in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_rat_compare(in[2], out[0]) != 0) {
    mp_rat_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_qsub(t, ofp) */

int test_qsub(testspec_t *t, FILE *ofp)
{
  mp_rat in[3], out[1];
  mp_result res, expect;

  if(!parse_rat_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_rat_sub(in[0], in[1], in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_rat_compare(in[2], out[0]) != 0) {
    mp_rat_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_qmul(t, ofp) */

int test_qmul(testspec_t *t, FILE *ofp)
{
  mp_rat in[3], out[1];
  mp_result res, expect;

  if(!parse_rat_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_rat_mul(in[0], in[1], in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_rat_compare(in[2], out[0]) != 0) {
    mp_rat_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_qdiv(t, ofp) */

int test_qdiv(testspec_t *t, FILE *ofp)
{
  mp_rat in[3], out[1];
  mp_result res, expect;

  if(!parse_rat_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_rat_div(in[0], in[1], in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_rat_compare(in[2], out[0]) != 0) {
    mp_rat_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_qaddz(t, ofp) */

int test_qaddz(testspec_t *t, FILE *ofp)
{
  mp_rat in[3], out[1];
  mp_result res, expect;

  if(!parse_rat_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if(!mp_rat_is_integer(in[1])) {
    fprintf(stderr, "Line %d: Second argument must be an integer (test_qaddz)\n",
	    t->line);
    return imath_errno = MP_BADARG, 0;
  }
  
  if((res = mp_rat_add_int(in[0], MP_NUMER_P(in[1]), in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_rat_compare(in[2], out[0]) != 0) {
    mp_rat_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_qsubz(t, ofp) */

int test_qsubz(testspec_t *t, FILE *ofp)
{
  mp_rat in[3], out[1];
  mp_result res, expect;

  if(!parse_rat_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if(!mp_rat_is_integer(in[1])) {
    fprintf(stderr, "Line %d: Second argument must be an integer (test_qsubz)\n",
	    t->line);
    return imath_errno = MP_BADARG, 0;
  }
  
  if((res = mp_rat_sub_int(in[0], MP_NUMER_P(in[1]), in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_rat_compare(in[2], out[0]) != 0) {
    mp_rat_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_qmulz(t, ofp) */

int test_qmulz(testspec_t *t, FILE *ofp)
{
  mp_rat in[3], out[1];
  mp_result res, expect;

  if(!parse_rat_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if(!mp_rat_is_integer(in[1])) {
    fprintf(stderr, "Line %d: Second argument must be an integer (test_qmulz)\n",
	    t->line);
    return imath_errno = MP_BADARG, 0;
  }
  
  if((res = mp_rat_mul_int(in[0], MP_NUMER_P(in[1]), in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_rat_compare(in[2], out[0]) != 0) {
    mp_rat_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_qdivz(t, ofp) */

int test_qdivz(testspec_t *t, FILE *ofp)
{
  mp_rat in[3], out[1];
  mp_result res, expect;

  if(!parse_rat_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if(!mp_rat_is_integer(in[1])) {
    fprintf(stderr, "Line %d: Second argument must be an integer (test_qdivz)\n",
	    t->line);
    return imath_errno = MP_BADARG, 0;
  }
  
  if((res = mp_rat_div_int(in[0], MP_NUMER_P(in[1]), in[2])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK && mp_rat_compare(in[2], out[0]) != 0) {
    mp_rat_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_qexpt(t, ofp) */

int test_qexpt(testspec_t *t, FILE *ofp)
{
  mp_rat in[3], out[1];
  mp_result res, expect;
  int power;

  if(!parse_rat_values(t, in, out, &expect))
    return imath_errno = MP_BADARG, 0;

  if(!mp_rat_is_integer(in[1])) {
    fprintf(stderr, "Line %d: Second argument must be an integer (test_qexpt)\n",
	    t->line);
    return imath_errno = MP_BADARG, 0;
  }

  if((res = mp_int_to_int(MP_NUMER_P(in[1]), &power)) != MP_OK)
    return imath_errno = res, 0;
  
  if((res = mp_rat_expt(in[0], power, in[2])) != expect)
    return imath_errno = res, 0;
  
  if(expect == MP_OK && mp_rat_compare(in[2], out[0]) != 0) {
    mp_rat_to_string(in[2], 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* {{{ test_qtostr(t, ofp) */

int test_qtostr(testspec_t *t, FILE *ofp)
{
  mp_rat    in[2];
  long      radix;
  mp_result res, len;

  if(!parse_rat_values(t, in, NULL, NULL))
    return imath_errno = MP_BADARG, 0;

  trim_line(t->input[1]);
  if(!read_long(&radix, t->input[1]))
    return imath_errno = MP_BADARG, 0;

  if(radix < MP_MIN_RADIX || radix > MP_MAX_RADIX) {
    fprintf(stderr, "Line %d: Radix %ld out of range\n", 
	    t->line, radix);
    return imath_errno = MP_RANGE, 0;
  }

  trim_line(t->output[0]);
  len = mp_rat_string_len(in[0], radix);

  if((res = mp_rat_to_string(in[0], radix, g_output, len)) != MP_OK)
    return imath_errno = res, 0;

  if(strcmp(t->output[0], g_output) != 0)
    return imath_errno = OTHER_ERROR, 0;

  return 1;
}

/* }}} */

/* {{{ test_qtodec(t, ofp) */

int test_qtodec(testspec_t *t, FILE *ofp)
{
  mp_rat in[4];
  long   radix, prec;
  mp_round_mode rmode;
  mp_result res, expect = MP_OK, len;

  if(!parse_rat_values(t, in, NULL, NULL))
    return imath_errno = MP_BADARG, 0;

  if(t->output[0][0] == '$' && !parse_result_code(t->output[0], &expect)) {
    fprintf(stderr, "Line %d: Invalid result code [%s]\n", 
	    t->line, t->output[0]);
    return 0;
  }
  
  trim_line(t->input[1]);
  trim_line(t->input[2]);
  trim_line(t->input[3]);
  if(!read_long(&radix, t->input[1]))
    return imath_errno = MP_BADARG, 0;
  if(!read_long(&prec, t->input[2]))
    return imath_errno = MP_BADARG, 0;
  if(!read_long((long *)&rmode, t->input[3]))
    return imath_errno = MP_BADARG, 0;

  if(prec < 0) {
    fprintf(stderr, "Line %d: Precision %ld out of range\n", 
	    t->line, prec);
    return imath_errno = MP_RANGE, 0;
  }
  
  trim_line(t->output[0]);
  len = mp_rat_decimal_len(in[0], radix, prec);

  if((res = mp_rat_to_decimal(in[0], radix, prec, 
                              rmode, g_output, len)) != expect)
    return imath_errno = res, 0;
  
  if(res == MP_OK && strcmp(t->output[0], g_output) != 0)
    return imath_errno = OTHER_ERROR, 0;
  
  return 1;
}

/* }}} */

/* {{{ test_qrdec(t, ofp) */

int test_qrdec(testspec_t *t, FILE *ofp)
{
  mp_rat out[1], reg = g_qreg + 1;
  long   radix;
  mp_result res, expect;

  if(!parse_rat_values(t, NULL, out, &expect)) 
    return imath_errno = MP_BADARG, 0;
  
  trim_line(t->input[1]);
  if(!read_long(&radix, t->input[1]))
    return imath_errno = MP_BADARG, 0;

  if((res = mp_rat_read_decimal(reg, radix, t->input[0])) != expect)
    return imath_errno = res, 0;

  if(expect == MP_OK &&
     mp_rat_compare(reg, out[0]) != 0) {
    mp_rat_to_string(reg, 10, g_output, OUTPUT_LIMIT);
    return imath_errno = OTHER_ERROR, 0;
  }

  return 1;
}

/* }}} */

/* HERE THERE BE DRAGONS */
