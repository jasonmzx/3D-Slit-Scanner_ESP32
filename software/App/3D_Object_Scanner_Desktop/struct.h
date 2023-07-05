#ifndef STRUCT_H
#define STRUCT_H
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>

#include <string>
#include <vector>

//! CONSTANTS: 

constexpr double pi = 3.14159265358979323846;

//! DATA STRUCTURES:

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

#endif 