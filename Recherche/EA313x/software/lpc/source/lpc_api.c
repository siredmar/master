/***********************************************************************
 * $Id:: lpc_api.c 745 2008-05-13 19:59:29Z pdurgesh                   $
 *
 * Project: Standard API
 *
 * Description:
 *  This file implements non hardware specific I/O system
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

#include "lpc_api.h"

/* Private io system table */
STATIC API_TABLE_T api[MAX_API_TABLE];

/* Private methods */
STATIC STATUS api_remove_device(INT_32 id);
STATIC INT_32 api_find_device(INT_32 id);
STATIC INT_32 api_find_empty(void);
STATIC STATUS api_add_device(INT_32 id,
                             void*  open,
                             void*  close,
                             void*  read,
                             void*  write,
                             void*  ioctl);

/* State variable for init */
STATIC INT_32 api_is_init = FALSE;

/* Max size of the device table */
#define MAX_API_DEVS NELEMENTS(api)

/***********************************************************************
*
* Function: lpc_api_init
*
* Purpose:
*     To initialize the api system
*
* Processing:
*     This function clears the api system table and marks it as
*     initialized. Once the table has been initialized the devices can
*     be bound to the io system and make use of the common API.
*
* Parameters:
*     config - Not used
*
* Outputs:
*     None
*
* Returns:
*     None
*
* Notes:
*     See lpc_api.h for structure definitions
*
************************************************************************/

void lpc_api_init(void* cfg)
{
  UNS_32 idx = 0;
  CHAR*  dst = NULL;

  /* Clear the table on startup */
  if (api_is_init == FALSE)
  {
    /* Clear the driver table */
    dst = (CHAR*) & api[0];
    for (idx = 0; idx < (sizeof(API_TABLE_T) * MAX_API_DEVS);
         idx++)
    {
      *dst++ = 0;
    }
  }

  /* Set the init flag */
  api_is_init = TRUE;
}

/***********************************************************************
*
* Function: lpc_api_register
*
* Purpose:
*     To register a device with the system
*
* Processing:
*     This funtion is used to bind a device to the system. Once bound
*     the device can make use of the common API layer.
*
* Parameters:
*     id    - device id.
*     open  - driver open method
*     close - driver close method
*     read  - driver read method
*     write - driver write method
*     ioctl - driver io control method
*
* Outputs:
*     None
*
* Returns:
*     None
*
* Notes:
*     See lpc_api.h for structure definitions
*
************************************************************************/

STATUS lpc_api_register(INT_32 id,
                        void*  open,
                        void*  close,
                        void*  read,
                        void*  write,
                        void*  ioctl)

{
  INT_32 status = _NO_ERROR;

  /* Add the device to the io system */
  status = api_add_device
           (id, open, close, read, write, ioctl);

  /* Return the status of the registration */
  return (status);
}

/***********************************************************************
*
* Function: lpc_open
*
* Purpose:
*     Connects to a system device
*
* Processing:
*     This routine calls the associated open method in the io subsystem
*     array. If the device asscoiated with the name is not registered an
*     error -1 is returned. If the device is registered and not already
*     opened a file descriptor that uniquely identifies this device is
*     returned.
*
* Parameters:
*     id   - Device id to open
*     arg  - Options used to open the device
*
* Outputs:
*     None
*
* Returns:
*     device file decriptor
*     -1 if the device does not exist
*
* Notes:
*     See sma_iosys.h for structure definitions
*
************************************************************************/

INT_32 lpc_open(INT_32 id,
                INT_32 arg)
{
  INT_32 fd     = 0;
  INT_32 devid  = 0;

  /* Get the index for the device */
  if ((fd = api_find_device(id)) == -1)
  {
    /* The device is not in the table */
    return (_ERROR);
  }

  /* Open the device - returns param to be passed into driver */
  devid = (*(PFI)(api[fd].driver.open))(id, arg);

  /* Sanity check */
  if (devid == 0)
  {
    /* Error opening the device */
    return (-1);
  }

  /* Save the param to be used by the device driver */
  api[fd].devid  = devid;

  /* Update the device state as opened */
  api[fd].opened = TRUE;

  /* Return the device file descriptor */
  return (fd);
}

/***********************************************************************
*
* Function: lpc_close
*
* Purpose:
*     closes a session with an device driver
*
* Processing:
*     This routine marks the device as closed and then calls the
*     associated close method at the device driver layer to disable
*     the hardware.
*
* Parameters:
*     fd - file descriptor of the device to be closed
*
* Outputs:
*     None
*
* Returns:
*     _NO_ERROR if the device has been closed
*     _ERROR if the device could not be closed
*
* Notes:
*     See lpc_api.h for structure definitions
*
************************************************************************/

