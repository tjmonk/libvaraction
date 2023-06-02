/*==============================================================================
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
==============================================================================*/

/*!
 * @defgroup varaction varaction
 * @brief Variable Action Script Support functions
 * @{
 */

/*============================================================================*/
/*!
@file varaction.c

    Variable Action Script Support functions

    The Variable Action Script Support functions provide functions
    for embedding var/action scripts into other applications.


*/
/*============================================================================*/

/*==============================================================================
        Includes
==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>
#include <syslog.h>
#include "varaction.h"
#include "varassign.h"
#include "varbitwise.h"
#include "varboolean.h"
#include "varcompare.h"
#include "varmath.h"
#include "vartypecast.h"
#include "vartimer.h"

/*==============================================================================
       Type defintions
==============================================================================*/

typedef int (*opfn)( VARSERVER_HANDLE hVarServer, \
                     Variable *pVariable, \
                     Variable *pLeft, \
                     Variable *pRight );

/*==============================================================================
       File Scoped Variables
==============================================================================*/

/*! operation map maps operation identifiers to their functions */
static opfn va_op[VA_OP_MAX] = {};

/*! pointer to the declaration list for the current action */
static Variable *g_pDeclarations = NULL;

/*! pointer to all system variables used across all actions */
static Variable *g_pFirstSysvar = NULL;
static Variable *g_pLastSysvar = NULL;

static char *opname[] = {
    "Illegal",
    "Assign",
    "Mul",
    "Div",
    "Add",
    "Sub",
    "Band",
    "Bor",
    "Xor",
    "Inc",
    "Dec",
    "LShift",
    "RShift",
    "And",
    "Or",
    "Not",
    "Equals",
    "NotEquals",
    "Gt",
    "Lt",
    "Gte",
    "Lte",
    "AndEquals",
    "OrEquals",
    "XorEquals",
    "DivEquals",
    "TimesEquals",
    "PlusEquals",
    "MinusEquals",
    "Sysvar",
    "ToFloat",
    "ToInt",
    "ToShort",
    "ToString",
    "Num",
    "FloatNum",
    "LocalVar",
    "String",
    "If",
    "Else",
    "Float",
    "Int",
    "Short",
    "CreateTick",
    "CreateTimer",
    "DeleteTimer",
    "ActiveTimer",
    "Timer"
};

/*==============================================================================
       Function declarations
==============================================================================*/

static int GetVar( VARSERVER_HANDLE hVarServer,
                   Variable *pVariable,
                   Variable *pLeft,
                   Variable *pRight );

static int NOP( VARSERVER_HANDLE hVarServer,
                Variable *pVariable,
                Variable *pLeft,
                Variable *pRight );

static int Unsupported( VARSERVER_HANDLE hVarServer,
                        Variable *pVariable,
                        Variable *pLeft,
                        Variable *pRight );

/*==============================================================================
       Function definitions
==============================================================================*/

void __attribute__ ((constructor)) initLibrary(void)
{
}

void __attribute__ ((destructor)) cleanUpLibrary(void)
{
}

