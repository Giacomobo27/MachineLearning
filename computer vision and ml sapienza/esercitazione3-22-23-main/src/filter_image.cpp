#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
//#include <eigen3/Eigen/Core>
#include <iostream>

#define M_PI 3.14159265358979323846

// HW1 #2.1
// Image& im: image to L1-normalize
void l1_normalize(Image &im) {

    // TODO: Normalize each channel
    for (int i=0;i<im.c;i++) {
        float sum=0;
        for (int j=0;j<im.w*im.h;j++)
            sum+=im.data[i*im.w*im.h+j];
        for (int j=0;j<im.w*im.h;j++)
            im.data[i*im.w*im.h+j]/=sum;    
    }


}

// HW1 #2.1
// int w: size of filter
// returns the filter Image of size WxW
Image make_box_filter(int w) {
    assert(w % 2); // w needs to be odd

    // TODO: Implement the filter
    Image box(w,w,1);
    for (int i=0;i<w;i++)
        for(int j=0;j<w;j++)
            box.pixel(i,j,0)=1;
    l1_normalize(box);
    return box;
}

// HW1 #2.2
// const Image&im: input image
// const Image& filter: filter to convolve with
// bool preserve: whether to preserve number of channels
// returns the convolved image
Image convolve_image(const Image &im, const Image &filter, bool preserve) {
    assert(filter.c == 1);
    int nc=preserve? im.c:1;
    Image ret(im.w,im.h,nc);
    // This is the case when we need to use the function clamped_pixel(x,y,c).
    // Otherwise you'll have to manually check whether the filter goes out of bounds

    // TODO: Make sure you set the sizes of ret properly. Use ret=Image(w,h,c) to reset ret
    // TODO: Do the convolution operator
    
        for (int i=0;i<im.w;i++) {
            for(int j=0;j<im.h;j++) {
                float sum=0;
                for (int col=0;col<im.c;col++) {
                    float q=0;
                    int xp=i-filter.w/2;
                    for (int x=0;x<filter.w;x++) {
                        int yp=j-filter.w/2;
                        for (int y=0;y<filter.w;y++) {
                            q+=im.clamped_pixel(xp,yp,col)*filter.pixel(x,y,0);
                            yp++;
                        }
                        xp++;
                    }
                    sum+=q;
                    if (ret.c==1) ret.pixel(i,j,0)=sum;
                    else ret.pixel(i,j,col)=q;
                    
             }
             
        }
    }

    // Make sure to return ret and not im. This is just a placeholder
    return ret;
}
/*
Image convolve_image_fast(const Image &im, const Image &filter, bool preserve) {
    assert(filter.c == 1);
    Image ret(im.w,im.h,preserve? im.c:1);
    // This is the case when we need to use the function clamped_pixel(x,y,c).
    // Otherwise you'll have to manually check whether the filter goes out of bounds

    // TODO: Make sure you set the sizes of ret properly. Use ret=Image(w,h,c) to reset ret
    // TODO: Do the convolution operator
    Eigen::MatrixXd red(im.w+filter.w-1,im.h+filter.w-1);
    Eigen::MatrixXd blue(im.w+filter.w-1,im.h-1+filter.w);
    Eigen::MatrixXd green(im.w+filter.w-1,im.h+filter.w-1);
    int w=im.w,h=im.h;
    int offset=filter.w/2;

    for (int i=0;i<w;i++) {
        for(int j=0;j<h;j++) {
            red(i+offset,j+offset)=im.pixel(i,j,0);
            green(i+offset,j+offset)=im.pixel(i,j,1);
            blue(i+offset,j+offset)=im.pixel(i,j,2);
        }
    }

    for (int i=0;i<offset;i++) {
        for (int j=-offset;j<h+offset;j++) {
            red(i,j+offset)=im.clamped_pixel(i-offset,j,0);
            green(i,j+offset)=im.clamped_pixel(i-offset,j,1);
            blue(i,j+offset)=im.clamped_pixel(i-offset,j,2);
            red(w+i+offset,j+offset)=im.clamped_pixel(w+i,j,0);
            green(w+i+offset,j+offset)=im.clamped_pixel(w+i,j,1);
            blue(w+i+offset,j+offset)=im.clamped_pixel(w+i,j,2);
        }
    }

    for (int i=-offset;i<w+offset;i++) {
        for (int j=0;j<offset;j++) {
            red(i+offset,j)=im.clamped_pixel(i,j-offset,0);
            green(i+offset,j)=im.clamped_pixel(i,j-offset,1);
            blue(i+offset,j)=im.clamped_pixel(i,j-offset,2);
            red(i+offset,h+j+offset)=im.clamped_pixel(i,h+j,0);
            green(i+offset,h+j+offset)=im.clamped_pixel(i,h+j,1);
            blue(i+offset,h+j+offset)=im.clamped_pixel(i,h+j,2);
        }
    }

    Eigen::MatrixXd mf(filter.w,filter.h);
    for (int i=0;i<filter.w;i++)
        for (int j=0;j<filter.h;j++)
            mf(i,j)=filter.pixel(i,j);
    
    switch(ret.c) {
        case 1: 
            for (int i=0;i<w;i++) 
                for(int j=0;j<h;j++) 
                    ret.pixel(i,j,0)=(red.block(i,j,filter.w,filter.h).array()*mf.array()).sum()+
                                     (blue.block(i,j,filter.w,filter.h).array()*mf.array()).sum()+
                                     (green.block(i,j,filter.w,filter.h).array()*mf.array()).sum();
            break;
        case 3:
            for (int i=0;i<w;i++) {
                for(int j=0;j<h;j++) {
                    ret.pixel(i,j,0)=(red.block(i,j,filter.w,filter.h).array()*mf.array()).sum();
                    ret.pixel(i,j,1)=(green.block(i,j,filter.w,filter.h).array()*mf.array()).sum();
                    ret.pixel(i,j,2)=(blue.block(i,j,filter.w,filter.h).array()*mf.array()).sum();
                }           
            }
            break;
    }    

    // Make sure to return ret and not im. This is just a placeholder
    return ret;
}
*/
// HW1 #2.3
// returns basic 3x3 high-pass filter
Image make_highpass_filter() {
    // TODO: Implement the filter
    Image ret=make_box_filter(3);
    ret.pixel(0,0,0)=0;
    ret.pixel(1,0,0)=-1;
    ret.pixel(2,0,0)=0;
    ret.pixel(0,1,0)=-1;
    ret.pixel(1,1,0)=4;
    ret.pixel(2,1,0)=-1;
    ret.pixel(0,2,0)=0;
    ret.pixel(1,2,0)=-1;
    ret.pixel(2,2,0)=0;
    return ret;

}

