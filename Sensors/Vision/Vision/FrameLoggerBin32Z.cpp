#include "FrameLoggerBin32Z.h"
#include "Frame.h"
#include "Common.h"
#include <direct.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <fstream>
#include <windows.h> //For Win32 File Writing
#include "zlib.h"

using namespace std;
using namespace cv;

using Pave_Libraries_Camera::Frame;
//static bool printDebug = true;

#define CHUNK 3686400

VOID CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode,DWORD dwNumberOfBytesTransfered,LPOVERLAPPED lpOverlapped)
{
if(dwErrorCode){
	printf("\nFatal error");
	return;
} else{
	printf("Wrote : %u", dwNumberOfBytesTransfered);
}

}

// Function to use zLib to decompress one file to another.
int inf(FILE *source, FILE *dest)
{
    int ret;
    unsigned have;
    z_stream strm;
	typedef scoped_array<unsigned char> scoped_buf;
    scoped_buf in(new unsigned char[CHUNK]);
    scoped_buf out(new unsigned char[CHUNK]);
    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;
   /* decompress until deflate stream ends or end of file */
    do {
       strm.avail_in = fread(in.get(), 1, CHUNK, source);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in.get();
        /* run inflate() on input until output buffer not full */
        do {

            strm.avail_out = CHUNK;
            strm.next_out = out.get();
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out.get(), 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
       /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);
    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}


// Function to use zLib to compress an unsigned char array into another one.
// level is the compression level between 1 and 8;  higher is more compressed but slower.
// sizeIn for this implementation should be the full size of the input array
// Function returns the size of the output array
int defmem(unsigned char *inData, unsigned char *outData, int level, int sizeIn = CHUNK){
	int ret, flush;
	unsigned have;
	z_stream strm;

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, level);
	if (ret != Z_OK){
		cerr << "Error" << endl;
		return ret;
	}

	strm.avail_in = sizeIn;
	strm.next_in = inData;

	/* run deflate() on input until output buffer not full, finish
	compression if all of source has been read in */
	strm.avail_out = sizeIn;
	strm.next_out = outData;
	ret = deflate(&strm, Z_FINISH);    /* no bad return value */
	assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
	have = sizeIn - strm.avail_out;
	assert(strm.avail_in == 0);     /* all input will be used */
	(void)deflateEnd(&strm);
	return have;
}

FrameLoggerBin32Z::FrameLoggerBin32Z(const std::string &baseDirectory)
: FrameLogger(baseDirectory)
{
	mkdir(getDirectory().c_str());
	manifest.open((getDirectory() + "/manifest.txt").c_str(), ios::in|ios::out|ios::app);
	if(printDebug) cout << "Log opened: " << getDirectory() << endl;
}
FrameLoggerBin32Z::FrameLoggerBin32Z(const std::string &baseDirectory, const std::string &name)
: FrameLogger(baseDirectory, name)
{
	mkdir(getDirectory().c_str());
	manifest.open((getDirectory() + "/manifest.txt").c_str());
	if(printDebug) cout << "Log opened: " << getDirectory() << endl;
	manifest.seekg (0, ios::beg);
}

FrameLoggerBin32Z::~FrameLoggerBin32Z()
{
	manifest.close();
	if(printDebug) cout << "Log closed: " << getDirectory() << endl;
}

