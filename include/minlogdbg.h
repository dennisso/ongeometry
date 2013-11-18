/*
 * MinLogDbg: Minimal Log and Debug library for C99	
 *	Dennis So
 *
 *	To use debug statements: define a DEBUG 
 *	(ie. gcc ... -D DEBUG ..., or #define DEBUG)
 */

#ifndef _MINLOGDBG_H_
#define _MINLOGDBG_H_

#define error_printf(...) \
	fprintf(stderr, "[error] %s:%d:%s(): ", __FILE__, \
	__LINE__, __func__); \
	fprintf(stderr, __VA_ARGS__);

#define warn_printf(...) \
	fprintf(stderr, "[warn] %s:%d:%s(): " , __FILE__, \
	__LINE__, __func__); \
	fprintf(stderr, __VA_ARGS__);

#define info_printf(...) \
	fprintf(stderr, "[info] %s:%d:%s(): " , __FILE__, \
	__LINE__, __func__); \
	fprintf(stderr, __VA_ARGS__);

#ifdef DEBUG
	#define debug_printf(...) \
		fprintf(stderr, "[debug] %s:%d:%s(): " , __FILE__, \
   	__LINE__, __func__); \
		fprintf(stderr, __VA_ARGS__);
#else
	#define debug_printf(...) 
#endif

#ifdef DEBUG
	#define debug_run(code) code
#else
	#define debug_run(code)
#endif

#endif // _MINLOGDBG_H_

