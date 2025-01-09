#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>

#include "image.h"

using namespace std;

// HW0 #3
// const Image& im: input image
// return the corresponding grayscale image
Image rgb_to_grayscale(const Image& im)
  {
  assert(im.c == 3); // only accept RGB images
  Image gray(im.w,im.h,1); // create a new grayscale image (note: 1 channel)
  for (int i=0;i<im.w*im.h;i++) {
      int mat=im.w*im.h;
      gray.data[i]=0.299*im.data[i]+0.587*im.data[mat+i]+0.114*im.data[2*mat+i];
  }
      
  // TODO: calculate the pixels of 'gray'
  
 
  
  return gray;
  }



// Example function that changes the color of a grayscale image
Image grayscale_to_rgb(const Image& im, float r, float g, float b)
  {
  assert(im.c == 1);
  Image rgb(im.w,im.h,3);
  
  for(int q2=0;q2<im.h;q2++)for(int q1=0;q1<im.w;q1++)
    {
    rgb(q1,q2,0)=r*im(q1,q2);
    rgb(q1,q2,1)=g*im(q1,q2);
    rgb(q1,q2,2)=b*im(q1,q2);
    }
  
  return rgb;
  }




// HW0 #4
// Image& im: input image to be modified in-place
// int c: which channel to shift
// float v: how much to shift
void shift_image(Image& im, int c, float v)
  {
  assert(c>=0 && c<im.c); // needs to be a valid channel
  
  // TODO: shift all the pixels at the specified channel
  for (int i=0;i<im.w*im.h;i++) im.data[i+c*im.w*im.h]+=v;
  
  
  }

// HW0 #8
// Image& im: input image to be modified in-place
// int c: which channel to scale
// float v: how much to scale
void scale_image(Image& im, int c, float v)
  {
  assert(c>=0 && c<im.c); // needs to be a valid channel
  
  // TODO: scale all the pixels at the specified channel
  for (int i=0;i<im.w*im.h;i++)
      im.data[c*im.w*im.h+i]*=v;
  
  }


// HW0 #5
// Image& im: input image to be modified in-place
void clamp_image(Image& im)
  {
  // TODO: clamp all the pixels in all channel to be between 0 and 1
  
      for (int i=0;i<im.c;i++) {
          for (int j=0;j<im.w*im.h;j++) {
              if (im.data[i*im.w*im.h+j]<0) im.data[i*im.w*im.h+j]=0;
              if (im.data[i*im.w*im.h+j]>1) im.data[i*im.w*im.h+j]=1;
          }
      }
  
  }

// These might be handy
float max(float a, float b, float c)
  {
  return max({a,b,c});
  }

float min(float a, float b, float c)
  {
  return min({a,b,c});
  }


// HW0 #6
// Image& im: input image to be modified in-place
void rgb_to_hsv(Image& im)
  {
  assert(im.c==3 && "only works for 3-channels images");
  
  // TODO: Convert all pixels from RGB format to HSV format
  int mat=im.w*im.h;
  for (int i=0;i<mat;i++) {
      float V=max(im.data[i],im.data[i+mat],im.data[i+2*mat]);
      float m=min(im.data[i],im.data[i+mat],im.data[i+2*mat]);
      float C=V-m; float S=0;
      if (V>0) S=1-m/V;
      float h;
      if (C>0) {
      /*
          if (i==0) { 
              puts("ok");
              printf("%s\n",(bool)(V-im.data[i]<0.0001)? "true":"false");
                            printf("%s\n",(bool)(V-im.data[i+mat]<0.0001)? "true":"false");
                                          printf("%s\n",(bool)(V-im.data[i+2*mat]<0.0001)? "true":"false");
          }
          */
          if ((bool)(V-im.data[i]<0.0001)) h=(im.data[i+mat]-im.data[i+2*mat])/C; 
          else if ((bool)(V-im.data[i+mat]<0.0001)) h=(im.data[i+2*mat]-im.data[i])/C+2;
          else if ((bool)(V-im.data[i+2*mat]<0.0001)) h=(im.data[i]-im.data[i+mat])/C+4;
      }
      else h=0;
      
      float H= h<0? h/6+1:h/6;
      
      //if (i==0)printf("RGB: %f,%f,%f,HSV: %f,%f,%f\n",im.data[i],im.data[mat+i],im.data[2*mat+i],H,S,V);
      im.data[i]=H; im.data[i+mat]=S; im.data[i+2*mat]=V;
  }
  
  
  
  }

// HW0 #7
// Image& im: input image to be modified in-place
void hsv_to_rgb(Image& im)
  {
  assert(im.c==3 && "only works for 3-channels images");
  
  // TODO: Convert all pixels from HSV format to RGB format
    int mat=im.w*im.h;
    for (int i=0;i<mat;i++) {
        float C=im.data[i+mat]*im.data[i+2*mat];
        float X=C*(1-fabs(fmod(6*im.data[i],2)-1));
        float m=im.data[i+2*mat]-C;
               // if (i==36) printf("HSV: %f,%f,%f;   ",im.data[i],im.data[i+mat],im.data[i+2*mat]);
        if (im.data[i]<(float)1/6) {
            im.data[i]=C;
            im.data[i+mat]=X;
            im.data[i+2*mat]=0;
        }
        else if (im.data[i]>=(float)1/6 && im.data[i]<(float)1/3)
        {

            im.data[i]=X;
            im.data[i+mat]=C;
            im.data[i+2*mat]=0;
        }
        else if (im.data[i]>=(float)1/3 && im.data[i]<(float)1/2)
        {

            im.data[i]=0;
            im.data[i+mat]=C;
            im.data[i+2*mat]=X;
        }
        else if (im.data[i]>=(float)1/2 && im.data[i]<(float)2/3)
        {
            im.data[i]=0;
            im.data[i+mat]=X;
            im.data[i+2*mat]=C;
        }
        else if (im.data[i]>=(float)2/3 && im.data[i]<(float)5/6)
        {

            im.data[i]=X;
            im.data[i+mat]=0;
            im.data[i+2*mat]=C;
        }
        else if (im.data[i]>=(float)5/6 && im.data[i]<1)
        {

            im.data[i]=C;
            im.data[i+mat]=0;
            im.data[i+2*mat]=X;
        }
        im.data[i]+=m;
        im.data[i+mat]+=m;
        im.data[i+2*mat]+=m;
      //  if (i==36) printf("RGB: %f,%f,%f\n",im.data[i],im.data[i+mat],im.data[i+2*mat]);
    }
    
  }

// HW0 #9
// Image& im: input image to be modified in-place
void rgb_to_lch(Image& im)
  {
  assert(im.c==3 && "only works for 3-channels images");
  
  // TODO: Convert all pixels from RGB format to LCH format
  
  
  NOT_IMPLEMENTED();
  
  }

// HW0 #9
// Image& im: input image to be modified in-place
void lch_to_rgb(Image& im)
  {
  assert(im.c==3 && "only works for 3-channels images");
  
  // TODO: Convert all pixels from LCH format to RGB format
  
  NOT_IMPLEMENTED();
  
  }



// Implementation of member functions
void Image::clamp(void) { clamp_image(*this); }
void Image::shift(int c, float v) { shift_image(*this,c,v); }
void Image::scale(int c, float v) { scale_image(*this,c,v); }

void Image::HSVtoRGB(void) { hsv_to_rgb(*this); }
void Image::RGBtoHSV(void) { rgb_to_hsv(*this); }
void Image::LCHtoRGB(void) { lch_to_rgb(*this); }
void Image::RGBtoLCH(void) { rgb_to_lch(*this); }
