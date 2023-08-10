#ifndef RENDERING_H
#define RENDERING_H

#include "struct.h" 

// Using declaration for ExtractFunc
using ExtractFunc = void (*)(LazerSlice&, cv::Mat, cv::Mat, cv::Mat, int, float);

VerticeObject generate_cube_point_cloud(VerticeObject& obj, std::vector<GLfloat>& xyz_slice, std::vector<LazerSlice>& preprocessed_dataset, ExtractFunc extract_cylindrical_pts__rot_mat);
VerticeObject generate_xyz(VerticeObject& obj, std::vector<GLfloat>& xyz_slice, std::vector<LazerSlice>& preprocessed_dataset, int IMAGE_MIDPOINT, float y_stretch, ExtractFunc extraction_fn);
#endif