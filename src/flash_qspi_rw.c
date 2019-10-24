/*****************************************************************************/
/**
*
* @file flash_qspi_rw.c
*
* Defines the functions and includes the proper headers for the Xapp1280.
*
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00 SSJ  04/05/2016 First release
******************************************************************************/

/***************************** Include Files *********************************/

#include "xparameters.h"	/* defines XPAR values */
// #include "xuartns550_l.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xspi.h"		/* SPI device driver */
// #include "xil_exception.h"
#include "xintc.h"
#include "pcimem.h"
#include <ctype.h>
#include <xil_types.h>
#include "xil_cache.h"
#include <fcntl.h>
/************************** Constant Definitions ****************************/
/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are only defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define INTC_BASEADDR 		XPAR_INTC_0_BASEADDR
#define UART_CLOCK_HZ		XPAR_UARTNS550_0_CLOCK_FREQ_HZ
#define UART_DEVICE_ID		XPAR_UARTNS550_0_DEVICE_ID
#define UART_IRPT_INTR		XPAR_INTC_0_UARTNS550_0_VEC_ID
#define	UART_INTR			XPAR_MICROBLAZE_0_AXI_INTC_AXI_UART16550_0_IP2INTC_IRPT_INTR
#define UART_INTR_MASK 		XPAR_AXI_UART16550_0_IP2INTC_IRPT_MASK
#define UART_BASEADDR		XPAR_UARTNS550_0_BASEADDR
#define TMRCTR_DEVICE_ID	XPAR_TMRCTR_0_DEVICE_ID
#define TIMER_INTR			XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR
#define TMRCTR_INTERRUPT_ID	XPAR_INTC_0_TMRCTR_0_VEC_ID
#define TIMER_INTR_MASK 	XPAR_AXI_TIMER_0_INTERRUPT_MASK
#define	TIMER_BASEADDR		XPAR_TMRCTR_0_BASEADDR
#define SPI_DEVICE_ID		XPAR_SPI_0_DEVICE_ID
#define SPI_INTR_ID			XPAR_INTC_0_SPI_0_VEC_ID
#define	SPI_INTR			XPAR_MICROBLAZE_0_AXI_INTC_AXI_QUAD_SPI_0_IP2INTC_IRPT_INTR
#define SPI_INTR_MASK 		XPAR_AXI_QUAD_SPI_0_IP2INTC_IRPT_MASK
#define SPI_BASEADDR		XPAR_SPI_0_BASEADDR
/*****************************************************************************************/
#define INTC		    	static XIntc
#define INTC_HANDLER		XIntc_InterruptHandler
#define TIMER_BASE_ADDR		XPAR_AXI_TIMER_0_BASEADDR
#define TIMER_CNTR_0	 	0
#define RESET_VALUE	 		0x00000000
#define UART_BAUDRATE		115200
#define STDOUT_BASEADDR XPAR_AXI_UART16550_0_BASEADDR
/*****************************************************************************************/
#define STDOUT_IS_16550
#ifndef ENABLE_ICACHE
#define ENABLE_ICACHE()		Xil_ICacheEnable()
#endif
#ifndef	ENABLE_DCACHE
#define ENABLE_DCACHE()		Xil_DCacheEnable()
#endif
#ifndef	DISABLE_ICACHE
#define DISABLE_ICACHE()	Xil_ICacheDisable()
#endif
#ifndef DISABLE_DCACHE
#define DISABLE_DCACHE()	Xil_DCacheDisable()
#endif
/*************************************************************************************************/
#define SPI_SELECT 									0x01
#define COMMAND_WRITE_STATUS_REGISTER  				0x01 /* WRITE STATUS REGISTER 01h  */
#define COMMAND_STATUSREG_READ						0x05 /* Status read command */
#define	COMMAND_WRITE_ENABLE						0x06 /* Write Enable command */
#define COMMAND_ENTER_QUAD_MODE						0x35 /* ENTER QUAD mode */
#define COMMAND_EXIT_QUAD_MODE						0xF5 /* EXIT QUAD = QUAD F5h  */
#define COMMAND_ENTER_4BYTE_ADDRESS_MODE			0xB7 /* ENTER 4-BYTE ADDRESS MODE B7 */
#define COMMAND_EXIT_4BYTE_ADDRESS_MODE				0xE9 /* EXIT 4-BYTE ADDRESS MODE E9h */
#define COMMAND_READ_FLAG_STATUS 					0x70
#define	COMMAND_CLEAR_FLAG_STATUS					0x50
#define	COMMAND_WRITE_DISABLE						0x04 /* Write Enable command */
#define COMMAND_READ_EXTENDED_ADDRESS				0xC8
#define COMMAND_WRITE_EXTENDED_ADDRESS				0xC5
/*************************************************************************************************/
#define COMMAND_PAGE_PROGRAM						0x02 /* Page Program command */
#define COMMAND_QUAD_WRITE							0x32 /* Quad Input Fast Program */
#define COMMAND_4BYTE_PAGE_PROGRAM 					0x12
#define COMMAND_EXTENDED_QUAD_INPUT_FAST_PROGRAM	0x32	//32, 12h 38h note SUPPORTED

/*************************************************************************************************/
#define COMMAND_READ_ID								0x9E /* READ ID 9E/9Fh  */
#define COMMAND_READ_DISCOVERY						0x5A /* READ SERIAL FLASH DISCOVERY PARAMETER 5Ah */
#define COMMAND_RANDOM_READ							0x03 /* Random read command */
#define COMMAND_DUAL_READ							0x3B /* Dual Output Fast Read */
#define COMMAND_DUAL_IO_READ						0xBB /* Dual IO Fast Read */
#define COMMAND_QUAD_READ							0x6B /* Quad Output Fast Read */
#define COMMAND_QUAD_IO_READ						0xEB /* Quad IO Fast Read */
#define COMMAND_4BYTE_READ							0x13 /* 4-BYTE READ */
#define COMMAND_4BYTE_FAST_READ 					0x0C /* 4-BYTE FAST READ */
#define COMMAND_4BYTE_DUAL_OUTPUT_FAST_READ			0x3C /* 4-BYTE DUAL OUTPUT FAST READ */
#define COMMAND_4BYTE_DUAL_INPUTOUTPUT_FAST_READ	0XBC /* 4-BYTE DUAL INPUT/OUTPUT FAST READ*/
#define COMMAND_4BYTE_QUAD_OUTPUT_FAST_READ			0x6C /* 4-BYTE QUAD OUTPUT FAST READ */
#define COMMAND_4BYTE_QUAD_INPUTOUTPUT_FASTREAD		0xEC /* 4-BYTE QUAD INPUT/OUTPUT FASTREAD*/
/*************************************************************************************************/
#define COMMAND_SECTOR_ERASE						0xD8 /* Sector Erase command */
#define COMMAND_BULK_ERASE							0xC7 /* Bulk Erase command */
#define COMMAND_DIE_ERASE							0xC4 /* Bulk Erase command */
#define COMMAND_SUBSECTOR_ERASE 					0x20 /* SUBSECTOR ERASE 20h */
#define COMMAND_4BYTE_SUBSECTOR_ERASE 				0x21 /* 4-BYTE SUBSECTOR ERASE 21h */
/*************************************************************************************************/
/**
 * This definitions specify the EXTRA bytes in each of the command
 * transactions. This count includes Command byte, address bytes and any don't care bytes needed.
 */
#define READ_WRITE_EXTRA_BYTES		4 /* Read/Write extra bytes */
#define	WRITE_ENABLE_BYTES			1 /* Write Enable bytes */
#define SECTOR_ERASE_BYTES			4 /* Sector erase extra bytes */
#define BULK_ERASE_BYTES			1 /* Bulk erase extra bytes */
#define STATUS_READ_BYTES			2 /* Status read bytes count */
#define STATUS_WRITE_BYTES			2 /* Status write bytes count */
#define FLASH_SR_IS_READY_MASK		0x01 /* Ready mask */
/*
 * Number of bytes per page in the flash device.
 */
#define PAGE_SIZE					256
#define NUMB_SECTORS				512
#define	BYTE_PER_SECTOR				65536
#define	NUMB_SUBSECTORS				8192
#define	BYTE_PER_SUBSECTOR			4096
#define NOB_PAGES					131072
/*
 * Address of the page to perform Erase, Write and Read operations.
 */
#define FLASH_TEST_ADDRESS0			0x00000000
#define FLASH_TEST_ADDRESS1			0x00F50000
#define BINFILESIZE					2242764
/*
 * Byte Positions.
 */
#define BYTE1						0 /* Byte 1 position */
#define BYTE2						1 /* Byte 2 position */
#define BYTE3						2 /* Byte 3 position */
#define BYTE4						3 /* Byte 4 position */
#define BYTE5						4 /* Byte 5 position */
#define BYTE6						5 /* Byte 6 position */
#define BYTE7						6 /* Byte 7 position */
#define BYTE8						7 /* Byte 8 position */
#define DUAL_READ_DUMMY_BYTES		2
#define QUAD_READ_DUMMY_BYTES		4
#define DUAL_IO_READ_DUMMY_BYTES	2
#define QUAD_IO_READ_DUMMY_BYTES	5
#define DDR_ADDR0 					0x81000000
#define DDR_ADDR1 					0x86000000

#define XPAR_SPI_0_NUM_TRANSFER_BITS 	8


