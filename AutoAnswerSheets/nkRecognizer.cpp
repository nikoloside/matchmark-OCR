//
//  nkRecognizer.cpp
//  AutoAnswerSheets
//
//  Created by Niko@LOS on 2017/03/28.
//
//

#include "nkRecognizer.hpp"

#include <iostream>
#include <unistd.h>

using namespace std;
using namespace cv;

vector < Mat >  formROI(Mat& input,int width, int height, float WHrateMin, float WHrateMax){
    
    
    Mat step1,step2,step3,step4,step5;
    vector < Mat > ans;
    vector < Point2f[4] > result_vertices;
    
    //step1　グレイスケール
    cvtColor(input,step1,CV_BGR2GRAY);
    //step2 二値化
    threshold(step1,step2,200,255,THRESH_BINARY_INV);
    //adaptiveThreshold(step1, step2, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 41, 9);
    
    //このサイズは画像のサイズに依存する,規定の画像は4680x3307のサイズで(4,3)文字検出, (8,8)枠線検出
    double e_size = 4 * input.cols / 4680;
    //cout<<e_size<<endl;
    Mat lineDetect_element = getStructuringElement(MORPH_RECT, Size(e_size*2 , e_size*2 ));
    Mat target_element = getStructuringElement(MORPH_RECT, Size(e_size, e_size));// * 3 / 4));
    // * 3 / 4));
    //step3 膨張
    dilate(step2,step3,lineDetect_element);
    //step4 腐食
    dilate(step2,step4,target_element);
    erode(step4,step5,target_element);
    
    
    
    Mat result = Mat::zeros(step3.size(), CV_8U);
    // 輪郭抽出
    vector< vector< Point > > contours;
    findContours(step4, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    
    // 輪郭が四角形かの判定
    Point2f vertices[4];
    for (auto contour = contours.begin(); contour != contours.end(); contour++){
        // 輪郭を直線近似
        vector< cv::Point > approx;
        approxPolyDP(Mat(*contour), approx, 50.0, true);
        // 近似が4線かつ面積が一定以上なら四角形
        double area = contourArea(approx);
        // 面積の一番小さい外接矩形で凸図形を囲み
        RotatedRect rect = minAreaRect(Mat(*contour));
        // 外接矩形の４つの点をRotatedRect rectから変換
        //Point2f vertices[4];
        rect.points(vertices);
        double width_image = rect.size.width;
        double height_image = rect.size.height;
        double rectArea = width_image * height_image;
        double WHrate = (width_image>height_image?width_image/height_image:height_image/width_image);
        
        if (approx.size() == 4 && area > 200000.0 && rectArea > 200000.0 && WHrate > WHrateMin && WHrate < WHrateMax){
            //polylines(result, vertices, true, Scalar(255), 2);
            
            //描く
            for (int i = 0; i < 4; i++) line(result, vertices[i], vertices[(i+1)%4], Scalar(255));
            stringstream sst;
            sst << "area : " << area << " width : " << rect.size.width << " height : " << rect.size.height;
            cout << "area : " << area << " width : " << rect.size.width << " height : " << rect.size.height << endl;
            putText(result, sst.str(), approx[0], CV_FONT_HERSHEY_PLAIN, 1.0, Scalar(255));
            
            result_vertices.push_back(vertices);
        }
    }
    
    
    //頂点判定
    for (int i = 0; i < result_vertices.size(); i++) {
        for(int j = 0; j < 4; j++){
            
        }
    }
    
    //射影変換
    Mat m_dImage_1 = Mat::zeros(Size(width,height), CV_8U);
    Mat m_dImage_2 = Mat::zeros(Size(width,height), CV_8U);
    Mat m_dImage_3 = Mat::zeros(Size(width,height), CV_8U);
    Point2f    dest_pt[4];
    RotatedRect    destRect = RotatedRect(Point2f(width/2, height/2), Size(width,height), 0);
    destRect.points(dest_pt);
    
    int q=0;
    vector < Point2f > toPoints, fromPoints;
    for(int i = q; i < q+4;i++) toPoints.push_back(dest_pt[i%4]);
    for(int i = 0; i < 4;i++) fromPoints.push_back(vertices[i]);
    Mat h = findHomography(fromPoints, toPoints);// findHomography(vertices, dest_pt);
    
    warpPerspective(input, m_dImage_1, h, m_dImage_1.size());//cvWarpPerspective(step5, m_dImage, h);
    imwrite("/Volumes/MainNikolos/Workspace/AutoAnswerSheets/AutoAnswerSheets/gragh/form.png", m_dImage_1);
    ans.push_back(m_dImage_1);
    
    warpPerspective(step3, m_dImage_2, h, m_dImage_2.size());//cvWarpPerspective(step5, m_dImage, h);
    imwrite("/Volumes/MainNikolos/Workspace/AutoAnswerSheets/AutoAnswerSheets/gragh/form_dilate.png", m_dImage_2);
    ans.push_back(m_dImage_2);
    
    warpPerspective(step5, m_dImage_3, h, m_dImage_3.size());//cvWarpPerspective(step5, m_dImage, h);
    imwrite("/Volumes/MainNikolos/Workspace/AutoAnswerSheets/AutoAnswerSheets/gragh/form_erode.png", m_dImage_3);
    ans.push_back(m_dImage_3);
    
    
    return ans;
}

Mat detectROI(Mat& input, Mat& form,int width, int height, float WHrateMin, float WHrateMax){
    
    Mat step1,step2,step3,step4,step5;
    
    //step1　グレイスケール
    cvtColor(input,step1,CV_BGR2GRAY);
    //step2 二値化 スキャンの状態による
    threshold(step1,step2,100,255,THRESH_BINARY_INV);//越高越好，需要有200浓度，但是为了让红笔涂改去掉，退回100
    //adaptiveThreshold(step1, step2, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 41, 9);
    
    //このサイズは画像のサイズに依存する,規定の画像は4680x3307のサイズで(4,3)文字検出, (8,8)枠線検出
    double e_size = 4 * input.cols / 4680;
    //cout<<e_size<<endl;
    Mat lineDetect_element = getStructuringElement(MORPH_RECT, Size(e_size*2 , e_size*2 ));
    Mat target_element = getStructuringElement(MORPH_RECT, Size(e_size, e_size));// * 3 / 4));
    //step3 膨張
    dilate(step2,step3,lineDetect_element);
    //step4 腐食
    dilate(step2,step4,target_element);
    erode(step4,step5,target_element);
    
    
    Mat result = Mat::zeros(step3.size(), CV_8U);
    Mat ans;
    // 輪郭抽出
    vector< vector< Point > > contours;
    findContours(step4, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    
    // 検出された輪郭線の描画
    //for (auto contour = contours.begin(); contour != contours.end(); contour++){
    //    polylines(result, *contour, true, cv::Scalar(255), 2);
    //}
    
    // 輪郭が四角形かの判定
    for (auto contour = contours.begin(); contour != contours.end(); contour++){
        // 輪郭を直線近似
        vector< cv::Point > approx;
        approxPolyDP(Mat(*contour), approx, 50.0, true);
        // 近似が4線かつ面積が一定以上なら四角形
        double area = contourArea(approx);
        // 面積の一番小さい外接矩形で凸図形を囲み
        RotatedRect rect = minAreaRect(Mat(*contour));
        // 外接矩形の４つの点をRotatedRect rectから変換
        Point2f vertices[4];
        rect.points(vertices);
        double width_image = rect.size.width;
        double height_image = rect.size.height;
        double rectArea = width_image * height_image;
        double WHrate = (width_image>height_image?width_image/height_image:height_image/width_image);
        
        if (approx.size() == 4 && area > 200000.0 && rectArea > 200000.0 && WHrate > WHrateMin && WHrate < WHrateMax){
            //polylines(result, vertices, true, Scalar(255), 2);
            for (int i = 0; i < 4; i++) line(result, vertices[i], vertices[(i+1)%4], Scalar(255));
            stringstream sst;
            sst << "area : " << area << " width : " << rect.size.width << " height : " << rect.size.height;
            cout << "area : " << area << " width : " << rect.size.width << " height : " << rect.size.height << endl;
            putText(result, sst.str(), approx[0], CV_FONT_HERSHEY_PLAIN, 1.0, Scalar(255));
            
            //射影変換
            
            Mat m_dImage = Mat::zeros(Size(width,height), CV_8U);
            Point2f    dest_pt[4];
            RotatedRect    destRect = RotatedRect(Point2f(width/2, height/2), Size(width,height), 0);
            destRect.points(dest_pt);
            
            for (int q = 0; q < 4; q++){
                //int q=0;
                vector < Point2f > toPoints, fromPoints;
                for(int i = q; i < q+4;i++) toPoints.push_back(dest_pt[i%4]);
                for(int i = 0; i < 4;i++) fromPoints.push_back(vertices[i]);
                Mat h = findHomography(fromPoints, toPoints);// findHomography(vertices, dest_pt);
                warpPerspective(step3, m_dImage, h, m_dImage.size());//cvWarpPerspective(step5, m_dImage, h);
                
                Mat result;
                matchTemplate(m_dImage, form, result, CV_TM_CCOEFF_NORMED);
                
                float res = result.at<float>(0,0);
                cout<<res<<endl;
                
                if (res > 0.7) warpPerspective(step5, ans, h, m_dImage.size());
                
                /*
                 imshow("test1",form);
                 waitKey(0);
                 imshow("test1",m_dImage);
                 waitKey(0);
                 */
            }
        }
        
        
    }
    
    
    
    imwrite("/Volumes/MainNikolos/Workspace/AutoAnswerSheets/AutoAnswerSheets/gragh/result.png", result);
    imwrite("/Volumes/MainNikolos/Workspace/AutoAnswerSheets/AutoAnswerSheets/gragh/ans.png", ans);
    /*
     waitKey(0);
     for(int i = 1; i < 255; i+=10) {
     namedWindow("Binary",WINDOW_NORMAL);
     Mat dst;
     resize(step2, dst, Size(), 0.5, 0.5, INTER_NEAREST);
     cout<<"i : "<<i<<endl;
     imshow("Binary",dst);
     waitKey(0);
     threshold(step1,step2,i,255,THRESH_BINARY_INV);
     }
     
     namedWindow("Binary",WINDOW_NORMAL);
     imshow("Binary",step2);
     waitKey(0);
     
     */
    return ans;
    
    
    
}


double coefficient(Mat& image1, Mat& image2){
    
    
    Mat result;
    matchTemplate(image1, image2, result, CV_TM_CCOEFF_NORMED);
    
    //    cout<<endl;
    //    cout<<endl;
    
    float min = 1;
    float max = 0;
    
    for(int i=0; i < result.cols; i++){
        for (int j=0; j < result.rows; j++) {
            float coef = result.at<float>(j,i);
            //            cout<<coef<<" ";
            if (min>coef) min = coef;
            if (max<coef) max = coef;
        }
        //        cout<<endl;
    }
    
    return max;
    
}

vector <Mat> ansROI(Mat& input, int x, int y, float dx, int width, int height, int num){
    vector <Mat> ans;
    for (int i = 0;i < num;i++) {
        Rect rect_f = Rect(x + i * dx, y, width, height);
        
        Mat roi(input,rect_f);
        /*
         imshow("roi",roi);
         waitKey(0);
         */
        ans.push_back(roi);
    }
    
    return ans;
    
    
}

void analysis(Mat& display, Mat& target_image, vector <Mat> sample_images, int x, int y, float dx, float dy, int width, int height, int num){
    
    for (int j = 0; j < 26; j++ ){
        for (int i = 0;i < num;i++) {
            Rect rect_c = Rect(x + i * dx, y + j * dy - 4,width,height + 4);//46,57);
            
            Mat roi_c(target_image,rect_c);
            
            
            double res_c = coefficient(sample_images[i], roi_c);
            //                cout<<endl;
            //                cout<<"res_c : "<<res_c<<" y : "<<y+j*dy<<endl;
            
            char chara = 65 + j;
            if (res_c < 0.65) {//0.6 < x < 0.7
                cout<<chara<<" : "<<i-1<<endl;
                cout<<"res_c : "<<res_c<<" y : "<<y+j*dy<<endl;
                
                rectangle(display, Point(rect_c.x,rect_c.y), Point(rect_c.x+rect_c.width, rect_c.y+rect_c.height), Scalar(255), -1, CV_AA);
                
                namedWindow("form",WINDOW_NORMAL);
                namedWindow("test1",WINDOW_NORMAL);
                imshow("form",sample_images[i]);
                imshow("test1",roi_c);
                waitKey(0);
                
            }
            
            
        }
    }
}

vector <Mat> getSample_images(){
    
    string path = "/Volumes/MainNikolos/Workspace/AutoAnswerSheets/AutoAnswerSheets/gragh/";
    Mat input_form = imread(path+"Data/MathEnterSheets_Page_1.png");
    
        
    //初期値の設定 比較画像のサイズ指定　width height  目標枠線の縦横比 WHRateMin WHRateMax
    int width = 2000;
    int height = 2000 * 1486 / 1455;
    float WHRateMin = 1.0;
    float WHRateMax = 1.2;
    
    int form_x = 1314;
    int form_y = 1195;
    int form_width = 35;
    int form_height = 45;
    float form_dx = 55.2;
    int num_cols = 11;
    //int num_rows = 26;
    
    //formからフォーム領域を抽出
    vector <Mat> form_images = formROI(input_form, width, height, WHRateMin, WHRateMax);
    //フォーム内の選択肢比較用画像抽出
    vector <Mat> sample_images = ansROI(form_images[2], form_x, form_y, form_dx, form_width, form_height, num_cols);
        
    return sample_images;
    
}
/*
Mat extractTemplate(){
    
    
}*/

/*
void ExampleMain(){
    string path = "/Volumes/MainNikolos/Workspace/AutoAnswerSheets/AutoAnswerSheets/gragh/";
    Mat input_target = imread(path+"pdf/S28C-6e17032215510 34.png");
    //Mat input_form = imread(path+"input_math_1-2.png");
    Mat input_form = imread(path+"Data/MathEnterSheets_Page_1.png");
    
    if(!input_form.data && !input_target.data) cout<<"No Data"<<endl;
    else {
        
        //初期値の設定 比較画像のサイズ指定　width height  目標枠線の縦横比 WHRateMin WHRateMax
        int width = 2000;
        int height = 2000 * 1486 / 1455;
        float WHRateMin = 1.0;
        float WHRateMax = 1.2;
        
        int form_x = 1314;
        int form_y = 1195;
        int form_width = 35;
        int form_height = 45;
        float form_dx = 55.2;
        int num_cols = 11;
        //int num_rows = 26;
        
        //formからフォーム領域を抽出
        vector <Mat> form_images = formROI(input_form, width, height, WHRateMin, WHRateMax);
        //フォーム内の選択肢比較用画像抽出
        vector <Mat> sample_images = ansROI(form_images[2], form_x, form_y, form_dx, form_width, form_height, num_cols);
        
        //解析目標画像の抽出(1枠対応済み 3枠未対応)
        Mat target_image = detectROI(input_target, form_images[1], width, height, WHRateMin, WHRateMax);
        
        //解析 開始位置　x y 項目の間隔 dx dy
        Mat display = target_image.clone();
        int x = 1858 - 551 - 1046;
        int y = 1244 - 605 - 54.9;
        float dx = 55.2;
        float dy = 54.9;
        int box_width = 46;
        int box_height = 57;
        int ix = x + 1046;
        
        //解析開始　縦の項目が26以上の時未対応　結果データ構造の構成は未対応
        analysis(display, target_image, sample_images, x, y, dx, dy, box_width, box_height, num_cols);
        
        analysis(display, target_image, sample_images, ix, y, dx, dy, box_width, box_height, num_cols);
        
        imwrite("/Volumes/MainNikolos/Workspace/AutoAnswerSheets/AutoAnswerSheets/gragh/display.png", display);
        
    }
 
}
*/

void ExampleMain(){
    string path = "/Volumes/MainNikolos/Workspace/AutoAnswerSheets/AutoAnswerSheets/gragh/";
    Mat input_target = imread(path+"Data/input_math_1-2.png");
    //Mat input_form = imread(path+"input_math_1-2.png");
    Mat input_form = imread(path+"Data/MathEnterSheets_Page_2.png");
    
    if(!input_form.data && !input_target.data) cout<<"No Data"<<endl;
    else {
        
        //初期値の設定 比較画像のサイズ指定　width height  目標枠線の縦横比 WHRateMin WHRateMax
        int width = 2000;
        int height = 2000 * 1486 / 1455;
        float WHRateMin = 1.8;
        float WHRateMax = 2.3;
        
        int num_cols = 11;
        
        //formからフォーム領域を抽出
        vector <Mat> form_images = formROI(input_form, width, height, WHRateMin, WHRateMax);
        //フォーム内の選択肢比較用画像抽出
        vector <Mat> sample_images = getSample_images();
        
        //解析目標画像の抽出(1枠対応済み 3枠未対応)
        Mat target_image = detectROI(input_target, form_images[1], width, height, WHRateMin, WHRateMax);
        
        //解析 開始位置　x y 項目の間隔 dx dy
        Mat display = target_image.clone();
        int x = 1858 - 551 - 1046;
        int y = 1244 - 605 - 54.9;
        float dx = 55.2;
        float dy = 54.9;
        int box_width = 46;
        int box_height = 57;
        int ix = x + 1046;
        
        //解析開始　縦の項目が26以上の時未対応　結果データ構造の構成は未対応
        analysis(display, target_image, sample_images, x, y, dx, dy, box_width, box_height, num_cols);
        
        analysis(display, target_image, sample_images, ix, y, dx, dy, box_width, box_height, num_cols);
        
        imwrite("/Volumes/MainNikolos/Workspace/AutoAnswerSheets/AutoAnswerSheets/gragh/display.png", display);
        
    }
    
}
