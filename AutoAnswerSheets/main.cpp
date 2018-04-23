//
//  main.cpp
//  AutoAnswerSheets
//
//  Created by Niko@LOS on 2017/03/07.
//
//

#define MATH1 1
#define MATH2 2
#define ART 3
#define PHYSICS 4
#define CHEMISTRY 5
#define BIOLOGY 6

#define OMOTE_SIDE 10001;
#define URA_SIDE 20002;


#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "tiffReader.hpp"
#include "nkRecognizer.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;

        int main(){
            
            
            //比較用画像の抽出抽出
            ExampleMain();
            return 0;
        }