// HW1 #2.3
// returns basic 3x3 sharpen filter
Image make_sharpen_filter() {
    // TODO: Implement the filter
       Image ret=make_box_filter(3);
    ret.pixel(0,0,0)=0;
    ret.pixel(1,0,0)=-1;
    ret.pixel(2,0,0)=0;
    ret.pixel(0,1,0)=-1;
    ret.pixel(1,1,0)=5;
    ret.pixel(2,1,0)=-1;
    ret.pixel(0,2,0)=0;
    ret.pixel(1,2,0)=-1;
    ret.pixel(2,2,0)=0;
    return ret;
}

// HW1 #2.3
// returns basic 3x3 emboss filter
Image make_emboss_filter() {
    // TODO: Implement the filter
        Image ret=make_box_filter(3);
    ret.pixel(0,0,0)=-2;
    ret.pixel(1,0,0)=-1;
    ret.pixel(2,0,0)=0;
    ret.pixel(0,1,0)=-1;
    ret.pixel(1,1,0)=1;
    ret.pixel(2,1,0)=1;
    ret.pixel(0,2,0)=0;
    ret.pixel(1,2,0)=1;
    ret.pixel(2,2,0)=2;
    return ret;

}

// HW1 #2.4
// float sigma: sigma for the gaussian filter
// returns basic gaussian filter
Image make_gaussian_filter(float sigma) {
    // TODO: Implement the filter
    
    Image ret=make_box_filter(6*sigma+1);
    int k=ret.w/2;
    //printf("%d,%d\n",ret.w,ret.h);
    for (int x=-k;x<=k;x++) {
        for(int y=-k;y<=k;y++) {
              float v=expf((-x*x-y*y)/(2*sigma*sigma))/(2*M_PI*sigma*sigma);
              ret.pixel(x+k,y+k,0)=v;
        }
    }
    //l1_normalize(ret);
    
    /*
    float sum=0;
    for (int x=0;x<ret.w;x++) {
        for(int y=0;y<ret.w;y++) {
              sum+=ret.pixel(x,y,0);
        }
    }
    printf("%f\n",sum);
    */
    /*
    
        int i, j, offset;
    int w = 6 * sigma;
    w = w > 0 ? ((w % 2 == 1) ? w : w + 1) : 1;
    Image gaussian_filter = make_box_filter(w);
    offset = w / 2;
    for (i = 0 - offset; i < w - offset; ++i){
      for (j = 0 - offset; j < w - offset; ++j){
        float v = 1 / (2*M_PI*sigma*sigma) * expf((- j*j - i*i) / (2*sigma*sigma));
        gaussian_filter.pixel( j+offset, i+offset, 0)=v;
      }
    }
    Image ret= gaussian_filter;
    */
    //l1_normalize(ret);
    /*
    for (int x=ret.w/2;x<ret.w;x++) {
        for(int y=ret.w/2;y<ret.w;y++) {
              printf("mine:%d,%d: %f\n",x,y,ret.pixel(x,y));
        }
    }
    */
    return ret;
}



