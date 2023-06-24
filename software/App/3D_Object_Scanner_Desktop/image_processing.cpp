#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include<glad/glad.h>

//Windows Import
#include <windows.h>

#include <iostream>
#include <cmath>

#include "image_processing.h" 

const double pi = 3.14159265358979323846;


GLfloat normalizeCoordinate(float value, float width) {
    GLfloat normalized;

    if (fabs(width) < 1e-7) {  // check if width is too close to zero
        std::cerr << "Error: Width cannot be too close to zero" << std::endl;
        normalized = 0;
    }
    else {
        normalized = (2.0f * value) / (width - 1) - 1.0f;
    }

    return normalized * -1;
}

VerticeObject detect_lazer_projection(cv::Mat image) {

    //VerticeObject creation 
    VerticeObject obj;

    //Image Height & Width:
    int n_rows = image.rows;
    int n_cols = image.cols;

    //Definitions
    std::vector<GLfloat> xyz_slice = {}; //Store 3D slices extrapolated from 2D CV processed img

    std::vector<cv::Point> xyPixels = {}; // Store bright pixels as points (X,Y)

// Define the camera matrix
    double fx = 550.0;
    double fy = 550.0;
    double cx = 440.0;
    double cy = 240.0;
    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);

    // Define the distortion coefficients
    double k1 = 0.1;
    double k2 = 0.01;
    double p1 = 0.0;
    double p2 = 0.0;
    double k3 = 0.01;
    cv::Mat distCoeffs = (cv::Mat_<double>(5, 1) << k1, k2, p1, p2, k3);


    cv::Mat newCameraMatrix = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, cv::Size(n_cols, n_rows), 1);
    // Loop through each pixel row by row
    for (int row = 0; row < n_rows; row++) {

        //For every row, we'll find 1 Ideal Column to represent that row. (In the middle of activated area) 

        std::vector<int> x_slice; // Horizontal Slice at each row

        //? TODO, change 1000 back to `n_cols`

        for (int k = 1; k < 360; k+= 2) {
            for (int col = 0; col < 1000; col++) {
                //for (int k = 1; k < 2; k += 2) {
                //    for (int col = 0; col < 2; col++) {


                // Get the RGB values of the current pixel
                cv::Vec3b pixel = image.at<cv::Vec3b>(row, col);
                int r = pixel[2];
                int g = pixel[1];
                int b = pixel[0];

                // Calculate the brightness of the pixel
                float brightness = (1 * r + 0.7 * g + 0.7 * b);

                // If the pixel is bright, generate a vertex for it
                if (brightness > 200) { // Brightness threshold of 150, adjust as needed

                    //Debugging purposes:

                    x_slice.push_back(col);

                    double result = col / tan(45 * pi / 180); // Divide value by tangent of 45 degrees
                    int finalResult = static_cast<int>(result); // Convert result to integer                                          

                }
            }

            //Look thru X Slice and grab middle value

            float offsetZ = 0;

            if (!x_slice.empty()) {
                int middle = x_slice[x_slice.size() / 2];
                cv::Point xyPoint(middle, row);

                //Get X, Y in Vec of Point2f (1 entry)

                std::vector<cv::Point2f> srcPoints;
                srcPoints.push_back(cv::Point2f(static_cast<float>(middle), static_cast<float>(row)));

                std::vector<cv::Point2f> dstPoints;
                
                // Apply the camera calibration and distortion correction
                cv::undistortPoints(srcPoints, dstPoints, cameraMatrix, distCoeffs, cv::noArray(), newCameraMatrix);


                xyPixels.push_back(xyPoint); //Push to OpenCV Image processing render

                GLfloat offset = 0.5;

                GLfloat normalX = normalizeCoordinate( static_cast<float>(dstPoints[0].x) , n_rows);
                GLfloat normalY = normalizeCoordinate(static_cast<float>(dstPoints[0].y), n_cols) + offset;

                double result = dstPoints[0].x / tan(45 * pi / 180); // Divide value by tangent of 45 degrees

                GLfloat normalZ = normalizeCoordinate(static_cast<float>(result), n_cols);


                GLfloat theta = k * pi / 180; // Convert angle to radians

                normalX = normalX * cos(theta); // Apply rotation matrix
                normalZ = normalZ * sin(theta); // Apply rotation matrix


                //[ X Y Z  R G B , ... , ... ]
                xyz_slice.reserve(6);

        
            GLfloat cubeSize = 0.01; // Size of the cube


            //Vanilla Insertion (just a point):
              //  xyz_slice.insert(xyz_slice.end(), { normalX, normalY, normalZ, 0.0f, 1.0f, 0.0f });
            // Inserting the vertices of the cube
                xyz_slice.insert(xyz_slice.end(), {
                    normalX - cubeSize, normalY - cubeSize, normalZ - cubeSize, 0.0f, 1.0f, 0.0f, // Vertex 1 (bottom-left-back)
                    normalX + cubeSize, normalY - cubeSize, normalZ - cubeSize, 0.0f, 1.0f, 0.0f, // Vertex 2 (bottom-right-back)
                    normalX + cubeSize, normalY + cubeSize, normalZ - cubeSize, 0.0f, 1.0f, 0.0f, // Vertex 3 (top-right-back)
                    normalX - cubeSize, normalY + cubeSize, normalZ - cubeSize, 0.0f, 1.0f, 0.0f, // Vertex 4 (top-left-back)
                    normalX - cubeSize, normalY - cubeSize, normalZ + cubeSize, 0.0f, 1.0f, 1.0f, // Vertex 5 (bottom-left-front)
                    normalX + cubeSize, normalY - cubeSize, normalZ + cubeSize, 0.0f, 1.0f, 1.0f, // Vertex 6 (bottom-right-front)
                    normalX + cubeSize, normalY + cubeSize, normalZ + cubeSize, 0.0f, 1.0f, 0.0f, // Vertex 7 (top-right-front)
                    normalX - cubeSize, normalY + cubeSize, normalZ + cubeSize, 0.0f, 1.0f, 0.0f  // Vertex 8 (top-left-front)
                 });
            } //endof col for loop
        
        } //endof k loop

    } //endof row loop

    std::cout << "Size Of XYZ_SLICES : " << xyz_slice.size() << std::endl;

    cv::Mat CV_result = image.clone();

    for (cv::Point point : xyPixels) {
        cv::circle(CV_result, point, 1, cv::Scalar(0, 255, 0), -1);
    }


    cv::namedWindow("Test", cv::WINDOW_NORMAL);
    cv::resizeWindow("Test", 1000, 1000);
    cv::imshow("Test", CV_result);
    cv::waitKey(0);



    GLfloat* xyz_slice_converted = new GLfloat[xyz_slice.size()]; // allocate memory for the array

    std::copy(xyz_slice.begin(), xyz_slice.end(), xyz_slice_converted); // copy the vector elements to the array

    // Populate object before return: (Set Vertices GLfloat* arr, and it's size (n. elements) )

    obj.vertices = xyz_slice_converted;
    obj.vertices_length = xyz_slice.size() * sizeof(GLfloat); 

    return obj;
}

