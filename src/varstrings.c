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
 * @defgroup varstrings varstrings
 * @brief Variable Action Script String Support functions
 * @{
 */

/*============================================================================*/
/*!
@file varstrings.c

    Variable Action Script String Support functions

    The Variable Action Script String Support functions provide functions
    for handling string variables in var/action scripts.

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
#include <syslog.h>
#include "varstrings.h"

/*==============================================================================
       Function declarations
==============================================================================*/

/*============================================================================*/
/*  AllocateString                                                            */
/*!
    Allocate string memory for a string of specified length

    The AllocateString function allocates memory for a string of the
    specified length in the specified variable node.

    If the specified string length is less than 32, then a 32-byte buffer
    will be allocated, otherwise a buffer of sufficient size to hold the
    string's specified length will be allocated.

@param[in]
    pVariable
        pointer to the Variable node which will have a string buffer allocated

@param[in]
    len
        the length of the string we need to accomodate

@retval EINVAL invalid argument
@retval ENOMEM memory allocation failure
@retval ENOTSUP unsupported type
@retval EOK the string buffer memory was successfully allocated

==============================================================================*/
int AllocateString( Variable *pVariable, size_t len )
{
    int result = EINVAL;
    size_t bufsize;

    if ( pVariable != NULL )
    {
        if( pVariable->obj.type == VARTYPE_STR )
        {
            /* calculate the string buffer size
             * which is a minimum of 32 bytes */
            bufsize = ( len < 32 ) ? 32 : len + 1;

            /* check if we need to reallocate an existing buffer */
            if ( ( pVariable->obj.val.str != NULL ) &&
                 ( pVariable->bufsize <= len ) )
            {
                /* re-allocate the previous buffer */
                pVariable->obj.val.str = realloc( pVariable->obj.val.str,
                                                  bufsize );
                if( pVariable->obj.val.str != NULL )
                {
                    /* success - set the buffer size */
                    pVariable->bufsize = bufsize;
                    result = EOK;
                }
                else
                {
                    /* cannot allocate memory for the string */
                    pVariable->bufsize = 0;
                    result = ENOMEM;
                }
            }

            /* check if we need to allocate a new buffer */
            if ( pVariable->obj.val.str == NULL )
            {
                /* allocate space for the new string */
                pVariable->obj.val.str = malloc( bufsize );
                if( pVariable->obj.val.str != NULL )
                {
                    /* success - set the buffer size */
                    pVariable->bufsize = bufsize;
                    result = EOK;
                }
                else
                {
                    /* cannot allocate memory for the string */
                    pVariable->bufsize = 0;
                    result = ENOMEM;
                }
            }
            else
            {
                result = EOK;
            }
        }
        else
        {
            /* cannot allocate memory for non-string variable */
            result = ENOTSUP;
        }
    }

    return result;
}

/*============================================================================*/
/*  AssignString                                                              */
/*!
    Perform a string variable assignment

    The Assign function performs the following operation for string types only:

    result <= left <= right

    The result string points to the left variable

@param[in]
    pResult
        pointer to the result node

@param[in]
    pLeft
        pointer to the left node

@param[in]
    pRight
        pointer to the right node

@retval EINVAL invalid argument
@retval ENOMEM memory allocation failure
@retval ENOTSUP unsupported type
@retval EOK the assignment was successfully performed

==============================================================================*/
int AssignString( Variable *pResult,
                  Variable *pLeft,
                  Variable *pRight )
{
    int result = EINVAL;
    size_t len;
    size_t bufsize;

    if ( ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        if ( ( pLeft->obj.type == VARTYPE_STR ) &&
             ( pRight->obj.type == VARTYPE_STR ) )
        {
            /* get the length of the source string */
            len = pRight->obj.len;

            /* (re)allocate memory in the left variable for the
             * size of string in the right variable */
            result = AllocateString( pLeft, len );
            if ( result == EOK )
            {
                /* copy the string */
                memcpy( pLeft->obj.val.str, pRight->obj.val.str, len );

                /* NUL terminate */
                pLeft->obj.val.str[len] = 0;

                /* set string attributes */
                pLeft->obj.len = len;
                pLeft->obj.type = VARTYPE_STR;

                /* update information in the result node */
                pResult->obj.val.str = pLeft->obj.val.str;
                pResult->obj.len = len;
                pResult->obj.type = VARTYPE_STR;
                pResult->bufsize = pLeft->bufsize;
            }
        }
        else
        {
            /* invalid argument types */
            result = ENOTSUP;
        }
    }

    return result;
}

/*============================================================================*/
/*  AddString                                                                 */
/*!
    Add one string to another

    The AddString performs the following operation:

    result = left + right

    where both operands are string variables

@param[in]
    pResult
        pointer to the result node

@param[in]
    pLeft
        pointer to the left node

@param[in]
    pRight
        pointer to the right node

@retval EINVAL invalid argument
@retval EOK the script was successfully processed

==============================================================================*/
int AddString( Variable *pResult,
               Variable *pLeft,
               Variable *pRight )
{
    int result = EINVAL;
    size_t len;
    size_t len1;
    size_t len2;

    if ( ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        if ( ( pLeft->obj.type == VARTYPE_STR ) &&
             ( pRight->obj.type == VARTYPE_STR ) )
        {
            if ( ( pLeft->obj.val.str != NULL ) &&
                 ( pRight->obj.val.str != NULL ) )
            {
                len1 = pLeft->obj.len;
                len2 = pRight->obj.len;
                len = len1 + len2;

                /* set the result type */
                pResult->obj.type = VARTYPE_STR;

                /* (re)allocate memory in the result variable
                 * to accomodate the concatenation of the left
                 * and right variables. */
                result = AllocateString( pResult, len );
                if ( result == EOK )
                {
                    /* concatenate the two strings */
                    strcpy( pResult->obj.val.str, pLeft->obj.val.str );
                    strcpy( &(pResult->obj.val.str[len1]),
                            pRight->obj.val.str );

                    pResult->obj.len = len;
                }
            }
            else
            {
                result = ENOTSUP;
            }
        }
        else
        {
            result = ENOTSUP;
        }
    }

    return result;
}

/*============================================================================*/
/*  ConcatString                                                              */
/*!
    Concatenate two strings

    The ConcatString performs the following operation:

    left += right
    result = left;

    where both operands are string variables

@param[in]
    pResult
        pointer to the result node

@param[in]
    pLeft
        pointer to the left node

@param[in]
    pRight
        pointer to the right node

@retval EINVAL invalid argument
@retval EOK the script was successfully processed

==============================================================================*/
int ConcatString( Variable *pResult,
                  Variable *pLeft,
                  Variable *pRight )
{
    int result = EINVAL;
    size_t len;
    size_t len1;
    size_t len2;

    if ( ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        if ( ( pLeft->obj.type == VARTYPE_STR ) &&
             ( pRight->obj.type == VARTYPE_STR ) )
        {
            if ( ( pLeft->obj.val.str != NULL ) &&
                 ( pRight->obj.val.str != NULL ) )
            {
                len1 = pLeft->obj.len;
                len2 = pRight->obj.len;
                len = len1 + len2;

                /* (re)allocate memory in the left variable
                 * to accomodate the concatenation of the left
                 * and right variables. */
                result = AllocateString( pLeft, len );
                if ( result == EOK )
                {
                    /* concatenate the two strings */
                    strcat( pLeft->obj.val.str, pRight->obj.val.str );
                    pLeft->obj.len = len;
                    pResult->obj.len = len;
                    pResult->obj.type = VARTYPE_STR;
                    pResult->obj.val.str = pLeft->obj.val.str;
                    pResult->bufsize = pLeft->bufsize;
                }
            }
            else
            {
                result = ENOTSUP;
            }
        }
        else
        {
            result = ENOTSUP;
        }
    }

    return result;
}

/*! @}
 * end of varstrings group */

