#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ast.h"
#include "semantic.h"


/* ----- errors -----*/
static int sem_errors = 0;

static void semError(int line, const char *fmt, ...){
    va_list ap;
    sem_errors++;
    fprintf(stderr, "Semantic error at line %d: ", line);
    va_start(ap, fmt); vfprintf(stderr, fmt, ap); va_end(ap);
    fputc('\n', stderr);
}

/* ----- shared types ------ */
static TYPE T_INT = {0, intT, NULL};
static TYPE T_BOOL = {0, boolT, NULL};
static TYPE T_INTARR = {0, intArrayT, NULL};
static TYPE T_NULL = {0, nullT, NULL};
static TYPE T_ERR = {0, errorT, NULL};

static const char *typeName(TYPE *t){
    switch (t->kind){
        case intT : return "int";
        case boolT: return "boolean";
        case intArrayT: return "int[]";
        case nullT: return "null";
        case errorT: return "<error>";
        case classT: return t->className;
    }
    return "?";
}

static int sameType(TYPE *a, TYPE *b){
    if (a->kind != b->kind) return 0;
    return a->kind != classT || strcmp(a->className, b->className) == 0;
}

/* ----- class table -----*/
typedef struct CLASS_INFO CLASS_INFO;
struct CLASS_INFO{
    CLASS_DECL *decl;
    char *name;
    CLASS_INFO *parent;
    CLASS_INFO *next;
};

static CLASS_INFO *table = NULL;

static CLASS_INFO *lookupClass(const char *name){
    for(CLASS_INFO *c = table; c; c = c->next)
        if(strcmp(c->name, name) == 0) return c;
    return NULL;
}

static int isSubtype(TYPE *sub, TYPE *sup){
    if(sub->kind == errorT || sup->kind == errorT) return 1;
    if(sub->kind == nullT){
        return sup->kind == classT || sup->kind == intArrayT || sup->kind == nullT;
    }
    if(sub->kind != classT) return sub->kind == sup->kind;
    if(sup->kind != classT) return 0;
    if(!lookupClass(sub->className)) return 1;
    for (CLASS_INFO *c = lookupClass(sub->className); c; c= c->parent){
        if (strcmp(c->name, sup->className) == 0) return 1;
    }
    return 0;
}

/*-----pass 1: class table ------*/

static void addClass(CLASS_DECL *decl, char *name, int line){
    if (lookupClass(name)){ semError(line, "duplicate class '%s'", name); return;}
    CLASS_INFO *ci = calloc(1, sizeof(CLASS_INFO));
    if (!ci) { perror("calloc"); exit(1);}
    ci->decl = decl; ci->name =name; ci->next = table; table = ci;
}

static void pass1(PROGRAM *p){
    if(!p->mainClass) semError(1, "missing main class");
    if (p->mainClass) addClass(NULL, p->mainClass->name, p->mainClass->line);
    for (CLASS_DECL *c = p->classes; c; c= c-> next) addClass(c, c->name, c->line);
}

/* ----- pass2: inheritence -----*/

static void pass2(void){
    for(CLASS_INFO *ci = table; ci; ci = ci->next){
        if(!ci->decl || !ci->decl->parent) continue;
        const char *pn = ci->decl->parent;
        if(strcmp(pn, ci->name) == 0){
            semError(ci->decl->line, "class '%s' extends itself", pn); continue;
        }
        CLASS_INFO *p = lookupClass(pn);
        if (!p)       semError(ci->decl->line, "undefined parent class '%s'", pn);
        else if (!p->decl)  semError(ci->decl->line, "cannot extend main class '%s'",pn);
        else                ci->parent =p;
    }
    int total = 0;
    for (CLASS_INFO *c = table; c; c= c->next) total++;
    for(CLASS_INFO *ci = table; ci; ci= ci->next){
        int steps = 0;
        for(CLASS_INFO *c = ci->parent; c; c= c->parent){
            if(c == ci || ++steps > total){
                semError(ci->decl->line, "inheritence cycle involving '%s'", ci->name);
                ci->parent = NULL;
                break;
            }
        }
    }
}