#define putnum(x) printf("%d ", (x))
#define print(x) printf("%s", (x))
// #define inbyte() fgetc(stdin)
/************************** Function Prototypes *****************************/
void init_platform(void);
void cleanup_platform(void);
int SpiFlashWriteEnable(XSpi *SpiPtr);
int SpiFlashWrite(XSpi *SpiPtr, u32 Addr, u32 ByteCount, u8 WriteCmd);
int SpiFlashWrite_File(XSpi *SpiPtr, u32 Addr, u32 ByteCount, u8 WriteCmd, unsigned char* pageBuffer);
int SpiFlashRead(XSpi *SpiPtr, u32 Addr, u32 ByteCount, u8 ReadCmd);
int SpiFlashBulkErase(XSpi *SpiPtr);
int SpiFlashSectorErase(XSpi *SpiPtr, u32 Addr);
int SpiFlashDieErase(XSpi *SpiPtr, u32 Addr);
int SpiFlashGetStatus(XSpi *SpiPtr);
int SpiFlashGetStatusPrint(XSpi *SpiPtr);
int SpiFlashQuadEnable(XSpi *SpiPtr);
int SpiFlashEnableHPM(XSpi *SpiPtr);
static int SpiFlashWaitForFlashReady(void);
void SpiHandler(void *CallBackRef, u32 StatusEvent, unsigned int ByteCount);
static int SetupInterruptSystem(XSpi *SpiPtr);
int qspi_flash_geo(void);
int qspi_flash_erase(void);
int pgm_flash_file(void);
int qspi_read_flash(u32 StartAddr, u32 NoByteToRead);
int System_init_startup (void);
int qspi_flash_erase (void);
int pgm_flash_file (void);
int qspi_ease_entire_flash (void);
int qspi_erase_sector_flash (u32 OfsetAddr, u32 SectCount);
int qspi_flash_erase_main (u32 OfsetAddr, u32 DieCount );
int icap ();
int SpiFlashReadID(XSpi *SpiPtr);
int SpiFlashReadRegister(XSpi *SpiPtr, unsigned int addRegister, unsigned int byteCount);
int SpiFlashReadRegisterNoWriteEnable(XSpi *SpiPtr, unsigned int addRegister, unsigned int byteCount);
int Spi_Blank_Check(u32 StartAddr, u32 NoByteToRead);
int qspi_program_flash (u32 StartAddr);
int qspi_readback_flash(u32 StartAddr,  u32 NoByteToRead);
unsigned int convertToDecimal(char const* hexstring);
static int  DownloadSerialDataToQSPIFlash(u32 StartDie);
static int  VerifySerialDataToQSPIFlash(u32 StartDie);
static int TeraTermFile_Receive ( u32 StartAddr,u32 NoByteToRead);
int read_rs232 (char* buf, int nbytes);
void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber);
void TimerCntOutHandler(void *CallBackRef, u8 TmrCtrNumber);
void TmrCtrDisableIntr(XIntc* IntcInstancePtr, u16 IntrId);
void TmrCtr_FastHandler(void) __attribute__ ((fast_interrupt));
int SpiFlash4byteexit(XSpi *SpiPtr);
/************************** Variable Definitions ****************************/
static XSpi Spi;
static Pcimem Pci;
static int qspi_init_flag=0;
INTC InterruptController;  /* The instance of the Interrupt Controller */
volatile static int TransferInProgress;
volatile static u64 FileByteCount;
volatile static int TimerExpired;
volatile static u8 TIMEPUTFLAG;
static int ErrorCount;
volatile static int TransferInProgress;
static u32 Address = FLASH_TEST_ADDRESS0;
static u8 ReadBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES + 5];
static u8 WriteBuffer[PAGE_SIZE + READ_WRITE_EXTRA_BYTES + 5];

char inbyte(void) {
	return 0;  //back
	// uart related
	// return XUartPs_RecvByte(STDIN_BASEADDRESS);
}


void outbyte(char c) {
	//back
	//uart related
	 // XUartPs_SendByte(STDOUT_BASEADDRESS, c);
}


/*****************************************************************************/
/**
*
* Main function to run the quad flash update.
*
* @param	None
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int main(void)
{
	printf("\n\rflash_qspi_rw entered\n\r");
    int choice, exit_flag = 0;
    int Status;
   	u32 SectCount=1, StartAddr, NoByteToRead;
	char* hex= FLASH_TEST_ADDRESS1;
	int nbytes_temp=16;
	int input_deci_data=0;	
	pcimem_init(&Pci);
	init_platform();
	
	if (qspi_init_flag ==0)
	{
		printf("\n\r *flash_qspi_rw before System_init_startup 1\n\r");
		fflush(stdout);
		Status = System_init_startup ();
		printf("\n\r *flash_qspi_rw before System_init_startup 2\n\r");
		fflush(stdout);		
		if (Status != XST_SUCCESS) {
					return XST_FAILURE;
				} else qspi_init_flag=1;

	}
		StartAddr = FLASH_TEST_ADDRESS0;
		NoByteToRead = 512;
		SectCount = 512;
		printf("\n\r *flash_qspi_rw before System_init_startup 3\n\r");
		fflush(stdout);		
		while(exit_flag != 1) {
			// printf("%c[2J",27);
			printf("\n\r*******************************************************************");
			printf("\n\r*******************************************************************\n\r");
    		printf("\n\rChoose from options below: \r\n");
    	    printf("1: Read Quad SPI flash ID\r\n");
    	    printf("2: Erase Quad SPI flash\r\n");
    	    printf("3: Blank Check Quad SPI flash\r\n");
    	    printf("4: Program (*.bin)\r\n");
    	    printf("5: Read Quad SPI flash\r\n");
			printf("6: Check Flag Status\r\n");
			printf("7: Enable Quad\r\n");
			printf("8: Verify (*.bin)\r\n");			
			printf("9: ICAP\r\n");
			fflush(stdout);
			int choice;
    	    // choice = inbyte();
			scanf("%d", &choice);
			printf("You entered: %d\n", choice);
        	/*if (isalpha(choice)) {
        	    choice = toupper(choice);
        	}*/
    		switch(choice) {
    			case 0:
					break;
				case 1:
				{
					printf("\n\r\t Read Quad SPI flash\t\r\n");
					fflush(stdout);
					qspi_flash_geo();
					printf("\n\rPress any Key to Main Menu\r\n");
					fflush(stdout);					
					inbyte();
					break;
				}
				case 2:
				{
					printf("\n\rDie number to erase (0-3): ");					
					int input_deci_data;    	    		
					scanf("%d", &input_deci_data);
					printf("You entered: %d\n", input_deci_data);
									
					qspi_flash_erase_main(Address, input_deci_data);
					break;
				}
				case 3:
				{
					printf ("\n\rQuad SPI flash Blank Check:\n\r");
					printf("\n\rBlank Check start address (hex): ");
					/*
					input_deci_data = read_rs232 (&hex, nbytes_temp);
					if (input_deci_data !=0)
						{
								StartAddr = convertToDecimal(&hex);
								printf("\r\nNumber of bytes to Blank Check (hex): ");
								input_deci_data =0;
						}else
						{
							printf("\n\rWrong Hex Address Entered\r\n");
							inbyte();
							break;
						}
					input_deci_data = read_rs232 (&hex, nbytes_temp);
					if (input_deci_data !=0)
						{
							NoByteToRead = convertToDecimal(&hex);
							printf("\r\n\r\nStart Address \t= 0x%08x\n\rEnd Address \t= 0x%08x\n\r", StartAddr,(NoByteToRead + StartAddr));
							printf ("\n\rPerforming Blank Check operation...\n\r");
							Status = Spi_Blank_Check(StartAddr, NoByteToRead);
								if (Status != XST_SUCCESS) {
												printf("\n\r\n\r\t\tBlank Check Operation Fail!.\r\n");
										}else  printf("\n\r\n\rBlank Check Operation Completed without error.\r\n");
								input_deci_data =0;
							}
					*/
					StartAddr = 0;
					NoByteToRead = 256;
					Status = Spi_Blank_Check(StartAddr, NoByteToRead);
					if (Status != XST_SUCCESS) {
						printf("\n\r\n\r\t\tBlank Check Operation Fail!.\r\n");
					}else  printf("\n\r\n\rBlank Check Operation Completed without error.\r\n");
								
					printf("\n\r\n\rPress any Key to Main Menu\r\n");
					inbyte();
					break;
				}
				case 4:
				{
					printf("\r\nProgram (*.bin)\r\n");
					printf("\n\rDie number to program (0-3): ");					
					int input_deci_data;    	    		
					scanf("%d", &input_deci_data);
					printf("You entered: %d\n", input_deci_data);
					
					
				
						/*input_deci_data = read_rs232 (&hex, nbytes_temp);
						if (input_deci_data !=0)
						{
							StartAddr = convertToDecimal(&hex);
							printf("\n\r\tNumber Of Bytes in BIN File(hex): ");
							input_deci_data =0;
						} 
						else
						{
							printf("\n\rWrong Hex Address Entered\r\n");
							inbyte();
							break;
						}
						input_deci_data = read_rs232 (&hex, nbytes_temp);
						if (input_deci_data !=0)
						{
							NoByteToRead = convertToDecimal(&hex);
							if (NoByteToRead <=0)
							{
								NoByteToRead = BINFILESIZE;
								input_deci_data =0;
							}
							else 	Status = DownloadSerialDataToQSPIFlash(StartAddr, NoByteToRead);
						}*/						
					Status = DownloadSerialDataToQSPIFlash(input_deci_data);
					printf("\n\r\n\rPress any Key to Main Menu\r\n");
					inbyte();
					break;
				}	
				case 5:
				{
					printf ("\n\r\n\rRead Quad SPI flash:");
					printf("\n\rStart Address: ");
					// input_deci_data = read_rs232 (&hex, nbytes_temp);
					int input_deci_data;
    	    		// choice = inbyte();
					scanf("%d", &input_deci_data);
					printf("You entered: %d\n", input_deci_data);
					
					StartAddr = input_deci_data;
					printf("\n\rNumber of Bytes to read:");
					input_deci_data = 0;
					scanf("%d", &input_deci_data);
					printf("You entered: %d\n", input_deci_data);
					NoByteToRead = input_deci_data;
					if( NoByteToRead )
					{
						qspi_read_flash(StartAddr, NoByteToRead);
					}

					printf("\n\rPress any Key to Main Menu\r\n");
					break;
				}
				case 6:
				{
					printf("\n\r\t Read FlagStatus\t\r\n");
					fflush(stdout);
					// qspi_flash_geo();
					int Status = SpiFlashGetStatusPrint(&Spi);	
					int FlagStatus = SpiFlashReadflagstatus(&Spi);
					SpiFlashClrflagstatus(&Spi);
					printf("FlagStatus Status 0x%X flagStatus 0x%x\n", Status, FlagStatus);
					fflush(stdout);
					break;
				}		
				case 7:
				{
					printf("\n\r\t Enable Quad flash\t\r\n");
					fflush(stdout);
					Status = SpiFlashQuadEnable(&Spi);
					if (Status != XST_SUCCESS) {
						printf("Enable Quad flash failed Status 0x%X\n", Status);
						fflush(stdout);
						return XST_FAILURE;						
					}
					break;
				}
				case 8:
				{
					printf("\r\nVerify (*.bin)\r\n");
					printf("\n\rDie number to Verify (0-3): ");					
					int input_deci_data;    	    		
					scanf("%d", &input_deci_data);
					printf("You entered: %d\n", input_deci_data);
					Status = VerifySerialDataToQSPIFlash(input_deci_data);
					printf("\n\r\n\rPress any Key to Main Menu\r\n");
					inbyte();
					break;
				}
				case 9:
				{
					icap();			
					break;
				}														
				default:
				{
					break;
				}
			}
			if(exit_flag != 1) {
			}
	}
	cleanup_platform();
	
    return 0;
}
/*****************************************************************************/
/**
*
* This function reads serial HEX value entered and converts into Integer value.
*
* @param	Buf pointer with number of bytes entered in Array
*
* @return	Number of Bytes in Array.
*
* @note		None
*
******************************************************************************/
int read_rs232 (char* buf, int nbytes)
{
  int i = 0;
  for (i = 0; i < nbytes; i++) {
    *(buf + i) = inbyte();
    outbyte (*(buf + i));
    if ((*(buf + i) == '\n' || *(buf + i) == '\r'))
        break;
    if (*(buf + i) == '\b')
 		if (i==0)
 		i=-1;
 		else i=(i-2);
  }
  return (i);
}
/*****************************************************************************/
/**
*
* This function converts To Decimal value.
*
* @param	Input the Character string.
*
* @return	Unsigned integer value.
*
* @note		None
*
******************************************************************************/
unsigned int convertToDecimal(char const* hexstring)
{
	unsigned long  result = 0;
	char const *inpstr = hexstring;
	char  charhex;
	int charhexint;
	while( ( (charhex = *inpstr) != NULL ) && ((charhex = *inpstr) != '\r') && ((charhex = *inpstr) != '\n'))
	{
		unsigned long add;
		charhexint = toupper(charhex);

		result <<= 4;
		if (charhexint != '\b')
		{
			if( charhexint >= 48 &&  charhexint <= 57 )
				add = charhexint - 48;
			else if( charhexint >= 65 && charhexint <= 70)
				add = charhexint - 65 + 10;
			else
			{
				printf("\n\rUnrecognized hex   "); putchar(charhex);
			}

			result += add;
			++inpstr;
		} else ++inpstr;

	}

	return result;
}
/*****************************************************************************/
/**
*
* This function Flash Erase includes the Bulk Erase for SPI flash.
*
* @param	Input the Offset Address and the Sector count.
*
* @return	None
*
* @note		None
*
******************************************************************************/
int qspi_flash_erase_main(u32 OfsetAddr, u32 DieCount)
{
	int choice, exit_subflag=0;
	// qspi_ease_entire_flash();
	// SpiFlashSectorErase(&Spi, 0);
	SpiFlashDieErase(&Spi, DieCount);
	/*
	while(exit_subflag != 1) {
			printf("\r\nChoose from options below: \r\n");
    	    printf("1: Entire flash erase (Bulk Erase)\r\n");
    	    printf("\r\nAny Key to Main Menu...");
    	    // choice = inbyte();
        	// if (isalpha(choice)) {
        	//    choice = toupper(choice);
        	// }
			choice = 1;
    	    switch(choice) {
    		case '1':
				{
					qspi_ease_entire_flash();
					printf("\n\rPress any Key to Main Menu\r\n");
					inbyte();
					break;
    			}
			case '0':
				break;
			default:
				break;
		}
    	if(exit_subflag != 1) {
			break;
		}
	}
	*/
	return XST_SUCCESS;
}

