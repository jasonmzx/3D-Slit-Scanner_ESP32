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

    std::vector<std::array<GLfloat, 3>> list_3d_coords;

    float angle;
};


// Function declaration for image processing function
VerticeObject detect_lazer_projection(cv::Mat image);
cv::Mat img_process();
VerticeObject gen();
VerticeObject gen2();

std::vector<LazerSlice> load_image_dataset(std::string dataset_folder_path);
std::vector<LazerSlice> preproc_image_dataset(std::string dataset_folder_path);
#endif // IMAGE_PROCESSING_H