STATUS lpc_close(INT_32 fd)
{
  STATUS status = _NO_ERROR;

  /* Sanity check */
  if (fd > MAX_API_DEVS)
  {
    /* Invalid file descriptor */
    return (_ERROR);
  }

  /* Is the device opened */
  if (api[fd].opened != TRUE)
  {
    /* Device is not opened */
    return (_ERROR);
  }

  /* Close the device at the driver layer */
  status = (*(PFI)(api[fd].driver.close))(api[fd].devid);

  /* If the device has been closed then mark it as so */
  if (status == _NO_ERROR)
  {
    /* Mark the device as closed */
    api[fd].opened = FALSE;

    /* Remove the device from the control table */
    (void) api_remove_device(fd);
  }

  /* Return the close call status */
  return (status);
}


/***********************************************************************
*
* Function: lpc_read
*
* Purpose:
*     reads data from a registered api system device.
*
* Processing:
*     This routine reads data from a registered api device by using the
*     callback method that has been bound to a driver. If the
*     device is not registered -1 is returned. If the device is
*     registered the user can pass in a buffer and a max number of
*     bytes for the driver to use.
*
* Parameters:
*     fd        - device file descriptor.
*     buffer    - data buffer.
*     max_bytes - max number of bytes to read.
*
* Outputs:
*     None
*
* Returns:
*     -1 if the device is not registered.
*     actual number of bytes read.
*
* Notes:
*     See lpc_api.h for structure definitions
*
************************************************************************/

INT_32 lpc_read(INT_32 fd,
                CHAR*  buffer,
                INT_32 max_bytes)
{
  INT_32 n_bytes = 0;

  /* Sanity check */
  if (fd > MAX_API_DEVS)
  {
    /* Non valid file descriptor */
    return (-1);
  }

  /* Method not bound to system */
  if (api[fd].driver.read == NULL)
  {
    return (0);
  }

  /* Get the data from the device */
  n_bytes = (*(PFI)(api[fd].driver.read))
            (api[fd].devid, buffer, max_bytes);

  /* Return the number of bytes read */
  return (n_bytes);
}

/***********************************************************************
*
* Function: lpc_write
*
* Purpose:
*     write data to a registered device
*
* Processing:
*     This routine writes data to a registered api device by using the
*     callback method that has been bound to a driver. If the device
*     is not registered -1 is returned. If the device is registered a
*     generic pointer and the number of bytes represented by the pointer
*     are being passed to the
*
* Parameters:
*     fd      - device file descriptor.
*     buffer  - generic arg.
*     n_bytes - number of bytes contained in the arg.
*
* Outputs:
*     None
*
* Returns:
*     -1 if the write operation failed
*     number of bytes written.
*
* Notes:
*     See sma_iosys.h for structure definitions
*
************************************************************************/

INT_32 lpc_write(INT_32 fd,
                 CHAR*  buffer,
                 INT_32 n_bytes)
{
  INT_32 actual_bytes = 0;

  /* Sanity check */
  if (fd > MAX_API_DEVS)
  {
    /* Non valid file descriptor */
    return (-1);
  }

  /* Method not bound to system */
  if (api[fd].driver.write == NULL)
  {
    return (0);
  }

  /* Write the data to the device */
  actual_bytes = (*(PFI)(api[fd].driver.write))
                 (api[fd].devid, buffer, n_bytes);

  /* Return the number of bytes written */
  return (actual_bytes);
}

/***********************************************************************
*
* Function: lpc_ioctl
*
* Purpose:
*     device io control routine
*
* Processing:
*     This routine controls the associated device driver via the
*     callback method that has been bound to a driver. If the device
*     is not registered -1 is returned else return code by the driver
*     ioctl is returned.
*
* Parameters:
*     fd  - device file descriptor.
*     cmd - command to execute.
*     arg - generic arg.
*
* Outputs:
*     None
*
* Returns:
*     _ERROR if the operation failed
*     return code of the ioctl associated with the io system.
*
* Notes:
*     See lpc_api.h for structure definitions
*
************************************************************************/

STATUS lpc_ioctl(INT_32 fd,
                 INT_32 cmd,
                 INT_32 arg)
{
  STATUS status = _ERROR;

  /* Sanity check */
  if (fd > MAX_API_DEVS)
  {
    /* Non valid file descriptor */
    return (status);
  }

  /* Perform the requested operation on the device driver */
  status = (*(PFI)(api[fd].driver.ioctl))(api[fd].devid, cmd, arg);

  /* return the status */
  return (status);
}

