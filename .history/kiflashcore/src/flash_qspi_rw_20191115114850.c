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
#include "flash_qspi_rw.h"
#include "xparameters.h"	/* defines XPAR values */
// #include "xuartns550_l.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xspi.h"		/* SPI device driver */
// #include "xil_exception.h"
#include "xintc.h"
#include <unistd.h>
#include <limits.h>
#include "pcimem.h"
#include <ctype.h>
#include <xil_types.h>
#include "xil_cache.h"
#include <fcntl.h>
#include <time.h>

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
#define COMMAND_READ_ID								0x9F /* READ ID 9E/9Fh  */
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
// #define NUMB_SECTORS				512
#define NUMB_SECTORS				512 * 8
#define	BYTE_PER_SECTOR				65536
// #define	NUMB_SUBSECTORS				8192
// #define	NUMB_SUBSECTORS				8192 * 2
// #define	BYTE_PER_SUBSECTOR			4096
// #define NOB_PAGES					131072
#define NOB_PAGES 					262144
/*
 * Address of the page to perform Erase, Write and Read operations.
 */
#define FLASH_TEST_ADDRESS0			0x00000000
#define FLASH_TEST_ADDRESS1			0x00F50000
#define BINFILESIZE					2242764

#define FLASH_GOLDEN_IMAGE_ADDRESS 	0x00000000
#define FLASH_TIMER1_IMAGE_ADDRESS 	0x07FFFC00
#define FLASH_USER_IMAGE_ADDRESS 	0x08000000
#define FLASH_TIMER2_IMAGE_ADDRESS 	0x0FFC0000

#define FLASH_GOLDEN_ERASE_START_ADDRESS 	0x00000000
#define FLASH_GOLDEN_ERASE_END_ADDRESS    	0x07FE0000
#define FLASH_TIMER1_ERASE_START_ADDRESS 	0x07FE0000
#define FLASH_TIMER1_ERASE_END_ADDRESS 	    0x08000000
#define FLASH_USER_ERASE_START_ADDRESS 		0x08000000
#define FLASH_USER_ERASE_END_ADDRESS 		0x0FFC0000
#define FLASH_TIMER2_ERASE_START_ADDRESS 	0x0FFC0000
#define FLASH_TIMER2_ERASE_END_ADDRESS 	    0x10000000

#define IMAGE_NAME_TIMER1			"timer1.bin"
#define IMAGE_NAME_TIMER2			"timer2.bin"
#define IMAGE_NAME_GOLDEN			"ant4x400g_golden.bin"
#define IMAGE_NAME_USER				"ANT16400101S1.bin"
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

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

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
int SpiFlashGetStatusPrint(XSpi *SpiPtr, u32 verbose);
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
int program_golden(u32 erase);
int qspi_flash_erase_main (u32 OfsetAddr, u32 DieCount );
int icap ();
int SpiFlashReadID(XSpi *SpiPtr);
int SpiFlashReadRegister(XSpi *SpiPtr, unsigned int addRegister, unsigned int byteCount);
int SpiFlashReadRegisterNoWriteEnable(XSpi *SpiPtr, unsigned int addRegister, unsigned int byteCount);
int Spi_Blank_Check(u32 StartAddr, u32 NoByteToRead);
int qspi_program_flash (u32 StartAddr);
int qspi_readback_flash(u32 StartAddr,  u32 NoByteToRead);
int qspi_write_flash(u32 addr, unsigned char* data);
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
static Kiflash FlashData;
char pathname[PATH_MAX];
char binfilename[NAME_MAX];
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

void printProgress (int percentage)
{
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
}

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

void kiflash_test()
{
	printf("**kiflash_test entered\n");
}


int kiflash_init()
{	
	printf("KiFlash Init\n");	
	FlashData.fd = NULL;
	FlashData.baseAddr = 0;		
	FlashData.eraseStartAddr = 0;
	FlashData.eraseEndAddr = 0;
   	memset(binfilename, '\0', sizeof(binfilename));
	FlashData.filename = binfilename;	   
   	if (getcwd(pathname, sizeof(pathname)) != NULL) {
    	printf("Current working dir: %s\n", pathname);
   	} else {
    	perror("getcwd() error");
    	return XST_FAILURE;
   	}	
	// FlashData.path = pathname;
	FlashData.path = '/home/ixiaadmin/Downloads';


	printf("\n\rflash_qspi_rw entered\n\r");
    int choice, exit_flag = 0;
    int Status;
   	u32 SectCount=1, StartAddr, NoByteToRead;	
	int nbytes_temp=16;
	int input_deci_data=0;	
	Status = kiflash_init(&FlashData);
	if (Status != XST_SUCCESS) {
		printf("\n\rfailed at kiflash_init\n\r");
		return XST_FAILURE;
	}

	pcimem_init(&Pci);
	init_platform();
	
	if (qspi_init_flag ==0)
	{
		// printf("\n\r *flash_qspi_rw before System_init_startup 1\n\r");
		// fflush(stdout);
		Status = System_init_startup ();
		// printf("\n\r *flash_qspi_rw before System_init_startup 2\n\r");
		// fflush(stdout);		
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		} else qspi_init_flag=1;

	}

	print_kiflash(&FlashData);
	return XST_SUCCESS;
}

void print_kiflash(Kiflash *self)
{		
	printf("fd 0x%x NoOfPage 0x%x NoOfSector 0x%x baseAddr 0x%x eraseStart 0x%x\n\r", self->fd, self->NoOfSector, self->NoOfSector, self->baseAddr, self->eraseStartAddr );
	printf("filename %s path %s\n\r", self->filename, self->path);
	fflush(stdout);
}

