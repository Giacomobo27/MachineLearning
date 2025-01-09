#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cassert>

#include "image.h"
#include "matrix.h"

#include <set>
#include <thread>

using namespace std;

// Comparator for matches
// const void *a, *b: pointers to the matches to compare.
// returns: result of comparison, 0 if same, 1 if a > b, -1 if a < b.
int match_compare(const void *a, const void *b)
  {
  Match *ra = (Match *)a;
  Match *rb = (Match *)b;
  if (ra->distance < rb->distance) return -1;
  else if (ra->distance > rb->distance) return  1;
  else return 0;
  }


// Place two images side by side on canvas, for drawing matching pixels.
// const Image& a, b: images to place.
// returns: image with both a and b side-by-side.
Image both_images(const Image& a, const Image& b)
  {
  assert(a.c==b.c);
  Image both(a.w + b.w, a.h > b.h ? a.h : b.h, a.c);
  
  for(int k = 0; k < both.c; ++k)
    for(int j = 0; j < a.h; ++j)
      for(int i = 0; i < a.w; ++i)
        both(i, j, k) = a(i, j, k);
  
  for(int k = 0; k < both.c; ++k)
    for(int j = 0; j < b.h; ++j)
      for(int i = 0; i < b.w; ++i)
        both(i+a.w, j, k) = b(i, j, k);
  return both;
  }

// Draws lines between matching pixels in two images.
// const Image& a, b: two images that have matches.
// const vector<Match>& matches: array of matches between a and b.
// int inliers: number of inliers at beginning of matches, drawn in green.
// returns: image with matches drawn between a and b on same canvas.
Image draw_matches(const Image& a, const Image& b, const vector<Match>& matches, const vector<Match>& inliers)
  {
  Image both = both_images(a, b);
  
  for(int i = 0; i < (int)matches.size(); ++i)
    {
    int bx = matches[i].a->p.x; 
    int ex = matches[i].b->p.x; 
    int by = matches[i].a->p.y;
    int ey = matches[i].b->p.y;
    for(int j = bx; j < ex + a.w; ++j)
      {
      int r = (float)(j-bx)/(ex+a.w - bx)*(ey - by) + by;
      both.set_pixel(j, r, 0, 1);
      both.set_pixel(j, r, 1, 0);
      both.set_pixel(j, r, 2, 0);
      }
    }
  for(int i = 0; i < (int)inliers.size(); ++i)
    {
    int bx = inliers[i].a->p.x; 
    int ex = inliers[i].b->p.x; 
    int by = inliers[i].a->p.y;
    int ey = inliers[i].b->p.y;
    for(int j = bx; j < ex + a.w; ++j)
      {
      int r = (float)(j-bx)/(ex+a.w - bx)*(ey - by) + by;
      both.set_pixel(j, r, 0, 0);
      both.set_pixel(j, r, 1, 1);
      both.set_pixel(j, r, 2, 0);
      }
    }
  return both;
  }

// Draw the matches with inliers in green between two images.
// const Image& a, b: two images to match.
// vector<Match> m: matches
// Matrix H: the current homography
// thresh: for thresholding inliers
Image draw_inliers(const Image& a, const Image& b, const Matrix& H, const vector<Match>& m, float thresh)
  {
  vector<Match> inliers = model_inliers(H, m, thresh);
  Image lines = draw_matches(a, b, m, inliers);
  return lines;
  }

// Find corners, match them, and draw them between two images.
// const Image& a, b: images to match.
// float sigma: gaussian for harris corner detector. Typical: 2
// float thresh: threshold for corner/no corner. Typical: 1-5
// int nms: window to perform nms on. Typical: 3
Image find_and_draw_matches(const Image& a, const Image& b, float sigma, float thresh, int window, int nms, int corner_method)
  {
  vector<Descriptor> ad= harris_corner_detector(a, sigma, thresh, window, nms, corner_method);
  vector<Descriptor> bd= harris_corner_detector(b, sigma, thresh, window, nms, corner_method);
  vector<Match> m = match_descriptors(ad, bd);
  
  
  Image A=mark_corners(a, ad);
  Image B=mark_corners(b, bd);
  Image lines = draw_matches(A, B, m, {});
  
  return lines;
  }

