
//#include <opencv2/opencv.hpp>
//#include <glm/glm.hpp>
//#include<GLFW/glfw3.h>
//#include<glm/glm.hpp>

#include "reconstruction.h"

const bool PREPROC_DEBUG = true;

//! ############ RECONSTRUCTION HELPERS: ############

int getMiddleElement(std::vector<int>& int_list) {
    if (int_list.empty()) {
        throw std::invalid_argument("Vector is empty.");
    }

    int middleIndex = int_list.size() / 2;
    return int_list[middleIndex];
}

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

//! ############ Main 2D -> 3D RECONSTRUCTION Functions ############

void extract_cylindrical_pts(LazerSlice& slice, cv::Mat cameraMatrix, cv::Mat distCoeffs, cv::Mat newCameraMatrix) {

    // Image Height & Width:
    int n_rows = slice.processed_matrix.rows;
    int n_cols = slice.processed_matrix.cols;

    //TODO: Remove the Hard Code on IMG_MIDPOINT
    const int IMAGE_MIDPOINT = 161;

    for (int row = 0; row < n_rows; row++) {
        std::vector<int> activated_cols; // Horizontal Slice (of columns) for each row

        for (int col = 0; col < n_cols; col++) { // You had 'row++' here
            if (row >= 0 && row < n_rows && col >= 0 && col < n_cols) {
                cv::Vec3b pixel = slice.processed_matrix.at<cv::Vec3b>(row, col);
                int r = pixel[2];
                int g = pixel[1];
                int b = pixel[0];

                float brightness = (1 * r + 0.5 * g + 0.7 * b);

                if (brightness > 35) {    //TODO: Remove the Hard Code?
                    activated_cols.push_back(col);
                }

            }
        }

        if (!activated_cols.empty()) {
            int middle = getMiddleElement(activated_cols);

            //? For Debug:
            cv::Point xyPoint(middle, row);

            std::vector<cv::Point2f> srcPoints;
            srcPoints.push_back(cv::Point2f(static_cast<float>(middle), static_cast<float>(row)));

            std::vector<cv::Point2f> dstPoints; //needs to be array for CV::OUTPUT_ARRAY

            // Apply the camera calibration and distortion correction
            cv::undistortPoints(srcPoints, dstPoints, cameraMatrix, distCoeffs, cv::noArray(), newCameraMatrix);


            //
            //int Z = dstPoints[0].y; //Z is up in Cylindrical
            int Y = row;
            float R = IMAGE_MIDPOINT - middle;

            float rawAngle = (slice.angle + 45) * pi / 180; // Convert angle to radians

            GLfloat X = (R - 5) * cos(rawAngle);
            GLfloat Z = (R + 5) * sin(rawAngle);

            GLfloat normalX = normalizeCoordinate(static_cast<float>(X), n_rows);
            GLfloat normalY = normalizeCoordinate(static_cast<float>(Y), n_cols);

            //double result = dstPoints[0].x / tan(45 * pi / 180); // Divide value by tangent of 45 degrees

            GLfloat normalZ = normalizeCoordinate(static_cast<float>(Z), n_cols);

            //normalX = normalX * cos(theta); // Apply rotation matrix
            //normalZ = normalZ * sin(theta); // Apply rotation matrix

            slice.list_3d_points.push_back(glm::vec3(normalX, normalY, normalZ)); // GLM::VEC3 works well with OpenGL
        }
    }

}


void extract_lazer_from_cv_mat(LazerSlice& slice, cv::Mat cameraMatrix, cv::Mat distCoeffs, cv::Mat newCameraMatrix) {

    std::vector<cv::Point> points_of_interest;
    // Image Height & Width:
    int n_rows = slice.processed_matrix.rows;
    int n_cols = slice.processed_matrix.cols;

    for (int row = 0; row < n_rows; row++) {
        std::vector<int> activated_cols; // Horizontal Slice (of columns) for each row

        for (int col = 0; col < n_cols; col++) { // You had 'row++' here
            if (row >= 0 && row < n_rows && col >= 0 && col < n_cols) {
                cv::Vec3b pixel = slice.processed_matrix.at<cv::Vec3b>(row, col);
                int r = pixel[2];
                int g = pixel[1];
                int b = pixel[0];

                float brightness = (1 * r + 0.7 * g + 0.7 * b);

                // TODO: Generate 3D points for point cloud
                if (brightness > 40) { // Brightness threshold of 150, adjust as needed
                    activated_cols.push_back(col);
                }

            }
        }

        if (!activated_cols.empty()) {
            int middle = getMiddleElement(activated_cols);

            //? For Debug:
            cv::Point xyPoint(middle, row);
            points_of_interest.push_back(xyPoint);

            std::vector<cv::Point2f> srcPoints;
            srcPoints.push_back(cv::Point2f(static_cast<float>(middle), static_cast<float>(row)));

            std::vector<cv::Point2f> dstPoints;

            // Apply the camera calibration and distortion correction
            cv::undistortPoints(srcPoints, dstPoints, cameraMatrix, distCoeffs, cv::noArray(), newCameraMatrix);

            GLfloat offset = 0.05;

            GLfloat normalX = normalizeCoordinate(static_cast<float>(dstPoints[0].x), n_rows);
            GLfloat normalY = normalizeCoordinate(static_cast<float>(dstPoints[0].y), n_cols) + offset;

            double result = dstPoints[0].x / tan(45 * pi / 180); // Divide value by tangent of 45 degrees

            GLfloat normalZ = normalizeCoordinate(static_cast<float>(result), n_cols);


            GLfloat theta = (slice.angle) * pi / 180; // Convert angle to radians

            //normalX = normalX * cos(theta); // Apply rotation matrix
            //normalZ = normalZ * sin(theta); // Apply rotation matrix


            cv::Matx33f rotationMatrix(
                cos(theta), 0, sin(theta),
                0, 1, 0,
                -sin(theta), 0, cos(theta)
            );

            cv::Vec3f point(normalX, normalY, normalZ);
            point = rotationMatrix * point;

            normalX = point[0];
            normalY = point[1];
            normalZ = point[2];


            slice.list_3d_points.push_back(glm::vec3(normalX, normalY, normalZ)); // GLM::VEC3 works well with OpenGL
        }
    }

    //? DEBUG: 
    if (PREPROC_DEBUG) {
        for (cv::Point point : points_of_interest) {
            cv::circle(slice.processed_matrix, point, 1, cv::Scalar(0, 255, 0), -1);
        }
    }
}