// HW1 #3
// const Image& a: input image
// const Image& b: input image
// returns their sum
Image add_image(const Image &a, const Image &b) {
    assert(a.w == b.w && a.h == b.h &&
           a.c == b.c); // assure images are the same size
    Image ret(a.w,a.h,a.c);
    // TODO: Implement addition
    for (int col=0;col<a.c;col++) {
        for (int x=0;x<a.w;x++) {
            for(int y=0;y<a.h;y++) {
                ret.pixel(x,y,col)=a.pixel(x,y,col)+b.pixel(x,y,col);
            }
        }
    }

    return ret;

}

// HW1 #3
// const Image& a: input image
// const Image& b: input image
// returns their difference res=a-b
Image sub_image(const Image &a, const Image &b) {
    assert(a.w == b.w && a.h == b.h &&
           a.c == b.c); // assure images are the same size
    Image ret(a.w,a.h,a.c);
    // TODO: Implement subtraction
    for (int col=0;col<a.c;col++) {
        for (int x=0;x<a.w;x++) {
            for(int y=0;y<a.h;y++) {
                ret.pixel(x,y,col)=a.pixel(x,y,col)-b.pixel(x,y,col);
            }
        }
    }

    return ret;

}

// HW1 #4.1
// returns basic GX filter
Image make_gx_filter() {
    // TODO: Implement the filter
    Image ret(3,3,1);
    ret.pixel(0,0,0)=-1;
    ret.pixel(1,0,0)=0;
    ret.pixel(2,0,0)=1;
    ret.pixel(0,1,0)=-2;
    ret.pixel(1,1,0)=0;
    ret.pixel(2,1,0)=2;
    ret.pixel(0,2,0)=-1;
    ret.pixel(1,2,0)=0;
    ret.pixel(2,2,0)=1;

    return ret;
}

// HW1 #4.1
// returns basic GY filter
Image make_gy_filter() {
    // TODO: Implement the filter
    Image ret(3,3,1);
        ret.pixel(0,0,0)=-1;
    ret.pixel(1,0,0)=-2;
    ret.pixel(2,0,0)=-1;
    ret.pixel(0,1,0)=0;
    ret.pixel(1,1,0)=0;
    ret.pixel(2,1,0)=0;
    ret.pixel(0,2,0)=1;
    ret.pixel(1,2,0)=2;
    ret.pixel(2,2,0)=1;

    return ret;
}

// HW1 #4.2
// Image& im: input image
void feature_normalize(Image &im) {
    assert(im.w * im.h); // assure we have non-empty image

    // TODO: Normalize the features for each channel
    float max=0,min=1;
    for (int col=0;col<im.c;col++) {
        for (int x=0;x<im.w;x++) {
            for(int y=0;y<im.h;y++) {
                if(im.pixel(x,y,col)>max) max=im.pixel(x,y,col);
                if(im.pixel(x,y,col)<min) min=im.pixel(x,y,col);
            }
        }
    }
    for (int col=0;col<im.c;col++) {        
        for (int x=0;x<im.w;x++) {
            for(int y=0;y<im.h;y++) {
                im.pixel(x,y,col)=(im.pixel(x,y,col)-min)/(max-min);
            }
        }
    }

}


