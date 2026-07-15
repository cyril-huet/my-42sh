#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

int execute_pipeline(const struct pipeline *pipeline);
int execute_pipeline_background(const struct pipeline *pipeline,
                                int *process_id);

#endif /* ! EXECUTOR_H */