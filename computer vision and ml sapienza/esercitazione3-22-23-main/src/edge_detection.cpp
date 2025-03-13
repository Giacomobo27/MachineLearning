#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"

#define M_PI 3.14159265358979323846


/*
Smooths a grayscale image by convolving it with a Gaussian kernel of standard deviation sigma.
Input:
    Image im: the input image
    float sigma: the standard deviation of the Gaussian kernel
Output:
    Image: the smoothed image (im.w, im.h, 1)
*/
Image smooth_image(const Image& im, float sigma)
{
    // TODO: Your code here
    return convolve_image(im,make_gaussian_filter(sigma),true);
}


/*
Computes the magnitude and direction of the gradient of an image.
Input:
    Image im: the input image
Output:
    pair<Image,Image>: the magnitude and direction of the gradient of the image
                       with magnitude in [0,1] and direction in [-pi,pi]
*/
pair<Image,Image> compute_gradient(const Image& im)
{
    // TODO: Your code here
    pair<Image,Image> sobel_filter=sobel_image(im);
    //feature_normalize(sobel_filter.second);
    feature_normalize(sobel_filter.first);
    float max=0,min=100000;
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            if (max<sobel_filter.second.pixel(x,y)) max=sobel_filter.second.pixel(x,y);
            if (min>sobel_filter.second.pixel(x,y)) min=sobel_filter.second.pixel(x,y);
        }
    }
    return  {sobel_filter.first,sobel_filter.second};
}


/*
Performs non-maximum suppression on an image.
Input:
    Image mag: the magnitude of the gradient of the image [0,1]
    Image dir: the direction of the gradient of the image [-pi,pi]
Output:
    Image: the image after non-maximum suppression
*/
Image non_maximum_suppression(const Image& mag, const Image& dir)
{
    Image nms(mag.w, mag.h, 1);
    float neighbor1, neighbor2;

    // Iterate through the image and perform non-maximum suppression
    for (int y = 0; y < mag.h; y++) {
        for (int x = 0; x < mag.w; x++) {
            int q=(round(4*dir.pixel(x,y)/M_PI));

            if (abs(q)==0 || abs(q)==4) {
                neighbor1=mag.clamped_pixel(x+1,y,0);
                neighbor2=mag.clamped_pixel(x-1,y,0);
            }
            else if (q==1 || q==-3) {
                neighbor1=mag.clamped_pixel(x+1,y+1,0);
                neighbor2=mag.clamped_pixel(x-1,y-1,0);
            }
            else if (abs(q)==2) {
                neighbor1=mag.clamped_pixel(x,y+1,0);
                neighbor2=mag.clamped_pixel(x,y-1,0);
            }
            else if (q==3 || q==-1) {
                neighbor1=mag.clamped_pixel(x+1,y-1,0);
                neighbor2=mag.clamped_pixel(x-1,y+1,0);
            }
            
            if (!(mag.pixel(x,y)<neighbor1) && !(mag.pixel(x,y)<neighbor2))
                nms.pixel(x,y)=mag.pixel(x,y);
            else 
                nms.pixel(x,y)=0;

        }
    }

    return nms;
}



/*
    Applies double thresholding to an image.
    Input:
        Image im: the input image
        float lowThreshold: the low threshold value
        float highThreshold: the high threshold value
        float strongVal: the value to use for strong edges
        float weakVal: the value to use for weak edges
    Output:
        Image: the thresholded image
*/
Image double_thresholding(const Image& im, float lowThreshold, float highThreshold, float strongVal, float weakVal)
{
    Image res(im.w, im.h, im.c);

    // TODO: Your code here
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            res.pixel(x,y)=0;
            if (im.pixel(x,y)>=lowThreshold) res.pixel(x,y)=weakVal;
            if (im.pixel(x,y)>=highThreshold) res.pixel(x,y)=strongVal;
        }
    }

    return res;
}


/*
    Applies hysteresis thresholding to an image.
    Input:
        Image im: the input image
        float weak: the value of the weak edges
        float strong: the value of the strong edges
    Output:
        Image: the image after hysteresis thresholding, with only strong edges
*/
Image edge_tracking(const Image& im, float weak, float strong)
{
    Image res(im.w, im.h, im.c);

    for (int y=0; y < im.h; ++y) {
        for (int x=0; x < im.w; ++x) {
            res.pixel(x,y)=im.pixel(x,y);
            if (!im.pixel(x,y)<weak && im.pixel(x,y)<strong) {
                bool near_strong=false;
                for (int i=x-1;i<=x+1;i++)
                    for (int j=y-1;j<=y+1;j++)
                        if (!(strong>im.clamped_pixel(i,j))) near_strong=true;
                res.pixel(x,y)=near_strong? strong:0;
            }
                
            // TODO: Your code here
            
            // Hint: use clamped_pixel when checking the neighbors to avoid going out of bounds
        }
    }

    return res;

}
