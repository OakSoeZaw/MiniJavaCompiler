#ifndef AST_H
#define AST_H

#include <stdbool.h>

typedef struct EXP EXP;
typedef struct EXP_LIST EXP_LIST;

struct EXP_LIST{
    EXP *head;
    EXP_LIST *next;
};

typedef enum{
    notOp,
}UnOp;

typedef enum{
    timesOp,
    plusOp,
    divOp,
    minusOp,
    modOp,
    andOp,
    orOp,
    lessOp,
    leOp,
    gtOp,
    geOp,
    eqOp,
    neqOp
} BinOp;

typedef enum {
    idK, intconstK, binOpK,
    nullK,thisK,unOpK,arrayNewK,
    arrayIndexK, arrayLengthK,methodCallK,
    objectNewK, boolconstK
} ExpKind;

typedef struct EXP{
    int line;
    ExpKind kind;
    union{
        char *idE;
        int intconstE;
        bool boolconstE;
        struct { BinOp op; struct EXP *left; struct EXP *right; } binOpE;
        struct { UnOp op; struct EXP *expr;} unOpE;
        struct { EXP *size;} arrayNewE;
        struct { EXP *array; EXP *index; } arrayIndexE;
        struct { EXP *array;} arrayLengthE;
        struct { EXP *receiver; char *methodName; EXP_LIST *args; } methodCallE;
        struct { char *className; } objectNewE; 
    } val;
} EXP;

EXP *makeEXPid(int line, char *id);
EXP *makeEXPintconst(int line, int intconst);
EXP *makeEXPboolconst(int line, bool boolconst);
EXP *makeEXPnull(int line);
EXP *makeEXPthis(int line);
EXP *makeEXPbinop(int line,BinOp op, EXP *left, EXP *right);
EXP *makeEXPunop(int line, UnOp op, EXP *expr);
EXP *makeEXParrayNew(int line, EXP *size);
EXP *makeEXParrayIndex(int line, EXP *array, EXP *index);
EXP *makeEXParrayLength(int line, EXP *array);
EXP *makeEXPmethodCall(int line, EXP *receiver, char *methodName, EXP_LIST *args);
EXP *makeEXPobjectNew(int line, char *className);

EXP_LIST *makeEXPList(EXP *head, EXP_LIST *next);


#endif