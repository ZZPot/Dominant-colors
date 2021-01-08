#include "DomColors.h"
#include "common.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <shlobj.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Comdlg32.lib")

#define WND_NAME_RGB_CUBE	"RGB cube"
#define WND_NAME_RGB_CIE76	"RGB CIE76"
#define WND_NAME_HSV_CUBE	"HSV cube"
#define WND_NAME_HSV_CIE76	"HSV CIE76"
#define WND_NAME_RGB_KMEANS	"RGB K-means"
#define WND_NAME_HSV_KMEANS	"HSV K-means"
#define WND_NAME_RGB_CIE94	"RGB CIE94"
#define WND_NAME_HSV_CIE94	"HSV CIE94"

#define IMG_WIDTH	400

int main()
{
	std::string img_file = ChooseFile(NULL, "Choose image");
	if(!img_file.length())
		return 1;
	cv::Mat img = cv::imread(img_file);
	cv::resize(img, img, cv::Size(IMG_WIDTH, (double)img.rows/img.cols * IMG_WIDTH));
	cv::Mat img_hsv;
	cv::cvtColor(img, img_hsv, cv::COLOR_BGR2HSV);

	dominant_colors_graber dcg;
	cv::Vec3i cube_param_rgb = {1, 1, 1};
	cv::Vec3i cube_param_hsv = {1, 2, 2};
	cv::Vec3i cie76_param = {25, 0, 0};
	cv::Vec3i cie94_param = {15, 0, 0};

	dcg.SetParam(cube_param_rgb);
	cv::Mat img_rgb_cube = ShowColors(img, dcg.GetDomColors(img, CS_BGR, DT_CUBE));
	dcg.SetParam(cube_param_hsv);
	cv::Mat img_hsv_cube = ShowColors(img_hsv, dcg.GetDomColors(img, CS_HSV, DT_CUBE));
	
	dcg.SetParam(cie76_param);
	cv::Mat img_rgb_cie76 = ShowColors(img, dcg.GetDomColors(img, CS_BGR, DT_CIE76));
	cv::Mat img_hsv_cie76 = ShowColors(img_hsv, dcg.GetDomColors(img, CS_HSV, DT_CIE76));

	cv::Mat img_rgb_kmeans = ShowColors(img, dcg.GetDomColors(img, CS_BGR, DT_KMEANS));
	cv::Mat img_hsv_kmeans = ShowColors(img_hsv, dcg.GetDomColors(img, CS_HSV, DT_KMEANS));

	dcg.SetParam(cie94_param);
	cv::Mat img_rgb_cie94 = ShowColors(img, dcg.GetDomColors(img, CS_BGR, DT_CIE94));
	cv::Mat img_hsv_cie94 = ShowColors(img_hsv, dcg.GetDomColors(img, CS_HSV, DT_CIE94));

	cv::cvtColor(img_hsv_cube, img_hsv_cube, cv::COLOR_HSV2BGR);
	cv::cvtColor(img_hsv_cie76, img_hsv_cie76, cv::COLOR_HSV2BGR);
	cv::cvtColor(img_hsv_kmeans, img_hsv_kmeans, cv::COLOR_HSV2BGR);
	cv::cvtColor(img_hsv_cie94, img_hsv_cie94, cv::COLOR_HSV2BGR);

	cv::imshow(WND_NAME_RGB_CUBE, img_rgb_cube);
	cv::imshow(WND_NAME_RGB_CIE76, img_rgb_cie76);
	cv::imshow(WND_NAME_RGB_KMEANS, img_rgb_kmeans);
	cv::imshow(WND_NAME_RGB_CIE94, img_rgb_cie94);
	cv::imshow(WND_NAME_HSV_CUBE, img_hsv_cube);
	cv::imshow(WND_NAME_HSV_CIE76, img_hsv_cie76);
	cv::imshow(WND_NAME_HSV_KMEANS, img_hsv_kmeans);
	cv::imshow(WND_NAME_HSV_CIE94, img_hsv_cie94);
	char key = cv::waitKey(0);
	return 0;
}