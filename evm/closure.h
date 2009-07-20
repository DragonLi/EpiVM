#ifndef _CLOSURE_H
#define _CLOSURE_H

# ifndef WIN32
#  include <pthread.h>
#  define GC_THREADS
# else
#  define GC_WIN32_THREADS
# endif

#include <gc/gc.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>

//#define EMALLOC GC_MALLOC
//#define EREALLOC GC_REALLOC
//#define EFREE GC_FREE

#define EMALLOC malloc
#define EREALLOC realloc
#define EFREE free

#define MKCON (con*)EMALLOC(sizeof(con))
#define MKFUN (fun*)EMALLOC(sizeof(fun))
#define MKTHUNK (thunk*)EMALLOC(sizeof(thunk))
#define MKCLOSURE (Closure*)EMALLOC(sizeof(Closure))
#define MKUNIT (void*)0

#define INTOP(op,x,y) MKINT((((int)x)>>1) op (((int)y)>>1))
#define CHECKEVALUATED(x) if(ISFUN(x) || ISTHUNK(x) \
    || ISFV(x)) return 0;

#define MKARGS(x) (void**)EMALLOC(sizeof(VAL)*(x));
#define MOREARGS(args,x) (void**)EREALLOC(args,sizeof(VAL)*(x));

typedef enum { 
    FUN, 
    THUNK, 
    CON, 
    INT, 
    BIGINT,
    FLOAT,
    BIGFLOAT,
    STRING, 
    UNIT, 
    PTR,
    FREEVAR 
} ClosureType;

typedef struct {
    int ty;
    void* info;
} Closure;

void dumpClosure(Closure* c);

typedef Closure* VAL;

#define GETTY(x) (ISINT(x) ? INT : ((ClosureType)(((x)->ty) >> 24)))
#define QGETTY(x) ((ClosureType)(((x)->ty) >> 24))
#define SETTY(x,t) (x)->ty = (((int)t) << 24)

#define REF(x) x
#define DEREF(x) 

typedef void*(*func)(void**);

typedef struct {
    func fn;
    void** args;
    void** arg_end;
    int arity;
} fun;

typedef struct {
    void* fn;
    void** args;
    int numargs;
} thunk;

typedef struct {
    int tag;
    void** args;
} con;

#define UPDATE(x,res) if (ISINT(res)) { x = MKINT(GETINT(res)); } else { \
                      SETTY(x, GETTY(res)); x->info=res->info; }
#define TAG(x) ((con*)((Closure*)x)->info)->tag

#define ISCON(x) GETTY(((Closure*)(x)))==CON
#define ISINT(x) ((((int)x)&1) == 1)
#define ISTHUNK(x) GETTY(((Closure*)(x)))==THUNK
#define ISFUN(x) GETTY(((Closure*)(x)))==FUN
#define ISFV(x) GETTY(((Closure*)(x)))==FREEVAR

#ifdef TRACEON
#define TRACE if(1)
#else
#define TRACE if(0)
#endif

// Evaluate x to head normal form
VAL DO_EVAL(VAL x);

//#define EVAL(x) DO_EVAL(x)
#define EVAL(x) ((x && (ISTHUNK(x) || ISFUN(x))) ? DO_EVAL(x) : x)

// Return a new constructor
VAL CONSTRUCTOR(int tag, int arity, void** block);
VAL CONSTRUCTOR1(int tag, VAL a1);
VAL CONSTRUCTOR2(int tag, VAL a1, VAL a2);
VAL CONSTRUCTOR3(int tag, VAL a1, VAL a2, VAL a3);
VAL CONSTRUCTOR4(int tag, VAL a1, VAL a2, VAL a3, VAL a4);
VAL CONSTRUCTOR5(int tag, VAL a1, VAL a2, VAL a3, VAL a4, VAL a5);

// Return a new function node
VAL CLOSURE(func x, int arity, int args, void** block);

// Add arguments to an already existing thunk
VAL CLOSURE_ADDN(VAL x, int args, void** block);
VAL CLOSURE_ADD1(VAL xin, VAL a1);
VAL CLOSURE_ADD2(VAL xin, VAL a1, VAL a2);
VAL CLOSURE_ADD3(VAL xin, VAL a1, VAL a2, VAL a3);
VAL CLOSURE_ADD4(VAL xin, VAL a1, VAL a2, VAL a3, VAL a4);
VAL CLOSURE_ADD5(VAL xin, VAL a1, VAL a2, VAL a3, VAL a4, VAL a5);

// Apply a closure to some arguments
VAL CLOSURE_APPLY(VAL x, int args, void** block);
VAL CLOSURE_APPLY1(VAL x, VAL a1);
VAL CLOSURE_APPLY2(VAL x, VAL a1, VAL a2);
VAL CLOSURE_APPLY3(VAL x, VAL a1, VAL a2, VAL a3);
VAL CLOSURE_APPLY4(VAL x, VAL a1, VAL a2, VAL a3, VAL a4);
VAL CLOSURE_APPLY5(VAL x, VAL a1, VAL a2, VAL a3, VAL a4, VAL a5);

// Project an argument from a constructor
#define PROJECT(x,arg) (((con*)((x)->info))->args[arg])
//void* DO_PROJECT(VAL x, int arg);

#define ASSIGNINT(t, x) t=MKINT(x);

//extern VAL one; 

void* MKINT(int x);
void* NEWBIGINT(char* bigint);
void* MKBIGINT(mpz_t* bigint);

void* MKSTR(char* str);
void* MKPTR(void* ptr);

// Get values from a closure
int GETINT(void* x);
mpz_t* GETBIGINT(void* x);
char* GETSTR(void* x);
void* GETPTR(void* x);

void* MKFREE(int x);

// Exit with fatal error
void ERROR(char* msg);

// Initialise everything
void init_evm();


#endif
