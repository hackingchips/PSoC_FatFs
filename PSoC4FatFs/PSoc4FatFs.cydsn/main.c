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

FATFS FatFs_W;
FIL Fil_W;

char* itoa(int value, char* result, int base);

int main()
{
    FATFS *FatFsPtr;
    char result = 0;
    char line[100];
    int result_w;
    
    unsigned char fortest[] = "This is a test.";
    UINT towrite, written;
    
    uint32 freeclusters, freesectors, totalsectors;
    uint32 size;

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
        /* read volume label. */
      
        result = f_getlabel("", line, 0);
        if (result == FR_OK)
        {
            UART_UartPutString("Volume label: ");
            UART_UartPutString(line);
            UART_UartPutString("\n\n");
        } else UART_UartPutString("Volume label: ERROR\n");
        
        /* get volume bytes info. */
        
        result = f_getfree("", &freeclusters, &FatFsPtr);
        if (result == FR_OK)
        {
            totalsectors = (FatFsPtr->n_fatent - 2) * FatFsPtr->csize;
            itoa(totalsectors, line, 10);
            UART_UartPutString("Total sectors: "); UART_UartPutString(line); UART_UartPutString("\n");
            freesectors = freeclusters * FatFsPtr->csize;
            itoa(freesectors, line, 10);
            UART_UartPutString("Total sectors: "); UART_UartPutString(line); UART_UartPutString("\n");
            /* Free space. 512 bytes/sector. */
            itoa((totalsectors /2), line, 10);
            UART_UartPutString("Drive space: "); UART_UartPutString(line); UART_UartPutString("\n");
            itoa((freesectors /2), line, 10);
            UART_UartPutString("Free space: "); UART_UartPutString(line); UART_UartPutString("\n\n");
        }
        
        /* get directory info. */
        
        result = f_getcwd(line, sizeof(line));
        if (result == FR_OK)
        {
            UART_UartPutString("Actual directory: "); 
            UART_UartPutString(line); 
            UART_UartPutString("\n\n");
        }
        
        // ****************************************** It looks it works if file in sdcard.
        
        UART_UartPutString("...opening file\n");
        result = f_open(&Fil, "newfile.txt", FA_READ);

    	if (result == FR_OK)
        {
            while (f_gets(line, sizeof(line), &Fil))
            {
                UART_UartPutString(line);
            }

    		f_close(&Fil);								/* Close the file */

    	} else UART_UartPutString("ERROR: opening \"newfile.txt\"");
        
        UART_UartPutString("...un_mounting: ");
	    result = f_mount(NULL, "", 1);
        if (result == FR_OK) UART_UartPutString("...OK\n\n");
        else UART_UartPutString("...FAILED\n\n");
        
    }
    
    UART_UartPutString("...mounting\n");
	result = f_mount(&FatFs_W, "", 1);
    
    if (result == FR_OK)
    {
        
        // *** Writing functions doesn´t work. 
        // ****************************************** SD CARD gets corrupted while writing
        
//        UART_UartPutString("...appending to file\n");
//        result = f_open(&Fil, "newfile.txt", FA_WRITE | FA_OPEN_ALWAYS);
//        
//        if (result == FR_OK) 
//        {
//            /* Seek to end of the file to append data */
//            size = f_size(&Fil);
//            result = f_lseek(&Fil, f_size(&Fil));
//            
//            if (result == FR_OK)
//            {
//                //size = f_printf(&Fil, "appended line");
//                size = f_puts("appended line", &Fil);
//                
//                f_close(&Fil);
//                
//                UART_UartPutString("...end appending\n");
//            } else UART_UartPutString("ERROR: appending to file\n");
//                
//        } else UART_UartPutString("ERROR: opening for appending to file\n");
        
        
        // ****************************************** SD CARD gets corrupted while writing
        
        UART_UartPutString("...create new file\n");
        result = f_open(&Fil_W, "newfileA.txt", FA_WRITE | FA_CREATE_NEW);
        
        if (result == FR_OK)
        {
            UART_UartPutString("...writing\n");
            
            /* this works. */
//            result_w = f_puts("write line 1", &Fil_W);
//            result_w = f_puts("write line 2", &Fil_W);
//            result_w = f_puts("write line 3", &Fil_W);
            
            /* this works too. */
            towrite = 9;
            result_w = f_write(&Fil_W, fortest, towrite, &written);

    		result = f_close(&Fil_W);								/* Close the file */
            
            UART_UartPutString("...end writing\n");

    	} else UART_UartPutString("ERROR: creating new file");
        
        UART_UartPutString("...un_mounting: ");
	    result = f_mount(NULL, "", 1);
        if (result == FR_OK) UART_UartPutString("...OK\n\n");
        else UART_UartPutString("...FAILED\n\n");
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

	/**
     * C++ version 0.4 char* style "itoa":
     * Written by LukÃ¡s Chmela
     * Released under GPLv3.
     * http://www.jb.man.ac.uk/~slowe/cpp/itoa.html
     */
    char* itoa(int value, char* result, int base) {
        // check that the base if valid
        if (base < 2 || base > 36) { *result = '\0'; return result; }
    
        char* ptr = result, *ptr1 = result, tmp_char;
        int tmp_value;
    
        do {
            tmp_value = value;
            value /= base;
            *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
        } while ( value );
    
        // Apply negative sign
        if (tmp_value < 0) *ptr++ = '-';
        *ptr-- = '\0';
        while(ptr1 < ptr) {
            tmp_char = *ptr;
            *ptr--= *ptr1;
            *ptr1++ = tmp_char;
        }
        return result;
    }


/* [] END OF FILE */
