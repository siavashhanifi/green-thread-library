cmake_minimum_required(VERSION 3.8)

project("gthread")

add_library(${PROJECT_NAME} 
"src/queue_utility.h"
"src/queue_utility.c"
"src/utility.h"
"src/utility.c"
"src/globals.h"
"src/globals.c"
"src/green.h"
"src/green.c"
"src/mutex.h"
"src/mutex.c"
"src/cond.h"
"src/cond.c"
)
