#include <stdio.h>
#include <stdlib.h>

#define SGI_RGB_HEADER_SIZE 511

int main (int argc, char *argv[])
{
  FILE *infile;
  FILE *outfile;
  int ret;
  long infile_size;
  int counter;
  char buffer[255];

  if (argc < 3 || argc > 3)
    {
      printf ("Usage: rgb2raw infile outfile\n");
      printf
	("Convert an image from the SGI RGB image format to RGB565 framebuffer format.\n");
      printf ("Armin Schlegel (C), 2014\n");
      exit (1);
    }

  infile = fopen (argv[1], "rb");
  if (infile == NULL)
    {
      printf ("Error: Cannot open %s\n", argv[1]);
      exit (2);
    }

  outfile = fopen (argv[2], "w+b");
  if (outfile == NULL)
    {
      printf ("Error: Cannot open %s\n", argv[2]);
      exit (3);
    }

  fseek(infile, 0, SEEK_END);
  infile_size = ftell(infile) - SGI_RGB_HEADER_SIZE;
  printf("infile_size: %i\n", infile_size);

  rewind(infile);

  ret = fseek(infile, SGI_RGB_HEADER_SIZE, SEEK_SET);
  if(ret != NULL)
  {
	printf("ERROR: %s not big enough\n", argv[1]);
	exit(4);
  }

  for(counter = 0; counter < infile_size; counter++)
  {
	fread(&buffer[counter], 1, 1, infile);
	printf("0x%.2X\t", buffer[counter]);
	if(counter % 3 == 0)
		printf("\n");
  }

  return 0;
}