// HW5 2.1
// Calculates L1 distance between two floating point arrays.
// vector<float>& a,b: arrays to compare.
// returns: l1 distance between arrays (sum of absolute differences).
float l1_distance(const vector<float>& a,const vector<float>& b)
  {
  assert(a.size()==b.size() && "Arrays must have same size\n");
  
  // TODO: return the correct number.
  float dist=0;
  for (int i=0;i<a.size();i++) {
    dist+=a[i]-b[i]>=0?a[i]-b[i]:b[i]-a[i];
  }

  return dist;
  }

// HW5 2.2a
// Finds best matches between descriptors of two images.
// const vector<Descriptor>& a, b: array of descriptors for pixels in two images.
// returns: best matches found. For each element in a[] find the index of best match in b[]
vector<int> match_descriptors_a2b(const vector<Descriptor>& a, const vector<Descriptor>& b)
  {
  vector<int> ind;
  for(int j=0;j<(int)a.size();j++)
    {
    int bind = -1; // <- find the best match (-1: no match)
    float best_distance=1e10f;  // <- best distance
    Descriptor da=a[j];
    for (int i=0;i<b.size();i++) {
      Descriptor db=b[i];
      if (l1_distance(da.data,db.data)<best_distance) {
        best_distance=l1_distance(da.data,db.data);
        bind=i;
        //printf("%f,%d,%d,%d\n",best_distance,bind,j,i);

      }

    }
    ind.insert(ind.begin()+j,bind);
    //printf("%d,%d\n",j,ind[j]);

    // TODO: find the best 'bind' descriptor in b that best matches a[j]
    // TODO: put your code here:

    }
  return ind;
  
  }


// HW5 2.2b
// Finds best matches between descriptors of two images.
// const vector<Descriptor>& a, b: array of descriptors for pixels in two images.
// returns: best matches found. each descriptor in a should match with at most
//          one other descriptor in b.
vector<Match> match_descriptors(const vector<Descriptor>& a, const vector<Descriptor>& b)
  {
  if(a.size()==0 || b.size()==0)return {};
  
  vector<Match> m;
  
  // TODO: use match_descriptors_a2b(a,b) and match_descriptors_a2b(b,a)
  // and populate `m` with good matches!
  vector<int> a_to_b=match_descriptors_a2b(a,b);
  vector<int> b_to_a=match_descriptors_a2b(b,a);
  int match_idx=0;
  for (int i=0;i<a_to_b.size();i++) {
    //printf("%d,%d\n",i,b_to_a[a_to_b[i]]);
    if (i==b_to_a[a_to_b[i]]) {
      Match match(&a[i],&b[a_to_b[i]],l1_distance(a[i].data,b[a_to_b[i]].data));
      m.insert(m.begin()+match_idx,match);
      match_idx++;
    }
  }
  //printf("%d\n",m.size());

  /*
puts("\n\n\n");
  printf("%d,%d\n\n",b.size(),b_to_a.size());
  for (int i=0;i<b_to_a.size();i++) {
    printf("%d ",b_to_a[i]);

  }
*/
    // TODO: find the best 'bind' descriptor in b that best matches a[j]
    // TODO: put your code here:

  
  return m;
  }


// HW5 3.1
// Apply a projective transformation to a point.
// const Matrix& H: homography to project point.
// const Point& p: point to project.
// returns: point projected using the homography.
Point project_point(const Matrix& H, const Point& p)
  {
  // TODO: project point p with homography H.
  // Remember that homogeneous coordinates are equivalent up to scalar.
  // Have to divide by.... something...
   // Convertiamo il punto in coordinate omogenee
    Matrix p_homog(3,1);
    p_homog(0,0)=p.x;
    p_homog(1,0)=p.y;
    p_homog(2,0)=1;

    
    // Effettuiamo la proiezione del punto tramite la matrice H
    Matrix res_homog = H * p_homog;
    
    // Normalizziamo le coordinate omogenee dividendo per l'ultimo elemento
    float res_x=res_homog(0,0)/res_homog(2,0);
    float res_y=res_homog(1,0)/res_homog(2,0);
    Point res(res_x,res_y);
    //printf("%f,%f",res.x,res.y);
    
    // Ritorniamo il punto proiettato in coordinate normali
    return res;
  }

