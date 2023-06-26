#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

//Structs

struct VerticeObject {
    GLfloat* vertices;
    int vertices_length;

    GLuint* indices;
    int indices_length;
};

struct LazerSlice {
    cv::Mat off_img;
    cv::Mat on_img;
    cv::Mat processed_matrix;

    std::vector<glm::vec3> list_3d_points;


    float angle;
};


// Function declaration for image processing function
VerticeObject detect_lazer_projection(cv::Mat image);
cv::Mat img_process();
VerticeObject gen();
VerticeObject gen2();

std::vector<LazerSlice> load_image_dataset(std::string dataset_folder_path);
std::vector<LazerSlice> preproc_image_dataset();
std::vector<LazerSlice> extract_lazer_from_cv_mat(cv::Mat image, cv::Mat cameraMatrix, cv::Mat distCoeffs, cv::Mat newCameraMatrix);
#endif // IMAGE_PROCESSING_H