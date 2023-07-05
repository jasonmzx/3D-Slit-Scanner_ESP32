#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "struct.h"

// Function declaration for image processing functions
VerticeObject detect_lazer_projection(cv::Mat image);
cv::Mat img_process();
VerticeObject gen();
VerticeObject gen2();

std::vector<LazerSlice> load_image_dataset(std::string dataset_folder_path);
std::vector<LazerSlice> preproc_image_dataset();
std::vector<LazerSlice> extract_lazer_from_cv_mat(cv::Mat image, cv::Mat cameraMatrix, cv::Mat distCoeffs, cv::Mat newCameraMatrix);

#endif // IMAGE_PROCESSING_H
