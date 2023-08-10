#include "io_handle.h"

//Windows Import
#include <windows.h> //For WIN32_FIND_DATAA, HANDLE, and all the FileSystem I.O Wrappers for Windows OS
#include <fstream>

//Local Imports
#include "struct.h"
#include "console_printing.h"

std::string loghead = "[ IO_HANDLE.CPP ] >> ";

bool folder_exists(const std::string& folderPath)
{
    DWORD fileAttributes = GetFileAttributesA(folderPath.c_str()); //Get the folder
    return (fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)); //Ternary check if it exists, return BOOLEAN
}

void create_folder_if_dont_exist(const std::string& filename) {
    DWORD ftyp = GetFileAttributesA(filename.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES) {
        // If the directory does not exist, create it
        CreateDirectoryA(filename.c_str(), NULL);
    }
    //else if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY)) {}
}

//! Loads Image Datasets in Vector of LazerSlice's ( Essentially loads in Dataset with Metadata )

std::vector<LazerSlice> load_image_dataset(std::string dataset_folder_path, float step_angle_interval, float adjustment_per_angle) {

    std::vector<LazerSlice> lazerSlices = {};

    if (folder_exists(dataset_folder_path)) { //Using Windows.h binding for FS
        WIN32_FIND_DATAA findFileData;
        HANDLE hFind;

        std::string search_path = dataset_folder_path + "/*.*";
        hFind = FindFirstFileA(search_path.c_str(), &findFileData);

        if (hFind != INVALID_HANDLE_VALUE) {

            //Do while is iterating over all imgs of that Dataset
            do {
                //?SKIP: if the current file is either the current directory (".") or the parent directory ("..")
                if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0)
                    continue;

                std::cout << loghead << findFileData.cFileName << std::endl;
                
                //Parse the filename

                int lazerInt;
                int num1, num2;
                sscanf_s(findFileData.cFileName, "%d_%d-%d", &lazerInt, &num1, &num2);

                //** Outputs:
                bool lazerOn = lazerInt != 1;
                float angle = num1 + num2 / pow(10, std::to_string(num2).length());

                //get active on lazer

                if (lazerOn) {

                    LazerSlice slice;

                    // Construct full file path - ON
                    std::string full_path = dataset_folder_path + "/" + findFileData.cFileName;

                    //Get corresponding off img to do binary diff - OFF
                    std::string off_img_path = findFileData.cFileName;
                    off_img_path[0] = '1';
                    std::string off_full_path = dataset_folder_path + "/" + off_img_path;

                    cv::Mat ON = cv::imread(full_path);
                    cv::Mat OFF = cv::imread(off_full_path);

                    //Lazer Correction  


                    //Add cv mats to object

                    if (!ON.empty()) {
                        slice.on_img = ON;
                        slice.angle = angle * adjustment_per_angle; //since we load angle info from ON img
                    }
                    if (!OFF.empty()) { 
                        slice.off_img = OFF; 
                    }

                    lazerSlices.push_back(slice);
                }

                std::cout << "LazerOn " << lazerOn << " angle : " << angle << std::endl;

            } while (FindNextFileA(hFind, &findFileData) != 0); //Finds the next file, and breaks upon last file check being 0

            FindClose(hFind);
        }
    }

    std::cout << loghead << "VECTOR DS SIZE: " << lazerSlices.size() << std::endl;

    return lazerSlices;
}



std::vector<std::vector<LazerSlice>> load_set_of_image_datasets(std::string set_folder_path) {
    std::vector<std::vector<LazerSlice>> loaded_datasets;

    //! CHECKING FOR ALL file types within `set_folder_path`, but will sort for Directories
    
    std::string search_path = set_folder_path + "/*.*";
    WIN32_FIND_DATAA fd;
    HANDLE hFind = ::FindFirstFileA(search_path.c_str(), &fd); //Goto first file

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // skip current directory and directory above
            if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0)
            {
                // Check if the found entity is a directory
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    std::string subdir_path = set_folder_path + "/" + fd.cFileName;
                    if (folder_exists(subdir_path)) {
                        //TODO: Actually get good values for this
                        loaded_datasets.push_back(load_image_dataset(subdir_path,0,0));
                    }
                }
            }
        } while (::FindNextFileA(hFind, &fd));
        ::FindClose(hFind);
    }

    return loaded_datasets;
}



//std::string basePath = "C:/Users/jason/Documents/GitHub/3D-IoT-Object-Scanner/camera-calibration-data/example_cam";

//Loads in a Dataset into a Vector of CV::Matrix , useful for simple img viewing and camera calib methods

std::vector<cv::Mat> load_mat_vector(std::string dataset_base_path) {
    std::vector<cv::Mat> images;

    if (!folder_exists(dataset_base_path)) {
        std::cout << loghead << "Folder does not exist: " << dataset_base_path << std::endl;
        return images;  // Return empty vector
    }

    std::vector<std::string> filenames;
    cv::glob(dataset_base_path + "/*.*", filenames, false); 
    //Grabs all filenames and from dataset Base Path and add it to filenames vec of strings
    // ! Glob matches to that /*.png pattern

    for (size_t i = 0; i < filenames.size(); ++i) {
        cv::Mat img = cv::imread(filenames[i], cv::IMREAD_COLOR);

        if (!img.empty()) {
            images.push_back(img);
        }
    }
    std::cout << loghead << "Loaded in Vector of " << images.size() << " image matrices." << std::endl;
    return images;
}

