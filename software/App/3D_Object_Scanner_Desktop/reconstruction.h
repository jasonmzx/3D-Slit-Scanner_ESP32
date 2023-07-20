#ifndef RECONSTRUCTION_H
#define RECONSTRUCTION_H

#include "struct.h" // Assuming that VerticeObject is defined in this file


//fns
void extract_cylindrical_pts(LazerSlice& slice, cv::Mat cameraMatrix, cv::Mat distCoeffs, cv::Mat newCameraMatrix, float angleOffset);
void extract_cylindrical_pts__rot_mat(LazerSlice& slice, cv::Mat cameraMatrix, cv::Mat distCoeffs, cv::Mat newCameraMatrix, float angleOffset);
void extract_cylindrical_pts__rot_mat_45(LazerSlice& slice, cv::Mat cameraMatrix, cv::Mat distCoeffs, cv::Mat newCameraMatrix, float angleOffset);
std::vector<glm::vec3> test_reconstruction();

#endif // RECONSTRUCTION_H
