#include "DomColors.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <shlobj.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Comdlg32.lib")

#define WND_NAME_COLORS	"Colors"
#define IMG_WIDTH	800

std::string ChooseFile(HWND hWndOwner, LPCSTR pCaption);

int main()
{
	std::string img_file = ChooseFile(NULL, "Choose image");
	if(!img_file.length())
		return 1;
	cv::Mat img = cv::imread(img_file);
	cv::resize(img, img, cv::Size(IMG_WIDTH, (double)img.rows/img.cols * IMG_WIDTH));
	cv::imshow(WND_NAME_COLORS, ShowColors(img));
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