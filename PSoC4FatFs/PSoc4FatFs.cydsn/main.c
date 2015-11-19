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
#include "..\..\PSoCFatFsLibrary\diskio.h"
#include "..\..\PSoCFatFsLibrary\ff.h"

FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */

int main()
{
    char result = 0;
    char line[100];


    /* Start serial port. */
    UART_Start();
    UART_UartPutString("<<< Test FatFs >>>\n");
    
    /* Start SPI bus. */
    SPI_Start();
    SPI_SpiSetActiveSlaveSelect(1);
    SPI_SS_Write(1);

    UART_UartPutString("...mounting\n");
	result = f_mount(&FatFs, "", 1);
    
    if (result == FR_OK)
    {
        UART_UartPutString("...opening\n");
        result = f_open(&Fil, "newfile.txt", FA_READ);

    	if (result == FR_OK)
        {
            while (f_gets(line, sizeof(line), &Fil))
            {
                UART_UartPutString(line);
            }

    		f_close(&Fil);								/* Close the file */

    	} else UART_UartPutString("ERROR: opening \"newfile.txt\"");
        
        // ****************************************** SD CARD gets corrupted while writing
        
        UART_UartPutString("...create new file\n");
        result = f_open(&Fil, "newfileA.txt", FA_WRITE | FA_CREATE_NEW);
        
        if (result == FR_OK)
        {
            UART_UartPutString("...writing\n");
            
            f_puts("write line 1", &Fil);
            f_puts("write line 2", &Fil);
            f_puts("write line 3", &Fil);

    		f_close(&Fil);								/* Close the file */
            
            UART_UartPutString("...end writing");

    	} else UART_UartPutString("ERROR: creating new file");
    }
    else
    {
        UART_UartPutString("ERROR: Can not mount sd-card.");
    }

    /* CyGlobalIntEnable; */ /* Uncomment this line to enable global interrupts. */
    for(;;)
    {
    }
}

/* [] END OF FILE */
