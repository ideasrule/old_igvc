#include "Camera.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "PGRFlyCaptureStereo.h"
#include <assert.h>
#include <stdio.h>
#include "Data.h"

using namespace Pave_Libraries_Common;

//Camera placement
bool found;
double AngleInRads = Data::getDouble("CameraAngle", found);	//12 degrees
double CameraHeight = Data::getDouble("CameraHeight", found); //(meters)

bool Camera::initializeTriclops(FILE *tformFile)
{
	TriclopsError tErr;

	tErr = triclopsSetResolution( triclops, HEIGHT, WIDTH);
	_HANDLE_ERROR(tErr, "triclopsSetResolution");

	/*
	if(tformFile)
	{
		rewind(tformFile);
		if(tformFile && fscanf(tformFile, "Height %f Pitch Angle %f", &CameraHeight, &AngleInRads) != 2)
		{
			printf("Transform file not valid. Must begin with height and pitch angle.");
			exit(1);
		}
	}
	*/

	TriclopsTransform transform;

	double transformMatrix[4][4] =
		{ 
	 		{1.0 , 0.0, 0.0, 0.0},
			{0.0, -1, -1, CameraHeight},
			{0.0, -1, -1, 0.0},
			{0.0, 0.0, 0.0, 1.0}
		};
	transformMatrix[1][1] = -cos(AngleInRads);
	transformMatrix[1][2] = -sin(AngleInRads);
	transformMatrix[2][1] = -sin(AngleInRads);
	transformMatrix[2][2] = cos(AngleInRads);
	
	for(short i=0; i<4; i++) 
		for(short j=0; j<4; j++)
			transform.matrix[i][j] = transformMatrix[i][j];

	tErr = triclopsSetTriclopsToWorldTransform(triclops, transform);
	_HANDLE_ERROR(tErr, "triclopsSetTriclopsToWorldTransform");

	
	// GENERAL SETTINGS:			
	//tErr = triclopsSetDisparity( triclops, 0, 32);
	//_HANDLE_ERROR(tErr, "triclopsSetDisparity");
	
	tErr = triclopsSetRectify(triclops, 1);
	_HANDLE_ERROR(tErr, "triclopsSetRectify");

	tErr = triclopsSetLowpass(triclops, 1);
	_HANDLE_ERROR(tErr, "triclopsSetLowpass");
	
	tErr = triclopsSetSubpixelInterpolation( triclops, 1 );
	_HANDLE_ERROR(tErr, "triclopsSetSubpixelInterpolation");

	tErr = triclopsSetDisparity(triclops, 1, MAX_DISPARITY);
	_HANDLE_ERROR(tErr, "triclopsSetDisparity");

	// VALIDATION ROUTINES:
	tErr = triclopsSetSurfaceValidation(triclops, 0);//1);
	_HANDLE_ERROR(tErr, "triclopsSetSurfaceValidation");
	
	tErr =  triclopsSetSurfaceValidationMapping( triclops, ERROR_COLOR );
	_HANDLE_ERROR(tErr, "triclopsSetSurfaceValidationMapping");

	tErr = triclopsSetSurfaceValidationDifference(triclops, 2);//.1);
	_HANDLE_ERROR(tErr, "triclopsSetSurfaceValidationDifference");

	tErr = triclopsSetSurfaceValidationSize(triclops, 250);//350);
	_HANDLE_ERROR(tErr, "triclopsSetSurfaceValidationSize");
	

	tErr = triclopsSetStrictSubpixelValidation(triclops, 1);
	_HANDLE_ERROR(tErr, "triclopsSetStrictSubpixelValidation");

	tErr =  triclopsSetSubpixelValidationMapping( triclops, ERROR_COLOR );
	_HANDLE_ERROR(tErr, "triclopsSetSubpixelValidationMapping");


	tErr = triclopsSetTextureValidation(triclops, 1);
	_HANDLE_ERROR(tErr, "triclopsSetTextureValidation");
	
	tErr = triclopsSetTextureValidationMapping( triclops, ERROR_COLOR );
	_HANDLE_ERROR(tErr, "triclopsSetTextureValidationMapping");

	tErr = triclopsSetTextureValidationThreshold( triclops, 1.0 );
	_HANDLE_ERROR(tErr, "triclopsSetTextureValidationThreshold");


	tErr = triclopsSetUniquenessValidation(triclops, 1);
	_HANDLE_ERROR(tErr, "triclopsSetUniquenessValidation");

	tErr = triclopsSetUniquenessValidationMapping(triclops, ERROR_COLOR);
	_HANDLE_ERROR(tErr, "triclopsSetUniquenessValidationMapping");

	tErr = triclopsSetUniquenessValidationThreshold(triclops, .7);
	_HANDLE_ERROR(tErr, "triclopsSetUniquenessValidationThreshold");
	

	return true;
}


