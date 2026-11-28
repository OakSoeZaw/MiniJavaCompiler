#include <stdio.h>
#include <string.h>
#include "codegen.h"

typedef struct { char s[32]; } Val;
/* an LLVM operand: "%t3", "42", "true" */

typedef struct {
    FILE *out;
    int tmp;
    /* temp counter: %t0, %t1, ... */
    int lbl;
    /* label counter, for unique block names */
    char cur[48];
    /* label of the block currently being emitted */
    int errors;
} CG;

static Val newTmp(CG *g){ Val v; snprintf(v.s, sizeof v.s, "%%t%d", g->tmp++); return v; }
static Val litInt(int n){ Val v; snprintf(v.s, sizeof v.s, "%d", n); return v;}
static Val litStr(const char *s){ Val v; snprintf(v.s, sizeof v.s, "%s", s); return v;}

static void startBlock(CG *g, const char *label){
    fprintf(g->out, "%s:\n", label);
    snprintf(g->cur, sizeof g->cur, "%s", label);
}

/* LLVM type of a MiniJava type, booleans are i1 everywhere */
static const char *llType(TYPE *t){
    if(!t) return "i32";
    switch(t->kind){
        case intT: return "i32";
        case boolT: return "i1";
        default:    return "ptr";
        /* int [], classes, null    */
    }
}

static Val genExp(CG *g, EXP *e);

static Val genBin(CG *g, EXP *e){
    BinOp op = e->val.binOpE.op;
    EXP *L = e->val.binOpE.left;
    EXP *R = e->val.binOpE.right;

    if(op == andOp || op == orOp){
        int id = g->lbl++;
        char lhsBlk[48], rhsLbl[48], endLbl[48];
        /* lhs is where the execution start, that why it is Blk */
        const char *name = ( op == andOp ) ? "and" : "or";
        snprintf(rhsLbl, sizeof rhsLbl, "%s.rhs.%d", name, id);
        snprintf(endLbl, sizeof endLbl, "%s.end.%d", name , id);

        Val l = genExp(g, L);
        snprintf(lhsBlk, sizeof lhsBlk, "%s", g->cur); /* Block that evaluate LHS*/
        if(op == andOp) fprintf(g->out, "  br i1 %s, label %%%s, label %%%s\n", l.s, rhsLbl, endLbl);
        else            fprintf(g->out, "  br i1 %s, label %%%s, label %%%s\n", l.s, endLbl, rhsLbl);
        
        startBlock(g, rhsLbl);
        Val r = genExp(g, R);
        char rhsBlk[48];
        snprintf(rhsBlk, sizeof rhsBlk, "%s", g->cur); /* */


    }
}

static Val genExp(CG *g, EXP *e){
    switch(e->kind){
        case intconstK: return litInt(e->val.intconstE);
        case boolconstK: return litStr(e->val.boolconstE ? "true" : "false");
        case nullK:     return litStr("null");
        case unOpK:{
            Val v = genExp(g, e->val.unOpE.expr);
            Val r = newTmp(g);
            fprintf(g->out, "  %s = xor i1 %s, true\n", r.s, v.s);
            return r;
        }
        case binOpK: genBin(g, e);
        default:
            fprintf(stderr, "codegen: line %d: expression kind %d not supported yet\n ", e->line, e->kind);
            g->errors++;
            return litInt(0);
    }
}

static void genStmt(CG *g, STMT *s){
    if(!s) return;
    switch(s->kind){
        case blockK: 
            for(STMT_LIST *l = s->val.blockS; l ; l = l->next) genStmt(g, l->head);
            break;
        case printK:
            EXP *e = s->val.printS;
            Val v = genExp(g, e);
            /* e->kind is checked first as a null-pointer check otherwise segfault might occur*/
            if(e->kind && e->type->kind == boolT){
                Val str = newTmp(g), call = newTmp(g);
            }

    }
}