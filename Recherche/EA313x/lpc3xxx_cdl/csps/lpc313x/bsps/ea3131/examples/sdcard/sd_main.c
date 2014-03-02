/***********************************************************************
 * $Id:: sd_main.c 6146 2011-01-11 12:25:24Z ing02124                  $
 *
 * Project: SD/MMC card driver example
 *
 * Description:
 *     A simple SD/MMC card driver example.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/
#include <stdio.h>
#include <string.h>

#include "lpc_types.h"
#include "lpc_irq_fiq.h"
#include "lpc_arm922t_cp15_driver.h"
#include "ea3131_board.h"
#include "lpc313x_timer_driver.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_mci_driver.h"
#include "lpc313x_ioconf_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc31xx_vmem_driver.h"
#include "ff.h"
#include "diskio.h"

#ifdef offsetof
#undef offsetof
#endif

#define offsetof(s,m)   (int)&(((s *)0)->m)
#define COMPILE_TIME_ASSERT(expr)   char constraint[expr]
#define DEMO_FILE_NAME "write.hex"
#define READ_CHUNK_SZ  (4 * 1024) /* 4K */
#define WRITE_CHUNK_SZ (1 * 1024) /* 1K */
#define WRITE_CNT      (32) /* 32 * 1K */

COMPILE_TIME_ASSERT(offsetof(MCI_REGS_T, hcon) == 0x70);
COMPILE_TIME_ASSERT(offsetof(MCI_REGS_T, data) == 0x100);

extern INT_32 g_mcidev;
static char buff[512];
static DMA_BUFFER BYTE readbuf[32768];
static INT_32 g_uartdev;
UNS_8 drv = 0; //We support only single drive
FILINFO Finfo;
FATFS Fatfs[_VOLUMES];		/* File system object for each logical drive */
BYTE di = 0; //Disk initialized flag
DIR dir;
#if _USE_LFN
char Lfname[_MAX_LFN+1];
#endif
int raw_read;
static FIL fpr;
static FIL fpw;
static FIL fp;

extern DSTATUS disk_initialize(UNS_8);
extern BOOL_32 disk_read(UNS_8, void *, UNS_32, UNS_32);
extern BOOL_32 disk_write(UNS_8, void *, UNS_32, UNS_32);



/***********************************************************************
 *
 * Function: uart_string_write
 *
 * Purpose: Uart output function
 *
 * Processing:
 *     Writes the buffer to UART port
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/

void uart_string_write(char *pbuff)
{
  UNS_32 len = strlen(pbuff);
  UNS_32 written;

  if (g_uartdev == 0)
    return;

  while (len > 0)
  {
    written = uart_write(g_uartdev, pbuff, len);
    len -= written;
    pbuff += written;
  }
}

static
void uart_string_read(char *buff, int len)
{
  CHAR c;
  int i = 0;

  for (;;)
  {
    while (!(uart_read(g_uartdev, &c, 1)));

    if (c == '\r')
    {
      uart_string_write("\r\n");
      break;
    }

    if ((c == '\b') && i)
    {
      i--;
      uart_write(g_uartdev, (void*)&c, 1);
      continue;
    }

    if (c >= ' ' && i < len - 1)  	/* Visible chars */
    {
      uart_write(g_uartdev, (void*)&c, 1);
      buff[i++] = c;
    }
  }
  buff[i] = 0;
}

DWORD get_fattime(void)
{
  DWORD tmr = 0;

// stub function for now
  return tmr;
}

static
FRESULT scan_files(
  char* path		/* Pointer to the working buffer with start path */
)
{
  DIR dirs;
  FRESULT res;
  int i;
  char *fn;
  WORD acc_files = 0;
  WORD acc_dirs = 0;
  DWORD acc_size = 0;

  res = f_opendir(&dirs, path);

  if (res == FR_OK)
  {
    i = strlen(path);
    while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0])
    {
      if (_FS_RPATH && Finfo.fname[0] == '.') continue;
#if _USE_LFN
      fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
      fn = Finfo.fname;
#endif
      if (Finfo.fattrib & AM_DIR)
      {
        acc_dirs++;
        *(path + i) = '/';
        strcpy(path + i + 1, fn);
        res = scan_files(path);
        *(path + i) = '\0';
        if (res != FR_OK) break;
      }
      else
      {
        acc_files++;
        acc_size += Finfo.fsize;
      }
    }
  }

  sprintf(buff, "\r%u files, %lu bytes. \r\n%u folders.\r\n"
          , acc_files, acc_size, acc_dirs);
  uart_string_write(buff);


  return res;
}

