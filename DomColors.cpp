#include "DomColors.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#pragma warning(disable: 4244 4267)

std::vector<cv::Scalar> GetDomColors(cv::Mat hist, double max_sum, int max_count)
{
	double sum = 0;
	std::vector<cv::Scalar> res;
	cv::Vec3i cube_size = {HIST_SIZE_H/9, HIST_SIZE_S/4, HIST_SIZE_V/4};
	float cie_dist = 20;
#ifdef RGB_HIST
	cube_size[0] = HIST_SIZE_B/8;
	cube_size[1] = HIST_SIZE_G/8;
	cube_size[2] = HIST_SIZE_R/8;
#endif
#ifdef CIE_DIST
	cube_size[0] = cie_dist;
#endif
	cv::Mat hist_mask = cv::Mat::Mat(3, hist.size, CV_8UC1, cv::Scalar(255));
	cv::Mat center_mask = hist_mask.clone();
	while((res.size() < max_count) && (sum <= max_sum))
	{
		center_mask *= 0;
		int max_pos[3] = {0, 0, 0};
		double max_val;
		cv::minMaxIdx(hist, NULL, &max_val, NULL, max_pos, hist_mask);
		cv::Point3i max_loc(max_pos[0], max_pos[1], max_pos[2]);
		// getting center and sum
		MarkNearColors(center_mask, max_loc, cube_size, 255); //mark near
		cv::Vec4f center_sum = GetCenter(hist, center_mask, hist_mask); // get center of near-maximum area
		sum += center_sum[3];
		res.push_back(center_sum);
		// exclude near-maximum cells
		center_mask = 255 - center_mask;
		bitwise_and(hist_mask, center_mask, hist_mask);	
	}
	return res;
}
void MarkNearColors(cv::Mat mask, cv::Point3i center, cv::Vec3f size, unsigned char value)
{
	std::vector<bool> cyclic_dims = {1, 0, 0}; // hue channel is cyclic
#ifdef RGB_HIST
	cyclic_dims[0] = false;
#endif
#ifdef CIE_DIST
	MarkNearColorsCIE(mask, center, size[0], value);
#else
	DrawCube(mask, center, size, value, cyclic_dims);
#endif
}
void DrawCube(cv::Mat img, cv::Point3i center, cv::Vec3i size, unsigned char value, std::vector<bool> cyclic)
{
	std::vector<cv::Vec3i> p = GetGabarits(center, size);
	return DrawCube(img, p[0], p[1], value, cyclic);
}
void DrawCube(cv::Mat img, cv::Vec3i p1, cv::Vec3i p2, unsigned char value, std::vector<bool> cyclic)
{
	for(unsigned i = 0; i < cyclic.size(); i++) // cut cubes if non cyclic
		if(!cyclic[i])
		{
			p1[i] = cv::max(0, p1[i]);
			p2[i] = cv::min(img.size[i]-1, p2[i]);
		}
	for(int i1 = p1[0]; i1 <= p2[0]; i1++)
	for(int i2 = p1[1]; i2 <= p2[1]; i2++)
	for(int i3 = p1[2]; i3 <= p2[2]; i3++)
	{
		cv::Vec3i p(i1, i2, i3);
		CyclePoint3d(p, img.size);
		img.at<unsigned char>(p) = value; // uchar because of mask
	}
}
cv::Vec4f GetCenter(cv::Mat img, cv::Mat w_mask, cv::Mat v_mask)
{
	cv::Vec4f res(0, 0, 0, 0);
	cv::Vec3f center(0, 0, 0);
	float sum = 0;
	for(int i1 = 0; i1 < img.size[0]; i1++)
	for(int i2 = 0; i2 < img.size[1]; i2++)
	for(int i3 = 0; i3 < img.size[2]; i3++)
	{
		cv::Vec3i p(i1, i2, i3);
		if(w_mask.at<unsigned char>(p) == 0)
			continue;
		float val = img.at<float>(p);
		if(!v_mask.empty() && v_mask.at<unsigned char>(p))
		{
			res[3] += val;
		}
		else
			val = 0;
		cv::Vec3f pf = p;
		center += pf * val;
		sum += val;
	}
	center /= sum;
	res[0] = center[0];
	res[1] = center[1];
	res[2] = center[2];
	return res;
}
double GetCIEDist(cv::Vec3i c1, cv::Vec3i c2)
{
	double res = 0;
	cv::Mat colors(1, 2, CV_8UC3);
	colors.at<cv::Vec3b>(0, 0) = c1;
	colors.at<cv::Vec3b>(0, 1) = c2;
#ifndef RGB_HIST
	cv::cvtColor(colors, colors, CV_HSV2BGR);
#endif
	cv::cvtColor(colors, colors, CV_BGR2Lab);
	c1 = colors.at<cv::Vec3b>(0, 0);
	c2 = colors.at<cv::Vec3b>(0, 1);
	res = cv::norm(c1 - c2);
	return res;
}
void MarkNearColorsCIE(cv::Mat mask, cv::Point3i color, double dist, unsigned char value)
{
	cv::Vec3i center_color;
#ifdef RGB_HIST
	center_color[0] = color.x * 256.0/HIST_SIZE_B;
	center_color[1] = color.y * 256.0/HIST_SIZE_G;
	center_color[2] = color.z * 256.0/HIST_SIZE_R;
#else
	center_color[0] = color.x * 180.0/HIST_SIZE_H;
	center_color[1] = color.y * 256.0/HIST_SIZE_S;
	center_color[2] = color.z * 256.0/HIST_SIZE_V;
#endif
	for(int i1 = 0; i1 < mask.size[0]; i1++)
	for(int i2 = 0; i2 < mask.size[1]; i2++)
	for(int i3 = 0; i3 < mask.size[2]; i3++)
	{
		cv::Vec3i cl(i1, i2, i3);
#ifdef RGB_HIST
		cl[0] *= 256.0/HIST_SIZE_B;
		cl[1] *= 256.0/HIST_SIZE_G;
		cl[2] *= 256.0/HIST_SIZE_R;
#else
		cl[0] *= 180.0/HIST_SIZE_H;
		cl[1] *= 256.0/HIST_SIZE_S;
		cl[2] *= 256.0/HIST_SIZE_V;
#endif
		cv::Vec3i p(i1, i2, i3);
		if(GetCIEDist(cl, center_color) < dist)
		{
			mask.at<unsigned char>(p) = value;
		}
	}
}