/*============================================================================*/
/*  InitVarAction                                                             */
/*!
    Initialize the Variable Actions

    The InitVarAction function initializes the operation map
    which maps the operation identifiers to their associated functions.

==============================================================================*/
void InitVarAction( void )
{
    int i;

    /* initialize the operation list */
    for ( i = 0; i < VA_OP_MAX ; i++ )
    {
        va_op[i] = Unsupported;
    }

    /* set up specific processing functions */
    va_op[VA_ASSIGN] = Assign;
    va_op[VA_MUL] = Multiply;
    va_op[VA_DIV] = Divide;
    va_op[VA_ADD] = Add;
    va_op[VA_SUB] = Sub;
    va_op[VA_BAND] = Band;
    va_op[VA_BOR] = Bor;
    va_op[VA_XOR] = Xor;
    va_op[VA_INC] = Inc;
    va_op[VA_DEC] = Dec;
    va_op[VA_LSHIFT] = LShift;
    va_op[VA_RSHIFT] = RShift;
    va_op[VA_AND] = And;
    va_op[VA_OR] = Or;
    va_op[VA_NOT] = Not;
    va_op[VA_EQUALS] = Equals;
    va_op[VA_NOTEQUALS] = NotEquals;
    va_op[VA_GT] = GreaterThan;
    va_op[VA_LT] = LessThan;
    va_op[VA_GTE] = GreaterThanOrEqual;
    va_op[VA_LTE] = LessThanOrEqual;
    va_op[VA_AND_EQUALS] = AndEquals;
    va_op[VA_OR_EQUALS] = AndEquals;
    va_op[VA_XOR_EQUALS] = XorEquals;
    va_op[VA_DIV_EQUALS] = DivEquals;
    va_op[VA_TIMES_EQUALS] = TimesEquals;
    va_op[VA_PLUS_EQUALS] = PlusEquals;
    va_op[VA_MINUS_EQUALS] = MinusEquals;
    va_op[VA_SYSVAR] = GetVar;
    va_op[VA_TOFLOAT] = ToFloat;
    va_op[VA_TOSTRING] = ToString;
    va_op[VA_TOSHORT] = ToShort;
    va_op[VA_TOINT] = ToInt;
    va_op[VA_NUM] = NOP;
    va_op[VA_FLOATNUM] = NOP;
    va_op[VA_LOCALVAR] = NOP;
    va_op[VA_STRING] = NOP;
    va_op[VA_CREATE_TICK] = VACreateTick;
    va_op[VA_CREATE_TIMER] = VACreateTimer;
    va_op[VA_DELETE_TIMER] = VADeleteTimer;
    va_op[VA_ACTIVE_TIMER] = VAGetActiveTimer;
    va_op[VA_TIMER] = NOP;
}

/*============================================================================*/
/*  ProcessCompoundStatement                                                  */
/*!
    Process a compound statement (list of statements)

    The ProcessCompoundStatement function performs all of the statements
    contained within a compound statement (list of statements), such as
    those you might find in an "if { } else { }" block

@param[in]
    hVarServer
        handle to the variable server

@param[in]
    pStatements
        pointer to the compound statement

@retval EINVAL invalid argument
@retval EOK the compound statement was successfully processed

==============================================================================*/
int ProcessCompoundStatement( VARSERVER_HANDLE hVarServer,
                              Statement *pStatements )
{
    int result = EINVAL;
    Statement *pStatement;
    int rc;

    if ( ( hVarServer != NULL ) &&
         ( pStatements != NULL ) )
    {
        result = EOK;
        pStatement = pStatements;
        while( pStatement != NULL )
        {
            rc = ProcessStatement( hVarServer, pStatement );
            if ( rc != EOK )
            {
                result = rc;
            }

            pStatement = pStatement->pNext;
        }
    }

    return result;
}

/*============================================================================*/
/*  ProcessStatement                                                          */
/*!
    Process an action statement

    The ProcessStatement function processes a single statement within
    an action.

@param[in]
    hVarServer
        handle to the variable server

@param[in]
    pStatement
        pointer to the statement to be executed

@retval EINVAL invalid argument
@retval EOK the action was successfully processed

==============================================================================*/
int ProcessStatement( VARSERVER_HANDLE hVarServer, Statement *pStatement )
{
    int result = EINVAL;
    int rc;

    if ( ( hVarServer != NULL ) &&
         ( pStatement != NULL ) )
    {
        if( pStatement->pVariable != NULL )
        {
            result = ProcessVariable( hVarServer, pStatement->pVariable );
        }
        else if ( pStatement->script != NULL )
        {
            result = ProcessScript( pStatement->script );
        }
        else
        {
            result = ENOTSUP;
        }
    }

    return result;
}

