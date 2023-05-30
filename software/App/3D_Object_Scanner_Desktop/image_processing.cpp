#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <cmath>

#include<glad/glad.h>

#include "image_processing.h" 

const double pi = 3.14159265358979323846;


GLfloat normalizeCoordinate(int value, int width) {
    GLfloat normalized;

    if (width == 0) {
        // Handle division by zero
        std::cerr << "Error: Width cannot be zero" << std::endl;
        // You can also throw an exception or return a default value here
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

    //Origin Point:

    std::vector<GLfloat> xyz_slice = { 0.0 , 0.0, 0.0 , 1.0f, 0.0f, 0.0f };

    std::vector<cv::Point> xyPixels; // Store bright pixels as points (X,Y)

    // Loop through each pixel row by row
    for (int row = 0; row < n_rows; row++) {

        //For every row, we'll find 1 Ideal Column to represent that row. (In the middle of activated area) 

        std::vector<int> x_slice; // Horizontal Slice at each row

        //? TODO, change 500 back to `n_cols`

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

                xyPixels.push_back(xyPoint); //Push to OpenCV Image processing render

                GLfloat offset = 0.5;

                GLfloat normalX = normalizeCoordinate(middle, n_rows);

                normalX = normalX * cos(k * pi / 180);

                GLfloat normalY = normalizeCoordinate(row, n_cols) + offset;

                double result = middle / tan(45 * pi / 180); // Divide value by tangent of 45 degrees
                int finalResult = static_cast<int>(result); // Convert result to integer 

                GLfloat normalZ = normalizeCoordinate(finalResult, n_cols);

                normalZ = normalZ * sin(k * pi / 180);
                //Get multiple angles: 

                //for (int k = 0; k < 3; k++) {

                //    GLfloat angledNormalX = normalX;
                //    GLfloat angledNormalZ = normalZ;
                //    xyz_slice.reserve(6);
                //    xyz_slice.insert(xyz_slice.end(), { angledNormalX, normalY, angledNormalZ, 0.0f, 1.0f, 0.0f });

                //}

                //offsetZ += 0.01;

               // std::cout << "ROW: " << row << " K: " << k << "normalX :" << normalX << " normalY: " << normalY << " normalZ: " << normalZ << std::endl;

                //[ X Y Z  R G B , ... , ... ]
                xyz_slice.reserve(6);
                xyz_slice.insert(xyz_slice.end(), { normalX, normalY, normalZ, 0.0f, 1.0f, 0.0f });
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

    //for (int i = 0; i < perspective_crop.size(); i++)
    //{
    //    cv::Point p1 = perspective_crop[i];
    //    cv::Point p2 = perspective_crop[(i + 1) % perspective_crop.size()]; // Connect the last point with the first point

    //    cv::line(proc_diff, p1, p2, neonGreen, 2);
    //}

    cv::namedWindow("imgProc", cv::WINDOW_NORMAL);
    cv::resizeWindow("imgProc", 1000, 1000);
    cv::imshow("imgProc", proc_diff);
    cv::waitKey(0);

    //cv::namedWindow("perspective", cv::WINDOW_NORMAL);
    //cv::imshow("perspective", rotated_image);
    //cv::waitKey(0);

    return rotated_image;
}


VerticeObject gen() {

    cv::Mat processed_matrix = img_process();

    VerticeObject obj = detect_lazer_projection(processed_matrix); 

    //Add Indices:
    std::cout << "Vertex Len: " << obj.vertices_length << std::endl;

    obj.indices = new GLuint[obj.vertices_length];

    int incr = 0;

    for (int j = 0; j < (obj.vertices_length / 2) ; j++) { // 0 1 2, 0 , 2, 3 0, 3,4
        obj.indices[j] = 0;
        obj.indices[j + 1] = incr + 1;
        obj.indices[j + 2] = incr + 2;
        incr++;
        j = j + 2;
    }

    obj.indices_length = incr * 3 * sizeof(GLuint); // Object Indices Size is N. Increments * N. in vertices * size of GL int

    return obj;

}