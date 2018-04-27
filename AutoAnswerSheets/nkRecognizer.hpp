//
//  nkRecognizer.hpp
//  AutoAnswerSheets
//
//  Created by Niko@LOS on 2017/03/28.
//
//

#ifndef nkRecognizer_hpp
#define nkRecognizer_hpp

#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/calib3d.hpp>

#include <stdio.h>

//外部関数

void ExampleMain();
std::vector <cv::Mat> getSample_images();

//内部関数
class MatchRecognizer {
    
private:
    
    
    
public:
    const int N = 100;
    enum {
        N123 = 123,
    };
    //.....
};
    //formからフォーム領域を抽出
std::vector < cv::Mat >  formROI(cv::Mat& input,int width, int height, float WHrateMin, float WHrateMax);
    //解析目標画像の抽出(1枠対応済み 3枠未対応)
cv::Mat detectROI(cv::Mat& input, cv::Mat& form,int width, int height, float WHrateMin, float WHrateMax);
    //相関計算
double coefficient(cv::Mat& image1, cv::Mat& image2);
    //フォーム内の選択肢比較用画像抽出
std::vector <cv::Mat> ansROI(cv::Mat& input, int x, int y, float dx, int width, int height, int num);
    //解析開始　縦の項目が26以上の時未対応　結果データ構造の構成は未対応
void analysis(cv::Mat& display, cv::Mat& target_image, std::vector <cv::Mat> sample_images, int x, int y, float dx, float dy, int width, int height, int num);


#endif /* nkRecognizer_hpp */