int icap()
{
	printf("\n icap \n\r");
	uint64_t icapBaseAddr = 0x00070000;
	uint32_t icapWriteFifo = 0x0200;
	uint32_t icapWriteControl = 0x0218;
	// write IPROG command sequence
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0xFFFFFFFF);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0xAA995566);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x20000000);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x30020001);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x00000000);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x30008001);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x0000000F);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x20000000);	

	// write control reg
	PciWriteReg(&Spi, icapBaseAddr, icapWriteControl, 0x00000001);	
	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* quad flash initialization.
*
* @param	None
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int System_init_startup ()
{
	int Status;
	XSpi_Config *ConfigPtr;	
	printf("\n\rflash_qspi_rw System_init_startup 1 SPI_DEVICE_ID %d", SPI_DEVICE_ID);		
	ConfigPtr = XSpi_LookupConfig(SPI_DEVICE_ID);
	printf("\n\rflash_qspi_rw System_init_startup 1.2 SPI_DEVICE_ID %d not found", SPI_DEVICE_ID);
	if (ConfigPtr == NULL) {
		printf("\n\rflash_qspi_rw System_init_startup 1.5 SPI_DEVICE_ID %d not found", SPI_DEVICE_ID);
		return XST_DEVICE_NOT_FOUND;
	}
	printf("\n\r flash_qspi_rw System_init_startup 2");	
	fflush(stdout);
	XSpi_PciInitialize(&Spi, &Pci);
	printf("\n\r flash_qspi_rw System_init_startup 3");	
	fflush(stdout);	
	Status = XSpi_CfgInitialize(&Spi, ConfigPtr,
				  ConfigPtr->BaseAddress);
	printf("\n\r flash_qspi_rw System_init_startup 4");	
	fflush(stdout);					  
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
//	Status = SetupInterruptSystem(&Spi);
//	if(Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
	XSpi_SetStatusHandler(&Spi, &Spi, (XSpi_StatusHandler)SpiHandler);
	printf("\n\rflash_qspi_rw System_init_startup 5");		
	Status = XSpi_SetOptions(&Spi, XSP_MASTER_OPTION |
				 XSP_MANUAL_SSELECT_OPTION);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	printf("\n\rflash_qspi_rw System_init_startup 6\n");	
	fflush(stdout);
	Status = XSpi_SetSlaveSelect(&Spi, SPI_SELECT);
	if(Status != XST_SUCCESS) {
		printf("\n\rflash_qspi_rw System_init_startup 6.5 %d\n", Status);	
		fflush(stdout);
		return XST_FAILURE;
	}
	printf("\n\rflash_qspi_rw System_init_startup 7\n");	
	fflush(stdout);
	XSpi_Start(&Spi);
	Status = SpiFlashWriteEnable(&Spi);
	printf("\n\rflash_qspi_rw System_init_startup 8\n");	
	fflush(stdout);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	printf("\n\rflash_qspi_rw System_init_startup 9");	
	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* quad flash Blank Check.
*
* @param	None
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int Spi_Blank_Check(u32 StartAddr, u32 NoByteToRead)
{
	int Status;
	int remaind_int, NoOfPage;
	u32 Index,StartOffsetAddr;
	StartOffsetAddr = StartAddr;
	if (qspi_init_flag ==0)
		{
			Status = System_init_startup ();
			if (Status != XST_SUCCESS) {
					} else qspi_init_flag=1;

			}
	NoOfPage = (NoByteToRead/PAGE_SIZE);
	remaind_int = (NoByteToRead - ( NoOfPage * PAGE_SIZE));

	      if (remaind_int != 0) {
	              NoOfPage = (NoOfPage+1);
	            }
	      printf("\n\rPerforming Blank Check Operation...\n\rStart Address\t= 0x%08x\n\rEnd Address\t= 0x%08x\n\rNumber Of Pages\t= 0x%08x\n\r", StartOffsetAddr, NoByteToRead, NoOfPage);
			while (NoOfPage !=0)
					{
						Status = SpiFlashWriteEnable(&Spi);
						if(Status != XST_SUCCESS) {
									}
						for(Index = 0; Index < PAGE_SIZE + READ_WRITE_EXTRA_BYTES; Index++)
							{
									ReadBuffer[Index] = 0x0;
								}
						Status = SpiFlashRead(&Spi, StartOffsetAddr, PAGE_SIZE, COMMAND_4BYTE_QUAD_OUTPUT_FAST_READ);
						//Status = SpiFlashRead(&Spi, StartOffsetAddr, PAGE_SIZE, COMMAND_4BYTE_QUAD_OUTPUT_FAST_READ);
								if(Status != XST_SUCCESS) {
										return XST_FAILURE;
										}
						for(Index = 4; Index < PAGE_SIZE; Index++)
							{
								if(ReadBuffer[Index + READ_WRITE_EXTRA_BYTES + DUAL_READ_DUMMY_BYTES] != 0xFF)
									{
										printf("Blank Check Fail at Address:0x%x = 0x%x\r\n", (StartOffsetAddr + Index), (ReadBuffer[Index + READ_WRITE_EXTRA_BYTES + DUAL_READ_DUMMY_BYTES]));
											return XST_FAILURE;
									} else printf("%c%c%c%c%c%c",95,8,92,8,47,8);
								}
								 NoOfPage--;
								 StartOffsetAddr = (StartOffsetAddr + PAGE_SIZE);
						}
	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* Main function to run quad flash bulk erase.
*
* @param	None
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int qspi_ease_entire_flash(void)
{
	printf("\n qspi_ease_entire_flash \n\r");
	fflush(stdout);

	int Status;

	if (qspi_init_flag ==0)
		{
			Status = System_init_startup ();
			if (Status != XST_SUCCESS) {
						return XST_FAILURE;
					} else qspi_init_flag=1;

			}

		Status = SpiFlashWriteEnable(&Spi);
		if(Status != XST_SUCCESS)
				{
		        	return XST_FAILURE;
				}
		Status = SpiFlashBulkErase(&Spi);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		} else 	printf("\n\rEntire flash erase takes several minutes \n\r \n\rPlease wait .....\n\r");
//		Status = SpiFlashQuadEnable(&Spi);
//		if (Status != XST_SUCCESS) {
//			return XST_FAILURE;
//		}
		printf("\n\rEntire flash erase completed\n\r");
	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* Main function to run quad flash sector erase.
*
* @param	None
*
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int qspi_erase_sector_flash (u32 OfsetAddr, u32 SectCount)
{
	int Status, Count_int=0;
	printf ("\n\r\n\rStart Address:\t"); putnum(OfsetAddr);
	printf ("\r\nEnd Address:\t"); putnum((OfsetAddr + (256 * SectCount)));
	printf("\n\r\n\rPerforming sector erase for Quad SPI, Please wait ...\n\r");
	if (qspi_init_flag ==0)
	{
		Status = System_init_startup ();
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		} else qspi_init_flag=1;
	}

	Status = SpiFlashWriteEnable(&Spi);
	if(Status != XST_SUCCESS) {
		printf("AppErr: SpiFlashWriteEnable Failed\n\r");
		return XST_FAILURE;
	}

	Status = XSpi_SetSlaveSelect(&Spi, SPI_SELECT);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	XSpi_Start(&Spi);
	Status = SpiFlashWriteEnable(&Spi);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	if ( (SectCount < NUMB_SECTORS) || (SectCount == NUMB_SECTORS))
	{

		for (Count_int=0; Count_int< SectCount; Count_int++)
		{
			Status = SpiFlashWriteEnable(&Spi);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			Status = SpiFlashSectorErase(&Spi, OfsetAddr);
			if(Status != XST_SUCCESS) {

				return XST_FAILURE;
			}else
			{
				OfsetAddr = (OfsetAddr + 65536);
			}
		}
		printf("\n\rSector Flash Erase Completed");
	}
	else
	{
		printf ("\n\rEntered sector value = \t0x"); putnum(NUMB_SECTORS);
		printf ("\n\rExceeds the available flash sector 0x200h(512).  Please re-enter a valid sector value =");
	}
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function Read the SPI Flash IDOCDE.
*
* @param	None
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int qspi_flash_geo(void)
{
	printf("qspi_flash_geo 1\n");
	fflush(stdout);
	int Status;

	if (qspi_init_flag ==0)
	{
		printf("qspi_flash_geo 1.1\n");
		fflush(stdout);
		Status = System_init_startup ();
		if (Status != XST_SUCCESS) {
			printf("qspi_flash_geo 1.11\n");
			fflush(stdout);
			return XST_FAILURE;
		} else qspi_init_flag=1;
		printf("qspi_flash_geo 1.2 Status %X\n", Status);
		fflush(stdout);
	}
	printf("qspi_flash_geo 2\n");
	fflush(stdout);
	Status = SpiFlashReadID(&Spi);
	printf("qspi_flash_geo 3 Status %X\n", Status);
	fflush(stdout);
	if(Status != XST_SUCCESS) {
		printf("qspi_flash_geo 3.1 Status %X\n", Status);
		fflush(stdout);
		return XST_FAILURE;
	}
	int FlagStatus = SpiFlashReadflagstatus(&Spi);
	SpiFlashClrflagstatus(&Spi);
	printf("qspi_flash_geo 4 Status 0x%X flagStatus 0x%x\n", Status, FlagStatus);
	fflush(stdout);

	Status = SpiFlashQuadEnable(&Spi);
	if (Status != XST_SUCCESS) {
		printf("qspi_flash_geo 4.1 SpiFlashQuadEnable failed status 0x%X\n", Status);
		return XST_FAILURE;		
	}

//	int regAddr = 0x0B;
//	int regSize = 2;

//	int readData = SpiFlashReadRegister(&Spi, regAddr, regSize);
//	printf("qspi_flash_geo 4.1 Status 0x%X regaddr 0x%X Bytecount %d regData 0x%X\n", Status, regAddr, regSize, readData);
//	fflush(stdout);
//	regSize = 2;
//	readData = SpiFlashReadRegister(&Spi, regAddr, regSize);
//	printf("qspi_flash_geo 4.2 Status 0x%X regaddr 0x%X Bytecount %d regData 0x%X\n", Status, regAddr, regSize, readData);
//	fflush(stdout);
//	regSize = 4;
//	readData = SpiFlashReadRegister(&Spi, regAddr, regSize);
//	printf("qspi_flash_geo 4.3 Status 0x%X regaddr 0x%X Bytecount %d regData 0x%X\n", Status, regAddr, regSize, readData);
//	fflush(stdout);
//	readData = SpiFlashGetStatus(&Spi);
//	printf("qspi_flash_geo 6 flashStatus regData 0x%X\n", readData);
//	fflush(stdout);
	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* This function Read the SPI Flash.
*
* @param	None
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int qspi_read_flash(u32 StartAddr, u32 NoByteToRead)
{
	int Status, CntLine =0;
	u32 Index;
	int remaind_int, NoOfPage=0, NoOfPage_strt=0, NoOfPage_tot=0;
	printf ("* qspi_read_flash 1 startAddr 0x%X ByteToRead 0x%X\n", StartAddr, NoByteToRead);
	if (qspi_init_flag ==0)
		{
			Status = System_init_startup ();
			if (Status != XST_SUCCESS) {
						return XST_FAILURE;
					} else qspi_init_flag=1;

			}
			printf ("\n\r\n\rPerforming Flash Read Operation...\r\n");
			printf ("\n\rFlash Start Address:\t");putnum(StartAddr);
			printf ("\n\rFlash End Address:\t");putnum((NoByteToRead +StartAddr));

			NoOfPage = (NoByteToRead/PAGE_SIZE);
			remaind_int = (NoByteToRead - ( NoOfPage * PAGE_SIZE));
			if (remaind_int != 0) {
				NoOfPage = (NoOfPage+1);
			}
			if ( StartAddr !=0x00000)
			{
				NoOfPage_strt = (StartAddr/PAGE_SIZE);
				remaind_int = (StartAddr -(NoOfPage_strt * PAGE_SIZE));
				if (remaind_int != 0)
				{
					NoOfPage_strt = (NoOfPage_strt+1);
				}
				NoOfPage_tot = NoOfPage_strt + NoOfPage;
			}
			// if ( (StartAddr==0x000000) || (NoOfPage_tot<=NOB_PAGES))
			if ( (StartAddr==0x000000) || (NoOfPage_tot<= (4*NOB_PAGES)))
			{
				printf("\n\rOffset(h):\t0x00\t0x01\t0x02\t0x03\t0x04\t0x05\t0x06\t0x07\n\r");
				CntLine =0;
				Index =0;
				printf("\n\r0x%08x:\t", (StartAddr));

				while  (NoOfPage !=0 )
				{
					// printf ("* qspi_read_flash 2 NoOfPage %d\n", NoOfPage);
					// fflush(stdout);
					Status = SpiFlashWriteEnable(&Spi);
					if(Status != XST_SUCCESS) {
						return XST_FAILURE;
					}
					for(Index = 0; Index < (PAGE_SIZE + READ_WRITE_EXTRA_BYTES); Index++)
					{
						ReadBuffer[Index] = 0x0;
					}
					// printf ("* qspi_read_flash 2.1 NoOfPage %d\n", NoOfPage);
					// fflush(stdout);
					// Status = SpiFlashRead(&Spi, StartAddr, PAGE_SIZE, COMMAND_RANDOM_READ);
					Status = SpiFlashRead(&Spi, StartAddr, PAGE_SIZE, COMMAND_4BYTE_QUAD_OUTPUT_FAST_READ);
					// printf ("* qspi_read_flash 2.2 NoOfPage %d Status 0x%X\n", NoOfPage, Status);
					// fflush(stdout);
					if(Status != XST_SUCCESS) {
						return XST_FAILURE;
					}
					for(Index = 5; Index <= ((PAGE_SIZE + READ_WRITE_EXTRA_BYTES)); Index++)
					{
						printf("0x%02x\t", (ReadBuffer[Index + READ_WRITE_EXTRA_BYTES]));
						if (CntLine > 6)
						{
							CntLine =0;
							printf("\n\r0x%08x:\t", (StartAddr + (Index-4)) );
						} else CntLine++;
					}
					NoOfPage--;
					StartAddr = (StartAddr + PAGE_SIZE);
				}
			}
			else
			{
				printf ("\n\rNumber of Pages Excced the device settings!");
			}

		return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* This function enables writes to the SPI Flash.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int SpiFlashWriteEnable(XSpi *SpiPtr)
{
//	printf("SpiFlashWriteEnable 1\n");		
	int Status;
	Status = SpiFlashWaitForFlashReady();
//	printf("SpiFlashWriteEnable 2\n");
//	fflush(stdout);
	if(Status != XST_SUCCESS) {
//		printf("SpiFlashWriteEnable 2.5\n");		
		return XST_FAILURE;
	} 
//	printf("SpiFlashWriteEnable 3\n");			
//	fflush(stdout);
	WriteBuffer[BYTE1] = COMMAND_WRITE_ENABLE;
	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
				WRITE_ENABLE_BYTES);
//	printf("SpiFlashWriteEnable 4 status %d\n", Status);			
	if(Status != XST_SUCCESS) {
//		printf("SpiFlashWriteEnable 4.5 status %d\n", Status);			
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
//	printf("SpiFlashWriteEnable 5 TransferInProgress %d \n", TransferInProgress);			
	if(ErrorCount != 0) {
		ErrorCount = 0;
//		printf("SpiFlashWriteEnable 5 TransferInProgress %d ErrorCount %d\n", TransferInProgress, ErrorCount);			
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* This function Disable writes to the SPI flash memory.
*
* @param	SpiPtr is a pointer to the instance of the SPI device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int SpiFlashWriteDisable(XSpi *SpiPtr)
{
	int Status;

	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_WRITE_DISABLE;

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
				WRITE_ENABLE_BYTES);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}

	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* This function enables 4-Byte Addressing mode to the SPI Flash memory.
*
* @param	SpiPtr is a pointer to the instance of the SPI device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int SpiFlash4bytemodeEnable(XSpi *SpiPtr)
{
	int Status;

	Status = SpiFlashWriteEnable(&Spi);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*
	 * Wait while the Flash is busy.
	 */
	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Prepare the COMMNAD_ENTER_4BYTE_ADDRESS_MODE.
	 */
	//WriteBuffer[BYTE1] = COMMAND_ENTER_4BYTE_ADDRESS_MODE;	
	WriteBuffer[BYTE1] = 0xB7;	

	/*
	 * Initiate the Transfer.
	 */
	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
				WRITE_ENABLE_BYTES);
//	Status = XSpi_Transfer_SkipCheck(SpiPtr, WriteBuffer, NULL,
//				WRITE_ENABLE_BYTES);

	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}

	/*
	 * Wait till the Transfer is complete and check if there are any errors
	 * in the transaction..
	 */
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function clear flag status register to the SPI Flash memory.
*
* @param	SpiPtr is a pointer to the instance of the SPI device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int SpiFlashclearflagstatus(XSpi *SpiPtr)
{
	int Status;

	/*
	 * Wait while the Flash is busy.
	 */
	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Prepare the COMMAND_CLEAR_FLAG_STATUS.
	 */
	WriteBuffer[BYTE1] = COMMAND_CLEAR_FLAG_STATUS;

	/*
	 * Initiate the Transfer.
	 */
	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
				WRITE_ENABLE_BYTES);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}

	/*
	 * Wait till the Transfer is complete and check if there are any errors
	 * in the transaction..
	 */
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* This function Extended Address Register Bit Definitions the Numonyx Serial Flash memory.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int SpiReadExtendedAddressRegister(XSpi *SpiPtr)
{
	int 	Status;
	Status = SpiFlashWriteEnable(&Spi);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = SpiFlashWaitForFlashReady();
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

	WriteBuffer[BYTE1] = COMMAND_READ_EXTENDED_ADDRESS;
	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, ReadBuffer,
			READ_WRITE_EXTRA_BYTES);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
		}
		//printf ("\n\rCOMMAND_READ_EXTENDED_ADDRESS"); putnum(ReadBuffer[1]);
	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* This function reads the flash status register of the Numonyx Flash.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		The status register content is stored at the second byte
*		pointed by the ReadBuffer.
*
******************************************************************************/
int SpiFlashReadflagstatus(XSpi *SpiPtr)
{
	int 	Status;
	Status = SpiFlashWriteEnable(&Spi);
	if(Status != XST_SUCCESS) {
			return XST_FAILURE;
	}
	Status = SpiFlashWaitForFlashReady();
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

	WriteBuffer[BYTE1] = COMMAND_READ_FLAG_STATUS;
	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, ReadBuffer,
			READ_WRITE_EXTRA_BYTES);
			
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
		}
		printf ("\n\rCOMMAND_READ_FLAG_STATUS: 0x%X\n", ReadBuffer[1]);
	return XST_SUCCESS;
}

