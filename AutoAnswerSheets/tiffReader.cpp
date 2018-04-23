//
//  tiffReader.cpp
//  AutoAnswerSheets
//
//  Created by Niko@LOS on 2017/03/28.
//
//

#include "tiffReader.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

vector < Mat > tiffRead(const char* imageName)
{
    //Define the format of result;
    vector < Mat > result_images;
    
    // Open the TIFF file using libtiff
    TIFF* tif = TIFFOpen(imageName, "r");
    
    // Create a matrix to hold the tif image in
    Mat image;
    
    // check the tif is open
    if (tif) {
        do {
            unsigned int width, height;
            uint32* raster;
            
            // get the size of the tiff
            TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
            TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
            
            uint npixels = width*height; // get the total number of pixels
            
            raster = (uint32*)_TIFFmalloc(npixels * sizeof(uint32)); // allocate temp memory (must use the tiff library malloc)
            if (raster == NULL) // check the raster's memory was allocaed
            {
                TIFFClose(tif);
                //cerr << "Could not allocate memory for raster of TIFF image" << endl;
                //return -1;
            }
            
            // Check the tif read to the raster correctly
            if (!TIFFReadRGBAImage(tif, width, height, raster, 0))
            {
                TIFFClose(tif);
                //cerr << "Could not read raster of TIFF image" << endl;
                //return -1;
            }
            
            image = Mat(width, height, CV_8UC4); // create a new matrix of w x h with 8 bits per channel and 4 channels (RGBA)
            
            // itterate through all the pixels of the tif
            for (int x = 0; x < width; x++)
                for (int y = 0; y < height; y++)
                {
                    uint32& TiffPixel = raster[y*width+x]; // read the current pixel of the TIF
                    Vec4b& pixel = image.at<Vec4b>(Point(y, x)); // read the current pixel of the matrix
                    pixel[0] = TIFFGetB(TiffPixel); // Set the pixel values as BGRA
                    pixel[1] = TIFFGetG(TiffPixel);
                    pixel[2] = TIFFGetR(TiffPixel);
                    pixel[3] = TIFFGetA(TiffPixel);
                }
            
            _TIFFfree(raster); // release temp memory
            // Rotate the image 90 degrees couter clockwise
            image = image.t();
            flip(image, image, 0);
            /*
             imshow("TIF Image", image); // show the image
            waitKey(0); // wait for anykey before displaying next
            */
            result_images.push_back(image);
        } while (TIFFReadDirectory(tif)); // get the next tif
        TIFFClose(tif); // close the tif file
    }
    return result_images;
}