bool Camera::stereoProcess(DepthImage& img)
{
	TriclopsError tErr;
	TriclopsInput * inputData = &(img.input);

	assert(inputData != NULL);
	// Preprocessing (rectifying) the images
	tErr = triclopsPreprocess( triclops, inputData );
	if(tErr != 0) {
		printf("Error in preprocessing.\n");		
		return false;
	}
	// get a pointer to the rectified image	
	triclopsGetImage( triclops, TriImg_RECTIFIED, TriCam_REFERENCE, &(img.rectImage));

	// stereo processing
	tErr =  triclopsStereo( triclops );
	if(tErr != 0)
	{
		printf("Error in stereo processing.\n");		
		return false;
	}
	
	// retrieve the depth image from the context
	tErr = triclopsGetImage16( triclops, TriImg16_DISPARITY, TriCam_REFERENCE, &(img.dImage));
	if(tErr != 0)
	{
		printf("Error getting depth image.\n");
		return false;
	}
	return true;
}

void Camera::toXYZ(int row, int col, DepthImage img, float* x, float*y, float* z)
{	
	triclopsRCD16ToWorldXYZ(triclops, row, col, img[row][col], x, y, z);
}

void Camera::toRCD(float x, float y, float z, float* row, float* col, float* disp)
{	
	TriclopsError e = triclopsWorldXYZToRCD(triclops, x, y, z, row, col, disp);
}

float Camera::getFocalLength() {
	float retVal;
	triclopsGetFocalLength( triclops, &retVal);
	return retVal;
}

bool Camera::rectifyImages(DepthImage &img, TriclopsInput &colorData, Frame * frm) {

	TriclopsPackedColorImage colorImage;
	TriclopsError te;

	stereoProcess(img);
	
     te = triclopsRectifyPackedColorImage( triclops, 
	 TriCam_REFERENCE, 
	 &colorData, 
	 &colorImage );
      _HANDLE_TRICLOPS_ERROR( "triclopsRectifyColorImage()", te );

	/* Create the color image, if necessary */
	if (frm->colorImage == NULL)
		frm->colorImage = cvCreateImage( cvSize(NCOLS,NROWS), IPL_DEPTH_8U, 3);
	
	/* Copy color data */		
	for(int i = 0; i < NROWS; i++) {

		for(int j = 0; j < NCOLS; j++) {
			cvSet2D(frm->colorImage, i , j, cvScalar(colorImage.data[(MINROW + i)*NCOLS + j].value[0], colorImage.data[(MINROW + i)*NCOLS + j].value[1], colorImage.data[(MINROW + i)*NCOLS + j].value[2]));
		}
	}
	frm->cData = RgbImage(frm->colorImage);
	
	/* Create the mono image, if necessary */
	if (frm->monoImage == NULL)
		frm->monoImage = cvCreateImage( cvSize(NCOLS,NROWS), IPL_DEPTH_32F, 1);

	/* Convert to mono floats */
	for(int i = 0; i < NROWS; i++) {
		for(int j = 0; j < NCOLS; j++) {			
			//Weighted average, according to perceptual response (from CS426 lecture notes)
			//Actually, the bayer matrix seems to be out of alignment. just use green.
			cvSet2D(frm->monoImage, i, j, cvScalar(cvGet2D(frm->colorImage, i, j).val[2]/256.0)); //* .587 + frm->cData[i][j].r * .299 + frm->cData[i][j].b * .144; 
		}
	}
	
	frm->mData = BwImageFloat(frm->monoImage);

	 //triclopsSavePackedColorImage(&colorImage, "right-rectified.ppm");

	return true;
}