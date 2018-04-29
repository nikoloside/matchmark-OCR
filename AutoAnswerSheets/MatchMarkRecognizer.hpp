//
//  MatchMarkRecognizer.hpp
//  AutoAnswerSheets
//
//  Created by Niko@LOS on 2018/04/27.
//
//


#ifndef MatchMarkRecognizer_hpp
#define MatchMarkRecognizer_hpp

//外部ライブラリ
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/calib3d.hpp>

//システムライブラリ
#include <stdio.h>

class MatchMarkRecognizer {

public:
    //コンストラクタ;
    MatchMarkRecognizer()
    {
      setFormPara(
        2000,
        2000*1486/1455,
        1.0,
        1.2,
        1314,
        1195,
        35,
        45,
        55.2,
        11,
        26
      );
      setTestPara(
        1858 - 551 - 1046,
        1244 - 605 - 54.9,
        55.2,
        54.9,
        46,
        57,
        1046
      );
      string path = "/Volumes/MainNikolos/Workspace/AutoAnswerSheets/AutoAnswerSheets/gragh/";
      string input_file = "Data/MathEnterSheets_Page_1.png";
      string output_file = "Output/display.png";
      setInputPath(path, input_file);
      setOutputPath(path, output_file);
    }

    //Public Functions
    bool setInputPath(
      string input_form_path,
      string input_form_filename
      int input_type
    );
    bool setOutputPath(
      string output_form_path,
      string output_form_filename
    );
    bool setFormPara(
      int form_width,
      int form_height,
      float WHRateMin,
      float WHRateMax,
      int form_x,
      int form_y,
      int form_width,
      int form_height,
      float form_dx,
      int num_cols, //行の数
      int num_rows, //列の数
    );
    bool setTestPara(
        int start_x,
        int start_y,
        float dx,
        float dy,
        int unit_box_width,
        int unit_box_height,
        int start_dx
    );
    bool paraSetCheck();
    vector <Mat> get_Sample_images();
    bool recoginize_Test_images(cv::Mat& result, vector < string >& result_arr);

private:

    //入力のフォームに関するデータ
    string input_form_path;
    string input_form_filename;
    cv::Mat input_form_image;
    //フォーム全体のパラメータ
    int form_width; //規定のフォーム画像サイズ
    int form_height;　//規定のフォーム画像サイズ
    float WHRateMin;　//フィルター 規定のフォーム画像の長さの最低比率
    float WHRateMax;　//フィルター 規定のフォーム画像の長さの最大比率
    //フォーム内の行列要素のパラメータ
    int form_x;
    int form_y;
    int form_width;
    int form_height;
    float form_dx;
    int num_cols;

    vector <cv::Mat> input_form_images;

    //入力のテストセットに関するデータ
    string input_test_path;
    string input_test_filename;
    cv::Mat input_test_image;

    vector < cv::Mat > input_test_images;

    //出力に関するデータ
    string output_path;
    string output_filename;
    cv::Mat output_image;

    //formからフォーム領域を抽出
    std::vector < cv::Mat >  formROI(cv::Mat& input);//,int width, int height, float WHrateMin, float WHrateMax);
    //解析目標画像の抽出(1枠対応済み 3枠未対応)
    cv::Mat detectROI(cv::Mat& input, cv::Mat& form);//,int width, int height, float WHrateMin, float WHrateMax);
    //相関計算
    double coefficient(cv::Mat& image1, cv::Mat& image2);
    //フォーム内の選択肢比較用画像抽出
    std::vector <cv::Mat> ansROI(cv::Mat& input, int x, int y, float dx, int width, int height, int num);
    //解析開始　縦の項目が26以上の時未対応　結果データ構造の構成は未対応
    void analysis(cv::Mat& display, cv::Mat& target_image, std::vector <cv::Mat> sample_images);//, int x, int y, float dx, float dy, int width, int height, int num);
};

#endif /* MatchMarkRecognizer_hpp */
