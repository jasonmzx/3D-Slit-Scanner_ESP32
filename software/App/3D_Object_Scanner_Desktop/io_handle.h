#ifndef IO_HANDLE_H
#define IO_HANDLE_H

#include "struct.h"

std::vector<LazerSlice> load_image_dataset(std::string dataset_folder_path, float step_angle_interval, float adjustment_per_angle);

std::vector<cv::Mat> load_mat_vector(std::string dataset_base_path);

std::vector<std::vector<LazerSlice>> load_set_of_image_datasets(std::string set_folder_path);

void write_to_xyz_file(const std::vector<GLfloat>& xyz_slice, const std::string& filename);

void WriteConfigToFile(DatasetConfig& command, std::string& filename, std::string config_directory);

DatasetConfig ReadConfigFromFile(const std::string& filename);

#endif