/*------ lookups ------*/
static VAR_DECL *findField(CLASS_INFO *c, const char *name){
    for(; c; c= c->parent){
        if (!c->decl) continue;
        for (VAR_DECL *f = c->decl->fields; f; f=f->next){
            if(strcmp(f->name, name) == 0) return f;
        }
    }
    return NULL;
}

static METHOD *findMethod(CLASS_INFO *c, const char *name){
    for(; c; c= c->parent){
        if(!c->decl) continue;
        for(METHOD *m = c->decl->methods; m; m=m->next){
            if(strcmp(m->name, name) == 0) return m;
        }
    }
    return NULL;
}

/*-----pass 3: members------*/

static void checkTypeExists(TYPE *t){
    if(!t || t->kind != classT) return;
    CLASS_INFO *ci = lookupClass(t->className);
    if(!ci)     semError(t->line, "undefined class '%s'", t->className);
    else if(!ci->decl)  semError(t->line, "cannot use main class '%s' as a type", t->className);
}

static void checkDups(VAR_DECL *list, const char *what){
    for(VAR_DECL *a = list; a; a= a->next){
        for(VAR_DECL *b = a->next; b; b= b->next){
            if(strcmp(a->name, b->name ) == 0){
                semError(b->line, "duplicate %s '%s'", what, b->name);
            }
        }
    }
}

static void pass3(PROGRAM *p){
    for(CLASS_DECL *c = p->classes; c ;c= c->next){
        CLASS_INFO *ci = lookupClass(c->name);
        if(!ci || ci->decl != c) continue;

        checkDups(c->fields, "field");
        for(VAR_DECL *f = c->fields; f; f= f->next) checkTypeExists(f->type);

        for (METHOD *m = c->methods; m; m = m->next){
            for (METHOD *o = m->next; o; o= o->next){
                if(strcmp(m->name, o->name) == 0){
                    semError(o->line, "Duplicate mehtod '%s' in class  '%s'", o->name, c->name);
                }
            }
            checkTypeExists(m->retType);
            checkDups(m->params, "parameter");
            checkDups(m->locals, "local variable");
            for( VAR_DECL *v = m->params; v; v = v->next) checkTypeExists(v->type);
            for( VAR_DECL *v = m->locals; v; v= v->next){
                checkTypeExists(v->type);
                for(VAR_DECL *q = m->params; q; q = q->next)
                    if(strcmp(v->name, q->name) == 0){
                        semError(v->line, "local '%s' redeclares a parameter", v->name);
                    }
            }
            /* no overloading: same name in ancestor must match */
            METHOD *pm = ci->parent ? findMethod(ci->parent, m->name): NULL;
            if (pm) {
                int ok = sameType(m->retType, pm->retType);
                VAR_DECL *a = m->params, *b = pm->params;
                for(; a && b; a = a->next, b = b->next){
                    if (!sameType(a->type, b->type)) ok = 0;
                }
                if(a || b) ok = 0;
                if(!ok){
                    semError(m->line, "method '%s' overrides parent method with a different signature", m-> name);
                }
            }
        }
    }
}

/* --- pass 4: bodies ------*/
typedef struct { CLASS_INFO *cls; METHOD *method; } Env;

static TYPE *lookupVar(Env *env, const char *name){
    if(env->method){
        for(VAR_DECL *v = env->method->locals; v; v=v->next){
            if(strcmp(v->name, name) == 0) return v->type;
        }
        for(VAR_DECL *v = env->method->params; v; v= v->next){
            if(strcmp(v->name, name) == 0) return v->type;
        }
    }
    VAR_DECL *f = findField(env->cls, name);
    return f ? f->type: NULL;
}

