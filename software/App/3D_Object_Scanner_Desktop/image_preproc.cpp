#include "image_preproc.h"

bool PREPROC_DEBUG = false;

void drawPoints(cv::Mat& image, const std::vector<cv::Point2f>& points, cv::Scalar color, int radius) {
    for (const cv::Point2f& pt : points) {
        cv::circle(image, pt, radius, color, -1); // -1 thickness means the circle is filled
    }
}

std::vector<LazerSlice> preproc_image_dataset_1(std::vector<LazerSlice>& dataset) {

    //Gaussian Blur Constants :
    int size = 3; double sigX = 3; double sigY = 3;

    for (std::size_t i = 0; i < dataset.size(); ++i) {

        cv::Mat proc_diff;
        LazerSlice& slice = dataset[i];

        //! Step 1: Gaussian Blur (Eliminates some noise)

        cv::Mat processed_off, processed_on;


        cv::GaussianBlur(slice.off_img, processed_off, cv::Size(size, size), sigX, sigY); //no_cast
        cv::GaussianBlur(slice.on_img, processed_on, cv::Size(size, size), sigX, sigY); //casted

        //! Step 2. Binary or Absolute Difference between Blurred imgs >> Binary Difference

        cv::absdiff(processed_off, processed_on, proc_diff);

        //! Step 3. Grayscale IMG

        cv::Mat proc_diff_gray;
        cv::cvtColor(proc_diff, proc_diff_gray, cv::COLOR_BGR2GRAY);

        //? Step 4. Canny and Dilation:
        // Apply Canny Edge Detection

        cv::Mat edges;
        double threshold1 = 50;  // threshold for the hysteresis procedure
        double threshold2 = 150; // threshold for the hysteresis procedure
        cv::Canny(proc_diff_gray, edges, threshold1, threshold2);

        // Use dilation to connect the broken lines

        cv::Mat dilated_edges;
        int dilation_size = 1; // adjust this parameter to get the desired thickness
        cv::Mat element = getStructuringElement(cv::MORPH_RECT,
            cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
            cv::Point(dilation_size, dilation_size));
        cv::dilate(edges, dilated_edges, element);

        //TODO: More Preprocessing ...

        std::vector<cv::Point2f> perspective_crop;

        //? Currently Hardcoded... FIX IT (X,Y)
        perspective_crop.push_back(cv::Point2f(200, -20)); //Top Right
        perspective_crop.push_back(cv::Point2f(200, 210)); //Bottom Right

        perspective_crop.push_back(cv::Point2f(6, 263)); //Bottom Left
        perspective_crop.push_back(cv::Point2f(6, 17)); //Top Left

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

        //! Append final Processed Image matrix:
        slice.processed_matrix = rotated_image;

        if (PREPROC_DEBUG) {
            // Make a deep copy of proc_diff
            cv::Mat proc_diff_copy = rotated_image.clone();

            // Draw a vertical line on the image copy
            cv::Point pt1(100, 0);
            cv::Point pt2(100, proc_diff_copy.rows); // Assuming the line spans the entire height of the image
            cv::Scalar color(0, 255, 0); // RGB color for the line. This is green.
            int thickness = 2; // Thickness of the line
            cv::line(proc_diff_copy, pt1, pt2, color, thickness);

            drawPoints(proc_diff_copy, perspective_crop, cv::Scalar(255, 0, 0), 2);  // Drawing with blue color and a radius of 5

            // Display the modified image
            cv::namedWindow("imgProc", cv::WINDOW_NORMAL);
            cv::resizeWindow("imgProc", 1000, 1000);
            cv::imshow("imgProc", rotated_image);
            cv::waitKey(0);
        }
    }
    return dataset;
}