int SpiFlashClrflagstatus(XSpi *SpiPtr)
{
	printf ("SpiFlashClrflagstatus\n");
	int 	Status;
	Status = SpiFlashWaitForFlashReady();
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

	WriteBuffer[BYTE1] = COMMAND_CLEAR_FLAG_STATUS;
	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL, 1);
			
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
		}		
	return XST_SUCCESS;
}

int SpiFlashReadRegister(XSpi *SpiPtr, unsigned int addRegister, unsigned int byteCount)
{
	int 	Status;
	Status = SpiFlashWriteEnable(&Spi);
	if(Status != XST_SUCCESS) {
			return XST_FAILURE;
	}
	Status = SpiFlashWaitForFlashReady();
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

	WriteBuffer[BYTE1] = addRegister;
	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, ReadBuffer,
			byteCount);
			
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
		}
		printf ("\n\rCOMMAND_READ address 0x%X, data 0x%X\n", addRegister, putnum(ReadBuffer[1]));
	return XST_SUCCESS;
}

int SpiFlashReadRegisterNoWriteEnable(XSpi *SpiPtr, unsigned int addRegister, unsigned int byteCount)
{
	int 	Status;
/*	Status = SpiFlashWriteEnable(&Spi);
	if(Status != XST_SUCCESS) {
			return XST_FAILURE;
	}
*/

	WriteBuffer[BYTE1] = addRegister;
	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, ReadBuffer,
			byteCount);

			
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
		}
		printf ("\n\rCOMMAND_READ address 0x%X, data 0x%X\n", addRegister, putnum(ReadBuffer[1]));
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function writes the data to the specified locations in the Numonyx Serial
* Flash memory.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
* @param	Addr is the address in the Buffer, where to write the data.
* @param	ByteCount is the number of bytes to be written.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int SpiFlashWrite_File(XSpi *SpiPtr, u32 Addr, u32 ByteCount, u8 WriteCmd, unsigned char* pageBuffer)
{
	u32 Index;
	int Status;
	u32 ddrvector = 0;
	// unsigned char *DDR_MEMB1 = (unsigned char *)DDR_ADDR0;
	Status = SpiFlash4bytemodeEnable(&Spi);
				if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
		Status = SpiFlashWriteEnable(&Spi);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
		}

	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	WriteBuffer[BYTE1] = WriteCmd;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;

	printf(" ** SpiFlashWrite_file 1 Addr 0x%X bytecount %d", Addr, ByteCount);
	// printf("Here is the pageBuffer:n\n");
	// for (int i = 0; i < PAGE_SIZE; i++) //tli back
	// for (int i = 0; i < 16; i++) //tli back
	// {
    //	printf("%02X", pageBuffer[i]);
	// }
	// printf(" end pageBuffer:n\n");	

	// fflush(stdout);
	for(Index = 5; Index < (ByteCount + READ_WRITE_EXTRA_BYTES +1); Index++, ddrvector++) {
			WriteBuffer[Index] = pageBuffer[ddrvector];
	}

	printf("Here is the writeBuffer:n\n");
	// for (int i = 0; i < 256; i++)
	for (int i = 0; i < (ByteCount + READ_WRITE_EXTRA_BYTES +1); i++) //tli back
	{
    	printf("%02X", WriteBuffer[i]);
	}
	printf(" end WriteBuffer:n\n");	
	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
				(ByteCount + READ_WRITE_EXTRA_BYTES +1));
	if(Status != XST_SUCCESS) {
		printf(" ** SpiFlashWrite_file 1.1 failed Status 0x%X", Status);
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}
	printf(" ** SpiFlashWrite_file 2", Status);
	Status = SpiFlash4byteexit(&Spi);
		if (Status != XST_SUCCESS) {
			printf(" ** SpiFlashWrite_file 2.1 failed Status 0x%X", Status);
				return XST_FAILURE;
		}
	printf(" ** SpiFlashWrite_file 3", Status);		
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function reads the data from the Numonyx Serial Flash Memory
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
* @param	Addr is the starting address in the Flash Memory from which the
*		data is to be read.
* @param	ByteCount is the number of bytes to be read.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None
*
******************************************************************************/
int SpiFlashRead(XSpi *SpiPtr, u32 Addr, u32 ByteCount, u8 ReadCmd)
{
	printf("* SpiFlashRead 1 Addr 0x%X ByteCount 0x%X command 0x%X\n", Addr, ByteCount, ReadCmd);
	fflush(stdout);
	int Status;
	Status = SpiFlash4bytemodeEnable(&Spi);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	printf("* SpiFlashRead 2 Status 0x%X", Status);
	fflush(stdout);
	Status = SpiFlashWriteEnable(&Spi);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	printf("* SpiFlashRead 3 Status 0x%X", Status);
	fflush(stdout);

	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	printf("* SpiFlashRead 4 Status 0x%X", Status);
	fflush(stdout);	
	WriteBuffer[BYTE1] = ReadCmd;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;

	if (ReadCmd == COMMAND_DUAL_READ) {
		ByteCount += DUAL_READ_DUMMY_BYTES;
	} else if (ReadCmd == COMMAND_DUAL_IO_READ) {
		ByteCount += DUAL_READ_DUMMY_BYTES;
	} else if (ReadCmd == COMMAND_QUAD_IO_READ) {
		ByteCount += QUAD_IO_READ_DUMMY_BYTES;
	} else if (ReadCmd==COMMAND_QUAD_READ) {
		ByteCount += QUAD_READ_DUMMY_BYTES;
	}else if (ReadCmd==COMMAND_4BYTE_QUAD_OUTPUT_FAST_READ) {
		ByteCount += QUAD_IO_READ_DUMMY_BYTES;
	}

	printf("* SpiFlashRead 5 WriteBuffer 0x%X %X %X %X %X ByteCount %d", WriteBuffer[BYTE1], WriteBuffer[BYTE2], WriteBuffer[BYTE3], WriteBuffer[BYTE4], WriteBuffer[BYTE5], ByteCount);
	fflush(stdout);	 

	TransferInProgress = TRUE;
	Status = XSpi_Transfer( SpiPtr, WriteBuffer, ReadBuffer,
				(ByteCount + READ_WRITE_EXTRA_BYTES));
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}
	printf("* SpiFlashRead 6\n");
	fflush(stdout);
	Status = SpiFlash4byteexit(&Spi);
			if (Status != XST_SUCCESS) {
					return XST_FAILURE;
				}
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function erases the entire contents of the Numonyx Serial Flash device.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		The erased bytes will read as 0xFF.
*
******************************************************************************/
int SpiFlashBulkErase(XSpi *SpiPtr)
{
	int Status;
	printf("* SpiFlashBulkErase 1\n");
	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	WriteBuffer[BYTE1] = COMMAND_BULK_ERASE;

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
						BULK_ERASE_BYTES);
	printf("* SpiFlashBulkErase 2\n");
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}
	printf("* SpiFlashBulkErase 3\n");
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function erases the contents of the specified Sector in the Numonyx
* Serial Flash device.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
* @param	Addr is the address within a sector of the Buffer, which is to
*		be erased.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		The erased bytes will be read back as 0xFF.
*
******************************************************************************/
int SpiFlashSectorErase(XSpi *SpiPtr, u32 Addr)
{
	int Status;
	printf("* SpiFlashSectorErase 1\n");
	Status = SpiFlash4bytemodeEnable(&Spi);
		if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	printf("* SpiFlashSectorErase 2\n");
	Status = SpiReadExtendedAddressRegister(&Spi);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	printf("* SpiFlashSectorErase 3\n");		
	Status = SpiFlashWriteEnable(&Spi);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_SECTOR_ERASE;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;
	/*	
	WriteBuffer[BYTE1] = COMMAND_WRITE_EXTENDED_ADDRESS;
	WriteBuffer[BYTE2] = (u8) (0xFF);
	WriteBuffer[BYTE3] = COMMAND_SECTOR_ERASE;		//COMMAND_SECTOR_ERASE;
	WriteBuffer[BYTE4] = (u8) (Addr >> 24);
	WriteBuffer[BYTE5] = (u8) (Addr >> 16);
	WriteBuffer[BYTE6] = (u8) (Addr >> 8);
	WriteBuffer[BYTE7] = (u8) (Addr);
	*/

	TransferInProgress = TRUE;
//	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
//					SECTOR_ERASE_BYTES+3);
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
					SECTOR_ERASE_BYTES+1);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}
	printf("* SpiFlashSectorErase 4\n");			
	return XST_SUCCESS;
}