/*============================================================================*/
/*  ProcessScript                                                             */
/*!
    Process a script

    The ProcessScript function executes a script statement using the
    system() function.

@param[in]
    script
        pointer to the script to be executed

@retval EINVAL invalid argument
@retval EOK the script was successfully processed

==============================================================================*/
int ProcessScript( char *script )
{
    int result = EINVAL;

    if ( script != NULL )
    {
        system( script );
        result = EOK;
    }

    return result;
}

/*============================================================================*/
/*  ProcessVariable                                                           */
/*!
    Process a variable tree

    The ProcessVariable function processes a variable tree to ultimately
    perform a SET on a local or system variable.

@param[in]
    hVarServer
        handle to the variable server

@param[in]
    pVariable
        pointer to the variable node to process

@retval EINVAL invalid argument
@retval EOK the variable expression was successfully processed

==============================================================================*/
int ProcessVariable( VARSERVER_HANDLE hVarServer, Variable *pVariable )
{
    int result = EINVAL;
    Variable *left;
    Variable *right;

    if ( ( hVarServer != NULL ) &&
         ( pVariable != NULL ) )
    {
        left = pVariable->left;
        right = pVariable->right;

        if ( pVariable->operation == VA_IF )
        {
            result = ProcessIF( hVarServer, left, right );
        }
        else
        {
            result = ProcessExpr( hVarServer, pVariable );
        }

        if( result != EOK )
        {
            fprintf( stderr,
                     "Error processing Action: %s (%d) %s\n",
                     opname[pVariable->operation],
                     result,
                     strerror( result ) );
        }
    }

    return result;
}

/*============================================================================*/
/*  ProcessIF                                                                 */
/*!
    Process an IF statement

    The ProcessIF function processes an IF statement variable tree to ultimately
    perform a SET on a local or system variable.

@param[in]
    hVarServer
        handle to the variable server

@param[in]
    left
        pointer to the variable expression tree containing the IF expression

@param[in]
    right
        pointer to the variable expression tree containing the THEN and ELSE
        expressions

@retval EINVAL invalid argument
@retval EOK the IF statement was successfully executed

==============================================================================*/
int ProcessIF( VARSERVER_HANDLE hVarServer, Variable *left, Variable *right )
{
    int rc;
    Variable *pVariable;
    bool check;
    Statement *pCompoundStatement;

    if ( ( left != NULL ) &&
         ( right != NULL ) &&
         ( right->operation == VA_ELSE ) )
    {
        /* evaluate the truth of the IF statement */
        rc = ProcessVariable( hVarServer, left );
        if ( rc == EOK )
        {
            /* evaluate the condition result */
            check = left->obj.val.ui == 0 ? false : true;

            /* evaluate the expression in either the left or right
             * subtree of the ELSE node (if the IF condition evaluates
             * to true) or the right subtree of the ELSE node (if the
             * IF condition evaluates to false) */
            pVariable = right;

            if( check )
            {
                /* IF then block */
                pCompoundStatement = (Statement *)pVariable->left;
                rc = ProcessCompoundStatement( hVarServer, pCompoundStatement );
            }
            else
            {
                /* IF else block */
                if ( pVariable->right != NULL )
                {
                    pCompoundStatement = (Statement *)pVariable->right;
                    rc = ProcessCompoundStatement( hVarServer,
                                                   pCompoundStatement );
                }
                else
                {
                    /* no ELSE block */
                    rc = EOK;
                }
            }
        }
    }

    return rc;
}

