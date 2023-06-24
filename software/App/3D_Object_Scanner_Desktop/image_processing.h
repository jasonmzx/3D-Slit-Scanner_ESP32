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

    float angle;
};


// Function declaration for image processing function
VerticeObject detect_lazer_projection(cv::Mat image);
cv::Mat img_process();
VerticeObject gen();
std::vector<LazerSlice> dataset_process(std::string dataset_folder_path);

#endif // IMAGE_PROCESSING_H