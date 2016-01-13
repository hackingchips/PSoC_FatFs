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
#include <stdlib.h>

#include "..\..\..\PSoC_FatFs_Library\diskio.h"
#include "..\..\..\PSoC_FatFs_Library\ff.h"

char uartInputBuffer[255];
uint8 uartIBufferPtr = 0;

void SendMenu();

void Test_A();
void Test_B();

void FatFsError(FRESULT result);

int main()
{
    char uartchar;

    /* Start serial port. */
    UART_Start();
    SendMenu();
    
    /* Start SPI bus. */
    SPI_Start();
    //SPI_SpiSetActiveSlaveSelect(1);
    SPI_SS_Write(1);

    /* CyGlobalIntEnable; */ /* Uncomment this line to enable global interrupts. */
    for(;;)
    {
        uartchar = UART_GetChar();
        
        if (uartchar != 0)
        {
            UART_PutChar(uartchar);
            
            switch (uartchar)
            {
                case 'a':
                case 'A':
                {
                    Test_A(); 
                    UART_PutString("\nSelect test :>"); break;
                }; break;
                
                case 'b':
                case 'B':
                {
                    Test_B(); 
                    UART_PutString("\nSelect test :>"); break;
                }; break;
                    
                case '?': 
                    SendMenu(); break;
                    
                case 'Y':
                    {}; break;
                    
                case 'Z':
                    {}; break;
                    
                default:
                    UART_PutString("\nSelect test :>"); break;
            }
        }

    }
}

void SendMenu()
{
    UART_PutString("\n\nPSoC FatFs. PSoC 5LP FatFs Test/Demo.\n");
    UART_PutString("Hacking Chips. 12/2015.\n\n");
    
    UART_PutString("\t(A) - Show 'root' directory listing.\n");
    UART_PutString("\t(B) - Create directories, create files.\n");    
    
    UART_PutString("\t(?) - Show menu.\n");
    
    UART_PutString("Select test :>");    
}

void Test_A()
{
    FATFS fatFs;
    FIL fileO;
    DIR directoryO;
    FILINFO fileInfo;
    uint8 resultF;
    FATFS *fatFsPtr;
    
    char str[100];
    uint32 freeclusters, freesectors, totalsectors;

    /* Mount sdcard. */
    resultF = f_mount(&fatFs, "", 1);
    
    if (resultF == RES_OK)
    {
        /* read volume label. */
        resultF = f_getlabel("", str, 0);
        
        if (resultF != FR_OK)
        {
            FatFsError(resultF);
            return;
        }
        
        UART_PutString("\nVolume label: ");
        UART_PutString(str);
        UART_PutString("\n\n");
        
        /* Read directory contents. */
        resultF = f_opendir(&directoryO, "/");      // open 'root' directory.
        if (resultF != 0)
        {
            FatFsError(resultF);
            return;
        }
        
        while(1)
        {
            resultF = f_readdir(&directoryO, &fileInfo);
            if (resultF != 0)
            {
                FatFsError(resultF);
                break;
            }
            
            if (fileInfo.fname[0] == 0)             // end of directory?
            {
                break;     
            }
            
            if (fileInfo.fattrib & AM_DIR)          // is it a directory?
            {
                UART_PutString(fileInfo.fname);
                UART_PutString("\t\t\t<DIR>\n");
            }
            else                                    // it is a file.
            {
                UART_PutString(fileInfo.fname);
                UART_PutString("\t\t\t");     
                
                itoa(fileInfo.fsize, str, 10);
                UART_PutString(str);
                UART_PutChar('\n');
            }
                        
        }
        
        /* get volume bytes info. */
        resultF = f_getfree("", &freeclusters, &fatFsPtr);
        
        if (resultF != FR_OK)
        {
            FatFsError(resultF);
            return;
        }

        totalsectors = (fatFsPtr->n_fatent - 2) * fatFsPtr->csize;
        freesectors = freeclusters * fatFsPtr->csize;  

        itoa(totalsectors, str, 10);
        UART_PutString("\n\tTotal sectors: "); UART_PutString(str); UART_PutString("\n");
        itoa(freesectors, str, 10);
        UART_PutString("\tFree sectors: "); UART_PutString(str); UART_PutString("\n");
        itoa((totalsectors /2), str, 10);
        UART_PutString("\tDrive space: "); UART_PutString(str); UART_PutString("\n");
        itoa((freesectors /2), str, 10);
        UART_PutString("\tFree space: "); UART_PutString(str); UART_PutString("\n\n");        
        
        f_closedir(&directoryO);
        
        /* unmount */
        resultF = f_mount(NULL, "", 1);
        if (resultF != FR_OK)
        {
            FatFsError(resultF);
            return;
        }
    }
    else FatFsError(resultF);
}

