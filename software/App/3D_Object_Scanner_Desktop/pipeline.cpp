#include "pipeline.h"

#include <iostream>
#include <map>
#include <functional>

// Define the pipeline function type.
typedef std::function<void()> PipelineFunction;

// Define your pipeline functions.
void pipeline1() {
    std::cout << "\n Running pipeline 1...\n";
    // img preproc, algorithm A on pre-proc data, vertex gen
}

void pipeline2() {
    std::cout << "\n Running pipeline 2...\n";
    // img preproc, algorithm A on pre-proc data, vertex gen
}

// Add your pipeline functions to a map.
std::map<std::string, PipelineFunction> pipelineMap = {
    {"pipeline1", pipeline1},
    {"pipeline2", pipeline2},
    // Add more pipelines here.
};

// Function to execute a pipeline by name.
void executePipeline(const std::string& pipelineName) {
    // Find the pipeline function in the map.
    auto it = pipelineMap.find(pipelineName);
    if (it == pipelineMap.end()) {
        std::cerr << "Unknown pipeline: " << pipelineName << "\n";
        printUsage();
        return;
    }

    // Run the pipeline function.
    it->second();
}

// Function to print usage information.
void printUsage() {
    std::cout << "Usage: program_name pipeline_name\n";
    std::cout << "Available pipelines:\n";
    for (const auto& pair : pipelineMap) {
        std::cout << "  " << pair.first << "\n";
    }
}

