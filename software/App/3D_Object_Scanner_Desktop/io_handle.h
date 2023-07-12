#ifndef IO_HANDLE_H
#define IO_HANDLE_H

#include "struct.h"

std::vector<LazerSlice> load_image_dataset(std::string dataset_folder_path);

std::vector<cv::Mat> load_mat_vector(std::string dataset_base_path);
 
#endif