#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <cmath>

#include "image_processing.h" 

const double pi = 3.14159265358979323846;

void detectBrightPixels(cv::Mat image) {

    std::vector<cv::Point> brightPixels; // Store bright pixels as points
    std::vector<cv::Point> zPixels; //

    // Loop through each pixel row by row
    for (int row = 0; row < image.rows; row++) {
        for (int col = 0; col < image.cols; col++) {
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
                int finalResult = static_cast<int>(result); // Convert result to integer

                // - X Y points

                cv::Point point(col, row); // Create a point for the bright pixel
                brightPixels.push_back(point); // Add the point to the list of bright pixels

                // Z axis points
                cv::Point zPoint(finalResult, row);
                zPixels.push_back(zPoint);
            }
        }
    }

    // Display the vertices on a cv::imshow window
    cv::Mat result = image.clone();
    for (cv::Point point : brightPixels) {
        cv::circle(result, point, 2, cv::Scalar(0, 255, 0), -1); // Draw a Green circle at the point
    }

    for (cv::Point point : zPixels) {
        cv::circle(result, point, 2, cv::Scalar(255, 0, 0), -1); // Draw a Green circle at the point
    }

    cv::imshow("Bright Pixels", result);
    cv::waitKey(0);
}


void process_img1() {

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
    cv::imshow("Show matrix", proc_diff);
    detectBrightPixels(proc_diff);
    cv::waitKey(0);
}