/*============================================================================*/
/*  ProcessExpression                                                         */
/*!
    Process a variable expression tree

    The ProcessExpression function processes a variable expression tree

@param[in]
    hVarServer
        handle to the variable server

@param[in]
    pVariable
        pointer to the variable node to process

@retval EINVAL invalid argument
@retval EOK the expression was successfully evaluated

==============================================================================*/
int ProcessExpr( VARSERVER_HANDLE hVarServer, Variable *pVariable )
{
    int result = EINVAL;
    Variable *left;
    Variable *right;
    int lrc;
    int rrc;
    int op;
    int (*fn)( VARSERVER_HANDLE hVarServer, Variable *pVariable,
               Variable *pLeft, Variable *pRight ) = NULL;

    if ( ( hVarServer != NULL ) &&
         ( pVariable != NULL ) )
    {
        left = pVariable->left;
        right = pVariable->right;

        /* recursively calculate the left side of the expression tree */
        lrc = ProcessVariable( hVarServer, right );

        /* recursively calculate the right side of the expression tree */
        rrc = ProcessVariable( hVarServer, left );

        op = pVariable->operation;
        if ( op < VA_OP_MAX )
        {
            fn = va_op[op];
            if ( fn != NULL )
            {
                result = fn( hVarServer, pVariable, left, right );
            }
            else
            {
                fprintf( stderr,
                         "Illegal function %s\n",
                         opname[pVariable->operation] );
            }
        }
        else
        {
            fprintf( stderr,
                     "Illegal operation %d\n",
                     pVariable->operation );

            result = ENOTSUP;
        }
    }

    return result;
}

/*============================================================================*/
/*  GetVar                                                                    */
/*!
    Get a system variable

    The GetVar function gets the system variable if the system variable
    is NOT an l-value.

@param[in]
    hVarServer
        handle to the variable server

@param[in]
    pVariable
        pointer to the variable to get

@param[in]
    pLeft
        pointer to the left subtree (unused)

@param[in]
    pRight
        pointer to the right subtree (unused)

@retval EINVAL invalid argument
@retval EOK the variable was successfully retrieved or did not need to be

==============================================================================*/
static int GetVar( VARSERVER_HANDLE hVarServer,
                   Variable *pVariable,
                   Variable *pLeft,
                   Variable *pRight )
{
    int result = EINVAL;

    if ( ( hVarServer != NULL ) &&
         ( pVariable != NULL ) &&
         ( pVariable->hVar != VAR_INVALID ) &&
         ( pVariable->operation == VA_SYSVAR ) )
    {
        if ( pVariable->lvalue == false )
        {
            result = VAR_Get( hVarServer,
                              pVariable->hVar,
                              &(pVariable->obj) );
        }
        else
        {
            /* no need to get an l-value sysvar that we are about to write */
            result = EOK;
        }
    }

    return result;
}

/*============================================================================*/
/*  Unsupported                                                               */
/*!
    Unsupported or unimplemented operation

    The Unsupported function generates an "Unsupported" error.  It is
    used as a place holder for unsupported or not-yet-implemented
    functionality.

@param[in]
    hVarServer
        handle to the variable server (unused)

@param[in]
    pVariable
        pointer to the variable node (unused)

@param[in]
    pLeft
        pointer to the left subtree (unused)

@param[in]
    pRight
        pointer to the right subtree (unused)

@retval EINVAL invalid argument
@retval ENOTSUP unsupported operation

==============================================================================*/
static int Unsupported( VARSERVER_HANDLE hVarServer,
                        Variable *pVariable,
                        Variable *pLeft,
                        Variable *pRight )
{
    int result = EINVAL;

    if ( pVariable != NULL )
    {
        fprintf( stderr,
                 "Unsupported operation %d\n",
                 pVariable->operation );

        result = ENOTSUP;
    }

    return result;
}


/*============================================================================*/
/*  NOP                                                                       */
/*!
    No OPeration

    The NOP function does nothing and is used for taking no action
    when encountering constants or other non-operations in the parse tree.

@param[in]
    hVarServer
        handle to the variable server (unused)

@param[in]
    pVariable
        pointer to the variable node (unused)

@param[in]
    pLeft
        pointer to the left subtree (unused)

@param[in]
    pRight
        pointer to the right subtree (unused)

@retval EOK no action required

==============================================================================*/
static int NOP( VARSERVER_HANDLE hVarServer,
                Variable *pVariable,
                Variable *pLeft,
                Variable *pRight )
{
    return EOK;
}

