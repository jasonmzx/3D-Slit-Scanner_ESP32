#include "pipeline.h"

#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

// Local Includes
#include "reconstruction.h"
#include "image_preproc.h"
#include "rendering.h"
#include "io_handle.h"

const GLfloat cubeSize = 0.005;


// Define your pipeline functions.
VerticeObject pipeline1(std::string dataset, int midpoint, int cutoff) {
    std::cout << "\n Running pipeline 1...\n";

    VerticeObject obj;

    // Load data from Dataset Location
    std::vector<LazerSlice> loaded_dataset = load_image_dataset(dataset);

    //! PRE PROCESSING STEP :

    std::vector<LazerSlice> preprocessed_dataset = preproc_image_dataset_1(loaded_dataset);

    //! RECONSTRUCTION & BUILDING STEP:

    std::vector<GLfloat> xyz_slice = {}; //Store 3D slices extrapolated from 2D CV processed img

    generate_xyz(obj, xyz_slice, preprocessed_dataset, extract_cylindrical_pts__rot_mat);
    return obj;
}

VerticeObject pipeline2(std::string dataset, int midpoint, int cutoff) {
    std::cout << "\n Running pipeline 2...\n";
    VerticeObject obj;
   float r = 0.0f; float g = 1.0f;  float b = 0.0f;
    std::vector<glm::vec3> glm_slices = test_reconstruction();

    
    std::vector<GLfloat> xyz_slice = {}; //Flattened XYZ Slice array
    

        for (glm::vec3 point : glm_slices) {

            xyz_slice.reserve(6);

            // Inserting the vertices of the cube
            xyz_slice.insert(xyz_slice.end(), {
                point[0] - cubeSize, point[1] - cubeSize, point[2] - cubeSize, r,g,b, // Vertex 1 (bottom-left-back)
                point[0] + cubeSize, point[1] - cubeSize, point[2] - cubeSize, r,g,b, // Vertex 2 (bottom-right-back)
                point[0] + cubeSize, point[1] + cubeSize, point[2] - cubeSize, r,g,b, // Vertex 3 (top-right-back)
                point[0] - cubeSize, point[1] + cubeSize, point[2] - cubeSize, r,g,b, // Vertex 4 (top-left-back)
                point[0] - cubeSize, point[1] - cubeSize, point[2] + cubeSize, r,g,b, // Vertex 5 (bottom-left-front)
                point[0] + cubeSize, point[1] - cubeSize, point[2] + cubeSize, r,g,b, // Vertex 6 (bottom-right-front)
                point[0] + cubeSize, point[1] + cubeSize, point[2] + cubeSize, r,g,b, // Vertex 7 (top-right-front)
                point[0] - cubeSize, point[1] + cubeSize, point[2] + cubeSize, r,g,b  // Vertex 8 (top-left-front)
                });

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

VerticeObject pipelineCombo(std::string set_of_datasets_path, int midpoint, int cutoff) {



    VerticeObject obj;

    //! Load in List of datasets :
    std::vector<std::vector<LazerSlice>> set_of_datasets = load_set_of_image_datasets(set_of_datasets_path);
    
    std::cout << "\n Amount of datasets Loaded IN: " << set_of_datasets.size() << std::endl;
    
    std::vector<std::vector<LazerSlice>> set_of_preprocessed_datasets = {};

    //! Preprocessing step on images: 
    for (size_t i = 0; i < set_of_datasets.size(); i++) {
        
        std::vector<LazerSlice> loaded_dataset = set_of_datasets[i];
        std::vector<LazerSlice> preprocessed_dataset = preproc_image_dataset_1(loaded_dataset);

        set_of_preprocessed_datasets.push_back(preprocessed_dataset);
    }

    //! Define the camera matrix

    double fx = 550.0;
    double fy = 550.0;
    double cx = 440.0;
    double cy = 320.0;
    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);

    //! Define the distortion coefficients

    double k1 = 0.1;
    double k2 = 0.01;
    double p1 = 0.0;
    double p2 = 0.0;
    double k3 = 0.01;
    cv::Mat distCoeffs = (cv::Mat_<double>(5, 1) << k1, k2, p1, p2, k3);

    //Get Cols & Rows #N

    int n_cols = set_of_preprocessed_datasets.at(0).at(0).on_img.cols;
    int n_rows = set_of_preprocessed_datasets.at(0).at(0).on_img.rows;

    cv::Mat newCameraMatrix = cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, cv::Size(n_cols, n_rows), 1);


    //! RECONSTRUCTION & BUILDING STEP:

    std::vector<GLfloat> xyz_slice = {}; //Flattened XYZ Slice array

    int i = 0;
    for (std::vector<LazerSlice> preprocessed_dataset : set_of_preprocessed_datasets) {

        float r;
        float g;
        float b;

        if (i == 0) {
            r = 0.0f; g = 1.0f; b = 0.0f;
        }
        else {
            r = 1.0f; g = 0.0f; b = 0.25f;
        }


        for (LazerSlice slice : preprocessed_dataset) {
            extract_cylindrical_pts__rot_mat(slice, cameraMatrix, distCoeffs, newCameraMatrix, (i*125.0f));
            std::cout << "Sl3D: " << slice.list_3d_points.size() << std::endl;

            //float angle_sensitive_color = slice.angle = slice.angle / 360.0;

            for (glm::vec3 point : slice.list_3d_points) {

                xyz_slice.reserve(6);

                // Inserting the vertices of the cube
                xyz_slice.insert(xyz_slice.end(), {
                    point[0] - cubeSize, point[1] - cubeSize, point[2] - cubeSize, r,g,b, // Vertex 1 (bottom-left-back)
                    point[0] + cubeSize, point[1] - cubeSize, point[2] - cubeSize, r,g,b, // Vertex 2 (bottom-right-back)
                    point[0] + cubeSize, point[1] + cubeSize, point[2] - cubeSize, r,g,b, // Vertex 3 (top-right-back)
                    point[0] - cubeSize, point[1] + cubeSize, point[2] - cubeSize, r,g,b, // Vertex 4 (top-left-back)
                    point[0] - cubeSize, point[1] - cubeSize, point[2] + cubeSize, r,g,b, // Vertex 5 (bottom-left-front)
                    point[0] + cubeSize, point[1] - cubeSize, point[2] + cubeSize, r,g,b, // Vertex 6 (bottom-right-front)
                    point[0] + cubeSize, point[1] + cubeSize, point[2] + cubeSize, r,g,b, // Vertex 7 (top-right-front)
                    point[0] - cubeSize, point[1] + cubeSize, point[2] + cubeSize, r,g,b  // Vertex 8 (top-left-front)
                    });

            }

        }
        i++;
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


// TODO: ADD MORE PIPELINES BELOW

std::map<std::string, PipelineFunction> pipelineMap = {
    {"pipeline1", pipeline1},
    {"pipeline2", pipeline2},
    {"pc", pipelineCombo}
};

// Function to execute a pipeline by name.
VerticeObject executePipeline(const std::string& pipelineName, const std::string& dataset, int& midpoint, int& cutoff) {
    // Find the pipeline function in the map.
    auto it = pipelineMap.find(pipelineName);
    if (it == pipelineMap.end()) {
        std::cerr << "Unknown pipeline: " << pipelineName << "\n";
        printUsage();
        return VerticeObject(); // return default VerticeObject if no matching pipeline found
    }

    // Run the pipeline function.
    return it->second(dataset, midpoint, cutoff);
}

// Function to print usage information.
void printUsage() {
    std::cout << "Usage: program_name pipeline_name parameter\n";
    std::cout << "Available pipelines:\n";
    for (const auto& pair : pipelineMap) {
        std::cout << "  " << pair.first << "\n";
    }
}
