#include <stdlib.h>
#include "ast.h"

#ifndef NEW
#define NEW(type) ((type *) malloc(sizeof(type)))
#endif

EXP *makeEXPid(int line, char *id){
    EXP *e = NEW(EXP);
    e->line = line;
    e->kind = idK;
    e->val.idE = id;
    return e;
}

EXP *makeEXPintconst(int line, int intconst){
    EXP *e = NEW(EXP);
    e->line = line;
    e->kind = intconstK;
    e->val.intconstE = intconst;
    return e;
}

EXP *makeEXPboolconst(int line, bool boolconst){
    EXP *e = NEW(EXP);
    e->line = line;
    e->kind = boolconstK;
    e->val.boolconstE = boolconst;
    return e;
}

EXP *makeEXPnull(int line){
    EXP *e = NEW(EXP);
    e->line = line;
    e->kind = nullK;
    return e;
}

EXP *makeEXPthis(int line){
    EXP *e = NEW(EXP);
    e->line = line;
    e->kind = thisK;
    return e;
}


EXP *makeEXPbinop(int line, BinOp op, EXP *left, EXP *right){
    EXP *e = NEW(EXP);
    e->line = line;
    e->kind = binOpK;
    e->val.binOpE.op = op;
    e->val.binOpE.left = left;
    e->val.binOpE.right = right;
    return e;
}

EXP *makeEXPunop(int line, UnOp op, EXP *expr){
    EXP *e = NEW(EXP);
    e->line = line;
    e->kind = unOpK;
    e->val.unOpE.op = op;
    e->val.unOpE.expr = expr;
    return e;
}

EXP *makeEXParrayNew(int line, char *typeName, EXP *size){
    EXP *e = NEW(EXP);
    e->kind = arrayNewK;
    e->val.arrayNewE.typeName = typeName;
    e->val.arrayNewE.size = size;
    return e; 
}

EXP *makeEXParrayIndex(int line, EXP *array, EXP *index){
    EXP *e = NEW(EXP);
    e->line = line;
    e->kind = arrayIndexK;
    e->val.arrayIndexE.array = array;
    e->val.arrayIndexE.index = index;
    return e; 
}

EXP *makeEXParrayLength(int line, EXP *array){
    EXP *e = NEW(EXP);
    e->line = line;
    e->kind = arrayLengthK;
    e->val.arrayLengthE.array = array;
    return e; 
}

EXP *makeEXPmethodCall(int line, EXP *receiver, char *methodName, EXP_LIST *args){
    EXP *e = NEW(EXP);
    e->line = line;
    e->kind = methodCallK;
    e->val.methodCallE.receiver = receiver;
    e->val.methodCallE.methodName = methodName;
    e->val.methodCallE.args = args;
    return e;
}

EXP *makeEXPobjectNew(int line, char *className){
    EXP *e = NEW(EXP );
    e->line = line;
    e->kind = objectNewK;
    e->val.objectNewE.className = className;
    return e;
}

EXP_LIST *makeEXPList(EXP *head, EXP_LIST *next){
    EXP_LIST *list = NEW(EXP_LIST);
    list->head = head;
    list->next = next;
    return list; 
}