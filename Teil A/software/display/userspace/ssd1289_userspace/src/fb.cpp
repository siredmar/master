/*
 *  userspace framebuffer-to-lcd driver
 *
 *  repeatedly copies the /dev/fb0 content to an SPI attached MI0283QT2 lcd display
 *
 *  as the lcd data transfer is very slow, we keep a local copy of the display content
 *  and compare it to the current framebuffer content. if any changes are encountered,
 *  it is transmitted in terms of full display rows (single pixels would add severe
 *  adressing overhead). As this is no framebuffer device, we don√'t have any hints
 *  when and what might have changed, and have to compare the full screen repeatedly.
 *
 *  compared to a kernel driver, this has some cons and pros:
 *  cons:
 *  - a true framebuffer driver gets some metainformation what pixels are modified if
 *    copy, fill or blit operations are called. however, for modern UI programs this is
 *    usually very coarse.
 *  - a true framebuffer driver replacing the SPI driver would save severe CPU memory
 *    moving and syscall overhead.
 *
 *  pros:
 *  - by copying the native framebuffer, hardware extensions that modify it should still
 *    work, like GPU rendering or hardware video decompression
 *  - it runs asynchronous to the real framebuffer, thus it nicely shares CPU cycles with
 *    user processes and coerces subsequent draw operations without stalling drawing
 *    processes by the slow lcd refresh.
 */

#include <stdint.h>
#include <stdio.h>
#include "MI0283QT2.h"

MI0283QT2 lcd("/dev/spidev0.1");

int main() 
{
  // init display 
  lcd.init(); 
  int w=lcd.getWidth();
  int h=lcd.getHeight();

  // open the source framebuffer device 
  FILE* fb=fopen("/dev/fb0","rb"); 

  // allocate two buffers for tracking the source framebuffer changes
  uint16_t buf_display [w*h*2]; // the current display content
  uint16_t buf_source  [w*h*2]; // the source framebuffer content

  // run forever...
  while(true){
   fseek(fb, 0, SEEK_SET);         // rewind source framebuffer
   fread(buf_source, 1, w*h*2, fb);  // read complete source framebuffer

   // statistics
   uint32_t changed=0; // how many pixels have changed since last refresh 
   uint32_t  drawed=0; // how many pixel actualy where transmitted since last refresh

   // state for merging adjacent changes into one transmission
   uint32_t i=0;           // current pixel index 
   bool drawing=false;     // if pixels are currently transmitted 
   uint32_t lastChange=0;  // index of first pixel of this transmission

   // for all pixels on screen...
   for(int y=0; y<h; y++){
    for(int x=0; x<w; x++){
      uint16_t color=buf_source[i]; // get this pixels source color
      if (color!=buf_display[i]){ 
       // the pixel has changed
       changed++;
       lastChange=i; // store last change index
       if(!drawing){
        // whe are not drawing, so start a run here.
        // we only draw full rows:
        // set display area to the rest of the screen, including this row
        lcd.setArea(0,y,w-1,h-1);
        lcd.drawStart();
        // re-transmit the unchanged pixels of current row up to here
        for(int j=0; j<x; j++) 
         lcd.draw(buf_display[y*w+j]);
        drawing=true;
       }
      }

      if(drawing){
       // where in a transmission run. 
       lcd.draw(color);     // send pixel
       buf_display[i]=color;// store change 
       drawed++;
       // end transmission, if the was no change for some time
       if(i>lastChange+400) {
        // stop drawing, lcd actually gets updated.
        lcd.drawStop();
        drawing=false;
       }
      }

      i++;
    }
   }
   if(changed>0) printf("Pixels changed: %i, drawed: %i\n",changed,drawed);
   usleep(40000L); // sleep for 40ms
  }

  // actually we never get here
  fclose(fb);
  return 0;
}