/*============================================================================*/
/*  CreateDeclaration                                                         */
/*!
    Create a new Variable declaration

    The CreateDeclaration function creates a new variable declaration
    which consists of a type and variable.

@param[in]
    type_specifier
        the type of the new variable (cast internally to an int)

@param[in]
    variable
        pointer to the variable to set as a declaration

@retval pointer to the updated variable declaration
@retval NULL if an error occurred

==============================================================================*/
Variable *CreateDeclaration( uintptr_t type_specifier, Variable *pVariable )
{
    Variable *result = NULL;

    if ( pVariable != NULL )
    {
        switch( type_specifier )
        {
            case VA_INT:
                pVariable->obj.len = sizeof(uint32_t);
                pVariable->obj.type = VARTYPE_UINT32;
                result = pVariable;
                break;

            case VA_SHORT:
                pVariable->obj.len = sizeof(uint16_t);
                pVariable->obj.type = VARTYPE_UINT16;
                result = pVariable;
                break;

            case VA_FLOAT:
                pVariable->obj.len = sizeof(float);
                pVariable->obj.type = VARTYPE_FLOAT;
                result = pVariable;
                break;

            case VA_STRING:
                pVariable->obj.type = VARTYPE_STR;
                result = pVariable;
                break;

            default:
                break;
        }
    }

    return result;
}

/*============================================================================*/
/*  CreateVariable                                                            */
/*!
    Create a variable operation node

    The CreateVariable function creates a varaiable operation node

@param[in]
    op
        operation to perform

@param[in]
    left
        pointer to the node to the left of this one in the evaluation tree

@param[in]
    right
        pointer to the node to the right of this one in the evaluation tree

@retval pointer to the variable node
@retval NULL if an error occurred

==============================================================================*/
Variable *CreateVariable( uintptr_t op, void *left, void *right )
{
    Variable *var = (Variable *)calloc( 1, sizeof( Variable ) );
    if ( var != NULL )
    {
        var->left = left;
        var->right = right;
        var->operation = op;

        switch( op )
        {
            case VA_FLOAT:
                var->obj.type = VARTYPE_FLOAT;
                break;

            case VA_STRING:
            case VA_TOSTRING:
                var->obj.type = VARTYPE_STR;
                break;

            case VA_IF:
                var->obj.type = VARTYPE_UINT16;
                break;

            case VA_ELSE:
                var->obj.type = VARTYPE_UINT16;
                break;

            default:
                var->obj.type = TypeCheck( left, right );
                break;
        }
    }

    return (void *)var;
}


/*============================================================================*/
/*  NewNumber                                                                 */
/*!
    Create a new constant number

    The NewNumber function creates a new constant integer which is
    stored in a Variable object

@param[in]
    number
        pointer to the text of a number

@retval pointer to the Variable that we created
@retval NULL if an error occurred

==============================================================================*/
Variable *NewNumber( void *number )
{
    Variable *var = NULL;
    int n;
    int len;
    int requestedType = -1;
    char *num = (char *)number;
    int type;
    int base = 10;

    if ( num != NULL )
    {
        if( num[0] == '0' && num[1] == 'x' )
        {
            base = 16;
        }

        /* determine requested numeric type */
        len = strlen( num );
        if ( toupper(num[len-1]) == 'U' )
        {
            requestedType = VARTYPE_UINT16;
        }

        if ( toupper(num[len-1]) == 'L' )
        {
            requestedType = VARTYPE_UINT32;
        }

        /* get the number */
        n = strtol( num, NULL, base );

        /* allocate memory for the number */
        var = (Variable *)calloc( 1, sizeof( Variable ) );
        if ( var != NULL )
        {
            var->operation = VA_NUM;

            /* perform range checking */
            if( n < -32768 || n > 65535 )
            {
                if( requestedType == VARTYPE_UINT16 )
                {
                    var->obj.type = VARTYPE_UINT32;
                }
                else
                {
                    var->obj.type = requestedType == -1 ? VARTYPE_UINT32
                                                       : requestedType;
                }
            }
            else
            {
                var->obj.type = requestedType == -1 ? VARTYPE_UINT16
                                                   : requestedType;
            }

            if ( var->obj.type == VARTYPE_UINT16 )
            {
                var->obj.val.ui = n;
                var->obj.len = sizeof( uint16_t );
            }
            else if ( var->obj.type == VARTYPE_UINT32 )
            {
                var->obj.val.ul = n;
                var->obj.len = sizeof( uint32_t );
            }
        }
    }

    return var;
}

