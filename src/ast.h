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

struct EXP{
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
};

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


typedef struct STMT STMT;
typedef struct STMT_LIST STMT_LIST;

struct STMT_LIST { 
    STMT *head; 
    STMT_LIST *next; 
};

typedef enum { 
    blockK, ifK, whileK, printK, assignK, arrayAssignK
}StmtKind;

struct STMT{
    int line;
    StmtKind kind;
    union{
        STMT_LIST *blockS;
        struct{ EXP *cond; STMT *thenBranch; STMT *elseBranch;} ifS;
        struct{ EXP *cond; STMT *body;} whileS;
        EXP *printS;
        struct { char *name; EXP *value;} assignS;
        struct{ char *name; EXP *index; EXP *value;} arrayAssignS;
    } val;
};

STMT *makeSTMTblock(int line, STMT_LIST *stmts);
STMT *makeSTMTif(int line, EXP *cond, STMT * thenBranch, STMT *elseBranch);
STMT *makeSTMTwhile(int line, EXP *cond, STMT *body);
STMT *makeSTMTprint(int line, EXP *e);
STMT *makeSTMTassign(int line, char *name, EXP *value);
STMT *makeSTMTarrayAssign(int line, char *name, EXP *index, EXP *value);
STMT_LIST *makeSTMTList(STMT *head, STMT_LIST *next);

typedef enum {intT, boolT, intArrayT, classT,  nullT, errorT} TypeKind;

typedef struct TYPE {
    int line;
    TypeKind kind;
    char *className;
}TYPE;  

typedef struct VAR_DECL VAR_DECL;
struct VAR_DECL { int line; TYPE *type; char *name; VAR_DECL *next; };

typedef struct BODY{
    VAR_DECL *locals;
    STMT_LIST *stmts;
    EXP *retExp;
} BODY;

typedef struct METHOD METHOD;
struct METHOD {
    int line;
    TYPE *retType;
    char *name;
    VAR_DECL *params;
    VAR_DECL *locals;
    STMT_LIST *stmts;
    EXP *retEXP;
    METHOD *next;
};

typedef struct CLASS_DECL CLASS_DECL;
struct CLASS_DECL {
    int line;
    char *name;
    char *parent;
    VAR_DECL *fields;
    METHOD *methods;
    CLASS_DECL *next;
};

typedef struct MAIN_CLASS{
    int line;
    char *name;
    char *argName;
    STMT *body;
} MAIN_CLASS;

typedef struct PROGRAM{
    MAIN_CLASS *mainClass;
    CLASS_DECL *classes;
} PROGRAM;


TYPE *makeTYPE(int line, TypeKind kind, char *className );
VAR_DECL *makeVARDECL(int line, TYPE *type, char *name, VAR_DECL *next);
BODY *makeBODY(VAR_DECL *locals, STMT_LIST * stmts, EXP *retExp);
METHOD *makeMETHOD(int line, TYPE *retType, char *name, VAR_DECL *params,
    BODY *body, METHOD *next);
CLASS_DECL *makeCLASS(int line, char *name, char *parent, VAR_DECL *fields, METHOD *methods, CLASS_DECL *next);
MAIN_CLASS *makeMAINCLASS(int line, char *name, char *argName, STMT *body);
PROGRAM *makePROGRAM(MAIN_CLASS *m, CLASS_DECL *classes);

#endif