#include "io_handle.h"

//Windows Import
#include <windows.h> //For WIN32_FIND_DATAA, HANDLE, and all the FileSystem I.O Wrappers for Windows OS


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


                std::cout << findFileData.cFileName << std::endl;



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
                    if (!OFF.empty()) { s.off_img = OFF; }

                    lazerSlices.push_back(s);
                }

                std::cout << "LazerOn " << lazerOn << " angle : " << angle << std::endl;

            } while (FindNextFileA(hFind, &findFileData) != 0);

            FindClose(hFind);
        }
    }

    std::cout << "VECTOR DS SIZE: " << lazerSlices.size() << std::endl;

    return lazerSlices;
}