int kiflash_program_user()
{
	printf("\n\rKiFlash Program User\t\r\n");					
	kiflash_init();
	fflush(stdout);
	clock_t start, end;
	int Status;
	start = clock();
	FlashData.filename = IMAGE_NAME_USER;
	FlashData.baseAddr = FLASH_USER_IMAGE_ADDRESS;
	FlashData.eraseStartAddr = FLASH_USER_ERASE_START_ADDRESS;
	FlashData.eraseEndAddr = FLASH_USER_ERASE_END_ADDRESS;
	printf("KiFlash Program User 1\n");					
	Status = program_user(0);
	if( Status != XST_SUCCESS )
	{
		printf("Program User Failed\n");				
		return XST_FAILURE;
	}						
	end = clock();
	double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Program user took  %f seconds to execute with Status 0x%x\n", cpu_time_used, Status); 
	fflush(stdout);

	printf("\n\rICAP\t\r\n");	
	fflush(stdout);
	icap(0);
	return XST_SUCCESS;
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

/* int main(void)
{
	printf("\n\rflash_qspi_rw entered\n\r");
    int choice, exit_flag = 0;
    int Status;
   	u32 SectCount=1, StartAddr, NoByteToRead;
	char* hex= FLASH_TEST_ADDRESS1;
	int nbytes_temp=16;
	int input_deci_data=0;	
	Status = kiflash_init(&FlashData);
	if (Status != XST_SUCCESS) {
		printf("\n\rfailed at kiflash_init\n\r");
		return XST_FAILURE;
	}

	pcimem_init(&Pci);
	init_platform();
	
	if (qspi_init_flag ==0)
	{
		// printf("\n\r *flash_qspi_rw before System_init_startup 1\n\r");
		// fflush(stdout);
		Status = System_init_startup ();
		// printf("\n\r *flash_qspi_rw before System_init_startup 2\n\r");
		// fflush(stdout);		
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		} else qspi_init_flag=1;

	}
		StartAddr = FLASH_TEST_ADDRESS0;
		// NoByteToRead = 512;
		// SectCount = 512;
		//  printf("\n\r *flash_qspi_rw before System_init_startup 3\n\r");
		// fflush(stdout);		
		while(exit_flag != 1) {
			printf("\n\r*******************************************************************");
			printf("\n\r*******************************************************************\n\r");
    		printf("\n\rChoose from options below: \r\n");
			printf(" 0: Reflash whole device\r\n");
    	    printf(" 1: Read Quad SPI flash ID\r\n");
			printf(" 2: Set Path\r\n");
			printf(" 3: Set user bin filename\r\n");
			printf(" 4: Program Golden\r\n");
			printf(" 5: Program User\r\n");
			printf(" 6: Program Timer\r\n");
			printf(" 7: ICAP\r\n");
    	    printf(" 8: Erase Quad SPI flash\r\n");
    	    printf(" 9: Blank Check Quad SPI flash\r\n");
    	    printf(" 10: Program (*.bin)\r\n");
    	    printf(" 11: Read Quad SPI flash\r\n");
			printf(" 12: Check Flag Status\r\n");
			printf(" 13: Enable Quad\r\n");
			printf(" 14: Verify (*.bin)\r\n");						
			printf(" 15: PciSweep\r\n");
			printf(" 16: Erase sector SPI flash\r\n");
			printf(" 17: Corrupt data to location in flash\r\n");
			fflush(stdout);
			int choice;

			scanf("%d", &choice);
			printf(" You entered: %d\n", choice);

    		switch(choice) {
    			case 0:
					printf("\n\r\t Re image whole flash\t\r\n");
					clock_t start, end, total_start, total_end;					
					total_start = clock();

					printf("\n\rErase Flash\t\r\n");
					fflush(stdout);										
					start = clock();
					Status = SpiFlashDieErase(&Spi, 0);
					if( Status != XST_SUCCESS )
					{
						printf("erase die failed at erase die 0\n");						
						break;
					}

					Status = SpiFlashDieErase(&Spi, 1);
					if( Status != XST_SUCCESS )
					{
						printf("erase die failed at erase die 1\n");						
						break;
					}								

					Status = SpiFlashDieErase(&Spi, 2);
					if( Status != XST_SUCCESS )
					{
						printf("erase die failed at erase die 2\n");						
						break;
					}								

					Status = SpiFlashDieErase(&Spi, 3);
					if( Status != XST_SUCCESS )
					{
						printf("erase die failed at erase die 3\n");						
						break;
					}								

					end = clock();
					double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
					printf("Erase flash took  %f seconds to execute\n", cpu_time_used); 
					fflush(stdout);
										
					sleep(1);
					printf("\n\rProgram timer1\t\r\n");
					start = clock();															
					FlashData.filename =  IMAGE_NAME_TIMER1;
					FlashData.baseAddr = FLASH_TIMER1_IMAGE_ADDRESS;
					FlashData.eraseStartAddr = FLASH_TIMER1_ERASE_START_ADDRESS;
					FlashData.eraseEndAddr = FLASH_TIMER1_ERASE_END_ADDRESS;
					Status = program_timer(1, 1);
					if( Status != XST_SUCCESS )
					{
						printf("Program timer1 Failed\n");						
						break;
					}						
					end = clock();
					cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
					printf("Program timer %d took  %f seconds to execute with Status 0x%x\n", 1, cpu_time_used, Status); 
					fflush(stdout);
					sleep(1);
					printf("\n\rProgram timer2\t\r\n");
					start = clock();															
					FlashData.filename =  IMAGE_NAME_TIMER2;
					FlashData.baseAddr = FLASH_TIMER2_IMAGE_ADDRESS;
					FlashData.eraseStartAddr = FLASH_TIMER2_ERASE_START_ADDRESS;
					FlashData.eraseEndAddr = FLASH_TIMER2_ERASE_END_ADDRESS;
					Status = program_timer(2, 1);
					if( Status != XST_SUCCESS )
					{
						printf("Program timer2 Failed\n");						
						break;
					}						
					end = clock();
					cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
					printf("Program timer %d took  %f seconds to execute with Status 0x%x\n", 2, cpu_time_used, Status); 
					fflush(stdout);
					sleep(1);

					printf("\n\rProgram Golden\t\r\n");
					fflush(stdout);						
					start = clock();
					FlashData.filename = IMAGE_NAME_GOLDEN;
					FlashData.baseAddr = FLASH_GOLDEN_IMAGE_ADDRESS;
					FlashData.eraseStartAddr = FLASH_GOLDEN_ERASE_START_ADDRESS;
					FlashData.eraseEndAddr = FLASH_GOLDEN_ERASE_END_ADDRESS;
					Status = program_golden(0);
					if( Status != XST_SUCCESS )
					{
						printf("Program Golden Failed\n");						
						break;
					}								
					end = clock();
					cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
					printf("Program golden took  %f seconds to execute with Status 0x%x\n", cpu_time_used, Status); 
					fflush(stdout);

					printf("\n\rProgram User\t\r\n");					
					fflush(stdout);
					start = clock();
					FlashData.filename = IMAGE_NAME_USER;
					FlashData.baseAddr = FLASH_USER_IMAGE_ADDRESS;
					FlashData.eraseStartAddr = FLASH_USER_ERASE_START_ADDRESS;
					FlashData.eraseEndAddr = FLASH_USER_ERASE_END_ADDRESS;
					Status = program_user(0);
					if( Status != XST_SUCCESS )
					{
						printf("Program User Failed\n");						
						break;
					}						
					end = clock();
					cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
					printf("Program user took  %f seconds to execute with Status 0x%x\n", cpu_time_used, Status); 
					fflush(stdout);

					printf("\n\rICAP\t\r\n");	
					fflush(stdout);
					icap(0);	
					total_end = clock();
					cpu_time_used = ((double) (total_end - total_start)) / CLOCKS_PER_SEC;
					printf("Program whole flash took  %f seconds to execute\n", cpu_time_used); 
					break;
				case 1:
				{
					printf("\n\r\t Read Quad SPI flash\t\r\n");
					fflush(stdout);
					qspi_flash_geo();
					printf("\n\rPress any Key to Main Menu\r\n");
					fflush(stdout);					
					print_kiflash(&FlashData);
					break;
				}
				case 2:
				{
					printf("\n\rSet Path: ");					
				   	memset(pathname, '\0', sizeof(pathname));
					
					scanf("%s", pathname);					
					FlashData.path = pathname;	
					printf("You entered: %s current latched path %s\n", pathname, FlashData.path);
					fflush(stdout);
					break;
				}
				case 3:
				{
					printf("\n\rSet user bin filename: ");					
				   	memset(binfilename, '\0', sizeof(binfilename));
					
					scanf("%s", binfilename);	
					FlashData.filename = binfilename;	
					printf("You entered: %s current latched filename %s\n", binfilename, FlashData.filename);
					fflush(stdout);				   	
					break;
				}				
				case 4:
				{ 
					clock_t start, end;					
					printf("\n\rProgram Golden\t\r\n");
					fflush(stdout);					
					start = clock();
					FlashData.filename = IMAGE_NAME_GOLDEN;
					FlashData.baseAddr = FLASH_GOLDEN_IMAGE_ADDRESS;
					FlashData.eraseStartAddr = FLASH_GOLDEN_ERASE_START_ADDRESS;
					FlashData.eraseEndAddr = FLASH_GOLDEN_ERASE_END_ADDRESS;
					Status = program_golden(1);
					end = clock();
					double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
					printf("Program golden took  %f seconds to execute with Status 0x%x\n", cpu_time_used, Status); 
					fflush(stdout);
					break;
				}		
				case 5:
				{
					clock_t start, end;					
					printf("\n\rProgram User\t\r\n");					
					fflush(stdout);
					start = clock();
					FlashData.filename = IMAGE_NAME_USER;
					FlashData.baseAddr = FLASH_USER_IMAGE_ADDRESS;
					FlashData.eraseStartAddr = FLASH_USER_ERASE_START_ADDRESS;
					FlashData.eraseEndAddr = FLASH_USER_ERASE_END_ADDRESS;
					Status = program_user(1);
					end = clock();
					double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
					printf("Program user took  %f seconds to execute with Status 0x%x\n", cpu_time_used, Status); 
					fflush(stdout);
					break;
				}	
				case 6:
				{				
					clock_t start, end;		
					printf("\n\rTimer ID to program (1,2): ");					
					int input_deci_data;    	    		
					scanf("%d", &input_deci_data);
					printf("You entered: %d\n", input_deci_data);

					if(input_deci_data == 1)
					{
						start = clock();															
						FlashData.filename =  IMAGE_NAME_TIMER1;
						FlashData.baseAddr = FLASH_TIMER1_IMAGE_ADDRESS;
						FlashData.eraseStartAddr = FLASH_TIMER1_ERASE_START_ADDRESS;
						FlashData.eraseEndAddr = FLASH_TIMER1_ERASE_END_ADDRESS;
						Status = program_timer(input_deci_data, 1);
						end = clock();
						double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
						printf("Program timer %d took  %f seconds to execute with Status 0x%x\n", input_deci_data, cpu_time_used, Status); 
						fflush(stdout);
					}
					else if (input_deci_data == 2)
					{
						start = clock();															
						FlashData.filename =  IMAGE_NAME_TIMER2;
						FlashData.baseAddr = FLASH_TIMER2_IMAGE_ADDRESS;
						FlashData.eraseStartAddr = FLASH_TIMER2_ERASE_START_ADDRESS;
						FlashData.eraseEndAddr = FLASH_TIMER2_ERASE_END_ADDRESS;
						Status = program_timer(input_deci_data, 1);
						end = clock();
						double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
						printf("Program timer %d took  %f seconds to execute with Status 0x%x\n", input_deci_data, cpu_time_used, Status); 
						fflush(stdout);
					}
					else
					{
						printf("Invalid Timer ID specified\n");
					}				

					fflush(stdout);		

					break;
				}
				case 7:
				{
					printf("\n\rDie number to Jump (0,2): ");					
					int input_deci_data;    	    		
					scanf("%d", &input_deci_data);
					printf("You entered: %d\n", input_deci_data);

					if(input_deci_data == 0 || input_deci_data == 2)
					{
						icap(input_deci_data);	
					}
					else
					{
						printf("Invalid jump die specified\n");
					}				

					fflush(stdout);		
					break;
				}
				case 8:
				{
					printf("\n\rDie number to erase (0-3): ");					
					int input_deci_data;    	    		
					scanf("%d", &input_deci_data);
					printf("You entered: %d\n", input_deci_data);
									
					qspi_flash_erase_main(Address, input_deci_data);
					fflush(stdout);
					break;
				}
				case 9:
				{
					printf ("\n\rQuad SPI flash Blank Check:\n\r");
					printf("\n\rBlank Check start address (hex): ");
					// StartAddr = 0;
					// NoByteToRead = 256;
					printf("\n\rStart Address (hex): ");
					unsigned int input_deci_data;
					scanf("%x", &input_deci_data);
					
					printf("You entered: 0x%x\n", input_deci_data);
					
					StartAddr = input_deci_data;
					printf("\n\rNumber of Bytes to read (hex): ");
					input_deci_data = 0;
					scanf("%x", &input_deci_data);
					printf("You entered: 0x%x\n", input_deci_data);
					NoByteToRead = input_deci_data;

					Status = Spi_Blank_Check(StartAddr, NoByteToRead);
					if (Status != XST_SUCCESS) {
						printf("\n\r\n\r\t\tBlank Check Operation Fail!.\r\n");
					}else  printf("\n\r\n\rBlank Check Operation Completed without error.\r\n");
								
					fflush(stdout);
					break;
				}
				case 10:
				{
					clock_t start, end;					
					printf("\r\nProgram (*.bin)\r\n");
					printf("\n\rDie number to program (0-3): ");					
					int input_deci_data;    	    		
					scanf("%d", &input_deci_data);
					printf("You entered: %d\n", input_deci_data);
					start = clock();
					Status = DownloadSerialDataToQSPIFlash(input_deci_data);
					end = clock();
					double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
					printf("Program flash took  %f seconds to execute \n", cpu_time_used); 
					fflush(stdout);
					break;
				}	
				case 11:
				{
					clock_t start, end;
					printf ("\n\r\n\rRead Quad SPI flash:");
					printf("\n\rStart Address (hex): ");
					unsigned int input_deci_data;
					scanf("%x", &input_deci_data);
					
					printf("You entered: 0x%x\n", input_deci_data);
					
					StartAddr = input_deci_data;
					printf("\n\rNumber of Bytes to read (hex): ");
					input_deci_data = 0;
					scanf("%x", &input_deci_data);
					printf("You entered: 0x%x\n", input_deci_data);
					NoByteToRead = input_deci_data;
					if( NoByteToRead )
					{
						start = clock();
						qspi_read_flash(StartAddr, NoByteToRead);					
						end = clock();
						double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
						printf("Read Quad SPI flash took %f seconds to execute \n", cpu_time_used); 
						fflush(stdout);
					}

					printf("\n\rPress any Key to Main Menu\r\n");
					fflush(stdout);
					break;
				}
				case 12:
				{
					printf("\n\r\t Read FlagStatus\t\r\n");
					fflush(stdout);
					int Status = SpiFlashGetStatusPrint(&Spi, 1);	
					int FlagStatus = SpiFlashReadflagstatus(&Spi, 1);
					SpiFlashClrflagstatus(&Spi);
					// printf("FlagStatus Status 0x%X flagStatus 0x%x\n", Status, FlagStatus);
					fflush(stdout);
					break;
				}		
				case 13:
				{
					printf("\n\r\t Enable Quad flash\t\r\n");
					fflush(stdout);
					Status = SpiFlashQuadEnable(&Spi);
					if (Status != XST_SUCCESS) {
						printf("Enable Quad flash failed Status 0x%X\n", Status);
						fflush(stdout);
						return XST_FAILURE;						
					}
					fflush(stdout);
					break;
				}
				case 14:
				{
					clock_t start, end;										
					printf("\r\nVerify (*.bin)\r\n");
					printf("\n\rInput 1 golden 2 user 3 timer1 4 timer2: ");					
					int input_deci_data;    	    		
					scanf("%d", &input_deci_data);
					printf("You entered: %d\n", input_deci_data);
					start = clock();
					if (input_deci_data == 1)
					{
						FlashData.filename = IMAGE_NAME_GOLDEN;
						FlashData.baseAddr = FLASH_GOLDEN_IMAGE_ADDRESS;
						FlashData.eraseStartAddr = FLASH_GOLDEN_ERASE_START_ADDRESS;
						FlashData.eraseEndAddr = FLASH_GOLDEN_ERASE_END_ADDRESS;
						Status = qspi_verify_bin();
						if( Status != XST_SUCCESS )
						{
							printf("verify_golden failed at verify\n");							
						}
					}
					else if (input_deci_data == 2)
					{
						FlashData.filename = IMAGE_NAME_USER;
						FlashData.baseAddr = FLASH_USER_IMAGE_ADDRESS;
						FlashData.eraseStartAddr = FLASH_USER_ERASE_START_ADDRESS;
						FlashData.eraseEndAddr = FLASH_USER_ERASE_END_ADDRESS;
						Status = qspi_verify_bin();
						if( Status != XST_SUCCESS )
						{
							printf("verify_user failed at verify\n");							
						}					
					}
					else if (input_deci_data == 3)
					{
						FlashData.filename =  IMAGE_NAME_TIMER1;
						FlashData.baseAddr = FLASH_TIMER1_IMAGE_ADDRESS;
						FlashData.eraseStartAddr = FLASH_TIMER1_ERASE_START_ADDRESS;
						FlashData.eraseEndAddr = FLASH_TIMER1_ERASE_END_ADDRESS;
						Status = qspi_verify_bin();
						if( Status != XST_SUCCESS )
						{
							printf("verify_timer1 failed at verify\n");							
						}							
					}
					else if (input_deci_data == 4)
					{
						FlashData.filename =  IMAGE_NAME_TIMER2;
						FlashData.baseAddr = FLASH_TIMER2_IMAGE_ADDRESS;
						FlashData.eraseStartAddr = FLASH_TIMER2_ERASE_START_ADDRESS;
						FlashData.eraseEndAddr = FLASH_TIMER2_ERASE_END_ADDRESS;						
						if( Status != XST_SUCCESS )
						{
							printf("verify_timer2 failed at verify\n");							
						}							
					}
					else 
					{
						printf("Invalid verify destination specified\n");
					}				


					// Status = VerifySerialDataToQSPIFlash(input_deci_data);
					end = clock();
					double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
					printf("Verify flash took %f seconds to execute \n", cpu_time_used); 
					fflush(stdout);
					break;
				}
				case 15:
				{
					clock_t start, end;
					printf("\r\nPciSweep\r\n");
					printf("\n\rStart Addr (hex): ");					
					unsigned int input_deci_data;    	    		
					scanf("%x", &input_deci_data);
					printf("You entered: 0x%x\n", input_deci_data);
					start = clock();					
					Status = PciSweep(&Spi, input_deci_data);
					end = clock();
					double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
					printf("PciSweep took %f seconds to execute \n", cpu_time_used);					
					fflush(stdout);		
					break;
				}
				case 16:
				{
					printf("\n\rAddrto erase (hex): ");
										
					unsigned int input_deci_data;    	    		
					scanf("%x", &input_deci_data);
					Status = SpiFlashSectorErase(&Spi, input_deci_data);	
					if (Status != XST_SUCCESS) {
						printf("sector erase failed Status 0x%X\n", Status);
					} else {
						printf("sector erase command sent\n");
					}																		
					fflush(stdout);
					break;
				}
				case 17:
				{	
					printf("\n\rWrite flash 8 bytes 0,1,2,3,4,5,6,7 at starting addr: ");					
					printf("\n\rStart Address(hex): ");
					unsigned int input_deci_data;    	    		
					scanf("%x", &input_deci_data);
					printf("You entered: 0x%x\n", input_deci_data);
										
					unsigned char *buffer;	
					buffer = (unsigned char *)malloc(8 * sizeof(unsigned char)); // Enough memory for file + \0
					for(int Index = 0; Index < PAGE_SIZE; Index++)
					{
						buffer[Index] = Index;
					}
					
					Status  = qspi_write_flash(input_deci_data, buffer);
					if (Status != XST_SUCCESS) {
						printf("rWrite Quad SPI flash failed Status 0x%X\n", Status);
					} else {
						printf("rWrite Quad SPI flash %d bytes to addr 0x%X \n", sizeof(buffer), input_deci_data);
					}
					
					fflush(stdout);
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
*/


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

int program_golden(u32 erase)
{
	int Status;
	if( erase )
	{
		// u32 SectorsToErase = (FLASH_TIMER1_IMAGE_ADDRESS / BYTE_PER_SECTOR) - 1;
		u32 SectorsToErase = (FlashData.eraseEndAddr - FlashData.eraseStartAddr) / BYTE_PER_SECTOR;		
		Status = qspi_erase_sector_flash (FlashData.eraseStartAddr, SectorsToErase );
		if( Status != XST_SUCCESS )
		{
			printf("program_golden failed at erase\n");
			return XST_FAILURE;
		}
	}

	Status = qspi_program_bin();
	if( Status != XST_SUCCESS )
	{
		printf("program_golden failed at program\n");
		return XST_FAILURE;	
	}

	Status = qspi_verify_bin();
	if( Status != XST_SUCCESS )
	{
		printf("program_golden failed at verify\n");
		return XST_FAILURE;	
	}

	return XST_SUCCESS;
}

int program_user(u32 erase)
{
	printf("program_user 0 with erase flag %x\n", erase ); 
	int Status;
	if( erase )
	{		
		Status = SpiFlashDieErase(&Spi, 2);
		if( Status != XST_SUCCESS )
		{
			printf("program_user failed at erase die 2\n");
			return XST_FAILURE;
		}		
		u32 AddrToEraseBegin = 0x0C000000;  // sector erase from begining of die 3.		
		u32 SectorsToErase = (FlashData.eraseEndAddr - AddrToEraseBegin) / BYTE_PER_SECTOR;		
		
		Status = qspi_erase_sector_flash (AddrToEraseBegin, SectorsToErase);
		if( Status != XST_SUCCESS )
		{
			printf("program_user failed at sector erase in die 3\n");
			return XST_FAILURE;
		}
	}

	Status = qspi_program_bin();
	if( Status != XST_SUCCESS )
	{
		printf("program_user failed at program\n");
		return XST_FAILURE;	
	}

	Status = qspi_verify_bin();
	if( Status != XST_SUCCESS )
	{
		printf("program_user failed at verify\n");
		return XST_FAILURE;	
	}

	return XST_SUCCESS;
}

int program_timer(u32 timerid, u32 erase)
{
	int Status;
	if( erase )
	{
		// u32 SectorsToErase = (FLASH_TIMER1_IMAGE_ADDRESS / BYTE_PER_SECTOR) - 1;
		u32 SectorsToErase = (FlashData.eraseEndAddr - FlashData.eraseStartAddr) / BYTE_PER_SECTOR;		
		Status = qspi_erase_sector_flash (FlashData.eraseStartAddr, SectorsToErase );
		if( Status != XST_SUCCESS )
		{
			printf("program_timer %d failed at erase\n", timerid);
			return XST_FAILURE;
		}
	}

	Status = qspi_program_bin();
	if( Status != XST_SUCCESS )
	{
		printf("program_timer %d failed at program\n", timerid);
		return XST_FAILURE;	
	}

	Status = qspi_verify_bin();
	if( Status != XST_SUCCESS )
	{
		printf("program_timer %d failed at verify\n", timerid);
		return XST_FAILURE;	
	}

	return XST_SUCCESS;
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
	int Status = SpiFlashDieErase(&Spi, DieCount);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else 	printf("\n\rEntire flash erase takes several minutes \n\r \n\rPlease wait .....\n\r");
	Status = SpiFlashQuadEnable(&Spi);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int icap(u32 die)
{
	printf("\n icap die number %d \n\r", die);
	uint64_t icapBaseAddr = 0x00070000;
	uint32_t icapWriteFifo = 0x0200;
	uint32_t icapWriteControl = 0x0218;
	uint32_t jumpAddr = 0x00000000;
	if(die == 2)
	{
		jumpAddr = 0x08000000;
	}
	
	// write IPROG command sequence
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0xFFFFFFFF);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0xAA995566);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x20000000);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x30020001);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, jumpAddr);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x30008001);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x0000000F);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x20000000);	

	// write control reg
	PciWriteReg(&Spi, icapBaseAddr, icapWriteControl, 0x00000001);	

	sleep(5);
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
	fflush(stdout);
	XSpi_PciInitialize(&Spi, &Pci);

	Status = XSpi_CfgInitialize(&Spi, ConfigPtr,
				  ConfigPtr->BaseAddress);					  
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
//	Status = SetupInterruptSystem(&Spi);
//	if(Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
	XSpi_SetStatusHandler(&Spi, &Spi, (XSpi_StatusHandler)SpiHandler);
	Status = XSpi_SetOptions(&Spi, XSP_MASTER_OPTION |
				 XSP_MANUAL_SSELECT_OPTION);
	if(Status != XST_SUCCESS) {
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
	printf ("\n\r\n\rsector erase Start Address: 0x%x\t", OfsetAddr);
	printf ("\r\nsector erase End Address: 0x%x\t", OfsetAddr + (BYTE_PER_SECTOR * SectCount));
	printf("\n\r\n\rPerforming sector erase for Quad SPI, Please wait ...\n\r");

	if ( (SectCount < NUMB_SECTORS) || (SectCount == NUMB_SECTORS))
	{

		for (Count_int=0; Count_int< SectCount; Count_int++)
		{
			Status = SpiFlashSectorErase(&Spi, OfsetAddr);
			if(Status != XST_SUCCESS) {

				return XST_FAILURE;
			}else
			{
				OfsetAddr = (OfsetAddr + 65536);
			}
		}
		printf("\n\rSector Flash Erase Completed\n");
	}
	else
	{
		printf ("\n\rEntered sector value = \t0x"); putnum(NUMB_SECTORS);
		printf ("\n\rExceeds the available flash sector.  Please re-enter a valid sector value\n");
		return XST_FAILURE;
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
	int Status;

	if (qspi_init_flag ==0)
	{
		Status = System_init_startup ();
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		} else qspi_init_flag=1;
	}
	Status = SpiFlashReadID(&Spi);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	int FlagStatus = SpiFlashReadflagstatus(&Spi, 1);
	SpiFlashClrflagstatus(&Spi);

	Status = SpiFlashQuadEnable(&Spi);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;		
	}

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
	fflush(stdout);
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
			if ( (StartAddr==0x000000) || (NoOfPage_tot<= (4*NOB_PAGES)))
			{
				printf("\n\rOffset(h):\t0x00\t0x01\t0x02\t0x03\t0x04\t0x05\t0x06\t0x07\n\r");
				CntLine =0;
				Index =0;
				printf("\n\r0x%08x:\t", (StartAddr));

				while  (NoOfPage !=0 )
				{
					Status = SpiFlashWriteEnable(&Spi);
					if(Status != XST_SUCCESS) {
						return XST_FAILURE;
					}
					for(Index = 0; Index < (PAGE_SIZE + READ_WRITE_EXTRA_BYTES); Index++)
					{
						ReadBuffer[Index] = 0x0;
					}

					Status = SpiFlashRead(&Spi, StartAddr, PAGE_SIZE, COMMAND_4BYTE_QUAD_OUTPUT_FAST_READ);
									
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
				printf ("\n\rNumber of Pages Exceed the device settings!");
			}

		fflush(stdout);
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
	int Status;
	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} 
	WriteBuffer[BYTE1] = COMMAND_WRITE_ENABLE;
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
	WriteBuffer[BYTE1] = COMMAND_ENTER_4BYTE_ADDRESS_MODE;		

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
int SpiFlashReadflagstatus(XSpi *SpiPtr, u32 verbose)
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
		if (verbose)
			printf ("\n\rCOMMAND_READ_FLAG_STATUS: 0x%X\n", ReadBuffer[1]);
	return XST_SUCCESS;
}