static void list_files(char* path)
{

  FRESULT res;
  long p1;
  UINT s1, s2;
//		CHAR *ptr;
  FATFS *fs;

  res = f_opendir(&dir, path);
  if (res != FR_OK)
    uart_string_write("Unable to open directory \r\n");
  p1 = s1 = s2 = 0;
  for (;;)
  {
    res = f_readdir(&dir, &Finfo);
    if ((res != FR_OK) || !Finfo.fname[0])break;
    if (Finfo.fattrib & AM_DIR)
      s2++;
    else
    {
      s1++;
      p1 += Finfo.fsize;
    }
    sprintf(buff, "\r%c%c%c%c%c  %u/%02u/%02u  %02u:%02u %9lu  %s \r\n",
            (Finfo.fattrib & AM_DIR) ? 'D' : '-',
            (Finfo.fattrib & AM_RDO) ? 'R' : '-',
            (Finfo.fattrib & AM_HID) ? 'H' : '-',
            (Finfo.fattrib & AM_SYS) ? 'S' : '-',
            (Finfo.fattrib & AM_ARC) ? 'A' : '-',
            (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
            (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63,
            Finfo.fsize, &(Finfo.fname[0]));
    uart_string_write(buff);
#if _USE_LFN
    for (p2 = strlen(Finfo.fname); p2 < 14; p2++)
      uart_string_write(" ");
    sprintf(buff, "%s\n", Lfname);
    uart_string_write(buff);
#else
    uart_string_write("\n");
#endif
  }
  sprintf(buff, "\r%4u File(s),%10lu bytes total\n\r%4u Dir(s)", s1, p1, s2);
  uart_string_write(buff);

  if (f_getfree("/", (DWORD*)&p1, &fs) == FR_OK)
  {
    sprintf(buff, ",%10luK bytes free \r\n", p1 * fs->csize / 2);
    uart_string_write(buff);
  }

}

static void file_copy()
{
  FRESULT res;
  UINT bw = 0;
  UINT br = 0;
  int total_read = 0;
  long total_write = 0;

  uart_string_write("Open file to read from \r\n");
  res = f_open(&fpr, "filecopy.log", FA_OPEN_EXISTING | FA_READ);
  if (res)
  {
    sprintf(buff, "Error opening file with error %d \r\n", res);
    uart_string_write(buff);
    return;
  }

  uart_string_write("Open file to write to \r\n");
  res = f_open(&fpw, "flock.log", FA_CREATE_ALWAYS | FA_WRITE);
  if (res)
  {
    f_close(&fpr);
    sprintf(buff, "Error opening file with error %d \r\n", res);
    uart_string_write(buff);
    return;
  }
  uart_string_write("\nCopying ... \r\n");

  total_write = 0;

  for (;;)
  {
    res = f_read(&fpr, readbuf, sizeof(readbuf), &br);
    if (res || br == 0) break; //error or eof
    total_read += br;

    res = f_write(&fpw, readbuf, br, &bw);
    total_write += bw;
    if (res || bw < br)  break; // error of disk full
  }
  sprintf(buff, "Bytes copied = %ld \r\n", total_write);
  uart_string_write(buff);

  res = f_close(&fpr);
  if (res)
    uart_string_write("Error closing read file \r\n");
  res = f_close(&fpw);
  if (res)
    uart_string_write("Error closing write file \r\n");
}

static int file_write_demo(const char *filename, int sz)
{
  FRESULT res;
  int wcnt = 0, i = 0;
  unsigned int bw;
  int part = 0; /* handle partial reads */

  res = f_open(&fpw, filename, FA_WRITE | FA_CREATE_ALWAYS);
  if (res)
  {
    sprintf(buff, "Unable to open file %s for writing [Error:%d]\r\n", filename, res);
    uart_string_write(buff);
    return -1;
  }

  for (i = 0; i < sz; i ++)
  {
    /* Clean read buffer */
    memset(readbuf, i + 1, WRITE_CHUNK_SZ);
    do
    {
      res = f_write(&fpw, (UNS_8 *)readbuf + part, WRITE_CHUNK_SZ - part, &bw);
      part += bw;
      if (res || !bw)
      {
        f_close(&fpw);
        return -2;
      }
    }
    while (part < WRITE_CHUNK_SZ);
    wcnt += part;
    part = 0;
  }

  f_close(&fpw);
  return wcnt;
}

static void dump_buffer(const UNS_8 *buf, int sz)
{
  int i;
  uart_string_write("\r\n           0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F \r\n");
  uart_string_write("==========================================================");
  for (i = 0; i < sz; i ++)
  {
    if (!(i & 0xF))
    {
      sprintf(buff, "\r\n%08X : ", i);
      uart_string_write(buff);
    }
    sprintf(buff, "%02X ", buf[i]);
    uart_string_write(buff);
  }
  uart_string_write("\r\n");
}

static int file_read_demo(const char * filename)
{
  FRESULT res;
  int rcnt = 0;
  unsigned int br;
  int part = 0; /* handle partial reads */

  res = f_open(&fpr, filename, FA_READ);
  if (res)
  {
    sprintf(buff, "Unable to open file %s for reading [Error:%d]\r\n", filename, res);
    uart_string_write(buff);
    return -1;
  }
  sprintf(buff, "read buff at %p\r\n", readbuf);
  uart_string_write(buff);

  /* Clean read buffer */
  memset(readbuf, 0, READ_CHUNK_SZ);
  //while (!(res = f_read(&fpr, (UNS_8 *)readbuf + part, READ_CHUNK_SZ - part, &br)) && br)
  while (((res = f_read(&fpr, (UNS_8 *)readbuf + part, READ_CHUNK_SZ - part, &br)) == 0) && br)
  {
    part += br;
    if (part < READ_CHUNK_SZ)
      continue;
    rcnt += part;
    dump_buffer(readbuf, part);
    part = 0;
    memset(readbuf, 0, READ_CHUNK_SZ);
  }

  if (part)
  {
    rcnt += part;
    dump_buffer(readbuf, part);
  }
  f_close(&fpr);
  return res ? -res : rcnt;
}
/***********************************************************************
 *
 * Function: c_entry
 *
 * Purpose: Application entry point from the startup code
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns 1
 *
 * Notes: None
 *
 **********************************************************************/
int c_entry(void)
{
  int err = 0;
  BYTE res;
  char input[512];
  int j;
  unsigned long i = 0;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup MCI card detect */
  mci_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  enable_irq();

  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  g_uartdev = uart_open((void *) UART_BASE, 0);
  if (g_uartdev != 0)
  {

    for (;;)
    {
      //uart_string_write("Fatfs test module \r\n");
      //uart_string_write("Enter option \r\n");
      uart_string_write("------------------------ \r\n");
      uart_string_write("SD/MMC DEMO OPTIONS \r\n");
      uart_string_write("------------------------ \r\n");
      uart_string_write("Initialize disk and then mount the filesystem \r\n");
      uart_string_write("Enter 'di' to initialize disk \r\n");
      uart_string_write("Available disk options - \r\n dr: Disk read \r\n dw: Disk write \r\n");
      uart_string_write("Enter 'fi' to mount Filesystem \r\n");
      uart_string_write("Available filesystem options -\r\n fo: File open \r\n fr: File read \r\n fw: File write \r\n");
      uart_string_write(" fl: List files \r\n fc: File copy \r\n fs: file scan \r\n fd: file dump \r\n");
      uart_string_write("------------------------ \r\n");
      uart_string_write("Select option: ");

      uart_string_read(input, sizeof(input));

      switch (*input)
      {
        case 'd':
          switch (input[1])
          {
            case 'i': //initialize
              if ((err = disk_initialize(drv)) & STA_NODISK)
              {
                uart_string_write("Insert disk \r\n");
                /* Could not initialize disk */
                break;
              }
              if (err & STA_NOINIT)
              {
                uart_string_write("Error initing disk \r\n");
                break;
              }
              di = 1;
              uart_string_write("Disk initialized \r\n");
              uart_string_write("No you can proceed with raw disk accesses \r\n");
              uart_string_write("Mount the file system if you wish to access it with filesystem \r\n");
              break;

            case 'r':
              raw_read = 1;
              i = 512 * 16;// 8 K * 64*512 = 256MB
              j = 0;
              while (i--)
              {
                res = disk_read(0, readbuf, j * 64, 64);
                if (res != RES_OK)
                  uart_string_write("Read error \r\n");
                j++;
              }
              uart_string_write("Read completed \r\n");
              break;

            case 'w':
              i = 512 * 16; // 8k * 64*512 = 256
              j = 0;
              while (i--)
              {
                res = disk_write(0, readbuf, j * 64, 64);
                if (res != RES_OK)
                  uart_string_write("Write error \r\n");
                j++;
              }
              uart_string_write("Write completed \r\n");
              break;
            case '\0':
              uart_string_write("Only d has no meaning \r\n");
              break;

            default:
              uart_string_write("no such extended d option \n");
              break;
          }

          break;

        case 'f':
          if (!di)
          {
            uart_string_write("Initialize disk first \r\n");
            break;
          }
          switch (input[1])
          {
            case 'i':
              if (FR_OK != f_mount(0, &Fatfs[0]))
              {
                sprintf(buff, "Could not mount volume !!! error = %d \r\n", err);
                uart_string_write(buff);
                return RES_ERROR;
              }

              uart_string_write("Disk mounted \r\n");
              break;
            case 's': //scan
              scan_files("");
              break;

            case 'l': //list
              list_files("");
              break;

            case 'g': //chdir
              uart_string_write("Change dir \r\n");
              break;

            case 'o': //openfile

              uart_string_write("File open: \r\n");

              err = f_open(&fp, "file.log", FA_OPEN_EXISTING | FA_READ);
              if (err)
              {
                sprintf(buff, "Error opening file with error %d \r\n", err);
                uart_string_write(buff);
              }

              uart_string_write("File open succesfull \r\n");
              break;

            case 'd':
              sprintf(buff, "Demo: File dump [%s]\r\n", DEMO_FILE_NAME);
              uart_string_write(buff);
              err = file_read_demo(DEMO_FILE_NAME);
              if (err < 0)
              {
                sprintf(buff, "Dumping file %s failed with error number %d\r\n", DEMO_FILE_NAME, -err);
                uart_string_write(buff);
              }
              else
              {
                sprintf(buff, "Successfully dumped %d bytes from file %s\r\n", err, DEMO_FILE_NAME);
                uart_string_write(buff);
              }

              break;

            case 'c':
              file_copy();
              break;

            case 'w':
              sprintf(buff, "Demo: File write [%s]\r\n", DEMO_FILE_NAME);
              uart_string_write(buff);
              err = file_write_demo(DEMO_FILE_NAME, WRITE_CNT);
              if (err < 0)
              {
                sprintf(buff, "Writing file %s failed with error number %d\r\n", DEMO_FILE_NAME, -err);
                uart_string_write(buff);
              }
              else
              {
                sprintf(buff, "Successfully Written %d blocks of size %d "
                        "bytes [Total = %d bytes] to file %s\r\n", WRITE_CNT, WRITE_CHUNK_SZ, err, DEMO_FILE_NAME);
                uart_string_write(buff);
              }
              break;

            case '\0': //Only 'f' has no meaning
              uart_string_write("Enter the extension for f \r\n");
              break;

            default:
              uart_string_write("Unknown option \r\n");
              break;
          }
          break;

        default :
          uart_string_write("Unknown option \r\n");
          break;
      }
    }

    //uart_close(g_uartdev);
  }

  return 1;
}

