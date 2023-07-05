#ifndef RECONSTRUCTION_H
#define RECONSTRUCTION_H

#include "struct.h" // Assuming that VerticeObject is defined in this file


//fns
void extract_cylindrical_pts(LazerSlice& slice, cv::Mat cameraMatrix, cv::Mat distCoeffs, cv::Mat newCameraMatrix);

#endif // RECONSTRUCTION_H
