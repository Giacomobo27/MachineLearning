#include "image.h"

// HW0 #1
// const Image& im: input image
// int x,y: pixel coordinates
// int ch: channel of interest
// returns the 0-based location of the pixel value in the data array
int pixel_address(const Image& im, int x, int y, int ch)
  {
  // TODO: calculate and return the index
   
      return ch*im.w*im.h+y*im.w+x;
  }

// HW0 #1
// const Image& im: input image
// int x,y,ch: pixel coordinates and channel of interest
// returns the value of the clamped pixel at channel ch
float get_clamped_pixel(const Image& im, int x, int y, int ch)
  {
  // TODO: clamp the coordinates and return the correct pixel value
      if (x<0) x=0;
      else if (x>im.w) x=im.w-1;
      if (y<0) y=0;
      else if (y>im.h) y=im.h-1;
      if (ch<0) ch=0;
      else if (ch>im.c) ch=im.c-1;
      return im.data[pixel_address(im,x,y,ch)];
  }


// HW0 #1
// Image& im: input image
// int x,y,ch: pixel coordinates and channel of interest
void set_pixel(Image& im, int x, int y, int c, float value)
  {
  // TODO: Only set the pixel to the value if it's inside the image
      if (x>=0 && x<im.w && y>=0 && y<im.h && c>=0 && c<im.c)
          im.data[pixel_address(im,x,y,c)]=value;
  }



// HW0 #2
// Copies an image
// Image& to: destination image
// const Image& from: source image
void copy_image(Image& to, const Image& from)
  {
  // allocating data for the new image
  to.data=(float*)calloc(from.w*from.h*from.c,sizeof(float));
  to.c=from.c;
 
  // TODO: populate the remaining fields in 'to' and copy the data
  // You might want to check how 'memcpy' function works
  to.w=from.w;
  to.h=from.h;
  memcpy(to.data,from.data,to.h*to.w*to.c*sizeof(float));
 
  }

	