int SpiFlashDieErase(XSpi *SpiPtr, u32 uscDieNr)
{
	int Status;
	u32 Addr = uscDieNr << 26;
	printf("* SpiFlashDieErase 1\n");
	Status = SpiFlash4bytemodeEnable(&Spi);
		if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	printf("* SpiFlashDieErase 2\n");
	Status = SpiReadExtendedAddressRegister(&Spi);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	printf("* SpiFlashDieErase 3\n");		
	Status = SpiFlashWriteEnable(&Spi);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	WriteBuffer[BYTE1] = COMMAND_DIE_ERASE;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;
	/*	
	WriteBuffer[BYTE1] = COMMAND_WRITE_EXTENDED_ADDRESS;
	WriteBuffer[BYTE2] = (u8) (0xFF);
	WriteBuffer[BYTE3] = COMMAND_SECTOR_ERASE;		//COMMAND_SECTOR_ERASE;
	WriteBuffer[BYTE4] = (u8) (Addr >> 24);
	WriteBuffer[BYTE5] = (u8) (Addr >> 16);
	WriteBuffer[BYTE6] = (u8) (Addr >> 8);
	WriteBuffer[BYTE7] = (u8) (Addr);
	*/

	TransferInProgress = TRUE;
//	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
//					SECTOR_ERASE_BYTES+3);
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
					SECTOR_ERASE_BYTES+1);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}
	printf("* SpiFlashDieErase 4\n");			
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function sets the QuadEnable bit in Winbond/Micron flash.
*
* @param	None
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None.
*
******************************************************************************/
int SpiFlashQuadEnable(XSpi *SpiPtr)
{
	int Status;
	Status = SpiFlashWriteEnable(SpiPtr);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	WriteBuffer[BYTE1] = 0x01;
	WriteBuffer[BYTE2] = 0x01;
	WriteBuffer[BYTE3] = 0x01; /* QE = 1 */
	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL, 3);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}
	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	WriteBuffer[BYTE1] = COMMAND_ENTER_QUAD_MODE; //0x35;
	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, ReadBuffer,
						STATUS_READ_BYTES);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* This function sets the QuadEnable bit in Micron flash.