void FrameLoggerBin32Z::log(shared_ptr<Pave_Libraries_Camera::Frame> frm, 
	Pave_Libraries_Common::StateEstimationType *state)
{
	manifest << frm->framenum << "\t";
	manifest << frm->name << "\t";
	manifest << frm->focalLength << "\t";
	manifest << frm->height << "\t";
	manifest << frm->width << "\t";
	if (state) {
		manifest << state->Northing << "\t";
		manifest << state->Easting << "\t";
		manifest << state->Heading << "\t";
		manifest << state->Speed << endl;
	} else {
		manifest << "0\t0\t0\t0" << endl;
	}

	string basename = getDirectory() + "/" + frm->name;
	cv::imwrite(basename + "-color.png", frm->color);

	// Open and create the "valid" file, which shows which pixels had valid stereo disparity.
	string validName = (basename + "-valid");
	char *myValidFileName = new char[validName.length() + 1];
	strncpy(myValidFileName, validName.c_str(), validName.length() + 1);
	int myVFileNameLen = lstrlenA(myValidFileName);
	BSTR unicodeVFileName = SysAllocStringLen(NULL, myVFileNameLen);
	::MultiByteToWideChar(CP_ACP, 0, myValidFileName, myVFileNameLen, unicodeVFileName, myVFileNameLen);
	HANDLE hW32valid = CreateFile (unicodeVFileName, FILE_WRITE_DATA, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	::SysFreeString(unicodeVFileName);
	delete [] myValidFileName;

	// Code added to attempt WIN32 file write of log file.  email jcvalent with questions/problems
  	string fullName = (basename + "-transformed32-c.bin");
	char *myFileName = new char[fullName.length() + 1];
	strncpy(myFileName, fullName.c_str(), fullName.length() + 1);
	int myFileNameLen = lstrlenA(myFileName);
	BSTR unicodeFileName = SysAllocStringLen(NULL, myFileNameLen);
	::MultiByteToWideChar(CP_ACP, 0, myFileName, myFileNameLen, unicodeFileName, myFileNameLen);
	
	HANDLE hW32transformed = CreateFile (unicodeFileName, FILE_WRITE_DATA, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	::SysFreeString(unicodeFileName);
	delete [] myFileName;

	DWORD Written=0;
	// Compress the valid file.
	unsigned char * voutBuffer = new unsigned char [frm->height * frm->width * sizeof(bool)];  
	int compSizeValid = defmem((unsigned char *)frm->validArrPtr, voutBuffer, 8, frm->height * frm->width * sizeof(bool));
	WriteFile(hW32valid, (char *)voutBuffer, compSizeValid, &Written, 0);
	delete [] voutBuffer;
	CloseHandle ( hW32valid );

	Written=0;
	unsigned char * outBuffer = new unsigned char [frm->height*frm->width*sizeof(float)];  // Assumes 3x compression!

	float *writeCloud = new float[frm->width*frm->height*3];
	cv::MatIterator_<Point3f> it = frm->transformedCloud.begin();
	cv::MatIterator_<Point3f> itEnd = frm->transformedCloud.end();
	for (int i = 0; it != itEnd; it++, i += 3) {
		float *pt = writeCloud + i;
		pt[0] = (*it).x;
		pt[1] = (*it).y;
		pt[2] = (*it).z;
	}

	int compSize = defmem((unsigned char*)writeCloud, outBuffer, 8, frm->height*frm->width*sizeof(float)*3);
	WriteFile(hW32transformed, (char*)outBuffer, compSize, &Written, 0);
	delete[] writeCloud;
	delete [] outBuffer;
	if(printDebug) cerr << endl << "Wrote " << Written << " bytes to compressed transform file." << endl;

	CloseHandle ( hW32transformed );
}

shared_ptr<Frame> FrameLoggerBin32Z::readLog(
	Pave_Libraries_Common::StateEstimationType *state)
{
	shared_ptr<Frame> frm(new Frame());

	// Clunky EOF checking but it works.
	if(printDebug) cout << "Reading at: " << manifest.tellg() << endl;
	manifest >> frm->framenum;
	if (!manifest.good())
		return shared_ptr<Frame>();
	else
		if(printDebug)
			cout << "Manifest Is Good." << endl;
	manifest >> frm->name;
	manifest >> frm->focalLength;
	int h, w;
	manifest >> h >> w;
	frm->setSize(w, h);
	Pave_Libraries_Common::StateEstimationType temp;
	if (!state) state = &temp;
	manifest >> state->Northing;
	manifest >> state->Easting;
	manifest >> state->Heading;
	manifest >> state->Speed;

	string basename = getDirectory() + "/" + frm->name;
	frm->color = cv::imread(basename + "-color.png", 1);
	cv::Mat_<unsigned char> monoConverted = cv::imread(basename + "-mono.png", 0);
	monoConverted.convertTo(frm->mono, frm->mono.type());

	ifstream valid((basename + "-valid").c_str());

	FILE *iFile = fopen((basename + "-transformed32-c.bin").c_str(), "rb");
	FILE *oFile = fopen((basename + "-transformed32.$$$").c_str(), "wb");

	inf(iFile, oFile);
	fclose(iFile);
	fclose(oFile);
    ifstream transformed(("-transformed32.$$$"), ios::binary);
	valid.read((char *)frm->validArrPtr, frm->height * frm->width * sizeof(bool));
	valid.close();
	
	if(printDebug) cout << "Transformed read started at " << transformed.tellg() << endl;
    int numpts = 0;

	float *rawCloud = new float[frm->width*frm->height*3];
	transformed.read((char *) rawCloud, frm->width*frm->height*3*sizeof(float));
	MatIterator_<Point3f> it = frm->transformedCloud.begin();
	MatIterator_<Point3f> itEnd = frm->transformedCloud.end();
	for (int i = 0; it != itEnd; it++, i += 3) {
		numpts++;
		float *pt = rawCloud + i;
		*it = Point3f(pt[0], pt[1], pt[2]);
	}
	delete[] rawCloud;

    if(printDebug) cerr << "Number of points is " << numpts << endl;
	if(printDebug) cout << "Transformed read ended at " << transformed.tellg() << endl;
	transformed.close();

	return frm;
}
