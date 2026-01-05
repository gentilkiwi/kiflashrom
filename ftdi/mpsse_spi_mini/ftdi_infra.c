/*!
 * \file infra.c
 *
 * \author FTDI
 * \date 20110317
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
 * Revision History:
 * 0.1 - initial version
 * 0.2 - 20110708 - exported Init_libMPSSE & Cleanup_libMPSSE for Microsoft toolchain support
 * 0.3 - 20111103 - commented & cleaned up
 */

/******************************************************************************/
/*								Include files					  			  */
/******************************************************************************/
#define FTDI_EXPORTS
#include "ftdi_infra.h"		/*portable infrastructure(datatypes, libraries, etc)*/
#include "ftdi_common.h"	/*Common across I2C, SPI, JTAG modules*/

/******************************************************************************/
/*								Macro defines					  			  */
/******************************************************************************/

/******************************************************************************/
/*								Global variables							  */
/******************************************************************************/

/******************************************************************************/
/*								Local function declarations					  */
/******************************************************************************/

/******************************************************************************/
/*						Global function definitions						  */
/******************************************************************************/

/******************************************************************************/
/*						Local function definitions						  */
/******************************************************************************/

/******************************************************************************/
/*						Public function definitions						  */
/******************************************************************************/

/**
 * Construct a DWORD with major, minor and build version numbers.
 *
 * The format matches that published in the D2XX Programmer's Guide,
 * where each individual byte contains a value which, when displayed
 * as hexadecimal, represents the version number.  For example, "15"
 * is stored as 21.
 *
 * @return Version numbers as a single DWORD.
 */
static DWORD versionNumberToHex(void)
{
#if !defined(FT_VER_MAJOR) || !defined(FT_VER_MINOR) || !defined(FT_VER_BUILD)
    #error "The Makefile must define major, minor and build version numbers."
    return (DWORD)0;
#else
    char  buf[7]; /* enough for '123456' and terminator */
    char *endPtr = NULL;
    long int versionNumber;

#define COMPILE_TIME_ASSERT(pred) switch(0){case 0:case pred:;}
    COMPILE_TIME_ASSERT(FT_VER_MAJOR < 99);
    COMPILE_TIME_ASSERT(FT_VER_MINOR < 99);
    COMPILE_TIME_ASSERT(FT_VER_BUILD < 99);

#ifdef _WIN32
    sprintf_s(buf, 7, "%02d%02d%02d", FT_VER_MAJOR, FT_VER_MINOR, FT_VER_BUILD);
#else
    sprintf(buf, "%02d%02d%02d", FT_VER_MAJOR, FT_VER_MINOR, FT_VER_BUILD);
#endif
    versionNumber = strtol(buf, &endPtr, 16);

    return (DWORD)versionNumber;

#endif /* !defined(FT_VER_MAJOR) || !defined(FT_VER_MINOR) || !defined(FT_VER_BUILD) */
}

/*!
 * \brief Version Number Function
 *
 * Returns libMPSSE and libFTD2XX version number
 *
 * \param[out]  *libmpsse	MPSSE version number is returned
 * \param[out]  *libftd2xx	D2XX version number is returned
 * \return Returns status code of type FT_STATUS(see D2XX Programmer's Guide)
 * \sa
 * \warning
 */
FTDIMPSSE_API FT_STATUS Ver_libMPSSE(LPDWORD libmpsse, LPDWORD libftd2xx)
{
	FT_STATUS status = FT_INVALID_PARAMETER;
	
	FN_ENTER;
	
    if ((libmpsse) && (libftd2xx))
    {
        *libmpsse = versionNumberToHex();
		status = FT_GetLibraryVersion(libftd2xx);
	}

	FN_EXIT;
	return status;
}