*
* @param	None
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None.
*
******************************************************************************/
int SpiFlash4byteexit(XSpi *SpiPtr)
{
	int Status;

	Status = SpiFlashWriteEnable(&Spi);
	if(Status != XST_SUCCESS) {
			return XST_FAILURE;
	}
		/*
		 * Wait while the Flash is busy.
		 */
		Status = SpiFlashWaitForFlashReady();
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Prepare the WriteBuffer.
		 */
		WriteBuffer[BYTE1] = COMMAND_EXIT_4BYTE_ADDRESS_MODE;

		/*
		 * Initiate the Transfer.
		 */
		TransferInProgress = TRUE;
		Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
					WRITE_ENABLE_BYTES);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		} else {
			TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
		}

		/*
		 * Wait till the Transfer is complete and check if there are any errors
		 * in the transaction..
		 */
		while(TransferInProgress);
		if(ErrorCount != 0) {
			ErrorCount = 0;
			return XST_FAILURE;
		}
		return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* This function reads the Flash ID register of the Numonyx Flash.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		The status register content is stored at the second byte
*		pointed by the ReadBuffer.
*
******************************************************************************/
int SpiFlashReadID(XSpi *SpiPtr)
{
	printf("SpiFlashReadID 1\n");
	fflush(stdout);
	int 	Status;
	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	printf("SpiFlashReadID 2 Status %X\n", Status);
	fflush(stdout);
	// WriteBuffer[BYTE1] = COMMAND_READ_ID;
	WriteBuffer[BYTE1] = 0x9F;
	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, ReadBuffer,
			READ_WRITE_EXTRA_BYTES);
	printf("SpiFlashReadID 3 Status %X\n", Status);			
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}
	
	printf("SpiFlashReadID 4\n");				
	fflush(stdout);
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}
	printf("SpiFlashReadI 5\n");				
	fflush(stdout);	
		 if ( (ReadBuffer[1] == 0x20))
		 {
			 printf("\n\rManufacturer ID:\t0x%x\t:= MICRON\n\r", ReadBuffer[1]);
			 if ( (ReadBuffer[2] == 0xBA))
			 {
				 printf("Memory Type:\t\t0x%x\t:= N25Q 3V0\n\r", ReadBuffer[2]);
			 }
			 else
			 {
				 if ((ReadBuffer[2] == 0xBB)) 
				 	 {
					 	 printf("Memory Type:\t\t0x%x\t:= N25Q 1V8\n\r", ReadBuffer[2]);
				 	 } else printf("Memory Type:\t\t0x%x\t:= QSPI Data\n\r", ReadBuffer[2]);
			 }
			 if ((ReadBuffer[3] == 0x18))
			 	 {
				 printf("Memory Capacity:\t0x%x\t:=\t128Mbit\n\r", ReadBuffer[3]);
			 	 }
			 	 else if ( (ReadBuffer[3] == 0x19))
			 	 	 {
			 		 	 printf("Memory Capacity:\t0x%x\t:= 256Mbit\n\r", ReadBuffer[3]);
			 	 	 }
			 	 	 else if ((ReadBuffer[3] == 0x20))
			 	 	 	 {
				 	 	 	 printf("Memory Capacity:\t0x%x\t:= 512Mbit\n\r", ReadBuffer[3]);
			 			 }
			 	 	 	 else if ((ReadBuffer[3] == 0x21))
			 			 	 {
			 	 	 		 	 printf("Memory Capacity:\t0x%x\t:= 1024Mbit\n\r", ReadBuffer[3]);
			 			 	 	 }
		 }
		 else if ((ReadBuffer[1] == 0x01))
		 {
			 printf("\n\rManufacturer ID: \tSPANSION\n\r");
			 if ((ReadBuffer[3] == 0x18))
			  	 {
			 	 	 printf("Memory Capacity\t=\t256Mbit\n\r");
			  	 }
			  	 else if ((ReadBuffer[3] == 0x19))
			  	 	 {
			  	 	 	 printf("Memory Capacity\t=\t512Mbit\n\r");
			 		 }
			  	 	 else if ((ReadBuffer[3] == 0x20))
				 		{
			  	 	 	 	 printf("Memory Capacity\t=\t1024Mbit\n\r");

				 			 	 }
		 }
		 else if ((ReadBuffer[1] == 0xEF))
		 		 {
		 			 printf("\n\rManufacturer ID\t=\tWINBOND\n\r");
		 			 if ((ReadBuffer[3] == 0x18))
		 			  	 {
		 			 	 	 printf("Memory Capacity\t=\t128Mbit\n\r");
		 			  	 }
		 }

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function reads the Status register of the Numonyx Flash.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		The status register content is stored at the second byte
*		pointed by the ReadBuffer.
*
******************************************************************************/
int SpiFlashGetStatus(XSpi *SpiPtr)
{
//	printf("SpiFlashGetStatus 1\n");
//	fflush(stdout);
	int Status;

	WriteBuffer[BYTE1] = COMMAND_STATUSREG_READ;

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, ReadBuffer,
						STATUS_READ_BYTES);
