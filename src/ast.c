#include <stdlib.h>
#include <stdio.h>
#include "ast.h"

static void *xcalloc(size_t n, size_t sz){
    void *p = calloc(n, sz);
    if(!p) { perror("error"), exit(1);}
    return p;
}
#define ALLOC(type) ((type *) xcalloc(1, sizeof(type)))

EXP *makeEXPid(int line, char *id){
    EXP *e = ALLOC(EXP);
    e->line = line;
    e->kind = idK;
    e->val.idE = id;
    return e;
}

EXP *makeEXPintconst(int line, int intconst){
    EXP *e = ALLOC(EXP);
    e->line = line;
    e->kind = intconstK;
    e->val.intconstE = intconst;
    return e;
}

EXP *makeEXPboolconst(int line, bool boolconst){
    EXP *e = ALLOC(EXP);
    e->line = line;
    e->kind = boolconstK;
    e->val.boolconstE = boolconst;
    return e;
}

EXP *makeEXPnull(int line){
    EXP *e = ALLOC(EXP);
    e->line = line;
    e->kind = nullK;
    return e;
}

EXP *makeEXPthis(int line){
    EXP *e = ALLOC(EXP);
    e->line = line;
    e->kind = thisK;
    return e;
}


EXP *makeEXPbinop(int line, BinOp op, EXP *left, EXP *right){
    EXP *e = ALLOC(EXP);
    e->line = line;
    e->kind = binOpK;
    e->val.binOpE.op = op;
    e->val.binOpE.left = left;
    e->val.binOpE.right = right;
    return e;
}

EXP *makeEXPunop(int line, UnOp op, EXP *expr){
    EXP *e = ALLOC(EXP);
    e->line = line;
    e->kind = unOpK;
    e->val.unOpE.op = op;
    e->val.unOpE.expr = expr;
    return e;
}

EXP *makeEXParrayNew(int line, EXP *size){
    EXP *e = ALLOC(EXP);
    e->line = line;
    e->kind = arrayNewK;
    e->val.arrayNewE.size = size;
    return e; 
}

EXP *makeEXParrayIndex(int line, EXP *array, EXP *index){
    EXP *e = ALLOC(EXP);
    e->line = line;
    e->kind = arrayIndexK;
    e->val.arrayIndexE.array = array;
    e->val.arrayIndexE.index = index;
    return e; 
}

EXP *makeEXParrayLength(int line, EXP *array){
    EXP *e = ALLOC(EXP);
    e->line = line;
    e->kind = arrayLengthK;
    e->val.arrayLengthE.array = array;
    return e; 
}

EXP *makeEXPmethodCall(int line, EXP *receiver, char *methodName, EXP_LIST *args){
    EXP *e = ALLOC(EXP);
    e->line = line;
    e->kind = methodCallK;
    e->val.methodCallE.receiver = receiver;
    e->val.methodCallE.methodName = methodName;
    e->val.methodCallE.args = args;
    return e;
}

EXP *makeEXPobjectNew(int line, char *className){
    EXP *e = ALLOC(EXP);
    e->line = line;
    e->kind = objectNewK;
    e->val.objectNewE.className = className;
    return e;
}

EXP_LIST *makeEXPList(EXP *head, EXP_LIST *next){
    EXP_LIST *list = ALLOC(EXP_LIST);
    list->head = head;
    list->next = next;
    return list; 
}

static STMT *newSTMT(int line, StmtKind kind){
    STMT *s = calloc(1, sizeof(STMT));
    if(!s) { perror("calloc"); exit(1);}
    s->line = line;
    s->kind = kind;
    return s;
}

STMT *makeSTMTwhile(int line, EXP *cond, STMT *body){
    STMT *s = newSTMT(line, whileK);
    s->val.whileS.cond = cond;
    s->val.whileS.body = body;
    return s;
}

STMT *makeSTMTblock(int line, STMT_LIST *stmts){
    STMT *s = newSTMT(line, blockK);
    s->val.blockS = stmts;
    return s;
}

STMT *makeSTMTif(int line, EXP *cond,STMT *thenBranch, STMT *elseBranch){
    STMT *s = newSTMT(line, ifK );
    s->val.ifS.cond = cond;
    s->val.ifS.thenBranch = thenBranch;
    s->val.ifS.elseBranch = elseBranch;
    return s;
}

STMT *makeSTMTprint(int line, EXP *e){
    STMT *s = newSTMT(line, printK);
    s->val.printS = e;
    return s;
}

STMT *makeSTMTassign(int line, char *name, EXP *value){
    STMT *s = newSTMT(line, assignK);
    s->val.assignS.name = name;
    s->val.assignS.value = value;
    return s;
}

STMT *makeSTMTarrayAssign(int line, char*name, EXP *index, EXP *value){
    STMT *s = newSTMT(line, arrayAssignK);
    s->val.arrayAssignS.name = name;
    s->val.arrayAssignS.index = index;
    s->val.arrayAssignS.value = value;
    return s;
}

STMT_LIST *makeSTMTList(STMT *head, STMT_LIST *next){
    STMT_LIST * list = calloc(1, sizeof(STMT_LIST));
    if(!list) { perror("calloc"); exit(1);}
    list->head = head;
    list->next = next;
    return list;
}

TYPE *makeTYPE(int line, TypeKind kind, char *className ){
    TYPE *t = calloc(1, sizeof(TYPE));
    t->line = line; t->kind = kind; t->className = className;
    return t;
}

VAR_DECL *makeVARDECL(int line, TYPE *type, char *name, VAR_DECL *next){
    VAR_DECL *v = calloc(1, sizeof(VAR_DECL));
    v->line = line; v->type = type; v->name = name; v->next = next;
    return v;
}

BODY *makeBODY(VAR_DECL *locals, STMT_LIST *stmts, EXP *retEXP){
    BODY *b = calloc(1, sizeof(BODY));
    b->locals = locals; b->stmts = stmts; b->retExp = retEXP;
    return b;
}

METHOD *makeMETHOD(int line, TYPE *retTYPE, char *name, VAR_DECL *params
            , BODY *body, METHOD *next){
    METHOD *m = calloc(1, sizeof(METHOD));
    m->line = line;
    m->retType = retTYPE; m->name = name; m->params = params;
    m->next = next;
    
    if (body) {
        m->locals = body->locals;
        m->stmts  = body->stmts;
        m->retEXP = body->retExp;
        free(body);
    }

    return m;
}

CLASS_DECL *makeCLASS(int line, char *name, char *parent, VAR_DECL *fields, METHOD *methods, CLASS_DECL *next){
    CLASS_DECL *c = calloc(1, sizeof(CLASS_DECL));
    c->line = line; c->name = name; c->parent = parent;
    c->fields = fields; c->methods = methods; c->next = next;
    return c;
}

MAIN_CLASS *makeMAINCLASS(int line, char *name, char *argName, STMT *body){
    MAIN_CLASS *mc = calloc(1, sizeof(MAIN_CLASS));
    mc->line = line; mc->name =name; mc->argName = argName;
    mc->body = body;
    return mc;
}

PROGRAM *makePROGRAM(MAIN_CLASS *m, CLASS_DECL *classes){
    PROGRAM *p = calloc(1, sizeof(PROGRAM));
    p->mainClass = m; p->classes = classes;
    return p;
}