/***********************************************************************
*
* Function: api_find_device
*
* Purpose:
*     To find a device using a numerical representation
*
* Processing:
*     Search the device table for an id and return return the
*     index of the device in the table.
*
* Parameters:
*     id - device id.
*
* Outputs:
*     None
*
* Returns:
*     index of the device bound to the id
*     -1 if the device does not exist
*
* Notes:
*     See lpc_api.h for structure definitions
*
************************************************************************/

STATIC INT_32 api_find_device(INT_32 id)
{
  INT_32 fd = 0;

  /* Find the device id in the table */
  for (fd = 0; fd < MAX_API_DEVS; fd++)
  {
    /* Look for device id in the table */
    if (api[fd].id == id)
    {
      /* Return index into the routing table */
      return (fd);
    }
  }

  /* If we have got to here the device is not in the table */
  return (-1);
}

/***********************************************************************
*
* Function: api_find_empty
*
* Purpose:
*     To find a vacant table entry
*
* Processing:
*     Search the device table for a vacant space and return the
*     index in the table.
*
* Parameters:
*     None
*
* Outputs:
*     None
*
* Returns:
*     index of the device bound to the name
*     -1 if the device does not exist
*
* Notes:
*     See lpc_api.h for structure definitions
*
************************************************************************/

STATIC INT_32 api_find_empty(void)
{
  UNS_32 fd = 0;

  /* Find the name on the table */
  for (fd = 0; fd < MAX_API_DEVS; fd++)
  {
    /* Look for device id in the table */
    if (api[fd].id == 0)
    {
      /* Return unused index into the table */
      return (fd);
    }
  }

  /* If we have got to here the table is full */
  return (-1);
}

/***********************************************************************
*
* Function: api_add_device
*
* Purpose:
*     To add a device to the api table
*
* Processing:
*     This function checks for a device id collision in the api system.
*     If the id is valid it looks for a vacant entry. If the table is not
*     full it binds itself to the api system.
*
* Parameters:
*     id    - Device id
*     open  - Driver open method
*     close - Driver close method
*     read  - Driver read method
*     write - Driver write method
*     ioctl - Driver ioctl method
*
* Outputs:
*     None
*
* Returns:
*     _NO_ERROR if the device is added to the io system.
*     _ERROR if the table is full or the name is not valid.
*
* Notes:
*     See lpc_api.h for structure definitions
*
************************************************************************/

STATIC STATUS api_add_device(INT_32 id,
                             void*  open,
                             void*  close,
                             void*  read,
                             void*  write,
                             void*  ioctl)
{
  INT_32 idx = 0;

  /* Check for a name space collision */
  if (api_find_device(id) != -1)
  {
    /* We have found a collision */
    return (_ERROR);
  }

  /* Add the device to the table */
  if ((idx = api_find_empty()) == -1)
  {
    /* table is full */
    return (_ERROR);
  }

  /* Add the device the table  */
  api[idx].id           = id;
  api[idx].fd           = idx;
  api[idx].driver.open  = (PFI)open;
  api[idx].driver.close = (PFI)close;
  api[idx].driver.read  = (PFI)read;
  api[idx].driver.write = (PFI)write;
  api[idx].driver.ioctl = (PFI)ioctl;

  /* If we have made it to here the device has been added */
  return (_NO_ERROR);
}

/***********************************************************************
*
* Function: api_remove_device
*
* Purpose:
*     To remove a device from the api table
*
* Processing:
*     This function finds the table entry that is associated with the
*     devid. Once the entry is found it is cleared which will set it to
*     the idle state. When a table entry is in the idle state a new
*     device my use this entry to bind itself to the system.
*
* Parameters:
*     id - Device id
*
* Outputs:
*     None
*
* Returns:
*     _NO_ERROR on success
*     _ERROR on error
*
* Notes:
*     See lpc_api.h for structure definitions
*
************************************************************************/

STATIC STATUS api_remove_device(INT_32 id)
{
  INT_32 fd  = 0;
  INT_32 idx = 0;
  CHAR*  dst = NULL;

  /* Find the device in the device system */
  if ((fd = api_find_device(id)) == -1)
  {
    /* The device is not in the table */
    return (_ERROR);
  }

  /* Remove the device from the table */
  dst = (CHAR*) & api[fd];
  for (idx = 0; idx < sizeof(API_TABLE_T); idx++)
  {
    *dst++ = 0;
  }

  /* Return OK */
  return (_NO_ERROR);
}

/* EOF */
