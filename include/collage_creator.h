#ifndef COLLAGE_CREATOR_H
#define COLLAGE_CREATOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <filesystem>

using namespace cv;
namespace fs = std::filesystem;

cv::Mat resize_and_pan(const cv::Mat& img, int cell_side, const cv::Scalar& bgcolor = cv::Scalar(255, 255, 255));

void create_collage(const std::string& folder_path, int canvas_width, int canvas_height, int images_count);

#endif