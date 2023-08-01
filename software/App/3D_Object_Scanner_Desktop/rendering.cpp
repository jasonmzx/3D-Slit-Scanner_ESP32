#include <iostream>

//local stuff
#include "rendering.h"
#include "struct.h" 
#include "reconstruction.h"
#include "io_handle.h"
#include "KDTree.hpp"

//! Define the camera matrix

double fx = 550.0;
double fy = 550.0;
double cx = 440.0;
double cy = 320.0;
//cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);

//ESP found camera mat
cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << 330.37685371, 0, 168.74716003, 0, 337.66475247, 129.659852, 0, 0, 1);

//! Define the distortion coefficients

double k1 = 0.1;
double k2 = 0.01;
double p1 = 0.0;
double p2 = 0.0;
double k3 = 0.01;
//cv::Mat distCoeffs = (cv::Mat_<double>(5, 1) << k1, k2, p1, p2, k3);
cv::Mat distCoeffs = (cv::Mat_<double>(5, 1) << 0.10513071, -0.62082709,  0.00165353, -0.00295972,  1.04980143);

// Cube Size
const GLfloat cubeSize = 0.005;


VerticeObject generate_cube_point_cloud(VerticeObject& obj, std::vector<GLfloat>& xyz_slice, std::vector<LazerSlice>& preprocessed_dataset, ExtractFunc extraction_fn) {

    int n_cols = preprocessed_dataset.at(0).on_img.cols;
    int n_rows = preprocessed_dataset.at(0).on_img.rows;

    cv::Mat newCameraMatrix = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, cv::Size(n_cols, n_rows), 1);

    for (LazerSlice slice : preprocessed_dataset) {
        extraction_fn(slice, cameraMatrix, distCoeffs, newCameraMatrix, 0);
        std::cout << "Sl3D: " << slice.list_3d_points.size() << std::endl;

        float debug_n = slice.angle = slice.angle / 360.0;

        for (glm::vec3 point : slice.list_3d_points) {

            xyz_slice.reserve(6);

            // Inserting the vertices of the cube
            xyz_slice.insert(xyz_slice.end(), {
                point[0] - cubeSize, point[1] - cubeSize, point[2] - cubeSize, debug_n, 1.0f, debug_n, // Vertex 1 (bottom-left-back)
                point[0] + cubeSize, point[1] - cubeSize, point[2] - cubeSize, debug_n, 1.0f,  debug_n, // Vertex 2 (bottom-right-back)
                point[0] + cubeSize, point[1] + cubeSize, point[2] - cubeSize, debug_n, 1.0f,  debug_n, // Vertex 3 (top-right-back)
                point[0] - cubeSize, point[1] + cubeSize, point[2] - cubeSize, debug_n, 1.0f,  debug_n, // Vertex 4 (top-left-back)
                point[0] - cubeSize, point[1] - cubeSize, point[2] + cubeSize, debug_n, 1.0f,  debug_n, // Vertex 5 (bottom-left-front)
                point[0] + cubeSize, point[1] - cubeSize, point[2] + cubeSize, debug_n, 1.0f,  debug_n, // Vertex 6 (bottom-right-front)
                point[0] + cubeSize, point[1] + cubeSize, point[2] + cubeSize, debug_n, 1.0f,  debug_n, // Vertex 7 (top-right-front)
                point[0] - cubeSize, point[1] + cubeSize, point[2] + cubeSize, debug_n, 1.0f,  debug_n  // Vertex 8 (top-left-front)
                });

        }

    }

    GLfloat* xyz_slice_converted = new GLfloat[xyz_slice.size()]; // allocate memory for the array

    std::copy(xyz_slice.begin(), xyz_slice.end(), xyz_slice_converted); // copy the vector elements to the array

    // Populate object before return: (Set Vertices GLfloat* arr, and it's size (n. elements) )

    obj.vertices = xyz_slice_converted;
    obj.vertices_length = xyz_slice.size() * sizeof(GLfloat);

    std::cout << "Vertex Len: " << obj.vertices_length << std::endl;

    //! Indice Population

    std::vector<GLuint> indices = {
         0, 1, 2, // First triangle (bottom face)
         0, 2, 3, // Second triangle (bottom face)
         4, 5, 6, // Third triangle (top face)
         4, 6, 7, // Fourth triangle (top face)
         0, 4, 7, // Fifth triangle (left face)
         0, 7, 3, // Sixth triangle (left face)
         1, 5, 6, // Seventh triangle (right face)
         1, 6, 2, // Eighth triangle (right face)
         0, 1, 5, // Ninth triangle (back face)
         0, 5, 4, // Tenth triangle (back face)
         3, 2, 6, // Eleventh triangle (front face)
         3, 6, 7  // Twelfth triangle (front face)
    };

    int numCubes = (obj.vertices_length / sizeof(GLfloat)) / 8;

    for (int i = 0; i < numCubes; i++) {
        GLuint offset = i * 8;

        indices.insert(indices.end(), {
            0 + offset, 1 + offset, 2 + offset, // First triangle (bottom face)
            0 + offset, 2 + offset, 3 + offset, // Second triangle (bottom face)
            4 + offset, 5 + offset, 6 + offset, // Third triangle (top face)
            4 + offset, 6 + offset, 7 + offset, // Fourth triangle (top face)
            0 + offset, 4 + offset, 7 + offset, // Fifth triangle (left face)
            0 + offset, 7 + offset, 3 + offset, // Sixth triangle (left face)
            1 + offset, 5 + offset, 6 + offset, // Seventh triangle (right face)
            1 + offset, 6 + offset, 2 + offset, // Eighth triangle (right face)
            0 + offset, 1 + offset, 5 + offset, // Ninth triangle (back face)
            0 + offset, 5 + offset, 4 + offset, // Tenth triangle (back face)
            3 + offset, 2 + offset, 6 + offset, // Eleventh triangle (front face)
            3 + offset, 6 + offset, 7 + offset  // Twelfth triangle (front face)
            });
    }

    obj.indices = new GLuint[indices.size()];
    std::copy(indices.begin(), indices.end(), obj.indices);

    obj.indices_length = indices.size() * sizeof(GLuint);

    return obj;
}