// Normalizes features across all channels
void feature_normalize_total(Image &im) {
    assert(im.w * im.h * im.c); // assure we have non-empty image

    int nc = im.c;
    im.c = 1;
    im.w *= nc;

    feature_normalize(im);

    im.w /= nc;
    im.c = nc;

}


// HW1 #4.3
// Image& im: input image
// return a pair of images of the same size
pair<Image, Image> sobel_image(const Image &im) {
    // TODO: Your code here 
    Image gx=make_gx_filter();
    Image gy=make_gy_filter();
    Image gx_conv=convolve_image(im,gx,false);
    Image gy_conv=convolve_image(im,gy,false);
    Image g(im.w,im.h,1);
    Image theta(im.w,im.h,1);
    for (int x=0;x<im.w;x++) {
        for(int y=0;y<im.h;y++) {
            g.pixel(x,y,0)=sqrtf(gx_conv(x,y,0)*gx_conv(x,y,0)+gy_conv(x,y,0)*gy_conv(x,y,0));
            theta.pixel(x,y,0)=atan2(gy_conv(x,y,0),gx_conv(x,y,0));
        }
    }   
    return {g, theta};
}


// HW1 #4.4
// const Image& im: input image
// returns the colorized Sobel image of the same size
Image colorize_sobel(const Image &im) {
    // TODO: Your code here
    Image res(im.w,im.h,im.c);
    pair<Image,Image> sobel_filter=sobel_image(im);
    feature_normalize(sobel_filter.first);
    //feature_normalize(sobel_filter.second);
    
    for (int x=0;x<im.w;x++) {
        for (int y=0;y<im.h;y++) {
            sobel_filter.second.pixel(x,y,0)=sobel_filter.second.pixel(x,y,0)/(2*M_PI)+0.5;
       }
    }  
    
    for (int x=0;x<im.w;x++) {
        for (int y=0;y<im.h;y++) {
            res.pixel(x,y,0)=sobel_filter.second.pixel(x,y,0);
            res.pixel(x,y,1)=sobel_filter.first.pixel(x,y,0);
            res.pixel(x,y,2)=res.pixel(x,y,1);
       }
    }    
    hsv_to_rgb(res);
    Image filter=make_gaussian_filter(4);
    res=convolve_image(res,filter,true);
    return res;
}


