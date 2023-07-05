#include "pipeline.h"

#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

// Local Includes
#include "reconstruction.h"
#include "image_preproc.h"
#include "io_handle.h"


const GLfloat cubeSize = 0.005; // Size of the cube

// Define your pipeline functions.
VerticeObject pipeline1(std::string dataset) {
    std::cout << "\n Running pipeline 1...\n";

    VerticeObject obj;

    //load data
    //std::vector<LazerSlice> dataset = load_image_dataset("C:/Users/jason/Documents/GitHub/3D-IoT-Object-Scanner/proto2-dataset/p2_monk");
    std::vector<LazerSlice> loaded_dataset = load_image_dataset(dataset);

    //! PRE PROCESSING STEP :

    std::vector<LazerSlice> preprocessed_dataset = preproc_image_dataset_1(loaded_dataset);

    //! Define the camera matrix

    double fx = 550.0;
    double fy = 550.0;
    double cx = 440.0;
    double cy = 240.0;
    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);

    //! Define the distortion coefficients

    double k1 = 0.1;
    double k2 = 0.01;
    double p1 = 0.0;
    double p2 = 0.0;
    double k3 = 0.01;
    cv::Mat distCoeffs = (cv::Mat_<double>(5, 1) << k1, k2, p1, p2, k3);

    //Get Cols & Rows #N

    int n_cols = preprocessed_dataset.at(0).on_img.cols;
    int n_rows = preprocessed_dataset.at(0).on_img.rows;

    cv::Mat newCameraMatrix = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, cv::Size(n_cols, n_rows), 1);


    //! RECONSTRUCTION & BUILDING STEP:

    std::vector<GLfloat> xyz_slice = {}; //Store 3D slices extrapolated from 2D CV processed img

    for (LazerSlice slice : preprocessed_dataset) {
        extract_cylindrical_pts(slice, cameraMatrix, distCoeffs, newCameraMatrix);
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

VerticeObject pipeline2(std::string dataset) {
    std::cout << "\n Running pipeline 2...\n";
    VerticeObject obj;
    obj.indices_length = 1;
    // img preproc, algorithm A on pre-proc data, vertex gen

    return obj;  // assuming you've modified obj as necessary
}

// Add your pipeline functions to a map.
std::map<std::string, PipelineFunction> pipelineMap = {
    {"pipeline1", pipeline1},
    {"pipeline2", pipeline2},
    // Add more pipelines here.
};

// Function to execute a pipeline by name.
VerticeObject executePipeline(const std::string& pipelineName, const std::string& dataset) {
    // Find the pipeline function in the map.
    auto it = pipelineMap.find(pipelineName);
    if (it == pipelineMap.end()) {
        std::cerr << "Unknown pipeline: " << pipelineName << "\n";
        printUsage();
        return VerticeObject(); // return default VerticeObject if no matching pipeline found
    }

    // Run the pipeline function.
    return it->second(dataset);
}

// Function to print usage information.
void printUsage() {
    std::cout << "Usage: program_name pipeline_name parameter\n";
    std::cout << "Available pipelines:\n";
    for (const auto& pair : pipelineMap) {
        std::cout << "  " << pair.first << "\n";
    }
}
