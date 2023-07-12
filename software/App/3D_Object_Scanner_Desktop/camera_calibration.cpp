#include <iostream>

//Local Includes:
#include "camera_calibration.h"
#include "io_handle.h"

using namespace cv;
using namespace std;

std::string loghead1 = "[ CAMERA_CALIBRATION.CPP ] >> ";

void calibrate_camera_from_directory(std::string& directory, int grid_width, int grid_height, bool DEBUG) {
    // Load images
    std::vector<cv::Mat> mat_list = load_mat_vector(directory);

    // Prepare object points
    std::vector<std::vector<cv::Point3f>> object_points;
    std::vector<cv::Point3f> obj;
    for (int i = 0; i < grid_height; i++)
        for (int j = 0; j < grid_width; j++)
            obj.push_back(cv::Point3f((float)j, (float)i, 0));

    // Prepare image points
    std::vector<std::vector<cv::Point2f>> image_points;
    std::vector<cv::Point2f> corners;

    for (auto& img : mat_list) {
        bool found = cv::findChessboardCorners(img, cv::Size(grid_width, grid_height), corners);

        if (found) {
            object_points.push_back(obj);
            image_points.push_back(corners);

            if (DEBUG) {
                // Draw and display the corners
                cv::drawChessboardCorners(img, cv::Size(grid_width, grid_height), corners, found);
                cv::imshow("Image with Chessboard Corners", img);
                cv::waitKey(0);  // Wait for a key press before continuing
            }
        }
    }

    // Perform calibration
    cv::Mat camera_matrix, dist_coeffs;
    std::vector<cv::Mat> rvecs, tvecs;

    //Assert for if some Chessboard Corners where found:

    if (image_points.size() > 0) {
        cv::calibrateCamera(object_points, image_points, mat_list[0].size(), camera_matrix, dist_coeffs, rvecs, tvecs);
    }
    else {
        std::cout << loghead1 << " The images in " << directory << "\n COULD NOT be calibrated..." << std::endl;
        return;
    }

    // Print camera matrix and distortion coefficients
    std::cout << "Camera matrix: " << camera_matrix << std::endl;
    std::cout << "Distortion coefficients: " << dist_coeffs << std::endl;
    return;
}

void calibrateCameraWithImages(const std::vector<std::string>& imagePaths) {
    vector<vector<Point3f>> objectPoints;
    vector<vector<Point2f>> imagePoints;
    Size imageSize;

    // 3D points in real world space
    vector<Point3f> obj;
    for (int i = 0; i < CHECKERBOARD_SIZE.height; i++) {
        for (int j = 0; j < CHECKERBOARD_SIZE.width; j++) {
            obj.push_back(Point3f(j, i, 0));
        }
    }

    // Detect corners in each image and save corresponding image points and object points
    for (const string& imagePath : imagePaths) {
        Mat image = imread(imagePath);
        if (image.empty()) {
            cout << "Could not open or find the image: " << imagePath << endl;
            continue;
        }

        imageSize = image.size();
        vector<Point2f> corners;
        bool patternFound = findChessboardCorners(image, CHECKERBOARD_SIZE, corners);

        imshow("img", image);
        if (patternFound) {
            imagePoints.push_back(corners);
            objectPoints.push_back(obj);
        }
    }

    // Camera calibration
    Mat cameraMatrix, distCoeffs;
    vector<Mat> rvecs, tvecs;

    if (imagePoints.size() > 1)
    {
        calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);
    }
    else {
        cout << "ERROR : No Checkerboard Patterns found..." << endl;
    }
    // Now cameraMatrix and distCoeffs are the camera calibration parameters.
    // You can use them to undistort images captured by this specific camera.

    // Output parameters for testing
    cout << "Camera matrix: " << cameraMatrix << endl;
    cout << "Distortion coefficients: " << distCoeffs << endl;
}