// HW1 #4.5
// const Image& im: input image
// float sigma1,sigma2: the two sigmas for bilateral filter
// returns the result of applying bilateral filtering to im
Image bilateral_filter(const Image &im, float sigma1, float sigma2) {
    Image bf = make_gaussian_filter(sigma1);
    Image ret(im.w,im.h,im.c);

    // TODO: Your bilateral code
    for (int col=0;col<im.c;col++) {
        for (int x=0;x<im.w;x++) {
            for (int y=0;y<im.h;y++) {
                for (int i=-3*sigma1;i<=3*sigma1;i++) {
                    for (int j=-3*sigma1;j<=3*sigma1;j++) {
		                float diff=im.clamped_pixel(x,y,col)-im.clamped_pixel(x+i,y+j,col);
		                float value=expf(-(diff*diff)/(2*sigma2*sigma2))/sqrtf(2.0*M_PI*sigma2*sigma2);
                        bf.pixel(i+3*sigma1,j+3*sigma1,0)*=value;
                    }
                }
                l1_normalize(bf);
                float q=0;
                int xp=x-bf.w/2;
                for (int i=0;i<bf.w;i++) {
                   int yp=y-bf.w/2;
                   for (int j=0;j<bf.w;j++) {
                        q+=im.clamped_pixel(xp,yp,col)*bf.pixel(i,j,0);
                        yp++;
                    }
                   xp++;
                }
                ret.pixel(x,y,col)=q;
                //if (x==701 && y==0 && col==0) ret.pixel(x,y,col)=0.784314;
                bf=make_gaussian_filter(sigma1);
            }
        }
    }

    return ret;
}
/*
Image bilateral_filter_fast(const Image &im, float sigma1, float sigma2) {

    Image bf = make_gaussian_filter(sigma1);
    Image ret(im.w,im.h,im.c);

    Eigen::MatrixXd red(im.w+bf.w-1,im.h+bf.w-1);
    Eigen::MatrixXd blue(im.w+bf.w-1,im.h-1+bf.w);
    Eigen::MatrixXd green(im.w+bf.w-1,im.h+bf.w-1);
    int w=im.w,h=im.h;
    int offset=bf.w/2;

    for (int i=0;i<w;i++) {
        for(int j=0;j<h;j++) {
            red(i+offset,j+offset)=im.pixel(i,j,0);
            green(i+offset,j+offset)=im.pixel(i,j,1);
            blue(i+offset,j+offset)=im.pixel(i,j,2);
        }
    }

    for (int i=0;i<offset;i++) {
        for (int j=-offset;j<h+offset;j++) {
            red(i,j+offset)=im.clamped_pixel(i-offset,j,0);
            green(i,j+offset)=im.clamped_pixel(i-offset,j,1);
            blue(i,j+offset)=im.clamped_pixel(i-offset,j,2);
            red(w+i+offset,j+offset)=im.clamped_pixel(w+i,j,0);
            green(w+i+offset,j+offset)=im.clamped_pixel(w+i,j,1);
            blue(w+i+offset,j+offset)=im.clamped_pixel(w+i,j,2);
        }
    }

    for (int i=-offset;i<w+offset;i++) {
        for (int j=0;j<offset;j++) {
            red(i+offset,j)=im.clamped_pixel(i,j-offset,0);
            green(i+offset,j)=im.clamped_pixel(i,j-offset,1);
            blue(i+offset,j)=im.clamped_pixel(i,j-offset,2);
            red(i+offset,h+j+offset)=im.clamped_pixel(i,h+j,0);
            green(i+offset,h+j+offset)=im.clamped_pixel(i,h+j,1);
            blue(i+offset,h+j+offset)=im.clamped_pixel(i,h+j,2);
        }
    }
    Eigen::MatrixXd colore;
    // TODO: Your bilateral code
    for (int col=0;col<im.c;col++) {
        switch (col) {
                    case 0:
                        colore=red;
                        break;
                    case 1:
                        colore=green;
                        break;
                    case 2:
                        colore=blue;
                        break;
        }
        for (int x=0;x<w;x++) {
            for (int y=0;y<h;y++) {
                float normalize=0;
                Eigen::MatrixXd mf(bf.w,bf.h);
                for (int i=0;i<bf.w;i++) {
                    for (int j=0;j<bf.h;j++) {
                        float diff=colore(x+offset,y+offset)-colore(x+i,y+j);
		                float value=expf(-(diff*diff)/(2*sigma2*sigma2))/sqrtf(2.0*M_PI*sigma2*sigma2);
                        mf(i,j)=bf.pixel(i,j)*value;
                        normalize+=mf(i,j);
                    }
                }
                mf=mf.array()/normalize;
                ret.pixel(x,y,col)=(colore.block(x,y,bf.w,bf.h).array()*mf.array()).sum();
            }
        }
    }
    return ret;
}
*/
// HM #5
//
float *compute_histogram(const Image &im, int ch, int num_bins) {
    float *hist = (float *) malloc(sizeof(float) * num_bins);
    for (int i = 0; i < num_bins; ++i) {
        hist[i]=0;
    }
    for (int x=0;x<im.w;x++) {
        for (int y=0;y<im.h;y++) {
            hist[(int)(im.pixel(x,y,ch)*(num_bins-1))]++;
        }
    }
    for (int i = 0; i < num_bins ; ++i) {
        //printf("%d,%f\n",i,hist[i]/(im.w*im.h));
        hist[i]/=(im.w*im.h);
    }
    // TODO: Your histogram code

    return hist;
}

float *compute_CDF(float *hist, int num_bins) {
    float *cdf = (float *) malloc(sizeof(float) * num_bins);

    cdf[0] = hist[0];
    
    for (int i=1;i<num_bins;i++) {
        cdf[i]=cdf[i-1]+hist[i];
        //printf("%d,%f\n",i,cdf[i]);
    }
    
    return cdf;
}

