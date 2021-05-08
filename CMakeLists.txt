cmake_minimum_required(VERSION 3.12)
project(Test VERSION 1.0.0)

message("Trying to find glfw3 package")
find_package(glfw3 3.3 REQUIRED)
message("Found glfw3 package")


add_library(
    glad-library
    src/glad.c
    )


add_executable(main src/main.c src/glad.c src/circle_creator.c) 

###########################
# Big question as to why this doesn't work
# add_executable(main src/main.c)
# link_libraries(main glad-library)  
############################

message("Trying to link glfw")
target_link_libraries(main glfw) 
message("Linked glfw")
target_link_libraries(main -lX11) 
target_link_libraries(main -pthread) 
target_link_libraries(main -lGLU) 
target_link_libraries(main -lGL) 
target_link_libraries(main -lXrandr) 
target_link_libraries(main -lXxf86vm) 
target_link_libraries(main -lXi)  
target_link_libraries(main -lXinerama)  
target_link_libraries(main -lrt)  
target_link_libraries(main -ldl)  
target_link_libraries(main -lm)  


add_executable(tests tests/main.c src/circle_creator.c) 
target_link_libraries(tests -lm)  







