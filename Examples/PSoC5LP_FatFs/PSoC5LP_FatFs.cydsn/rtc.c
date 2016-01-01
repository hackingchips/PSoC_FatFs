/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <project.h>
#include "rtc.h"
#include "..\..\PSoCFatFsLibrary\diskio.h"
#include "..\..\PSocFatFsLibrary\ff.h"

/*---------------------------------------------------------*/
/* User Provided RTC Function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support an RTC.                     */
/* This function is not required in read-only cfg.         */

const unsigned long rtcYear = 2015;
const unsigned long rtcMonth = 10;
const unsigned long rtcDay = 16;
const unsigned long rtcHour = 10;
const unsigned long rtcMinutes = 32;
const unsigned long rtcSeconds = 0;

DWORD get_fattime()
{
	/* Pack date and time into a DWORD variable */
	return (((DWORD)rtcYear - 1980) << 25) | ((DWORD)rtcMonth << 21) | ((DWORD)rtcDay << 16)| (WORD)(rtcHour << 11)| 
            (WORD)(rtcMinutes << 5)	| (WORD)(rtcSeconds >> 1);;
}

/* [] END OF FILE */
