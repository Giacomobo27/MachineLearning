#include <cmath>
#include "image.h"

using namespace std;

// HW1 #1
// float x,y: inexact coordinates
// int c: channel
// returns the nearest neighbor to pixel (x,y,c)
float Image::pixel_nearest(float x, float y, int c) const
  {
  // Since you are inside class Image you can
  // use the member function pixel(a,b,c)
  
  // TODO: Your code here
   int i=x-(int)x<0.5?x:x+1;
   int j=y-(int)y<0.5?y:y+1;
  
  
  return pixel(i,j,c);
  }

// HW1 #1
// float x,y: inexact coordinates
// int c: channel
// returns the bilinearly interpolated pixel (x,y,c)
float Image::pixel_bilinear(float x, float y, int c) const
  {
  // Since you are inside class Image you can
  // use the member function pixel(a,b,c)

  // TODO: Your code here
  float q1=0,q2=0,q3=0,q4=0;
  float v1=clamped_pixel((int) x,(int)y,c); 
  float v2=clamped_pixel((int)x+1,(int) y,c);
  float v3=clamped_pixel((int) x,(int) y+1,c);
  float v4=clamped_pixel((int) x+1,(int)y+1,c);
  if (x<0.0 || x+1>(float)this->w) {
      q1=v1*((int)y+1-y);
      q3=v3*(y-(int)y);
  }
  if (y<0.0 || y+1>(float)this->h) {
      q1=v1*((int)x+1-x);
      q2=v2*(x-(int)x);
  }
  if ((x<0.0 || x+1>(float)this->w) && (y<0.0 || y+1>(float)this->h)) {
      q1=v1;
      q2=0; 
      q3=0;
  }
  if (!(x<0.0 || x+1>(float)this->w) && !(y<0.0 || y+1>(float)this->h)){
      q1=v1*((int)x+1-x)*((int)y+1-y);
      q2=v2*(x-(int)x)*((int)y+1-y);
      q3=v3*((int)x+1-x)*(y-(int)y);
      q4=v4*(x-(int)x)*(y-(int)y);
  }
  //if (x-7.625<0.0001 && y<0 )printf("%f,%f,%f,%f\n",q1,q2,q3,q4);
  
  return q1+q2+q3+q4;
  }

// HW1 #1
// int w,h: size of new image
// const Image& im: input image
// return new Image of size (w,h,im.c)
Image nearest_resize(const Image& im, int w, int h)
  {
  Image ret(w,h,im.c);
  
  // TODO: Your code here
  for (int col=0;col<im.c;col++) {
          for (int i=0;i<w;i++) {
	          for (int j=0;j<h;j++) {
			  float x,y;
			  x=(i+0.5)*im.w/w-0.5;
			  y=(j+0.5)*im.h/h-0.5;
			  ret.pixel(i,j,col)=im.pixel_nearest(x,y,col);
		  }
	  }
  }
  return ret;
  }


// HW1 #1
// int w,h: size of new image
// const Image& im: input image
// return new Image of size (w,h,im.c)
Image bilinear_resize(const Image& im, int w, int h)
  {
  
 Image ret(w,h,im.c);

  for (int col=0;col<im.c;col++) {
          for (int i=0;i<w;i++) {
	          for (int j=0;j<h;j++) {
			  float x,y;
			  x=(i+0.5)*im.w/w-0.5;
			  y=(j+0.5)*im.h/h-0.5;
			 // 			  if (i==766 && j==0) printf("%f,%f\n",x,y);
			  ret.pixel(i,j,col)=im.pixel_bilinear(x,y,col);

		  }
	  }
  }
  return ret;
  }
