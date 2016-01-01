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
    int result_w = 0;
    
    unsigned char fortest[] = "This is a test.";
    UINT towrite, written;
    
    uint32 freeclusters, freesectors, totalsectors;
    uint32 size;

    /* Start serial port. */
    UART_Start();
    UART_PutString("  ");
    UART_PutString("<<< Test FatFs >>>\n");
    
    /* Start SPI bus. */
    SPI_Start();
    //SPI_SpiSetActiveSlaveSelect(1);
    SPI_SS_Write(1);

    UART_PutString("...mounting\n");
	result = f_mount(&FatFs, "", 1);
    
    /*** test reading. Be sure, "newfile.txt" exists with some text lines inside it. */
    if (result == FR_OK)
    {
        /* read volume label. */
      
        result = f_getlabel("", line, 0);
        if (result == FR_OK)
        {
            UART_PutString("Volume label: ");
            UART_PutString(line);
            UART_PutString("\n\n");
        } else UART_PutString("Volume label: ERROR\n");
        
        /* get volume bytes info. */
        
        result = f_getfree("", &freeclusters, &FatFsPtr);
        if (result == FR_OK)
        {
            totalsectors = (FatFsPtr->n_fatent - 2) * FatFsPtr->csize;
            itoa(totalsectors, line, 10);
            UART_PutString("Total sectors: "); UART_PutString(line); UART_PutString("\n");
            freesectors = freeclusters * FatFsPtr->csize;
            itoa(freesectors, line, 10);
            UART_PutString("Total sectors: "); UART_PutString(line); UART_PutString("\n");
            /* Free space. 512 bytes/sector. */
            itoa((totalsectors /2), line, 10);
            UART_PutString("Drive space: "); UART_PutString(line); UART_PutString("\n");
            itoa((freesectors /2), line, 10);
            UART_PutString("Free space: "); UART_PutString(line); UART_PutString("\n\n");
        }
        
        /* get directory info. */
        
        result = f_getcwd(line, sizeof(line));
        if (result == FR_OK)
        {
            UART_PutString("Actual directory: "); 
            UART_PutString(line); 
            UART_PutString("\n\n");
        }
        
        // ****************************************** It looks it works if file in sdcard.
        
        UART_PutString("...opening file\n");
        result = f_open(&Fil, "newfile.txt", FA_READ);

    	if (result == FR_OK)
        {
            while (f_gets(line, sizeof(line), &Fil))
            {
                UART_PutString(line);
            }

    		f_close(&Fil);								/* Close the file */

    	} else UART_PutString("ERROR: opening \"newfile.txt\"");
        
        UART_PutString("...un_mounting: ");
	    result = f_mount(NULL, "", 1);
        if (result == FR_OK) UART_PutString("...OK\n\n");
        else UART_PutString("...FAILED\n\n");
        
    }
    else
    {
        UART_PutString("ERROR: Can not mount sd-card.");
    }
    
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    //************************************************************************************
    
    UART_PutString("...mounting\n");
	result = f_mount(&FatFs_W, "", 1);
    
    if (result == FR_OK)
    {
        // *** Test appending to "newfile.txt"
        UART_PutString("...appending to file\n");
        result = f_open(&Fil, "newfile.txt", FA_WRITE | FA_OPEN_ALWAYS);
        
        if (result == FR_OK) 
        {
            /* Seek to end of the file to append data */
            size = f_size(&Fil);
            result = f_lseek(&Fil, f_size(&Fil));
            
            if (result == FR_OK)
            {
                //size = f_printf(&Fil, "appended line");
                size = f_puts("appended line", &Fil);
                
                f_close(&Fil);
                
                UART_PutString("...end appending\n");
            } else UART_PutString("ERROR: appending to file\n");
                
        } else UART_PutString("ERROR: opening for appending to file\n");
        
        // ****************************************** 
        // *** create new file "newfileA" and write something inside it. Using f_puts function.
        // *** be sure "newfileA.txt" doesnÃ‚Â´t exist yet.
        
        UART_PutString("...create new file\n");
        result = f_open(&Fil_W, "newfileA.txt", FA_WRITE | FA_CREATE_NEW);
        
        if (result == FR_OK)
        {
            UART_PutString("...writing\n");
            
            result = f_puts("write line 1", &Fil_W);
            result = result + 1;
            if (result != FR_OK) UART_PutString("...oh!!! error writing\n");
            result = f_puts("write line 2", &Fil_W);
            result = result + 1;
            result = f_puts("write line 3", &Fil_W);
            result = result + 1;
            
    		result = f_close(&Fil_W);								/* Close the file */
            result = result + 1;
            UART_PutString("...end writing\n");

    	} else UART_PutString("ERROR: creating new file");
        
        if (result == FR_OK) UART_PutString("...OK\n\n");
        else UART_PutString("...FAILED\n\n");
        
        // ****************************************** 
        // *** create new file "newfileB" and write something inside it.
        // *** be sure "newfileB.txt" doesnÃ‚Â´t exist yet.
        
        UART_PutString("...create new file\n");
        result = f_open(&Fil_W, "newfileB.txt", FA_WRITE | FA_CREATE_NEW);
        
        if (result == FR_OK)
        {
            UART_PutString("...writing\n");
            
            /* this writes only 9 bytes, not complete array "fortest" */
            towrite = 9;                
            result_w = f_write(&Fil_W, fortest, towrite, &written);
            
            result_w = result_w + 1;

    		result = f_close(&Fil_W);								/* Close the file */
            
            UART_PutString("...end writing\n");

    	} else UART_PutString("ERROR: creating new file");
        
        UART_PutString("...un_mounting: ");
	    result = f_mount(NULL, "", 1);
        if (result == FR_OK) UART_PutString("...OK\n\n");
        else UART_PutString("...FAILED\n\n");
    }
    else
    {
        UART_PutString("ERROR: Can not mount sd-card.");
    }
    
    
    

    /* CyGlobalIntEnable; */ /* Uncomment this line to enable global interrupts. */
    for(;;)
    {
    }
}

    //*** this function used for testing. incompatible licenses???
    //*** write my own and remove this.
	/**
     * C++ version 0.4 char* style "itoa":
     * Written by LukÃƒÆ’Ã‚Â¡s Chmela
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