//	printf("SpiFlashGetStatus 2 Status %X TransferInProgress %d\n", Status, TransferInProgress);
//	fflush(stdout);
	if(Status != XST_SUCCESS) {
//		printf("SpiFlashGetStatus 2.1 Status failed %X\n", Status);
//		fflush(stdout);
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}

	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
//		printf("SpiFlashGetStatus 2.2 ErrorCount %X failed TransferInProgress %d\n", ErrorCount, TransferInProgress);
//		fflush(stdout);
		return XST_FAILURE;
	}

//	printf("SpiFlashGetStatus 3\n ");
//	fflush(stdout);
	return XST_SUCCESS;
}

int SpiFlashGetStatusPrint(XSpi *SpiPtr)
{
//	printf("SpiFlashGetStatus 1\n");
//	fflush(stdout);
	int Status;

	WriteBuffer[BYTE1] = COMMAND_STATUSREG_READ;

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, ReadBuffer,
						STATUS_READ_BYTES);
//	printf("SpiFlashGetStatus 2 Status %X TransferInProgress %d\n", Status, TransferInProgress);
//	fflush(stdout);
	if(Status != XST_SUCCESS) {
//		printf("SpiFlashGetStatus 2.1 Status failed %X\n", Status);
//		fflush(stdout);
		return XST_FAILURE;
	} else {
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
	}

	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
//		printf("SpiFlashGetStatus 2.2 ErrorCount %X failed TransferInProgress %d\n", ErrorCount, TransferInProgress);
//		fflush(stdout);
		return XST_FAILURE;
	} else
	{
		printf ("\n\COMMAND_STATUSREG_READ: 0x%X\n", ReadBuffer[1]);
	}
	

//	printf("SpiFlashGetStatus 3\n ");
//	fflush(stdout);
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function waits till the Numonyx serial Flash is ready to accept next
* command.
*
* @param	None
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		This function reads the status register of the Buffer and waits
*.		till the WIP bit of the status register becomes 0.
*
******************************************************************************/
int SpiFlashWaitForFlashReady(void)
{
//	printf("SpiFlashWaitForFlashReady 1\n");
//	fflush(stdout);
	int Status;
	u8 StatusReg;

	while(1) {

		Status = SpiFlashGetStatus(&Spi);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		StatusReg = ReadBuffer[1];
		if((StatusReg & FLASH_SR_IS_READY_MASK) == 0) {
			break;
		} else printf("%c%c%c%c%c%c",95,8,92,8,47,8);

	}
 return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function is the handler which performs processing for the SPI driver.
* It is called from an interrupt context such that the amount of processing
* performed should be minimized. It is called when a transfer of SPI data
* completes or an error occurs.
*
* This handler provides an example of how to handle SPI interrupts and
* is application specific.
*
* @param	CallBackRef is the upper layer callback reference passed back
*		when the callback function is invoked.
* @param	StatusEvent is the event that just occurred.
* @param	ByteCount is the number of bytes transferred up until the event
*		occurred.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void SpiHandler(void *CallBackRef, u32 StatusEvent, unsigned int ByteCount)
{
	TransferInProgress = FALSE;
	if (StatusEvent != XST_SPI_TRANSFER_DONE) {
		ErrorCount++;
	}
}

/*****************************************************************************/
/**
*
* This function setups the interrupt system such that interrupts can occur
* for the Spi device. This function is application specific since the actual
* system may or may not have an interrupt controller. The Spi device could be
* directly connected to a processor without an interrupt controller.  The
* user should modify this function to fit the application.
*
* @param	SpiPtr is a pointer to the instance of the Spi device.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None
*
******************************************************************************/
static int SetupInterruptSystem(XSpi *SpiPtr)
{

/*	int Status;

	Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = XIntc_Connect(&InterruptController,
				SPI_INTR_ID,
				(XInterruptHandler)XSpi_InterruptHandler,
				(void *)SpiPtr);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	XIntc_Enable(&InterruptController, SPI_INTR_ID);

	// Xil_ExceptionInit();

	// Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
	// 			(Xil_ExceptionHandler)INTC_HANDLER,
	// 			&InterruptController);
	// Xil_ExceptionEnable();
*/
	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* This Function Download a file via serial port
*
* @param	File Data will be stored in DDR memory
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE or error messages.
*
* @note		None
*
******************************************************************************/
static int  DownloadSerialDataToQSPIFlash(u32 StartDie)
	{
		  printf ("\r\nProgramming QSPI flash Start");
		  qspi_program_flash(StartDie);
		  printf ("\r\nProgramming QSPI flash end");
		  return XST_SUCCESS;
	}

static int  VerifySerialDataToQSPIFlash(u32 StartDie)
	{
		printf ("\r\nVerify QSPI flash Start\n");
		fflush(stdout);

		if( qspi_verify_flash(StartDie) == XST_FAILURE ){
			printf ("\r\nVerify QSPI flash end\n");	
			fflush(stdout);
			return XST_FAILURE ;
		};
		printf ("\r\nVerify QSPI flash SUCCEEDED\n");
		fflush(stdout);
		return XST_SUCCESS;
	}	
/*****************************************************************************/
/**
*
* This Function Program the SPI flash with DDR data received from File via serial port
*
* @param	File Data will be stored in DDR memory
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE or error messages.
*
* @note		None
*
******************************************************************************/
int qspi_program_flash(u32 StartDie)
{
		int quq_int, remaind_int, NoOfSector, NoOfPage;
		FILE* fp;
		uint64_t FileByteCount;
		u32 baseStartAddr = StartDie << 26;
		u32 StartAddr = 0;
		// printf("\r\n\tWaiting for the file to be sent from TeraTerm...\n\r");
		//  FileByteCount = TeraTermFile_Receive(StartAddr, NoByteToRead);

		char* filename = "/home/ixiaadmin/Downloads/ant4x400g.bin";
		printf("\nqspi_program_flash 1\n");					
		fflush(stdout);

    	// if((filed = open(filename, "O_RDONLY | O_SYNC")) == -1)		
    	if ((fp = fopen(filename, "r")) == NULL) {
        	printf("Fpga Download open failed %s.\n", filename);			
			fflush(stdout);
        	return XST_FAILURE;
    	}
	    printf("Fpga Download %s.\n", filename);		
		fflush(stdout);
		
		// FileByteCount = fileSize(fp);	    	
		fseek(fp, 0, SEEK_END);
		FileByteCount = ftell(fp);
		printf("\r\nFpga Download TotalByteRecived =\t"); putnum(FileByteCount);
		printf ("\r\nFpga Download FlashAddress Offset = \t"); putnum(baseStartAddr);
	    fflush(stdout);
		

		NoOfSector = (FileByteCount/BYTE_PER_SECTOR);
		NoOfPage = (FileByteCount/PAGE_SIZE);
		// NoOfPage = 1;

		quq_int = (FileByteCount / BYTE_PER_SECTOR);
		remaind_int = (FileByteCount - (quq_int * BYTE_PER_SECTOR));

		if (remaind_int != 0) {
				  NoOfSector = (NoOfSector +1);
		}
		quq_int = (FileByteCount / PAGE_SIZE);
		remaind_int = (FileByteCount - ( quq_int * PAGE_SIZE));

		if (remaind_int != 0) {
			NoOfPage = (NoOfPage+1);
		}
		// NoOfPage = 1;

		printf ("\r\nNoOfSector= "); putnum(NoOfSector);
		printf ("\r\nNoOfPage= "); putnum(NoOfPage);
		fflush(stdout);

		// u32 ddrvector=fd;
		int Status;
		if (qspi_init_flag ==0)
		{
			Status = System_init_startup ();
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			} else qspi_init_flag=1;
		}
		Status = XSpi_SetSlaveSelect(&Spi, SPI_SELECT);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		XSpi_Start(&Spi);
		Status = SpiFlashWriteEnable(&Spi);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		// char* pageBuffer = buffer;
		printf ("*qspi_program_flash 3 NoOfPage %d StartAddr 0x%X\n", NoOfPage, baseStartAddr);
		fflush(stdout);

		/*
		while (NoOfPage !=0)
		{
			printf ("*qspi_program_flash 3.1 NoOfPage %d\n", NoOfPage);
			fflush(stdout);
			fseek(fp, StartAddr, SEEK_SET);
			printf ("*qspi_program_flash 3.11 NoOfPage %d\n", NoOfPage);
			fflush(stdout);
			unsigned char *buffer;	
			uint32_t progAddress = StartAddr;
			int sumStatus;
			buffer = (unsigned char *)malloc((16 + 2) * sizeof(unsigned char)); // Enough memory for file + \0
			for(int i = 0; i< 16+2; i++)
			{
				char hex[5];
				sprintf(hex, "%x", i);
				buffer[i] = hex;
			}
			
			printf ("*qspi_program_flash 3.12 NoOfPage %d\n", NoOfPage);
			fflush(stdout);
			
			for( int i = 0; i < 16; i++)			
			{
				progAddress = StartAddr + i*16;
				fseek(fp, progAddress, SEEK_SET);
				// fread(buffer, 1, 16, fp); 
				printf("Here is the message: progAddress 0x%X\n", progAddress);
				for (int i = 0; i < 16; i++)
				{
	    			printf("%02X", buffer[i]);
				}
				printf(" end message:n\n");
				printf ("*qspi_program_flash 3.2 NoOfPage %d StartAddr 0x%X\n", NoOfPage, StartAddr);
				fflush(stdout);
				Status = SpiFlashWriteEnable(&Spi);
				if(Status != XST_SUCCESS) {
					sumStatus |= XST_FAILURE;
				}
				Status = SpiFlashWrite_File(&Spi, progAddress, 16, COMMAND_4BYTE_PAGE_PROGRAM, buffer);
				printf ("*qspi_program_flash 3.3 NoOfPage %d", NoOfPage);
				fflush(stdout);
				if(Status != XST_SUCCESS) {
					sumStatus |= XST_FAILURE;
				} 
			}
			if(sumStatus != XST_SUCCESS) {
				return XST_FAILURE;
			} 
			else {
				NoOfPage--;
				StartAddr = (StartAddr + PAGE_SIZE);
					// pageBuffer = (pageBuffer + PAGE_SIZE);
			}

		}
		*/		

		while (NoOfPage !=0)
		{
			printf ("*qspi_program_flash 3.1 NoOfPage %d\n", NoOfPage);
			fflush(stdout);
			fseek(fp, StartAddr, SEEK_SET);
			printf ("*qspi_program_flash 3.11 NoOfPage %d\n", NoOfPage);
			fflush(stdout);
			unsigned char *buffer;	
			buffer = (unsigned char *)malloc((PAGE_SIZE + 10) * sizeof(unsigned char)); // Enough memory for file + \0
			printf ("*qspi_program_flash 3.12 NoOfPage %d\n", NoOfPage);
			fflush(stdout);
		
			fread(buffer, 1, PAGE_SIZE, fp); // Read in the entire file
			/*for(int i = 0; i< 16+2; i++)
			{
				char hex[5];
				sprintf(hex, "%x", i);
				buffer[i] = hex;
			}*/	

			printf("Here is the message:n\n");
			for (int i = 0; i < PAGE_SIZE; i++)
			{
    			printf("%02X", buffer[i]);
			}
			printf(" end message:n\n");
			printf ("*qspi_program_flash 3.2 NoOfPage %d StartAddr 0x%X\n", NoOfPage, StartAddr);
			fflush(stdout);
			Status = SpiFlashWriteEnable(&Spi);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			Status = SpiFlashWrite_File(&Spi, StartAddr + baseStartAddr, PAGE_SIZE, COMMAND_4BYTE_PAGE_PROGRAM, buffer);
			printf ("*qspi_program_flash 3.3 NoOfPage %d", NoOfPage);
			fflush(stdout);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			} else
			{
				NoOfPage--;
				StartAddr = (StartAddr + PAGE_SIZE);
				// pageBuffer = (pageBuffer + PAGE_SIZE);
			}
		}

		fclose(fp); // Close the file
		return XST_SUCCESS;
}