// HW5 3.2a
// Calculate L2 distance between two points.
// const Point& p, q: points.
// returns: L2 distance between them.
double point_distance(const Point& p, const Point& q)
  {
  // TODO: should be a quick one.
  return sqrtf(pow((p.x-q.x),2)+pow((p.y-q.y),2));
  }

// HW5 3.2b
// Count number of inliers in a set of matches. Should also bring inliers
// to the front of the array.
// const Matrix& H: homography between coordinate systems.
// const vector<Match>& m: matches to compute inlier/outlier.
// float thresh: threshold to be an inlier.
// returns: inliers whose projected point falls within thresh of their match in the other image.
vector<Match> model_inliers(const Matrix& H, const vector<Match>& m, float thresh)
  {
  vector<Match> inliers;
  // TODO: fill inliers
  // i.e. distance(H*a.p, b.p) < thresh
  int inl_idx=0;
   for (int i=0;i<m.size();i++) {
     Match match=m[i];
     Descriptor a=*match.a;
     Descriptor b=*match.b;
     if (point_distance(project_point(H,a.p),b.p)<thresh) {
       inliers.insert(inliers.begin()+inl_idx,match);
       inl_idx++;
     }
   }
  
  return inliers;
  }

// HW5 3.3
// Randomly shuffle matches for RANSAC.
// vector<Match>& m: matches to shuffle in place.
void randomize_matches(vector<Match>& m)
  {
  // TODO: implement Fisher-Yates to shuffle the array.
  // You might want to use the swap function like:
  // swap(m[0],m[1]) which swaps the first and second element
  for (int i=m.size()-1;i>0;i--) {
    int j=rand()%(i+1);
    //printf("%d,%d\n",i,j);
    swap(m[i],m[j]);
  }

  }

// HW5 3.4
// Computes homography between two images given matching pixels.
// const vector<Match>& matches: matching points between images.
// int n: number of matches to use in calculating homography.
// returns: matrix representing homography H that maps image a to image b.
Matrix compute_homography_ba(const vector<Match>& matches)
  {
  if(matches.size()<4)printf("Need at least 4 points for homography! %zu supplied\n",matches.size());
  if(matches.size()<4)return Matrix::identity(3,3);
  
  Matrix M(matches.size()*2,8);
  Matrix b(matches.size()*2);
  
  
  for(int i = 0; i < (int)matches.size(); ++i)
    {
    double mx = matches[i].a->p.x;
    double my = matches[i].a->p.y;
    
    double nx = matches[i].b->p.x;
    double ny = matches[i].b->p.y;
    // TODO: fill in the matrices M and b.
    
    M(i*2,0)=mx;
    M(i*2,1)=my;
    M(i*2,2)=1;
    M(i*2+1,3)=mx;
    M(i*2+1,4)=my;
    M(i*2+1,5)=1;
    M(i*2,6)=-mx*nx;
    M(i*2+1,6)=-mx*ny;
    M(i*2,7)=-my*nx;
    M(i*2+1,7)=-my*ny;
    b(i*2,0)=nx;
    b(i*2+1,0)=ny;

    }
  
  
  
  Matrix a = solve_system(M, b);
  
  Matrix Hba(3, 3);
  // TODO: fill in the homography H based on the result in a.

  Hba(0,0)=a(0,0);
  Hba(0,1)=a(1,0);
  Hba(0,2)=a(2,0);
  Hba(1,0)=a(3,0);
  Hba(1,1)=a(4,0);
  Hba(1,2)=a(5,0);
  Hba(2,0)=a(6,0);
  Hba(2,1)=a(7,0);
  Hba(1,2)=a(5,0);
  Hba(2,0)=a(6,0);
  Hba(2,2)=1;
   
  return Hba;
  }

