//
//  tiffReader.hpp
//  AutoAnswerSheets
//
//  Created by Niko@LOS on 2017/03/28.
//
//

#ifndef tiffReader_hpp
#define tiffReader_hpp

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdio.h>
#include <iostream>
#include "tiff.h"
#include "tiffio.h"

std::vector < cv::Mat > tiffRead(const char* imageName);

#endif /* tiffReader_hpp */
