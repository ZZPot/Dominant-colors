#pragma once
#include <vector>
#include <opencv2/core.hpp>

#define HIST_SIZE_H	18
#define HIST_SIZE_S	8
#define HIST_SIZE_V	8

#define HIST_SIZE_R	16
#define HIST_SIZE_G	16
#define HIST_SIZE_B	16

#define DOM_COLORS_COUNT	25
#define DOM_COLORS_PART		95 // colors % of whole image
#define COLOR_HEIGHT		20

#define RGB_HIST

//#define CIE_DIST

std::vector<cv::Scalar> GetDomColors(cv::Mat hist, double max_sum, int max_count);

void MarkNearColors(cv::Mat mask, cv::Point3i center, cv::Vec3f size, unsigned char value);

void DrawCube(cv::Mat img, cv::Point3i center, cv::Vec3i size, unsigned char value = 0, std::vector<bool> cyclic = {0, 0, 0});
void DrawCube(cv::Mat img, cv::Vec3i p1, cv::Vec3i p2, unsigned char value = 0, std::vector<bool> cyclic = {0, 0, 0});

void MarkNearColorsCIE(cv::Mat mask, cv::Point3i color, double dist, unsigned char value);
cv::Vec4f GetCenter(cv::Mat img, cv::Mat w_mask, cv::Mat v_mask = cv::Mat());
std::vector<cv::Vec3i> GetGabarits(cv::Point3i center, cv::Vec3i size);
double GetCIEDist(cv::Vec3i c1, cv::Vec3i c2);

template<class val_type> val_type CycleRange(val_type val, val_type val1, val_type val2);
void CyclePoint3d(cv::Vec3i& p, cv::MatSize size);

cv::Mat ShowColors(cv::Mat img);