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
    uminusOP
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
    ExpKind kind;
    union{
        char *idE;
        int intconstE;
        bool boolconstE;
        struct { BinOp op; struct EXP *left; struct EXP *right; } binOpE;
        struct { UnOp op; struct EXP *expr;} unOpE;
        struct { char *typeName; EXP *size;} arrayNewE;
        struct { EXP *array; EXP *index; } arrayIndexE;
        struct { EXP *array;} arrayLengthE;
        struct { EXP *receiver; char *methodName; EXP_LIST *args; } methodCallE;
        struct { char *className; } objectNewE; 
    } val;
} EXP;

EXP *makeEXPid(char *id);
EXP *makeEXPintconst(int intconst);
EXP *makeEXPboolconst(bool boolconst);
EXP *makeEXPnull(void);
EXP *makeEXPthis(void);
EXP *makeEXPbinop(BinOp op, EXP *left, EXP *right);
EXP *makeEXPunop(UnOp op, EXP *expr);
EXP *makeEXParrayNew(char *typeName, EXP *size);
EXP *makeEXParrayIndex(EXP *array, EXP *index);
EXP *makeEXParrayLength(EXP *array);
EXP *makeEXPmethodCall(EXP *receiver, char *methodName, EXP_LIST *args);
EXP *makeEXPobjectNew(char *className);

EXP_LIST *makeEXPList(EXP *head, EXP_LIST *next);


#endif