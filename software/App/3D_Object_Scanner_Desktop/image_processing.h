#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

//Structs

struct VerticeObject {
    GLfloat* vertices;
    int vertices_length;
};


// Function declaration for image processing function
VerticeObject detect_lazer_projection(cv::Mat image);
cv::Mat img_process();
VerticeObject gen();

#endif // IMAGE_PROCESSING_H