#ifndef PIPELINE_H
#define PIPELINE_H
#include <string>
#include <map>
#include <functional>
#include "struct.h" // Assuming that VerticeObject is defined in this file

// Define the pipeline function type.
typedef std::function<VerticeObject(std::string)> PipelineFunction;

// Function to execute a pipeline by name.
VerticeObject executePipeline(const std::string& pipelineName, const std::string& dataset, int& midpoint, int& cutoff);

// Function to print usage information.
void printUsage();

#endif // PIPELINE_H
