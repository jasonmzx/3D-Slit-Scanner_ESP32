#include "io_handle.h"

//Windows Import
#include <windows.h> //For WIN32_FIND_DATAA, HANDLE, and all the FileSystem I.O Wrappers for Windows OS


std::string loghead = "[ IO_HANDLE.CPP ] >> ";

bool folder_exists(const std::string& folderPath)
{
    DWORD fileAttributes = GetFileAttributesA(folderPath.c_str()); //Get the folder
    return (fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)); //Ternary check if it exists, return BOOLEAN
}

//! Loads Image Datasets in Vector of LazerSlice's ( Essentially loads in Dataset with Metadata )

std::vector<LazerSlice> load_image_dataset(std::string dataset_folder_path) {

    std::vector<LazerSlice> lazerSlices = {};

    if (folder_exists(dataset_folder_path)) { //Using Windows.h binding for FS
        WIN32_FIND_DATAA findFileData;
        HANDLE hFind;

        std::string search_path = dataset_folder_path + "/*.*";
        hFind = FindFirstFileA(search_path.c_str(), &findFileData);

        if (hFind != INVALID_HANDLE_VALUE) {

            //Do while is iterating over all imgs of that Dataset
            do {
                //? Skip "." and ".."
                if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0)
                    continue;


                std::cout << loghead << findFileData.cFileName << std::endl;
                
                // Parse the filename

                int lazerInt;
                int num1, num2;
                sscanf_s(findFileData.cFileName, "%d_%d-%d", &lazerInt, &num1, &num2);

                //** Outputs:
                bool lazerOn = lazerInt != 1;
                float angle = num1 + num2 / pow(10, std::to_string(num2).length());

                //get active on lzer

                if (lazerOn) {

                    LazerSlice s;

                    // Construct full file path - ON
                    std::string full_path = dataset_folder_path + "/" + findFileData.cFileName;

                    //Get corresponding off img to do binary diff - OFF
                    std::string off_img_path = findFileData.cFileName;
                    off_img_path[0] = '1';
                    std::string off_full_path = dataset_folder_path + "/" + off_img_path;

                    cv::Mat ON = cv::imread(full_path);
                    cv::Mat OFF = cv::imread(off_full_path);

                    //Add cv mats to object

                    if (!ON.empty()) {
                        s.on_img = ON;
                        s.angle = angle; //since we load angle info from ON img
                    }
                    if (!OFF.empty()) { 
                        s.off_img = OFF; 
                    }

                    lazerSlices.push_back(s);
                }

                std::cout << "LazerOn " << lazerOn << " angle : " << angle << std::endl;

            } while (FindNextFileA(hFind, &findFileData) != 0);

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
                        loaded_datasets.push_back(load_image_dataset(subdir_path));
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



