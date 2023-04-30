#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <cmath>

#include<glad/glad.h>

#include "image_processing.h" 

const double pi = 3.14159265358979323846;

GLfloat normalize(int input, int width) {
    GLfloat normalized = (input / (width / 2)) - 1.0;
    return normalized;
}



GLfloat* detect_lazer_projection(cv::Mat image) {

    //Image Height & Width:

    int n_rows = image.rows;
    int n_cols = image.cols;




    std::vector<GLfloat> xyz_slice;


    std::vector<cv::Point> xyPixels; // Store bright pixels as points (X,Y)
    std::vector<cv::Point> zPixels; // (Z, Y)

    // Loop through each pixel row by row
    for (int row = 0; row < n_rows; row++) {
        for (int col = 0; col < n_cols; col++) {
            // Get the RGB values of the current pixel
            cv::Vec3b pixel = image.at<cv::Vec3b>(row, col);
            int r = pixel[2];
            int g = pixel[1];
            int b = pixel[0];

            // Calculate the brightness of the pixel
            float brightness = (0.5126 * r + 0.2152 * g + 0.0722 * b);

            // If the pixel is bright, generate a vertex for it
            if (brightness > 50) { // Brightness threshold of 150, adjust as needed

                double result = col / tan(42 * pi / 180); // Divide value by tangent of 45 degrees
                //? int finalResult = static_cast<int>(result); // Convert result to integer 

                xyz_slice.push_back(normalize(col, n_cols)); //X
                xyz_slice.push_back(normalize(row, n_cols)); //Y
                xyz_slice.push_back(normalize(result, n_cols)); //Z --> X / tan(theta) 
                                                                   
                xyz_slice.reserve(3); // reserve space for 3 elements

                xyz_slice.insert(xyz_slice.end(), { 0.0f, 1.0f, 0.0f });

            }
        }
    }

    GLfloat* xyz_slice_converted = new GLfloat[xyz_slice.size()]; // allocate memory for the array

    std::copy(xyz_slice.begin(), xyz_slice.end(), xyz_slice_converted); // copy the vector elements to the array
    return xyz_slice_converted;
}


GLfloat* img_proc() {

	std::string no_cast_img_path = "Resources/proto_no_lazer.png";
	std::string casted_img_path = "Resources/proto_lazer.png";

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


    // Display output image
    cv::namedWindow("Show matrix", cv::WINDOW_NORMAL);
    cv::resizeWindow("Show matrix", 600, 600);
   // cv::imshow("Show matrix", proc_diff);
    return detect_lazer_projection(proc_diff);
}
