#include <stdlib.h>
#include "ast.h"

#ifndef NEW
#define NEW(type) ((type *) malloc(sizeof(type)))
#endif

EXP *makeEXPid(char *id){
    EXP *e = NEW(EXP);
    e->kind = idK;
    e->val.idE = id;
    return e;
}

EXP *makeEXPintconst(int intconst){
    EXP *e = NEW(EXP);
    e->kind = intconstK;
    e->val.intconstE = intconst;
    return e;
}

EXP *makeEXPboolconst(bool boolconst){
    EXP *e = NEW(EXP);
    e->kind = boolconstK;
    e->val.boolconstE = boolconst;
    return e;
}

EXP *makeEXPnull(void){
    EXP *e = NEW(EXP);
    e->kind = nullK;
    return e;
}

EXP *makeEXPthis(void){
    EXP *e = NEW(EXP);
    e->kind = thisK;
    return e;
}


EXP *makeEXPbinop(BinOp op, EXP *left, EXP *right){
    EXP *e = NEW(EXP);
    e->kind = binOpK;
    e->val.binOpE.op = op;
    e->val.binOpE.left = left;
    e->val.binOpE.right = right;
    return e;
}

EXP *makeEXPunop(UnOp op, EXP *expr){
    EXP *e = NEW(EXP);
    e->kind = unOpK;
    e->val.unOpE.op = op;
    e->val.unOpE.expr = expr;
    return e;
}

EXP *makeEXParrayNew(char *typeName, EXP *size){
    EXP *e = NEW(EXP);
    e->kind = arrayNewK;
    e->val.arrayNewE.typeName = typeName;
    e->val.arrayNewE.size = size;
    return e; 
}

EXP *makeEXParrayIndex(EXP *array, EXP *index){
    EXP *e = NEW(EXP);
    e->kind = arrayIndexK;
    e->val.arrayIndexE.array = array;
    e->val.arrayIndexE.index = index;
    return e; 
}

EXP *makeEXParrayLength(EXP *array){
    EXP *e = NEW(EXP);
    e->kind = arrayLengthK;
    e->val.arrayLengthE.array = array;
    return e; 
}

EXP *makeEXPmethodCall(EXP *receiver, char *methodName, EXP_LIST *args){
    EXP *e = NEW(EXP);
    e->kind = methodCallK;
    e->val.methodCallE.receiver = receiver;
    e->val.methodCallE.methodName = methodName;
    e->val.methodCallE.args = args;
    return e;
}

EXP *makeEXPobjectCall(char *className){
    EXP *e = NEW(EXP );
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