// HW5 3.5
// Perform RANdom SAmple Consensus to calculate homography for noisy matches.
// vector<Match> m: set of matches.
// float thresh: inlier/outlier distance threshold.
// int k: number of iterations to run.
// int cutoff: inlier cutoff to exit early.
// returns: matrix representing most common homography between matches.
Matrix RANSAC(vector<Match> m, float thresh, int k, int cutoff)
  {
  if(m.size()<4)
    {
    //printf("Need at least 4 points for RANSAC! %zu supplied\n",m.size());
    return Matrix::identity(3,3);
    }
  
  int best = 0;
  Matrix Hba = Matrix::translation_homography(256, 0);
  // TODO: fill in RANSAC algorithm.
  // for k iterations:
  for (int i=0;i<k;i++) {
  //     shuffle the matches
    randomize_matches(m);
  //     compute a homography with a few matches (how many??)
    vector<Match> used;
    for (int i=0;i<(m.size()/3);i++) {
      used.insert(used.begin()+i,m[i]);
    }
    Matrix homography=compute_homography_ba(used);
  //     if new homography is better than old (how can you tell?):
    vector<Match> inliers=model_inliers(homography,used,thresh);
    if (inliers.size()>best) {
      best=inliers.size();
  //         compute updated homography using all inliers
      homography=compute_homography_ba(inliers);
  //         remember it and how good it is
      Hba=homography;
    }
  //         if it's better than the cutoff:
    if(inliers.size()>cutoff)
  //             return it immediately
      return Hba;
  }
  // if we get to the end return the best homography
    
  return Hba;
  }


Image trim_image(const Image& a)
  {
  int minx=a.w-1;
  int maxx=0;
  int miny=a.h-1;
  int maxy=0;
  
  for(int q3=0;q3<a.c;q3++)for(int q2=0;q2<a.h;q2++)for(int q1=0;q1<a.w;q1++)if(a(q1,q2,q3))
    {
    minx=min(minx,q1);
    maxx=max(maxx,q1);
    miny=min(miny,q2);
    maxy=max(maxy,q2);
    }
  
  if(maxx<minx || maxy<miny)return a;
  
  Image b(maxx-minx+1,maxy-miny+1,a.c);
  
  for(int q3=0;q3<a.c;q3++)for(int q2=miny;q2<=maxy;q2++)for(int q1=minx;q1<=maxx;q1++)
    b(q1-minx,q2-miny,q3)=a(q1,q2,q3);
  
  return b;
  }

// HW5 3.6
// Stitches two images together using a projective transformation.
// const Image& a, b: images to stitch.
// Matrix H: homography from image a coordinates to image b coordinates.
// float acoeff: blending coefficient
// returns: combined image stitched together.
Image combine_images(const Image& a, const Image& b, const Matrix& Hba, float ablendcoeff)
  {
  TIME(1);
  Matrix Hinv=Hba.inverse();
  
  // Project the corners of image b into image a coordinates.
  Point c1 = project_point(Hinv, Point(0,0));
  Point c2 = project_point(Hinv, Point(b.w-1, 0));
  Point c3 = project_point(Hinv, Point(0, b.h-1));
  Point c4 = project_point(Hinv, Point(b.w-1, b.h-1));
  
  // Find top left and bottom right corners of image b warped into image a.
  Point topleft, botright;
  botright.x = max(c1.x, max(c2.x, max(c3.x, c4.x)));
  botright.y = max(c1.y, max(c2.y, max(c3.y, c4.y)));
  topleft.x = min(c1.x, min(c2.x, min(c3.x, c4.x)));
  topleft.y = min(c1.y, min(c2.y, min(c3.y, c4.y)));
  
  // Find how big our new image should be and the offsets from image a.
  int dx = min(0, (int)topleft.x);
  int dy = min(0, (int)topleft.y);
  int w = max(a.w, (int)botright.x) - dx;
  int h = max(a.h, (int)botright.y) - dy;
  
  //printf("%d %d %d %d\n",dx,dy,w,h);
  
  // Can disable this if you are making very big panoramas.
  // Usually this means there was an error in calculating H.
  if(w > 15000 || h > 4000)
    {
    printf("Can't make such big panorama :/ (%d %d)\n",w,h);
    return Image(100,100,1);
    }
  
  Image c(w, h, a.c);
  
  // Paste image a into the new image offset by dx and dy.
  for(int k = 0; k < a.c; ++k)
    for(int j = 0; j < a.h; ++j)
      for(int i = 0; i < a.w; ++i)
        {
        // TODO: fill in.
        if (i-dx>=0 && i-dx<c.w && j-dy>=0 && j-dy<c.h)
          c.pixel(i-dx,j-dy,k)=a.pixel(i,j,k);
        }
  
  // TODO: Blend in image b as well.
  // You should loop over some points in the new image (which? all?)
  // and see if their projection from a coordinates to b coordinates falls
  // inside of the bounds of image b. If so, use bilinear interpolation to
  // estimate the value of b at that projection, then fill in image c.
  for(int k = 0; k < c.c; ++k)
    for(int j = 0; j < c.h; ++j)
      for(int i = 0; i < c.w; ++i)
        {
        // TODO: fill in.
          Point proj=project_point(Hba,Point(i+dx,j+dy));
          if (proj.x>=0 && proj.y>=0 && proj.x<b.w && proj.y<b.h) {
            float b_pixel=b.pixel_bilinear(proj.x,proj.y,k);
            if (c.pixel(i,j,k)>0) 
              c.pixel(i,j,k)=(ablendcoeff*c.pixel(i,j,k)+(1-ablendcoeff)*b_pixel);
            else 
              c.pixel(i,j,k)=b_pixel;
          }
          
        }
  

  // When doing cylindrical and spherical, how do we cope with the missing 
  // image values due to the warping process? Consider skipping them? 
  // How do we know they are empty? Look in the image class for the function
  // "is_nonempty_patch" and try to figure out why it might be useful.
  // The member 
  
  // TODO: Put your code here.
  
  
  
  // We trim the image so there are as few as possible black pixels.
  return trim_image(c);
  }