static void expect(TYPE *t, TypeKind k, TYPE *want, int line, const char *ctx){
    if( t->kind != errorT && t->kind != k){
        semError(line, "%s must be %s, found %s", ctx, typeName(want), typeName(t));
    }
}

static TYPE *typeOf(EXP *e, Env *env);

static TYPE *binCheck(EXP *e, TypeKind k, TYPE *operand, TYPE *result, Env *env){
    TYPE *l = typeOf(e->val.binOpE.left, env);
    TYPE *r = typeOf(e->val.binOpE.right, env);
    expect(l, k, operand, e->line, "left operand");
    expect(r, k, operand, e->line, "right operand");
    return result;
}

static TYPE *typeOf(EXP *e, Env *env){
    if(!e) return &T_ERR;
    switch (e->kind){
        case intconstK: return &T_INT;
        case boolconstK: return &T_BOOL;
        case nullK: return &T_NULL;
        case thisK:
            if(!env->cls){ semError(e->line, "'this' used in main"); return &T_ERR;}
            return makeTYPE(e->line, classT, env->cls->name);
        case idK:{
            TYPE *t = lookupVar(env, e->val.idE);
            if(!t) { semError(e->line, "undefined variable '%s'", e->val.idE); return &T_ERR;}
            return t;
        }
        case binOpK:
            switch(e->val.binOpE.op){
                case plusOp: case minusOp: case timesOp: case divOp: case modOp:
                    return binCheck(e, intT, &T_INT, &T_INT, env);
                case lessOp: case leOp: case gtOp: case geOp:
                    return binCheck(e, intT, &T_INT, &T_BOOL, env);
                case andOp: case orOp:
                    return binCheck(e, boolT, &T_BOOL, &T_BOOL, env);
                case eqOp: case neqOp:{
                    TYPE *l = typeOf(e->val.binOpE.left, env);
                    TYPE *r = typeOf(e->val.binOpE.right, env);
                    if(!isSubtype(l, r) && !isSubtype(r, l)){
                        semError(e->line, "cannot compare %s with %s", typeName(l), typeName(r));
                    }
                    return &T_BOOL;
                }

            }
            return &T_ERR;
        case unOpK:
            expect(typeOf(e->val.unOpE.expr, env), boolT, &T_BOOL, e->line, "operan of '!'");
            return &T_BOOL;
        case arrayNewK:
            expect(typeOf(e->val.arrayNewE.size, env), intT, &T_INT, e->line, "array size");
            return &T_INTARR;
        case arrayIndexK:
            expect(typeOf(e->val.arrayIndexE.array, env), intArrayT, &T_INTARR, e->line, "indexed expression" );
            expect(typeOf(e->val.arrayIndexE.index, env), intT, &T_INT, e->line, "array index");
            return &T_INT;
        case arrayLengthK:
            expect(typeOf(e->val.arrayLengthE.array, env), intArrayT, &T_INTARR, e->line, "operand of .length");
            return &T_INT;
        case objectNewK:{
            CLASS_INFO *ci = lookupClass(e->val.objectNewE.className);
            if(!ci || !ci->decl){
                semError(e->line, "cannot instantiate '%s'", e->val.objectNewE.className);
                return &T_ERR;
            }
            return makeTYPE(e->line, classT, ci->name);
        }
        case methodCallK:{
            TYPE *rt = typeOf(e->val.methodCallE.receiver, env);
            EXP_LIST *a = e->val.methodCallE.args;
            TYPE *result = &T_ERR;
            METHOD *m = NULL;
            if(rt->kind == classT){
                CLASS_INFO *ci = lookupClass(rt->className);
                if(ci){
                    m = findMethod(ci, e->val.methodCallE.methodName);
                    if(!m) semError(e->line, "no method '%s' in class '%s'", e->val.methodCallE.methodName, rt->className);
                }
            }else if(rt->kind != errorT){
                semError(e->line, "cannot call a method on '%s'", typeName(rt));
            }
            VAR_DECL *p = m ? m->params : NULL;
            int n = 1;
            for(; a && p; a= a->next, p= p->next, n++){
                TYPE *at = typeOf(a->head, env);
                if(!isSubtype(at, p->type)){
                    semError(a->head->line, "argument %d of '%s': cannot pass %s as %s", n, m->name, typeName(at), typeName(p->type));
                }
            }
            if( m && (a || p)) semError(e->line, "wrong number of arguemts to '%s'", m->name);
            for(; a; a = a->next) typeOf(a->head, env);
            if(m) result = m ->retType;
            return result;
        }
    }
    return &T_ERR;
}

