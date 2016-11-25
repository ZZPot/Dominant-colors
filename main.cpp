#include "DomColors.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <shlobj.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Comdlg32.lib")

#define WND_NAME_RGB_CUBE	"RGB cube"
#define WND_NAME_RGB_CIE	"RGB CIE"
#define WND_NAME_HSV_CUBE	"HSV cube"
#define WND_NAME_HSV_CIE	"HSV CIE"

#define IMG_WIDTH	400

std::string ChooseFile(HWND hWndOwner, LPCSTR pCaption);

int main()
{
	std::string img_file = ChooseFile(NULL, "Choose image");
	if(!img_file.length())
		return 1;
	cv::Mat img = cv::imread(img_file);
	cv::resize(img, img, cv::Size(IMG_WIDTH, (double)img.rows/img.cols * IMG_WIDTH));
	cv::Mat img_hsv;
	cv::cvtColor(img, img_hsv, CV_BGR2HSV);
	dominant_colors_graber dcg;
	cv::Vec3i cube_param_rgb = {1, 1, 1};
	cv::Vec3i cube_param_hsv = {1, 2, 2};
	cv::Vec3i cie76_param = {25, 0, 0};
	dcg.SetParam(cube_param_rgb);
	cv::Mat img_rgb_cube = ShowColors(img, dcg.GetDomColors(img, CS_BGR, DT_CUBE));
	dcg.SetParam(cie76_param);
	cv::Mat img_rgb_cie = ShowColors(img, dcg.GetDomColors(img, CS_BGR, DT_CIE76));
	dcg.SetParam(cube_param_hsv);
	cv::Mat img_hsv_cube = ShowColors(img_hsv, dcg.GetDomColors(img, CS_HSV, DT_CUBE));
	dcg.SetParam(cie76_param);
	cv::Mat img_hsv_cie = ShowColors(img_hsv, dcg.GetDomColors(img, CS_HSV, DT_CIE76));

	cv::cvtColor(img_hsv_cube, img_hsv_cube, CV_HSV2BGR);
	cv::cvtColor(img_hsv_cie, img_hsv_cie, CV_HSV2BGR);

	cv::imshow(WND_NAME_RGB_CUBE, img_rgb_cube);
	cv::imshow(WND_NAME_RGB_CIE, img_rgb_cie);
	cv::imshow(WND_NAME_HSV_CUBE, img_hsv_cube);
	cv::imshow(WND_NAME_HSV_CIE, img_hsv_cie);
	char key = cv::waitKey(0);
	return 0;
}

std::string ChooseFile(HWND hWndOwner, LPCSTR pCaption)
{
	std::string res = "";
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWndOwner;
	ofn.lpstrFilter = "All files\0*\0";
	ofn.lpstrFile = new TCHAR[MAX_PATH*2]; ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH*2-1;
	ofn.lpstrTitle = pCaption;
	ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	if(GetOpenFileName(&ofn))
		res = ofn.lpstrFile;
	delete [] ofn.lpstrFile;
	return res;
}