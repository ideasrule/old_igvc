/*******************************************************************************
 *  Image.h
 *
 *  Provides easy access to image data.
 *  
 *  blank
 *
 *  Usage:
 *  blank
 *  
 *  Dependancies: cv.h, cxcore.h, highgui.h
 *
 *  Author: copied from:
 *  http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.html
 *******************************************************************************/

#ifndef IMAGE_H
#define IMAGE_H

#include <cv.hpp>
#include <cxcore.hpp>
#include <highgui.hpp>

template<class T> class Image
{
public:
    int height() { return mat->rows; }
    int width()  { return mat->cols; }
    Image(cv::Mat* _mat) { mat = _mat; }
    ~Image() { mat = NULL; }
    inline T* operator[](const int rowIndex)
    {
        return (T*)(mat->data + rowIndex * mat->step);
    }

private:
    cv::Mat *mat;
};

typedef struct {
  unsigned char b, g, r;
} RgbPixel;

typedef struct {
  float b, g, r;
} RgbPixelFloat;

typedef struct {
  unsigned char h, s, v;
} HsvPixel;

typedef struct {
	unsigned char r, g, b, u;
} RgbuPixel;

typedef Image<RgbPixel>       RgbImage;
typedef Image<RgbPixelFloat>  RgbImageFloat;
typedef Image<unsigned char>  BwImage;
typedef Image<char>			  BwSignedImage;
typedef Image<float>          BwImageFloat;
typedef Image<double>         BwImageDouble;
typedef Image<HsvPixel>       HsvImage;
typedef Image<RgbuPixel>      RgbuImage;

#endif  // IMAGE_H