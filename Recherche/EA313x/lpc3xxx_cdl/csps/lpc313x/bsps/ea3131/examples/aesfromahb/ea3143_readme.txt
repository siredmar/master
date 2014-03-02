-------------------------------------------------------------------------------
The aesfromahb project is intended for LPC3143 and LPC3154 customers to program
AES keays in the poly-fuses. LPC3141 & LPC3152 customers can use this as an
example on how to program poly-fuses to store customer specific data. 

For LPC3143 & LPC3154 customers "encrypt.key" file is provided in this directory
to aid customers to use this file with "-k" option with SecImgCr.exe tool to
create secure images. The tool creates otp_demo.rom file in the current directory
which is the signed & encrypted image.

To create secure images use the following commands:

- Secure DFU image
	secimgcr -ad -k encrypt.key -K 0 -o ./ -i otp_demo.bin
- Secure UART image	
	secimgcr -au -k encrypt.key -o ./ -i otp_demo.bin
- Secure SPI-NOR image	
	secimgcr -as -k encrypt.key -o ./ -i otp_demo.bin
- Secure NAND image	
	secimgcr -an -k encrypt.key -o ./ -i otp_demo.bin
- Secure SD/MMC image	
	secimgcr -am -k encrypt.key -o ./ -i otp_demo.bin

------------------------------------------------------------------------------
Usage: SecImgCr [options] inputfile
 -pd             Creates image for plaintext_dfu boot mode.
 -pu             Creates image for plaintext_uart boot mode.
 -ad             Creates image for aes_encrypted_dfu boot mode.
 -au             Creates image for creates image for aes_encrypted_uart
                 boot mode.
 -as             Creates image for aes_encrypted_spinor boot mode.
 -an             Creates image for aes_encrypted_nand boot mode.
 -am             Creates image for aes_encrypted_mci boot mode.(SD/MMC card)
 -o <dir>        Absolute path (w/o spaces) of the output directory.
 -k <file>       Absolute path (w/o spaces) and name of the file
                 containing 128-bit AES key.
                 Required for -ad, -au, -as and -an options.
 -K <index>      Index [range 0 - 63] within an array of pre-built TEA keys.
                 Required for -pd and -ad options.
 -i <inputfile>  Absolute path and name of the plaintext image to be signed.

<inputfile> is treated as the LPC31xx image. Signed & encrypted images are creat
ed in the specified output <dir>. It is assumed that the file1 contains uninitia
lized header that will be overwritten with signature data.
------------------------------------------------------------------------------