/*============================================================================*/
/*  NewString                                                                 */
/*!
    Create a new constant string

    The NewSting function creates a new constant string which is
    stored in a Variable object

@param[in]
    str
        pointer to the character string

@retval pointer to the Variable that we created
@retval NULL if an error occurred

==============================================================================*/
Variable *NewString( void *str )
{
    Variable *var = NULL;

    if ( str != NULL )
    {
        var = (Variable *)calloc( 1, sizeof( Variable ) );
        if ( var != NULL )
        {
            var->operation = VA_STRING;
            var->obj.val.str = strdup(str);
            var->obj.len = strlen(str);
            var->obj.type = VARTYPE_STR;
            var->bufsize = var->obj.len;
        }
    }

    return var;
}

/*============================================================================*/
/*  NewFloat                                                                  */
/*!
    Create a new floating point number

    The NewFloat function creates a new floating point number which is
    stored in a Variable object

@param[in]
    fstr
        pointer to the floating point number string

@retval pointer to the Variable that we created
@retval NULL if an error occurred

==============================================================================*/
Variable *NewFloat( void *fstr )
{
    Variable *var = NULL;
    float f;
    if ( fstr != NULL )
    {
        f = atof( fstr );

        var = (Variable *)calloc( 1, sizeof( Variable ) );
        if ( var != NULL )
        {
            var->operation = VA_FLOATNUM;
            var->obj.val.f = f;
            var->obj.len = sizeof( float );
            var->obj.type = VARTYPE_FLOAT;
        }
    }

    return var;
}

/*============================================================================*/
/*  NewIdentifier                                                             */
/*!
    Create a new variable identifier

    The NewIdentifier function creates a new variable which is
    stored in a Variable object and looked up in the variable server
    using the variable identifier

@param[in]
    hVarServer
        handle to the variable server

@param[in]
    id
        pointer to the variable identifier

@param[in]
    declaration
        indicates if the identifier is in a declaration

@retval pointer to the Variable that we created
@retval NULL if an error occurred

==============================================================================*/
Variable *NewIdentifier( VARSERVER_HANDLE hVarServer,
                         void *id,
                         bool declaration )
{
    Variable *var = NULL;
    VAR_HANDLE hVar;
    int result;

    if ( id != NULL )
    {
        if( declaration == false )
        {
            var = FindLocalVariable( id );
            if ( var == NULL )
            {
                var = FindVariable( id );
            }
        }

        if ( var == NULL )
        {
            /* allocate memory for the new variable */
            var = (Variable *)calloc( 1, sizeof( Variable ) );
            if ( var != NULL )
            {
                var->id = strdup( id );
                var->local = declaration;
                var->assigned = false;

                if ( var->local == false )
                {
                    hVar = VAR_FindByName( hVarServer, var->id );
                    if ( hVar != VAR_INVALID )
                    {
                        var->hVar = hVar;
                        var->operation = VA_SYSVAR;

                        result = VAR_Get( hVarServer,
                                          var->hVar,
                                          &(var->obj) );
                        if( result == EOK )
                        {
                            if ( g_pFirstSysvar == NULL )
                            {
                                /* add variable at the beginning of the list */
                                g_pFirstSysvar = var;
                                g_pLastSysvar = var;
                            }
                            else
                            {
                                /* add variable at the end of the list */
                                g_pLastSysvar->pNext = var;
                                g_pLastSysvar = var;
                            }
                        }
                        else
                        {
                            free( var );
                            var = NULL;
                        }
                    }
                    else
                    {
                        free( var );
                        var = NULL;
                    }
                }
                else
                {
                    var->operation = VA_LOCALVAR;
                }
            }
        }
    }

    return var;
}

