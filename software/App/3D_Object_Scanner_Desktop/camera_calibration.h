#ifndef CAMERA_CALIBRATION_H
#define CAMERA_CALIBRATION_H

#include <opencv2/opencv.hpp>
#include <vector>

// Define the checkerboard size (number of inner corners per chessboard row and column)
const cv::Size CHECKERBOARD_SIZE = cv::Size(5,7);

void calibrate_camera_from_directory(std::string& directory, int grid_width, int grid_height, bool DEBUG);

void calibrateCameraWithImages(const std::vector<std::string>& imagePaths);

#endif // CAMERA_CALIBRATION_H