cv::Mat img_process() {

	std::string no_cast_img_path = "Resources/proto1_no_lazer.png";
	std::string casted_img_path = "Resources/proto1_lazer.png";

	//Defining image matrices

	cv::Mat raw_diff;
	cv::Mat no_cast = cv::imread(no_cast_img_path);
	cv::Mat casted = cv::imread(casted_img_path);

	//Some preprocessing:
	cv::Mat no_cast_pp, casted_pp;

	//Trying to blur the images first

	//Gaussian Blur parameters
	int size = 3;
	double sigX = 3; double sigY = 3;

	//Pre-Processing:

	cv::Mat proc_diff;

	cv::GaussianBlur(no_cast, no_cast_pp, cv::Size(size, size), sigX, sigY); //no_cast
	cv::GaussianBlur(casted, casted_pp, cv::Size(size, size), sigX, sigY); //casted

	//Abs without any preprocessing
	cv::absdiff(no_cast, casted, raw_diff);
	//Abs with preproc
	cv::absdiff(no_cast_pp, casted_pp, proc_diff);

    //Crop Perspective Technique?

    std::vector<cv::Point2f> perspective_crop;

    //? Currently Hardcoded... FIX IT
    perspective_crop.push_back(cv::Point2f(950, 0)  ); //Top Right
    perspective_crop.push_back(cv::Point2f(950, 650)   ); //Bottom Right
    perspective_crop.push_back(cv::Point2f(150, 940)); //Bottom Left
    perspective_crop.push_back(cv::Point2f(150, 338)  ); //Top Left

    // 500 x 500 square selection

    //perspective_crop.push_back(cv::Point2f(0, 0));         // Top Left
    //perspective_crop.push_back(cv::Point2f(500, 0));       // Top Right
    //perspective_crop.push_back(cv::Point2f(500, 500));     // Bottom Right
    //perspective_crop.push_back(cv::Point2f(0, 500));       // Bottom Left

// Define the target shape for the rotated image
    std::vector<cv::Point2f> target_shape;
    target_shape.push_back(cv::Point2f(proc_diff.cols, 0));           // Top Right
    target_shape.push_back(cv::Point2f(proc_diff.cols, proc_diff.rows));// Bottom Right (POINT 1)
    target_shape.push_back(cv::Point2f(0, proc_diff.rows));            // Bottom Left (POINT 2)
    target_shape.push_back(cv::Point2f(0, 0));

    // Get the perspective transformation matrix
    
     cv::Mat transformation = cv::getPerspectiveTransform(perspective_crop, target_shape);

    // Apply the perspective transformation
    cv::Mat rotated_image;
    cv::warpPerspective(proc_diff, rotated_image, transformation, proc_diff.size());

    cv::Scalar neonGreen(57, 255, 20); // BGR color values for neon green


    cv::namedWindow("imgProc", cv::WINDOW_NORMAL);
    cv::resizeWindow("imgProc", 1000, 1000);
    cv::imshow("imgProc", proc_diff);
    cv::waitKey(0);

    //cv::namedWindow("perspective", cv::WINDOW_NORMAL);
    //cv::imshow("perspective", rotated_image);
    //cv::waitKey(0);

    return proc_diff;
}

