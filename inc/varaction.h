/*============================================================================
MIT License

Copyright (c) 2023 Trevor Monk

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
============================================================================*/

#ifndef VARACTION_H
#define VARACTION_H

/*============================================================================
        Includes
============================================================================*/

#include <varserver/varserver.h>

/*============================================================================
        Public Definitions
============================================================================*/

/*! variable operations */
typedef enum {
    VA_ILLEGAL = 0,
    VA_ASSIGN,
    VA_MUL,
    VA_DIV,
    VA_ADD,
    VA_SUB,
    VA_BAND,
    VA_BOR,
    VA_XOR,
    VA_INC,
    VA_DEC,
    VA_LSHIFT,
    VA_RSHIFT,
    VA_AND,
    VA_OR,
    VA_NOT,
    VA_EQUALS,
    VA_NOTEQUALS,
    VA_GT,
    VA_LT,
    VA_GTE,
    VA_LTE,
    VA_AND_EQUALS,
    VA_OR_EQUALS,
    VA_XOR_EQUALS,
    VA_DIV_EQUALS,
    VA_TIMES_EQUALS,
    VA_PLUS_EQUALS,
    VA_MINUS_EQUALS,
    VA_SYSVAR,
    VA_TOFLOAT,
    VA_TOINT,
    VA_TOSHORT,
    VA_TOSTRING,
    VA_NUM,
    VA_FLOATNUM,
    VA_LOCALVAR,
    VA_STRING,
    VA_IF,
    VA_ELSE,
    VA_FLOAT,
    VA_INT,
    VA_SHORT,
    VA_CREATE_TICK,
    VA_CREATE_TIMER,
    VA_DELETE_TIMER,
    VA_ACTIVE_TIMER,
    VA_TIMER,
    VA_OP_MAX
} VarOperation;

/*! the Variable object is used to track values of external
 * variables and partial values within a calculation */
typedef struct _variable
{
    /*! variable operation */
    int operation;

    /*! line number */
    int lineno;

    /*! pointer to the variable name (may be NULL) */
    char *id;

    /*! indicate if the variable is local */
    bool local;

    /*! indicate if the variable has been assigned a value,
     *  used to check for used-before-assigned errors */
    bool assigned;

    /*! indicates if the variable is an L-Value */
    bool lvalue;

    /*! true if we have requested a calc notification for this variable */
    bool calcNotification;

    /*! true if we have requested a modified notification for this variable */
    bool modifiedNotification;

    /*! buffer size (for string variables) */
    size_t bufsize;

    /*! handle to an external variable (may be NULL) */
    VAR_HANDLE hVar;

    /*! variable object containing the variable type and value */
    VarObject obj;

    /*! pointer to the left hand side of the variable tree */
    struct _variable *left;

    /*! pointer to the right hand side of the variable tree */
    struct _variable *right;

    /*! pointer to the next variable in a declaration list */
    struct _variable *pNext;

} Variable;

/*! statement */
typedef struct _statement
{
    /*! pointer to the variable tree to evaluate */
    Variable *pVariable;

    /*! pointer to the script to execute */
    char *script;

    /*! line number */
    int lineno;

    /*! pointer to the next statement to execute */
    struct _statement *pNext;
} Statement;

/*============================================================================
        Public Function Declarations
============================================================================*/

void InitVarAction( void );

int ProcessCompoundStatement( VARSERVER_HANDLE hVarServer,
                              Statement *pStatements );

int ProcessStatement( VARSERVER_HANDLE hVarServer, Statement *pStatement );

int ProcessScript( char *script );

int ProcessVariable( VARSERVER_HANDLE hVarServer, Variable *pVariable );

int ProcessIF( VARSERVER_HANDLE hVarServer, Variable *left, Variable *right );

int ProcessExpr( VARSERVER_HANDLE hVarServer, Variable *pVariable );


Variable *CreateDeclaration( int type_specifier, Variable *pVariable );
Variable *CreateVariable( int op, void *left, void *right );
Variable *NewNumber( void *number );
Variable *NewString( void *str );
Variable *NewFloat( void *fstr );
Variable *NewIdentifier( VARSERVER_HANDLE hVarServer,
                         void *id,
                         bool declaration );
bool CheckUseBeforeAssign( Variable *pVariable );
int TypeCheck( Variable *pVar1, Variable *pVar2 );
Variable *FindLocalVariable( char *id );
Variable *FindVariable( char *id );
void SetDeclarations( Variable *pVariable );

void SetTimer( int id );

#endif