std::vector<cv::Vec3i> GetGabarits(cv::Point3i center, cv::Vec3i size)
{
	std::vector<cv::Vec3i> res = {center, center};
	res[0][0] -= size[0];
	res[0][1] -= size[1];
	res[0][2] -= size[2];
	res[1][0] += size[0];
	res[1][1] += size[1];
	res[1][2] += size[2];
	return res;
}

template<class val_type> val_type CycleRange(val_type val, val_type val1, val_type val2)
{
	val_type ranged_val = (val-val1) % (val2 - val1);
	if(ranged_val < 0)
		ranged_val += (val2 - val1);
	return ranged_val + val1;
}
void CyclePoint3d(cv::Vec3i& p, cv::MatSize size)
{
	for(unsigned i = 0; i < 3; i++)
		p[i] = CycleRange(p[i], 0, size[i]);
}

cv::Mat ShowColors(cv::Mat img)
{
	cv::Mat img_colors;
#ifdef RGB_HIST
	std::vector<int> channels = {0, 1, 2};
	std::vector<int> hist_size = {HIST_SIZE_B, HIST_SIZE_G, HIST_SIZE_R};
	std::vector<float> ranges = {0, 256, 0, 256, 0, 256};
	std::vector<cv::Mat> bgr_channels;
	cv::split(img, bgr_channels);
	cv::Mat color_hist;
	cv::calcHist(bgr_channels, channels, cv::Mat(), color_hist, hist_size, ranges); // 3D histogram
	cv::copyMakeBorder(img, img_colors, COLOR_HEIGHT, 0, 0, 0,
							cv::BORDER_CONSTANT, cv::Scalar::all(0));
#else
	cv::Mat img_hsv;
	cvtColor(img, img_hsv, CV_BGR2HSV);
	std::vector<int> channels = {0, 1, 2};
	std::vector<int> hist_size = {HIST_SIZE_H, HIST_SIZE_S, HIST_SIZE_V};
	std::vector<float> ranges = {0, 180, 0, 256, 0, 256};
	std::vector<cv::Mat> hsv_channels;
	cv::split(img_hsv, hsv_channels);
	cv::Mat color_hist;
	cv::calcHist(hsv_channels, channels, cv::Mat(), color_hist, hist_size, ranges); // 3D histogram
	cv::copyMakeBorder(img_hsv, img_colors, COLOR_HEIGHT, 0, 0, 0,
							cv::BORDER_CONSTANT, cv::Scalar::all(0));
#endif
	cv::Scalar full_w = cv::sum(color_hist);
	color_hist /= full_w[0]; 
	std::vector<cv::Scalar> colors = GetDomColors(color_hist, DOM_COLORS_PART/100.0, DOM_COLORS_COUNT);
	cv::Rect color_rect(cv::Point(0, 0), cv::Size(img_colors.cols / colors.size(), COLOR_HEIGHT));
	for(unsigned i = 0; i < colors.size(); i++)
	{
#ifdef RGB_HIST
		colors[i][0] = colors[i][0] * (ranges[1]/HIST_SIZE_B) + (ranges[1]/HIST_SIZE_B);
		colors[i][1] = colors[i][1] * (ranges[3]/HIST_SIZE_G) + (ranges[3]/HIST_SIZE_G);
		colors[i][2] = colors[i][2] * (ranges[5]/HIST_SIZE_R) + (ranges[5]/HIST_SIZE_R);
#else
		colors[i][0] = colors[i][0] * (ranges[1]/HIST_SIZE_H) + (ranges[1]/HIST_SIZE_H);
		colors[i][1] = colors[i][1] * (ranges[3]/HIST_SIZE_S) + (ranges[3]/HIST_SIZE_S);
		colors[i][2] = colors[i][2] * (ranges[5]/HIST_SIZE_V) + (ranges[5]/HIST_SIZE_V);
#endif
		color_rect.x = (color_rect.width + 1) * i;
		cv::rectangle(img_colors, color_rect, colors[i], CV_FILLED);
	}
#ifndef RGB_HIST
	cvtColor(img_colors, img_colors, CV_HSV2BGR);
#endif
	return img_colors;
}