VerticeObject gen() {

    cv::Mat processed_matrix = img_process();

    VerticeObject obj = detect_lazer_projection(processed_matrix); // Generates Vertices only essentially

    //Add Indices:
    std::cout << "Vertex Len: " << obj.vertices_length << std::endl;

            std::vector<GLuint> indices = {
        0, 1, 2, // First triangle (bottom face)
        0, 2, 3, // Second triangle (bottom face)
        4, 5, 6, // Third triangle (top face)
        4, 6, 7, // Fourth triangle (top face)
        0, 4, 7, // Fifth triangle (left face)
        0, 7, 3, // Sixth triangle (left face)
        1, 5, 6, // Seventh triangle (right face)
        1, 6, 2, // Eighth triangle (right face)
        0, 1, 5, // Ninth triangle (back face)
        0, 5, 4, // Tenth triangle (back face)
        3, 2, 6, // Eleventh triangle (front face)
        3, 6, 7  // Twelfth triangle (front face)
            };
    //obj.indices = new GLuint[obj.vertices_length];
            int numCubes = (obj.vertices_length / sizeof(GLfloat)) / 8;

            for (int i = 0; i < numCubes; i++) {
                GLuint offset = i * 8;

                indices.insert(indices.end(), {
                    0 + offset, 1 + offset, 2 + offset, // First triangle (bottom face)
                    0 + offset, 2 + offset, 3 + offset, // Second triangle (bottom face)
                    4 + offset, 5 + offset, 6 + offset, // Third triangle (top face)
                    4 + offset, 6 + offset, 7 + offset, // Fourth triangle (top face)
                    0 + offset, 4 + offset, 7 + offset, // Fifth triangle (left face)
                    0 + offset, 7 + offset, 3 + offset, // Sixth triangle (left face)
                    1 + offset, 5 + offset, 6 + offset, // Seventh triangle (right face)
                    1 + offset, 6 + offset, 2 + offset, // Eighth triangle (right face)
                    0 + offset, 1 + offset, 5 + offset, // Ninth triangle (back face)
                    0 + offset, 5 + offset, 4 + offset, // Tenth triangle (back face)
                    3 + offset, 2 + offset, 6 + offset, // Eleventh triangle (front face)
                    3 + offset, 6 + offset, 7 + offset  // Twelfth triangle (front face)
                    });
            }

            obj.indices = new GLuint[indices.size()];
            std::copy(indices.begin(), indices.end(), obj.indices);

    //? Old Implementation with points being connected to a root point
    //int incr = 0;
    
    //for (int j = 0; j < (obj.vertices_length / 2) ; j++) { // 0 1 2, 0 , 2, 3 0, 3,4
    //    obj.indices[j] = 0;
    //    obj.indices[j + 1] = incr + 1;
    //    obj.indices[j + 2] = incr + 2;
    //    incr++;
    //    j = j + 2;
    //}
    //obj.indices_length = incr * 3 * sizeof(GLuint); // Object Indices Size is N. Increments * N. in vertices * size of GL int

    /*std::cout << "INCR N#: " << incr << std::endl;*/
    //? ENDOF: Old Implementation with points being connected to a root point

    obj.indices_length = indices.size() * sizeof(GLuint);
    return obj;

}

bool folderExists2(const std::string& folderPath)
{
    DWORD fileAttributes = GetFileAttributesA(folderPath.c_str()); //Get the folder
    return (fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)); //Ternary check if it exists, return BOOLEAN
}

std::vector<LazerSlice> dataset_process(std::string dataset_folder_path) {
    
    std::vector<LazerSlice> lazerSlices = {};

    if (folderExists2(dataset_folder_path)) { //Using Windows.h binding for Filesystem I.O
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
                int captureInt;
                int num1, num2;
                sscanf_s(findFileData.cFileName, "%d_%d-%d", &captureInt, &num1, &num2);
                
                bool lazerOn = captureInt != 1;
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