void Test_B()
{
    FATFS fatFs;
    FIL fileO;
    DIR directoryO;
    FILINFO fileInfo;
    uint8 resultF;
    
    char str[100];
    uint32 freeclusters, freesectors, totalsectors;

    /* Mount sdcard. */
    resultF = f_mount(&fatFs, "", 1);
    
    if (resultF == RES_OK)
    {

        /* Create directory 'testb1' inside 'root' directory. */
        UART_PutString("\n...Creating directory 'testb1' inside root directory");
        resultF = f_mkdir("/testb1");
        
        if (resultF == RES_OK)
        {
            UART_PutString("\n...Creation of 'testb1' OK...");
        }
        else
        {
            if (resultF == FR_EXIST)
            {
                UART_PutString("\n...Directory 'testb1' exists... DELETING...");
                resultF = f_unlink("/testb1");
                if (resultF != RES_OK) FatFsError(resultF);
                UART_PutString("\n...Creating directory 'testb1' inside root directory");
                resultF = f_mkdir("/testb1");
                if (resultF != RES_OK) FatFsError(resultF);
            }
            else
            {
                FatFsError(resultF);
                return;
            }
        }
        
        /* Create directory 'testb2' inside 'root' directory. */
        UART_PutString("\n...Creating directory 'testb2' inside root directory");
        resultF = f_mkdir("/testb2");
        
        if (resultF == RES_OK)
        {
            UART_PutString("\n...Creation of 'testb2' OK...");
        }
        else
        {
            if (resultF == FR_EXIST)
            {
                UART_PutString("\n...Directory 'testb2' exists... DELETING...");
                resultF = f_unlink("/testb2");
                if (resultF != RES_OK) FatFsError(resultF);
                UART_PutString("\n...Creating directory 'testb2' inside root directory");
                resultF = f_mkdir("/testb2");
                if (resultF != RES_OK) FatFsError(resultF);
            }
            else
            {
                FatFsError(resultF);
                return;
            }
        }
        
        /* Create file inside 'testb2' */
        UART_PutString("\n...CD into 'testb2' directory.");
        resultF = f_chdir("/testb2");
        if (resultF != RES_OK) 
        {
            FatFsError(resultF);
            return;
        }
        
        UART_PutString("\n...Creating file 'testtxt.txt' inside 'testb2' directory.");
        resultF = f_open(&fileO, "testtxt.txt", FA_WRITE | FA_CREATE_NEW);
        
        if (resultF == FR_OK)
        {
            UART_PutString("...writing\n");
            
            resultF = f_puts("write line 1\r\n", &fileO);
            resultF = f_puts("write line 2\r\n", &fileO);
            resultF = f_puts("write line 3\r\n", &fileO);
            
    		resultF = f_close(&fileO);								/* Close the file */
            
            UART_PutString("...end writing\n");

    	} 
        else 
        {
            if (resultF == FR_EXIST)
            {
                UART_PutString("\n...File exists... DELETING...");
                resultF = f_unlink("testtxt.txt");
                if (resultF != RES_OK) FatFsError(resultF);
                UART_PutString("\n...Repeat test...");
            }
            else
            {
                FatFsError(resultF);
                return;
            }
        }
    }
    else FatFsError(resultF);    
}

void FatFsError(FRESULT result)
{
    switch (result)
    {
        case FR_DISK_ERR:
            UART_PutString("\n    error: (FR_DISK_ERR) low level error.\n"); break;
            
        case FR_INT_ERR:
            UART_PutString("\n    error: (FR_INT_ERR)\n"); break; 
            
        case FR_NOT_READY:
            UART_PutString("\n    error: (FR_NOT_READY) sdcard not ready.\n"); break;
            
        case FR_NO_FILE:
            UART_PutString("\n    error: (FR_NO_FILE) invalid file.\n"); break;
            
        case FR_NO_PATH:
            UART_PutString("\n    error: (FR_NO_PATH) invalid path.\n"); break;
            
        case FR_INVALID_NAME:
            UART_PutString("\n    error: (FR_INVALID_NAME) invalid name.\n"); break;
            
        case FR_DENIED:
            UART_PutString("\n    error: (FR_DENIED) operation denied.\n"); break;
            
        case FR_EXIST:
            UART_PutString("\n    error: (FR_EXIST) it exists yet...\n"); break;
            
        case FR_INVALID_OBJECT:
            UART_PutString("\n    error: (FR_INVALID_OBJECT)\n"); break;
            
        case FR_WRITE_PROTECTED:
            UART_PutString("\n    error: (FR_WRITE_PROTECTED)\n"); break;
            
        case FR_INVALID_DRIVE:
            UART_PutString("\n    error: (FR_INVALID_DRIVE)\n"); break;
            
        case FR_NOT_ENABLED:
            UART_PutString("\n    error: (FR_NOT_ENABLED) sdcard unmounted.\n"); break;
            
        case FR_NO_FILESYSTEM:
            UART_PutString("\n    error: (FR_NO_FILESYSTEM) no valid FAT volume.\n"); break;  
            
        case FR_MKFS_ABORTED:
            UART_PutString("\n    error: (FR_MKFS_ABORTED)\n"); break;
            
        case FR_TIMEOUT:
            UART_PutString("\n    error: (FR_TIMEOUT)\n"); break;
            
        case FR_LOCKED:
            UART_PutString("\n    error: (FR_LOCKED)\n"); break;
            
        case FR_NOT_ENOUGH_CORE:
            UART_PutString("\n    error: (FR_NOT_ENOUGH_CORE)\n"); break;     
            
        case FR_TOO_MANY_OPEN_FILES:
            UART_PutString("\n    error: (FR_TOO_MANY_OPEN_FILES)\n"); break;
            
        case FR_INVALID_PARAMETER:
            UART_PutString("\n    error: (FR_INVALID_PARAMETER)\n"); break; 
            
        default: {}; break;
    }
}


/* [] END OF FILE */