// Create a panoramam between two images.
// const Image& a, b: images to stitch together.
// float sigma: gaussian for harris corner detector. Typical: 2
// float thresh: threshold for corner/no corner. Typical: 1-5
// int nms: window to perform nms on. Typical: 3
// float inlier_thresh: threshold for RANSAC inliers. Typical: 2-5
// int iters: number of RANSAC iterations. Typical: 1,000-50,000
// int cutoff: RANSAC inlier cutoff. Typical: 10-100
Image panorama_image(const Image& a, const Image& b, float sigma, int corner_method, float thresh, int window, int nms, float inlier_thresh, int iters, int cutoff, float acoeff)
  {
  // Calculate corners and descriptors
  vector<Descriptor> ad;
  vector<Descriptor> bd;
  
  // doing it multithreading...
  thread tha([&](){ad = harris_corner_detector(a, sigma, thresh, window, nms, corner_method);});
  thread thb([&](){bd = harris_corner_detector(b, sigma, thresh, window, nms, corner_method);});
  tha.join();
  thb.join();
  
  // Find matches
  vector<Match> m = match_descriptors(ad, bd);
  
  // Run RANSAC to find the homography
  Matrix Hba = RANSAC(m, inlier_thresh, iters, cutoff);
  
  // Stitch the images together with the homography
  return combine_images(a, b, Hba, acoeff);
  }

// HW5 4.1
// Project an image onto a cylinder.
// const Image& im: image to project.
// float f: focal length used to take image (in pixels).
// returns: image projected onto cylinder, then flattened.
Image cylindrical_project(const Image& im, float f)
  {
  //TODO: project image onto a cylinder
  double hfov=atan(im.w/(2*f));
  double vfov=im.h/2./f;
  
  // For your convenience we have computed the output size
  Image c(im.w/cos(hfov),im.h/cos(hfov),im.c);
  
  NOT_IMPLEMENTED();
  
  return c;
  }

// HW5 4.2
// Project an image onto a cylinder.
// const Image& im: image to project.
// float f: focal length used to take image (in pixels).
// returns: image projected onto cylinder, then flattened.
Image spherical_project(const Image& im, float f)
  {
  //TODO: project image onto a sphere
  double hfov=atan(im.w/(2*f));
  double vfov=atan(im.h/(2*f));
  
  // For your convenience we have computed the output size
  Image c(im.w/cos(hfov),im.h/cos(hfov),im.c);
  
  NOT_IMPLEMENTED();
  
  return c;
  }
