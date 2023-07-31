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

//! DATA STRUCTURES For Dataset Representations:

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

//! FILE Structure of Dataset Configurations

struct DatasetConfig {

    bool is_found;

    //! Dataset Configuration | REQUIRED PARAMETERS:
    std::string directory;
    std::string config_title;
    float step_angle_interval;
    float adjustment_per_angle;



    //! CYLINDRICAL Method | EXCLUSIVE PARAMETERS (optional)
    float lazer_angle_relative_2_cam;
    int   pixel_midpoint_x; //Midpoint for the Extraction Process, which is a pixel column along the X-Axis
     
    //! PLANAR EQUATION Method | EXCLUSIVE PARAMETERS (optional)
        // Given x & y in 2D, If and only if the Point lies on the Lazer Plane;  z = A*x + B*y + C 
    double pe_A;
    double pe_B;
    double pe_C;
    cv::Vec3f translation_vector; //Also known as `tvec`
    
    //! Extra Stuff (optional)

    int top_cutoff; //From 0 to top_cutoff
    int bottom_cutoff; //From bottom_cutoff to Mat.n_rows
};


#endif 