static void checkStmt(STMT *s, Env *env){
    if (!s) return;
    switch (s->kind){
        case blockK:
            for(STMT_LIST *l = s->val.blockS; l; l = l->next) checkStmt(l->head, env);
            break;
        case ifK:
            expect(typeOf(s->val.ifS.cond, env), boolT, &T_BOOL, s->line, "if condition");
            checkStmt(s->val.ifS.thenBranch, env);
            checkStmt(s->val.ifS.elseBranch, env);
            break;
        case whileK:
            expect(typeOf(s->val.whileS.cond, env), boolT, &T_BOOL, s->line, "while condition");
            checkStmt(s->val.whileS.body, env);
            break;
        case printK:{
            TYPE *pt = typeOf(s->val.printS, env);
            if (pt->kind != errorT && pt->kind != intT && pt->kind != boolT){
                semError(s->line, "println argument must be int or boolean, found %s", typeName(pt));
            }
            break;
        }  
        case assignK:{
            TYPE *vt = lookupVar(env, s->val.assignS.name);
            TYPE *et = typeOf(s->val.assignS.value, env);
            if(!vt) semError(s->line, "undefined variable '%s'", s->val.assignS.name);
            else if(!isSubtype(et, vt)){
                semError(s->line, "cannot assign %s to %s", typeName(et), typeName(vt));
            }
            break;
        }
        case arrayAssignK: {
            TYPE *vt = lookupVar(env, s->val.arrayAssignS.name);
            if(!vt) semError(s->line, "undefined varaible '%s'", s->val.arrayAssignS.name);
            else expect(vt, intArrayT, &T_INTARR, s->line, "indexed variable");
            expect(typeOf(s->val.arrayAssignS.index, env), intT, &T_INT, s->line, "array index");
            expect(typeOf(s->val.arrayAssignS.value, env), intT, &T_INT, s->line, "array element value");
            break;
        }
    }   
}

static void pass4(PROGRAM *p){
    if (p->mainClass){
        Env env = {NULL, NULL};
        checkStmt(p->mainClass->body, &env);
    }
    for(CLASS_DECL *c = p->classes; c; c = c->next){
        CLASS_INFO *ci = lookupClass(c->name);
        if(!ci || ci->decl != c) continue;
        for(METHOD *m = c->methods; m ; m = m->next){
            Env env = {ci, m};
            for (STMT_LIST *l = m->stmts; l; l = l->next) checkStmt(l->head, &env);
            if(m->retEXP){
                TYPE *rt = typeOf(m->retEXP, &env);
                if(!isSubtype(rt, m->retType)){
                    semError(m->retEXP->line, "method '%s' returns %s but declares %s",
                        m->name, typeName(rt), typeName(m->retType));
                }
            }
        }
    }
}

/* ---- entry -----*/

static void freeTable(void){
    CLASS_INFO *c = table;
    while(c) { CLASS_INFO * n = c->next; free(c); c= n;}
    table = NULL;
}

int checkProgram(PROGRAM *p){
    if(!p) return 0;
    freeTable();
    pass1(p);
    pass2();
    pass3(p);
    pass4(p);
    return sem_errors;
}