VerticeObject generate_xyz(VerticeObject& obj, std::vector<GLfloat>& xyz_slice, 
    std::vector<LazerSlice>& preprocessed_dataset, int IMAGE_MIDPOINT, ExtractFunc extraction_fn) {

    std::vector<GLfloat> xyz_slice_file = {};

    int n_cols = preprocessed_dataset.at(0).on_img.cols;
    int n_rows = preprocessed_dataset.at(0).on_img.rows;

    cv::Mat newCameraMatrix = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, cv::Size(n_cols, n_rows), 1);

    for (LazerSlice slice : preprocessed_dataset) {
        extraction_fn(slice, cameraMatrix, distCoeffs, newCameraMatrix, IMAGE_MIDPOINT);
        std::cout << "Sl3D: " << slice.list_3d_points.size() << std::endl;

        float debug_n = slice.angle = slice.angle / 360.0;

        for (glm::vec3 point : slice.list_3d_points) {

            xyz_slice.reserve(6);

            // Inserting the vertices of the cube
            xyz_slice.insert(xyz_slice.end(), {
                point[0] - cubeSize, point[1] - cubeSize, point[2] - cubeSize, debug_n, 1.0f, debug_n, // Vertex 1 (bottom-left-back)
                point[0] + cubeSize, point[1] - cubeSize, point[2] - cubeSize, debug_n, 1.0f,  debug_n, // Vertex 2 (bottom-right-back)
                point[0] + cubeSize, point[1] + cubeSize, point[2] - cubeSize, debug_n, 1.0f,  debug_n, // Vertex 3 (top-right-back)
                point[0] - cubeSize, point[1] + cubeSize, point[2] - cubeSize, debug_n, 1.0f,  debug_n, // Vertex 4 (top-left-back)
                point[0] - cubeSize, point[1] - cubeSize, point[2] + cubeSize, debug_n, 1.0f,  debug_n, // Vertex 5 (bottom-left-front)
                point[0] + cubeSize, point[1] - cubeSize, point[2] + cubeSize, debug_n, 1.0f,  debug_n, // Vertex 6 (bottom-right-front)
                point[0] + cubeSize, point[1] + cubeSize, point[2] + cubeSize, debug_n, 1.0f,  debug_n, // Vertex 7 (top-right-front)
                point[0] - cubeSize, point[1] + cubeSize, point[2] + cubeSize, debug_n, 1.0f,  debug_n  // Vertex 8 (top-left-front)
                });
        }
        for (glm::vec3 point : slice.list_3d_points) {

            xyz_slice_file.reserve(3);

            // Inserting the vertices of the cube
            xyz_slice_file.insert(xyz_slice_file.end(), {
                point[0], point[1], point[2] // Vertex 1 (bottom-left-back)
                });
        }

    }

    write_to_xyz_file(xyz_slice_file, "output.xyz");

    GLfloat* xyz_slice_converted = new GLfloat[xyz_slice.size()]; // allocate memory for the array

    std::copy(xyz_slice.begin(), xyz_slice.end(), xyz_slice_converted); // copy the vector elements to the array

    // Populate object before return: (Set Vertices GLfloat* arr, and it's size (n. elements) )

    obj.vertices = xyz_slice_converted;
    obj.vertices_length = xyz_slice.size() * sizeof(GLfloat);

    std::cout << "Vertex Len: " << obj.vertices_length << std::endl;

    //! Indice Population

    std::vector<GLuint> indices = {
         0, 1, 2, // First triangle (bottom face)
         0, 2, 3, // Second triangle (bottom face)
         4, 5, 6, // Third triangle (top face)
         4, 6, 7, // Fourth triangle (top face)
         0, 4, 7, // Fifth triangle (left face)
         0, 7, 3, // Sixth triangle (left face)
         1, 5, 6, // Seventh triangle (right face)
         1, 6, 2, // Eighth triangle (right face)
         0, 1, 5, // Ninth triangle (back face)
         0, 5, 4, // Tenth triangle (back face)
         3, 2, 6, // Eleventh triangle (front face)
         3, 6, 7  // Twelfth triangle (front face)
    };

    int numCubes = (obj.vertices_length / sizeof(GLfloat)) / 8;

    for (int i = 0; i < numCubes; i++) {
        GLuint offset = i * 8;

        indices.insert(indices.end(), {
            0 + offset, 1 + offset, 2 + offset, // First triangle (bottom face)
            0 + offset, 2 + offset, 3 + offset, // Second triangle (bottom face)
            4 + offset, 5 + offset, 6 + offset, // Third triangle (top face)
            4 + offset, 6 + offset, 7 + offset, // Fourth triangle (top face)
            0 + offset, 4 + offset, 7 + offset, // Fifth triangle (left face)
            0 + offset, 7 + offset, 3 + offset, // Sixth triangle (left face)
            1 + offset, 5 + offset, 6 + offset, // Seventh triangle (right face)
            1 + offset, 6 + offset, 2 + offset, // Eighth triangle (right face)
            0 + offset, 1 + offset, 5 + offset, // Ninth triangle (back face)
            0 + offset, 5 + offset, 4 + offset, // Tenth triangle (back face)
            3 + offset, 2 + offset, 6 + offset, // Eleventh triangle (front face)
            3 + offset, 6 + offset, 7 + offset  // Twelfth triangle (front face)
            });
    }

    obj.indices = new GLuint[indices.size()];
    std::copy(indices.begin(), indices.end(), obj.indices);

    obj.indices_length = indices.size() * sizeof(GLuint);

    return obj;
}