/*============================================================================*/
/*  CheckUseBeforeAssign                                                      */
/*!
    Check if a local variable is used before it is assigned

    The CheckUseBeforeAssign function checks to see if the specified
    variable is being referenced before it has been assigned a value

@param[in]
    pVariable
        pointer to the variable to check

==============================================================================*/
bool CheckUseBeforeAssign( Variable *pVariable )
{
    int result = false;

    if( pVariable != NULL )
    {
        if ( ( pVariable->local == true ) &&
             ( pVariable->assigned == false ) )
        {
            result = true;
        }
    }

    return result;
}

/*============================================================================*/
/*  TypeCheck                                                                 */
/*!
    Perform type checking on arithmetic operation

    The TypeCheck function checks that the types of the variables is the
    same.

@param[in]
    pVar1
        pointer to the first variable to check

@param[in]
    pVar2
        pointer to the second variable to check

@retval the type of both of the arguments
@retval 0 if the types do not match

==============================================================================*/
int TypeCheck( Variable *pVar1, Variable *pVar2 )
{
    int result = 0;

    if( ( pVar1 != NULL ) &&
        ( pVar2 != NULL ) )
    {
        if ( pVar1->obj.type == pVar2->obj.type )
        {
            result = pVar1->obj.type;
        }
        else
        {
            result = -1;
        }
    }
    else if ( pVar1 != NULL )
    {
        result = pVar1->obj.type;
    }
    else if ( pVar2 != NULL )
    {
        result = pVar2->obj.type;
    }
    else
    {
        result = -1;
    }

    return result;
}


/*============================================================================*/
/*  FindLocalVariable                                                         */
/*!
    Search for a local variable in the variable declarations list

    The FindLocalVariable function searches through the variable declaration
    list looking for a variable with the specified name.  If the variable
    is not found, it is assumed to be a system variable.

@param[in]
    id
        pointer to the variable identifier to search for

@retval pointer to the Variable that we found
@retval NULL if no variable was found

==============================================================================*/
Variable *FindLocalVariable( char *id )
{
    Variable *pVariable = NULL;

    pVariable = g_pDeclarations;
    while( pVariable != NULL )
    {
        if( strcmp( pVariable->id, id ) == 0 )
        {
            break;
        }

        pVariable = pVariable->pNext;

    }

    return pVariable;
}

/*============================================================================*/
/*  FindVariable                                                              */
/*!
    Search for a system variable in the global system variable list

    The FindVariable function searches through the global system variable
    list looking for a variable with the specified name.

@param[in]
    id
        pointer to the variable identifier to search for

@retval pointer to the Variable that we found
@retval NULL if no variable was found

==============================================================================*/
Variable *FindVariable( char *id )
{
    Variable *pVariable = NULL;

    pVariable = g_pFirstSysvar;
    while( pVariable != NULL )
    {
        if ( strcmp( pVariable->id, id ) == 0 )
        {
            break;
        }

        pVariable = pVariable->pNext;
    }

    return pVariable;
}

/*============================================================================*/
/*  SetDeclarations                                                           */
/*!
    Set the variable declarations list

    The SetDeclarations function sets the variable declaration list to
    the point to the specified variable.  The declaration list is used in
    the FindVariable functions when searching for local variables.

@param[in]
    pVariable
        pointer to the variable declaration list

==============================================================================*/
void SetDeclarations( Variable *pVariable )
{
    g_pDeclarations = pVariable;
}

/*============================================================================*/
/*  SetTimer                                                                  */
/*!
    Set the active timer

    The SetTimer function sets the active timer so that the timer <id>
    statements can be evaluated during transition evaluation.
    Setting the timer id to zero indicates that there is no active timer.

@param[in]
    id
        id of the active timer, zero for none

==============================================================================*/
void SetTimer( int id )
{
    VASetActiveTimer( id );
}


/*! @}
 * end of varaction group */