int qspi_verify_flash(u32 StartDie)
{
		int quq_int, remaind_int, NoOfSector, NoOfPage, Index;
		FILE* fp;
		uint64_t FileByteCount;
		u32 baseStartAddr = StartDie << 26;
		u32 StartAddr = 0;

		char* filename = "/home/ixiaadmin/Downloads/ant4x400g.bin";
		printf("\nqspi_verify_flash 1\n");					
		fflush(stdout);

    	// if((filed = open(filename, "O_RDONLY | O_SYNC")) == -1)		
    	if ((fp = fopen(filename, "r")) == NULL) {
        	printf("Fpga Verify open failed %s.\n", filename);			
			fflush(stdout);
        	return XST_FAILURE;
    	}
	    printf("Fpga Verify %s.\n", filename);		
		fflush(stdout);
		
		// FileByteCount = fileSize(fp);	    	
		fseek(fp, 0, SEEK_END);
		FileByteCount = ftell(fp);
		printf("\r\nFpga Verify TotalByteRecived =\t"); putnum(FileByteCount);
		printf ("\r\nFpga Verify FlashAddress Offset = \t"); putnum(baseStartAddr);
	    fflush(stdout);
		

		NoOfSector = (FileByteCount/BYTE_PER_SECTOR);
		NoOfPage = (FileByteCount/PAGE_SIZE);


		quq_int = (FileByteCount / BYTE_PER_SECTOR);
		remaind_int = (FileByteCount - (quq_int * BYTE_PER_SECTOR));

		if (remaind_int != 0) {
				  NoOfSector = (NoOfSector +1);
		}
		quq_int = (FileByteCount / PAGE_SIZE);
		remaind_int = (FileByteCount - ( quq_int * PAGE_SIZE));

		if (remaind_int != 0) {
			NoOfPage = (NoOfPage+1);
		}

		// NoOfPage = 10; //back debug

		printf ("\r\nNoOfSector= "); putnum(NoOfSector);
		printf ("\r\nNoOfPage= "); putnum(NoOfPage);
		fflush(stdout);

		// u32 ddrvector=fd;
		int Status;
		if (qspi_init_flag ==0)
		{
			Status = System_init_startup ();
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			} else qspi_init_flag=1;
		}
		Status = XSpi_SetSlaveSelect(&Spi, SPI_SELECT);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		XSpi_Start(&Spi);
		Status = SpiFlashWriteEnable(&Spi);
		if(Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		// char* pageBuffer = buffer;
		printf ("*qspi_verify_flash 3 NoOfPage %d StartAddr 0x%X\n", NoOfPage, baseStartAddr);
		fflush(stdout);
	

		while (NoOfPage !=0)
		{
			printf ("*qspi_verify_flash 3.1 NoOfPage %d\n", NoOfPage);
			fflush(stdout);
			fseek(fp, StartAddr, SEEK_SET);
			printf ("*qspi_verify_flash 3.11 NoOfPage %d\n", NoOfPage);
			fflush(stdout);
			unsigned char *buffer;	
			buffer = (unsigned char *)malloc((PAGE_SIZE + 10) * sizeof(unsigned char)); // Enough memory for file + \0
			printf ("*qspi_verify_flash 3.12 NoOfPage %d\n", NoOfPage);
			fflush(stdout);
		
			fread(buffer, 1, PAGE_SIZE, fp); // Read in the entire file

			printf("Here is the message:n\n");
			for (int i = 0; i < PAGE_SIZE; i++)
			{
    			printf("%02X", buffer[i]);
			}
			printf(" end message:n\n");
			printf ("*qspi_verify_flash 3.2 NoOfPage %d StartAddr 0x%X\n", NoOfPage, StartAddr);
			fflush(stdout);
			Status = SpiFlashWriteEnable(&Spi);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			// Status = SpiFlashWrite_File(&Spi, StartAddr, PAGE_SIZE, COMMAND_4BYTE_PAGE_PROGRAM, buffer);
			for(Index = 0; Index < (PAGE_SIZE + READ_WRITE_EXTRA_BYTES); Index++)
			{
				ReadBuffer[Index] = 0x0;
			}
			Status = SpiFlashRead(&Spi, baseStartAddr + StartAddr, PAGE_SIZE, COMMAND_4BYTE_QUAD_OUTPUT_FAST_READ);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			// printf("Here is the Read Buffer:n\n");
			int isequal = 1;
			for(Index = 5; Index <= (PAGE_SIZE + READ_WRITE_EXTRA_BYTES); Index++)
			{
				if(ReadBuffer[Index + READ_WRITE_EXTRA_BYTES] != buffer[Index - 5])
				{
					isequal = 0;
				}
				// printf("%02X", ReadBuffer[Index + READ_WRITE_EXTRA_BYTES]);
			}
			// printf(" end Read Buffer dump:n\n");			
			// int n = memcmp(buffer, ReadBuffer + 5, PAGE_SIZE);
  			if (isequal == 1)
			{
			   printf ("Page Start Addr 0x%X matched\n",StartAddr);
			   fflush(stdout);			   
			} else
			{
				printf ("** Page Start Addr 0x%X do not matched\n",StartAddr);
				fflush(stdout);	
				return XST_FAILURE;
			}
			
		
			printf ("*qspi_verify_flash 3.3 NoOfPage %d", NoOfPage);
			fflush(stdout);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			} else
			{
				NoOfPage--;
				StartAddr = (StartAddr + PAGE_SIZE);
				// pageBuffer = (pageBuffer + PAGE_SIZE);
			}
		}

		fclose(fp); // Close the file
		return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This Function receive File via serial port with terminal s/w
*
* @param	File Data will be stored in DDR memory
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE or error messages.
*
* @note		None
*
******************************************************************************/
static int TeraTermFile_Receive ( u32 StartAddr, u32 NoByteToRead)
{
   unsigned char *DDR_MEMB1 = (unsigned char* )DDR_ADDR0;
   FileByteCount=0;
  	  while (FileByteCount < NoByteToRead)
			{
  		  	  DDR_MEMB1[FileByteCount] = inbyte();
  			  FileByteCount++;
			}
  return FileByteCount;
}

/*****************************************************************************/
void enable_caches()
{
//    Xil_ICacheEnable();
// #ifdef XPAR_MICROBLAZE_USE_DCACHE
//     Xil_DCacheEnable();
// #endif
}

void disable_caches()
{
//    Xil_DCacheDisable();
//    Xil_ICacheDisable();
}

void init_uart()
{
//    XUartNs550_SetBaud(UART_BASEADDR, UART_CLOCK_HZ, UART_BAUDRATE);
//    XUartNs550_SetLineControlReg(STDOUT_BASEADDR, XUN_LCR_8_DATA_BITS);
}

void init_platform()
{
    // enable_caches();
    init_uart();
}

void cleanup_platform()
{
    // disable_caches();
}
/*****************************************************************************/