Image histogram_equalization_hsv(const Image &im, int num_bins) { 
    Image new_im(im);
     
    float eps = 1.0 / (num_bins * 1000);

 
    rgb_to_hsv(new_im);
    
    // TODO: Your histogram equalization code
    // convert to hsv
    // compute histograms for the luminance channel
    float* hist=compute_histogram(new_im,2,num_bins);

    // compute cdf
    float* cdf=compute_CDF(hist,num_bins);
    //for(int i=0;i<num_bins;i++) printf("%f\n",cdf[i]);
    // equalization
    for (int x=0;x<im.w;x++) {
         for (int y=0;y<im.h;y++) {
            //cdfx=kx=sum(xi<x)px
            int bin=new_im.clamped_pixel(x,y,2)*(num_bins-1);
            //printf("%f; ",new_im.pixel(x,y,2));
            new_im.pixel(x,y,2)=cdf[bin];
            //printf("%f\n",new_im.pixel(x,y,2));
         }
    }       
        // delete the allocated memory!
//        delete hist;
    delete[] hist;
//        delete cdf;
    delete[] cdf;
    
    hsv_to_rgb(new_im);
    
    return new_im;
    
    
}

Image histogram_equalization_rgb(const Image &im, int num_bins) {
    Image ret(im.w,im.h,im.c);
    float eps = 1.0 / (num_bins * 1000);

    // compute histograms for each color channel
    for (int c = 0; c < im.c; ++c) {
        float* hist=compute_histogram(im,c,num_bins);
        float* cdf=compute_CDF(hist,num_bins);
        // TODO: Your equalization code
        for (int x=0;x<im.w;x++) {
            for (int y=0;y<im.h;y++) {
                int bin=im.clamped_pixel(x,y,c)*(num_bins-1);
                ret.pixel(x,y,c)=cdf[bin];    
            }
        }    
        // delete the allocated memory!
//        delete hist;
        delete[] hist;
//        delete cdf;
        delete[] cdf;
    }

    return ret;
}
/*
//PANNACCI CODE

Image histogram_equalization_hsv(const Image &im, int num_bins) {
    Image new_im(im);
    float eps = 1.0 / (num_bins * 1000);

    // convert to hsv
    rgb_to_hsv(new_im);
    
    // compute histograms for the luminance channel
    float* hist = compute_histogram(new_im, 2, num_bins);
    
    // compute cdf
    float* cdf = compute_CDF(hist, num_bins);
    
    // equalization
	for(int i = 0; i < new_im.w; i++) {
		for(int j = 0; j < new_im.h; j++) {
		
			float value = new_im.clamped_pixel(i,j,2);
			int num = value*(num_bins-1);
			
			new_im.set_pixel(i,j,2, cdf[num]);
		
		}
	}    
    
    // convert back to rgb
    hsv_to_rgb(new_im);

    // delete the allocated memory!
	free(hist);
	free(cdf);

    return new_im;
}

Image histogram_equalization_rgb(const Image &im, int num_bins) {
    Image new_im(im);
    float eps = 1.0 / (num_bins * 1000);

    // compute histograms for each color channel
    for (int c = 0; c < im.c; ++c) {

		float* hist = compute_histogram(im, c, num_bins);
		float* cdf = compute_CDF(hist, num_bins);
		
		for(int i = 0; i < im.w; i++) {
			for(int j = 0; j < im.h; j++) {
			
				float value = im.clamped_pixel(i,j,c);
				int num = value*(num_bins-1);
				
				new_im.set_pixel(i,j,c, cdf[num]);
			
			}
		}

        // delete the allocated memory!
		
		free(hist);
		free(cdf);

    }

    return new_im;
    
}
*/
// HELPER MEMBER FXNS

void Image::feature_normalize(void) { ::feature_normalize(*this); }

void Image::feature_normalize_total(void) { ::feature_normalize_total(*this); }

void Image::l1_normalize(void) { ::l1_normalize(*this); }

Image operator-(const Image &a, const Image &b) { return sub_image(a, b); }

Image operator+(const Image &a, const Image &b) { return add_image(a, b); }
