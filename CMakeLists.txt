project(vis1)
cmake_minimum_required(VERSION 2.8.12)

# add CMakeLists.txt source dir and cmake module dir to the cmake module path
# cmake modules are cmake files used e.g. to find library files
set(CMAKE_MODULE_PATH 
    ${CMAKE_MODULE_PATH} 
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/CMakeModules"
    ) 

# specify build dir for out-of-source build (aka shadow build)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


### COMPILER FLAGS ###
# gcc flags
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++0x")
# find moc includes (ui files etc.) in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# instruct CMake to run qt moc (meta object compiler) automatically when needed.
set(CMAKE_AUTOMOC ON)


### EXTERNAL LIBRARIES ###

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5UiTools)
find_package(Qt5Widgets)
find_package(Qt5OpenGL)
find_package(Qt5OpenGLExtensions)


### SOURCE FILES ###

# relative path to every single h and cpp file
set(SRC_CLASSES 

    src/Main.cpp
    
    src/MainWindow.h
    src/MainWindow.cpp
    
    src/Vector.h
    src/Vector.cpp
    src/Volume.h
    src/Volume.cpp
    src/VectorField.h
    src/VectorField.cpp
    src/MultiSet.h
    src/MultiSet.cpp
    
    )
	
# relative path to shader files
set(SRC_SHADERS

    # nothing here yet
    # src/shaders/example.vert
    # src/shaders/example.frag

    )

QT5_WRAP_UI(UI_HEADERS

            src/MainWindow.ui
            
            )
		
# adds an executable target with given name to be built from the source files listed afterwards
add_executable(${PROJECT_NAME} ${SRC_CLASSES} ${SRC_SHADERS} ${UI_HEADERS})

### INCLUDE HEADER FILES ###
include_directories(${OPENGL_INCLUDE_DIRS} 
                    ${GLEW_INCLUDE_DIRS}
                    )

### LINK LIBRARIES ###
target_link_libraries(${PROJECT_NAME}
                      ${OPENGL_LIBRARIES}
                      ${GLEW_LIBRARY}
                      Qt5::Core
                      Qt5::Gui
                      Qt5::UiTools
                      Qt5::Widgets
                      Qt5::OpenGL
                      Qt5::OpenGLExtensions
                      )