std::vector<LazerSlice> preproc_image_dataset_2(std::vector<LazerSlice>& dataset) {

    //Gaussian Blur Constants :
    int size = 3; double sigX = 3; double sigY = 3;

    for (std::size_t i = 0; i < dataset.size(); ++i) {

        cv::Mat proc_diff;
        LazerSlice& slice = dataset[i];

        //! Step 1: Gaussian Blur (Eliminates some noise)

        cv::Mat processed_off, processed_on;


        cv::GaussianBlur(slice.off_img, processed_off, cv::Size(size, size), sigX, sigY); //no_cast
        cv::GaussianBlur(slice.on_img, processed_on, cv::Size(size, size), sigX, sigY); //casted

        //! Step 2. Binary or Absolute Difference between Blurred imgs >> Binary Difference

        cv::absdiff(processed_off, processed_on, proc_diff);

        //! Step 3. Grayscale IMG

        cv::Mat proc_diff_gray;
        cv::cvtColor(proc_diff, proc_diff_gray, cv::COLOR_BGR2GRAY);

        //? Step 4. Canny and Dilation:
        // Apply Canny Edge Detection

        cv::Mat edges;
        double threshold1 = 50;  // threshold for the hysteresis procedure
        double threshold2 = 150; // threshold for the hysteresis procedure
        cv::Canny(proc_diff_gray, edges, threshold1, threshold2);

        // Use dilation to connect the broken lines

        cv::Mat dilated_edges;
        int dilation_size = 1; // adjust this parameter to get the desired thickness
        cv::Mat element = getStructuringElement(cv::MORPH_RECT,
            cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
            cv::Point(dilation_size, dilation_size));
        cv::dilate(edges, dilated_edges, element);

        //TODO: More Preprocessing ...

        std::vector<cv::Point2f> perspective_crop;

        //? Currently Hardcoded... FIX IT
        perspective_crop.push_back(cv::Point2f(290, -119)); //Top Right
        perspective_crop.push_back(cv::Point2f(290, 230)); //Bottom Right
        perspective_crop.push_back(cv::Point2f(8, 283)); //Bottom Left
        perspective_crop.push_back(cv::Point2f(8, 0)); //Top Left

        std::vector<cv::Point2f> target_shape; //! Regular Sized Matrix | N.Cols * N.Rows

        target_shape.push_back(cv::Point2f(proc_diff.cols, 0));           // Top Right
        target_shape.push_back(cv::Point2f(proc_diff.cols, proc_diff.rows));// Bottom Right (POINT 1)
        target_shape.push_back(cv::Point2f(0, proc_diff.rows));            // Bottom Left (POINT 2)
        target_shape.push_back(cv::Point2f(0, 0));

        // Get the perspective transformation matrix

        cv::Mat transformation = cv::getPerspectiveTransform(perspective_crop, target_shape);

        // Apply the perspective transformation
        cv::Mat rotated_image;
        cv::warpPerspective(proc_diff, rotated_image, transformation, proc_diff.size());

        //! Append final Processed Image matrix:

        slice.processed_matrix = rotated_image;

        if (PREPROC_DEBUG) {
            // Make a deep copy of proc_diff
            cv::Mat proc_diff_copy = rotated_image.clone();

            // Draw a vertical line on the image copy
            cv::Point pt1(280, 0);
            cv::Point pt2(280, proc_diff_copy.rows); // Assuming the line spans the entire height of the image
            cv::Scalar color(0, 255, 0); // RGB color for the line. This is green.


            cv::Scalar cr(255, 0, 0); // RGB color for the line. RED
            int thickness = 2; // Thickness of the line
            cv::line(proc_diff_copy, pt1, pt2, color, thickness);

            cv::Point p_c_1(290, -110);
            cv::Point p_c_2(290, 230);
            cv::Point p_c_3(8, 283);
            cv::Point p_c_4(8, 0);

            cv::line(proc_diff_copy, p_c_1, p_c_2, cr, 1);
            cv::line(proc_diff_copy, p_c_2, p_c_3, cr, 1);
            cv::line(proc_diff_copy, p_c_3, p_c_4, cr, 1);
            cv::line(proc_diff_copy, p_c_1, p_c_4, cr, 1);


            // Display the modified image
            cv::namedWindow("imgProc", cv::WINDOW_NORMAL);
            cv::resizeWindow("imgProc", 1000, 1000);
            cv::imshow("imgProc", proc_diff_copy);
            cv::waitKey(0);
        }

    }

    return dataset;
}