int SpiFlashClrflagstatus(XSpi *SpiPtr)
{
	// printf ("SpiFlashClrflagstatus\n");
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

	for(Index = 5; Index < (ByteCount + READ_WRITE_EXTRA_BYTES +1); Index++, ddrvector++) {
			WriteBuffer[Index] = pageBuffer[ddrvector];
	}

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
				(ByteCount + READ_WRITE_EXTRA_BYTES +1));
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
	Status = SpiFlash4byteexit(&Spi);
		if (Status != XST_SUCCESS) {
				return XST_FAILURE;
		}
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
	int Status;
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
	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	WriteBuffer[BYTE1] = COMMAND_BULK_ERASE;

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
						BULK_ERASE_BYTES);
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

int GetStatus(XSpi *SpiPtr, u32 verbose)
{
	int Status = SpiFlashGetStatusPrint(&Spi, verbose);	
	int FlagStatus = SpiFlashReadflagstatus(&Spi, verbose);
	SpiFlashClrflagstatus(&Spi);
	// printf("FlagStatus Status 0x%X flagStatus 0x%x\n", Status, FlagStatus);
	return Status;
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

	WriteBuffer[BYTE1] = COMMAND_SECTOR_ERASE;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
					SECTOR_ERASE_BYTES+1);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		Status = GetStatus(SpiPtr, 0);
		TransferInProgress = FALSE;  //tli, We are in polled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag

		if(Status != XST_SUCCESS)
			return XST_FAILURE;
	}
	while(TransferInProgress);
	if(ErrorCount != 0) {
		ErrorCount = 0;
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int SpiFlashDieErase(XSpi *SpiPtr, u32 uscDieNr)
{
	printf ("\n\r\n\rdie erase for index: 0x%x\n", uscDieNr);
	int Status;
	u32 Addr = uscDieNr << 26;
	Status = SpiFlash4bytemodeEnable(&Spi);
		if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = SpiReadExtendedAddressRegister(&Spi);
		if(Status != XST_SUCCESS) {
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

	WriteBuffer[BYTE1] = COMMAND_DIE_ERASE;
	WriteBuffer[BYTE2] = (u8) (Addr >> 24);
	WriteBuffer[BYTE3] = (u8) (Addr >> 16);
	WriteBuffer[BYTE4] = (u8) (Addr >> 8);
	WriteBuffer[BYTE5] = (u8) Addr;

	TransferInProgress = TRUE;
	Status = XSpi_Transfer(SpiPtr, WriteBuffer, NULL,
					SECTOR_ERASE_BYTES+1);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} else {
		Status = GetStatus(SpiPtr, 0);
		TransferInProgress = FALSE;  //tli, We are in poSECTOR_ERASE_BYTESlled mode, if exit from XSpi_Transfer with XST_SUCCESS clear this flag
		if(Status != XST_SUCCESS)
			return XST_FAILURE;		
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
	int 	Status;
	Status = SpiFlashWaitForFlashReady();
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	WriteBuffer[BYTE1] = COMMAND_READ_ID;
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
	int Status;

	WriteBuffer[BYTE1] = COMMAND_STATUSREG_READ;

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

int SpiFlashGetStatusPrint(XSpi *SpiPtr, u32 verbose)
{
	int Status;

	WriteBuffer[BYTE1] = COMMAND_STATUSREG_READ;

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
	} else
	{
		if (verbose)
			printf ("\n\COMMAND_STATUSREG_READ: 0x%X\n", ReadBuffer[1]);
	}
	
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


static int  ProgramTimer(u32 TimerId)
{
	printf ("\r\nProgramming Timer Start %d \n\r", TimerId);	
	uint32_t timer1BaseAddr = 0x07FFFC00;
	uint32_t timer2BaseAddr = 0x0FFC0000;
	/* if(die == 2)
	{
		jumpAddr = 0x08000000;
	}
	
	// write IPROG command sequence
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0xFFFFFFFF);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0xAA995566);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x20000000);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x30020001);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, jumpAddr);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x30008001);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x0000000F);
	PciWriteReg(&Spi, icapBaseAddr, icapWriteFifo, 0x20000000);	

	// write control reg
	PciWriteReg(&Spi, icapBaseAddr, icapWriteControl, 0x00000001);	
*/

	// qspi_program_flash(StartDie);
	printf ("\r\nProgramming Timer end");
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

char *concat(const char *a, const char *b){
    int lena = strlen(a);
    int lenb = strlen(b);
    char *con = malloc(lena+lenb+1);
    // copy & concat (including string termination)
    memcpy(con,a,lena);
    memcpy(con+lena,b,lenb+1);        
    return con;
}

int qspi_program_bin()
{
		int quq_int, remaind_int, NoOfSector, NoOfPage;
		FILE* fp;
		uint64_t FileByteCount;
		u32 baseStartAddr = FlashData.baseAddr;
		u32 StartAddr = 0;
		// printf("qspi_program_bin 1\n");
		// fflush(stdout);
		char* path = concat(FlashData.path, "/");
		char* filename = concat(path, FlashData.filename);

    	if ((fp = fopen(filename, "r")) == NULL) {
        	printf("Fpga Download open failed %s.\n", filename);			
			fflush(stdout);
        	return XST_FAILURE;
    	}
		fseek(fp, 0, SEEK_END);
		FileByteCount = ftell(fp);
		printf("\r\nFpga Download TotalByteRecived = 0x%x\n", FileByteCount);
		printf ("\r\nFpga Download FlashAddress Offset = 0x%x\n", baseStartAddr);
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
		// NoOfPage = 1;

		printf ("\r\nNoOfSector= 0x%x\n", NoOfSector);
		printf ("\r\nNoOfPage= 0x%x\n", NoOfPage);
		fflush(stdout);

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

		double totalPage = NoOfPage;
		double curpercentage = 0.0;		
		while (NoOfPage !=0)
		{						
			double latched_percentage = curpercentage;
			curpercentage = (double)((totalPage - NoOfPage)/totalPage)*100.0;
			// printProgress((int)curpercentage);	
			if (latched_percentage != curpercentage)
				printf(" %d\r", (int)curpercentage);

			fseek(fp, StartAddr, SEEK_SET);
			unsigned char *buffer;	
			buffer = (unsigned char *)malloc((PAGE_SIZE + 10) * sizeof(unsigned char)); // Enough memory for file + \0
		
			fread(buffer, 1, PAGE_SIZE, fp); // Read in the entire file

			Status = SpiFlashWriteEnable(&Spi);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			Status = SpiFlashWrite_File(&Spi, StartAddr + baseStartAddr, PAGE_SIZE, COMMAND_4BYTE_PAGE_PROGRAM, buffer);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			} else
			{
				NoOfPage--;
				StartAddr = (StartAddr + PAGE_SIZE);
			}
		}

		fclose(fp); // Close the file
		return XST_SUCCESS;
}

int qspi_verify_bin()
{
		int quq_int, remaind_int, NoOfSector, NoOfPage, Index;
		FILE* fp;
		uint64_t FileByteCount;
		u32 baseStartAddr = FlashData.baseAddr;
		u32 StartAddr = 0;

		char* path = concat(FlashData.path, "/");
		char* filename = concat(path, FlashData.filename);

    	if ((fp = fopen(filename, "r")) == NULL) {
        	printf("Fpga Verify open failed %s.\n", filename);			
			fflush(stdout);
        	return XST_FAILURE;
    	}
		
		fseek(fp, 0, SEEK_END);
		FileByteCount = ftell(fp);
		printf("\r\nFpga Verify TotalByteRecived = 0x%x\n", FileByteCount);
		printf ("\r\nFpga Verify FlashAddress Offset = 0x%x\n", baseStartAddr);
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


		printf ("\r\nNoOfSector= 0x%x\n", NoOfSector);
		printf ("\r\nNoOfPage= 0x%x\n", NoOfPage);
		fflush(stdout);

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

		double totalPage = NoOfPage;
		double curpercentage = 0.0;				
		while (NoOfPage !=0)
		{
			double latched_percentage = curpercentage;
			curpercentage = (double)((totalPage - NoOfPage)/totalPage)*100.0;
			// printProgress((int)curpercentage);	
			if (latched_percentage != curpercentage)
				printf(" %d\r", (int)curpercentage);

			fseek(fp, StartAddr, SEEK_SET);
			unsigned char *buffer;	
			buffer = (unsigned char *)malloc((PAGE_SIZE + 10) * sizeof(unsigned char)); // Enough memory for file + \0
		
			fread(buffer, 1, PAGE_SIZE, fp); // Read in the entire file

			Status = SpiFlashWriteEnable(&Spi);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			for(Index = 0; Index < (PAGE_SIZE + READ_WRITE_EXTRA_BYTES); Index++)
			{
				ReadBuffer[Index] = 0x0;
			}
			Status = SpiFlashRead(&Spi, baseStartAddr + StartAddr, PAGE_SIZE, COMMAND_4BYTE_QUAD_OUTPUT_FAST_READ);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			
			int isequal = 1;
			for(Index = 5; Index <= (PAGE_SIZE + READ_WRITE_EXTRA_BYTES); Index++)
			{
				if(ReadBuffer[Index + READ_WRITE_EXTRA_BYTES] != buffer[Index - 5])
				{
					isequal = 0;
				}				
			}
  			if (isequal == 0)
			{
				printf ("** Page Start Addr 0x%X do not matched\n",StartAddr);
				fflush(stdout);	
				return XST_FAILURE;
			}
				
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			} else
			{
				NoOfPage--;
				StartAddr = (StartAddr + PAGE_SIZE);
			}
		}
		printf ("bin verify passed\n");
		 fflush(stdout);	
		fclose(fp); // Close the file
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

		char* filename = "/home/ixiaadmin/Downloads/ant4x400g.bin";

    	if ((fp = fopen(filename, "r")) == NULL) {
        	printf("Fpga Download open failed %s.\n", filename);			
			fflush(stdout);
        	return XST_FAILURE;
    	}
		fseek(fp, 0, SEEK_END);
		FileByteCount = ftell(fp);
		printf("\r\nFpga Download TotalByteRecived =\t"); putnum(FileByteCount);
		printf ("\r\nFpga Download FlashAddress Offset = \t"); putnum(baseStartAddr);
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
		// NoOfPage = 1;

		printf ("\r\nNoOfSector= "); putnum(NoOfSector);
		printf ("\r\nNoOfPage= "); putnum(NoOfPage);
		fflush(stdout);

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

		while (NoOfPage !=0)
		{
			fseek(fp, StartAddr, SEEK_SET);
			unsigned char *buffer;	
			buffer = (unsigned char *)malloc((PAGE_SIZE + 10) * sizeof(unsigned char)); // Enough memory for file + \0
		
			fread(buffer, 1, PAGE_SIZE, fp); // Read in the entire file

			Status = SpiFlashWriteEnable(&Spi);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			Status = SpiFlashWrite_File(&Spi, StartAddr + baseStartAddr, PAGE_SIZE, COMMAND_4BYTE_PAGE_PROGRAM, buffer);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			} else
			{
				NoOfPage--;
				StartAddr = (StartAddr + PAGE_SIZE);
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

    	if ((fp = fopen(filename, "r")) == NULL) {
        	printf("Fpga Verify open failed %s.\n", filename);			
			fflush(stdout);
        	return XST_FAILURE;
    	}
		
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


		printf ("\r\nNoOfSector= "); putnum(NoOfSector);
		printf ("\r\nNoOfPage= "); putnum(NoOfPage);
		fflush(stdout);

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

		while (NoOfPage !=0)
		{
			fseek(fp, StartAddr, SEEK_SET);
			unsigned char *buffer;	
			buffer = (unsigned char *)malloc((PAGE_SIZE + 10) * sizeof(unsigned char)); // Enough memory for file + \0
		
			fread(buffer, 1, PAGE_SIZE, fp); // Read in the entire file

			Status = SpiFlashWriteEnable(&Spi);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}

			for(Index = 0; Index < (PAGE_SIZE + READ_WRITE_EXTRA_BYTES); Index++)
			{
				ReadBuffer[Index] = 0x0;
			}
			Status = SpiFlashRead(&Spi, baseStartAddr + StartAddr, PAGE_SIZE, COMMAND_4BYTE_QUAD_OUTPUT_FAST_READ);
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			}
			int isequal = 1;
			for(Index = 5; Index <= (PAGE_SIZE + READ_WRITE_EXTRA_BYTES); Index++)
			{
				if(ReadBuffer[Index + READ_WRITE_EXTRA_BYTES] != buffer[Index - 5])
				{
					isequal = 0;
				}				
			}
  			if (isequal == 1)
			{
//			   printf ("Page Start Addr 0x%X matched\n",StartAddr);
//			   fflush(stdout);			   
			} else
			{
				printf ("** Page Start Addr 0x%X do not matched\n",StartAddr);
				fflush(stdout);	
				return XST_FAILURE;
			}
				
			if(Status != XST_SUCCESS) {
				return XST_FAILURE;
			} else
			{
				NoOfPage--;
				StartAddr = (StartAddr + PAGE_SIZE);
			}
		}

		fclose(fp); // Close the file
		return XST_SUCCESS;
}


int qspi_write_flash(u32 addr, unsigned char* data)
{
	int Status;


	Status = SpiFlashWrite_File(&Spi, addr, sizeof(data), COMMAND_4BYTE_PAGE_PROGRAM, data);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	} 
	
		
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
