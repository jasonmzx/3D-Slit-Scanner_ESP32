#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>
#include <map>
#include <functional>

// Define the pipeline function type.
typedef std::function<void()> PipelineFunction;

// Function to execute a pipeline by name.
void executePipeline(const std::string& pipelineName);

// Function to print usage information.
void printUsage();

#endif // PIPELINE_H