//! ####################### SERIALIZE / DESERIALIZE DATASET CONFIGURATION FILES  ####################### 

// Serialize a DatasetConfig to a file
void WriteConfigToFile(DatasetConfig& command, std::string& filename, std::string config_directory) {

    create_folder_if_dont_exist(config_directory);

    std::string fullPath = config_directory + "\\" + filename;
    std::ofstream file(fullPath, std::ios::binary);

    if (!file) {
        std::string e = "There was an issue opening the file binary ... (io_handle.cpp)";
        print_error(e);
        return;
    }

    // Write each member variable
    size_t length = command.directory.size();
    file.write(reinterpret_cast<const char*>(&length), sizeof(length));
    file.write(command.directory.c_str(), length);

    length = command.config_title.size();
    file.write(reinterpret_cast<const char*>(&length), sizeof(length));
    file.write(command.config_title.c_str(), length);

    file.write(reinterpret_cast<const char*>(&command.step_angle_interval), sizeof(command.step_angle_interval));
    file.write(reinterpret_cast<const char*>(&command.adjustment_per_angle), sizeof(command.adjustment_per_angle));
    file.write(reinterpret_cast<const char*>(&command.y_stretch), sizeof(command.y_stretch));

    file.write(reinterpret_cast<const char*>(&command.lazer_angle_relative_2_cam), sizeof(command.lazer_angle_relative_2_cam));
    file.write(reinterpret_cast<const char*>(&command.pixel_midpoint_x), sizeof(command.pixel_midpoint_x));
    file.write(reinterpret_cast<const char*>(&command.pe_A), sizeof(command.pe_A));
    file.write(reinterpret_cast<const char*>(&command.pe_B), sizeof(command.pe_B));
    file.write(reinterpret_cast<const char*>(&command.pe_C), sizeof(command.pe_C));
    file.write(reinterpret_cast<const char*>(&command.translation_vector), sizeof(command.translation_vector));
    file.write(reinterpret_cast<const char*>(&command.top_cutoff), sizeof(command.top_cutoff));
    file.write(reinterpret_cast<const char*>(&command.bottom_cutoff), sizeof(command.bottom_cutoff));

    file.close();
}

// Deserialize a DatasetConfig from a file
DatasetConfig ReadConfigFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Cannot open file to read: " << filename << "\n";
        
        DatasetConfig empty;
        empty.is_found = false;
        return empty;  // return empty struct
    }

    DatasetConfig command;

    // Read each member variable
    size_t length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    command.directory.resize(length);
    file.read(&command.directory[0], length);

    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    command.config_title.resize(length);
    file.read(&command.config_title[0], length);

    file.read(reinterpret_cast<char*>(&command.step_angle_interval), sizeof(command.step_angle_interval));
    file.read(reinterpret_cast<char*>(&command.adjustment_per_angle), sizeof(command.adjustment_per_angle));
    file.read(reinterpret_cast<char*>(&command.y_stretch), sizeof(command.y_stretch));
    file.read(reinterpret_cast<char*>(&command.lazer_angle_relative_2_cam), sizeof(command.lazer_angle_relative_2_cam));
    file.read(reinterpret_cast<char*>(&command.pixel_midpoint_x), sizeof(command.pixel_midpoint_x));
    file.read(reinterpret_cast<char*>(&command.pe_A), sizeof(command.pe_A));
    file.read(reinterpret_cast<char*>(&command.pe_B), sizeof(command.pe_B));
    file.read(reinterpret_cast<char*>(&command.pe_C), sizeof(command.pe_C));
    file.read(reinterpret_cast<char*>(&command.translation_vector), sizeof(command.translation_vector));
    file.read(reinterpret_cast<char*>(&command.top_cutoff), sizeof(command.top_cutoff));
    file.read(reinterpret_cast<char*>(&command.bottom_cutoff), sizeof(command.bottom_cutoff));

    command.is_found = true;

    file.close();
    return command;
}

//! ####################### POINT CLOUD & OTHER 3D | FILE EXPORTS  ####################### 

//Export to .xyz Point Cloud Format (Importable in Meshlab)
void write_to_xyz_file(const std::vector<GLfloat>& xyz_slice, const std::string& filename) {
    // Convert Filename to wide string
    std::wstring wide_filename(filename.begin(), filename.end());

    // Create file
    HANDLE hFile = CreateFile(wide_filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Unable to open file " << filename << " for writing." << std::endl;
        return;
    }

    // Write X, Y, Z coords from vector of Floats into file
    for (size_t i = 0; i < xyz_slice.size(); i += 3) {
        std::string line = std::to_string(xyz_slice[i]) + " " + std::to_string(xyz_slice[i + 1]) + " " + std::to_string(xyz_slice[i + 2]) + "\n";
        DWORD bytesWritten;
        WriteFile(hFile, line.c_str(), line.size(), &bytesWritten, NULL); //Append line by line
    }

    // Close file
    CloseHandle(hFile);
}

