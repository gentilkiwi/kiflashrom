/*!
 * \file ftdi_infra.h
 *
 * \author FTDI
 * \date 20110127
 *
 * Copyright © 2000-2014 Future Technology Devices International Limited
 *
 *
 * THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Project: libMPSSE
 * Module: Infra
 *
 * This file abstracts everything that is specific to CPU, OS and Platform to ensure portability
 * These abstractions are datatypes, function calls, etc
 *
 * Revision History:
 * 0.1 - initial version
 * 0.2 - 20110708 - added memory related macros
 * 0.3 - 20111103 - added 64bit linux support, cleaned up
 *
 */

#ifndef FTDI_INFRA_H
#define FTDI_INFRA_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*								Macro defines								  */
/******************************************************************************/

#include <windows.h>

#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ftd2xx.h"


/******************************************************************************/
/*								Macro defines								  */
/******************************************************************************/
#define FTDIMPSSE_API

/* Uncomment the #define INFRA_DEBUG_ENABLE in makefile to enable debug messages */
#define MSG_EMERG	0 /*Used for emergency messages, usually those that precede a crash*/
#define MSG_ALERT	1 /*A situation requiring immediate action */
#define MSG_CRIT	2 /*Critical conditions, often related to serious hw/sw failures */
#define MSG_ERR		3 /*Used to report error conditions */
#define MSG_WARN	4 /*Warnings about problematic situations that do not, in themselves,
						create serious problems with the system */
#define MSG_NOTICE	5 /*Situations that are normal, but still worthy of note. */
#define MSG_INFO	6 /*Informational messages */
#define MSG_DEBUG	7 /*Used for debugging messages */

#define DBG(level, fmt,...)	;

#define likely(x)       (x)
#define unlikely(x)     (x)

/* Macro to check null expression and exit if true */
#define CHECK_NULL(exp) {if (unlikely((exp) == NULL)){ printf("%s:%d:%s(): NULL" \
	" expression encountered\n", __FILE__, __LINE__, __FUNCTION__);exit(1);};};

/* Macro to check null expression and return if true */
#define CHECK_NULL_RET(exp){if (unlikely((exp) == NULL)){ printf("%s:%d:%s(): NULL" \
	" expression encountered\n", __FILE__, __LINE__, __FUNCTION__);\
	status=FT_INVALID_PARAMETER ; return(status);};};

/* Macro to check status  code and only print debug message */
#define CHECK_STATUS_NORET(exp) {if (unlikely((exp) != FT_OK)){DBG(MSG_ERR," status" \
	" != FT_OK\n"); };};

/* Macro to check status  code and return if not FT_OK */
#define CHECK_STATUS(exp) {if (unlikely(exp!=FT_OK)){DBG(MSG_ERR," status \
	!= FT_OK\n"); return(exp);}else{;}};

#define FN_ENTER	;
#define FN_EXIT     ;
#define FN_PT		;

#define INFRA_SLEEP(exp)			Sleep(exp);

/* Memory allocating, freeing & copying macros -  */
#define INFRA_MALLOC(exp)			malloc(exp); \
	DBG(MSG_DEBUG,"INFRA_MALLOC %ubytes\n", exp);
#define INFRA_FREE(exp)				free(exp); \
	DBG(MSG_DEBUG,"INFRA_FREE 0x%x\n", exp);
#define INFRA_MEMCPY(dest, src, siz)	memcpy(dest, src, siz);\
	DBG(MSG_DEBUG,"INFRA_MEMCPY dest:0x%x src:0x%x size:0x%x\n", dest, src, siz);

/******************************************************************************/
/*								Define platform								  */
/******************************************************************************/

/******************************************************************************/
/*								Type defines								  */
/******************************************************************************/

/* Match types with WinTypes.h for non-windows platforms. */
typedef UCHAR   uint8;
typedef USHORT  uint16;
typedef ULONGLONG uint64;

typedef signed char   int8;
typedef signed short  int16;
typedef signed long long int64;

// #ifndef __cplusplus   // bool is defined already in C++
//   typedef BOOL	bool; // bool is defined already in C++
// #endif                // bool is defined already in C++

typedef unsigned int   uint32;
typedef signed int   int32;

/******************************************************************************/
/*								External variables							  */
/******************************************************************************/

/******************************************************************************/
/*								Function declarations						  */
/******************************************************************************/

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif	/*FTDI_INFRA_H*/
