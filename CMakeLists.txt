##############################################################################################################
# SET UP CMAKE
cmake_minimum_required(VERSION 3.22)
project(Telos)
set(CMAKE_CXX_STANDARD 17)
set(MY_COMPILER "default" CACHE STRING "Choose a compiler: 'emcc', or 'gcc'")

##############################################################################################################
# GRAB ALL SOURCE FILES

# RECURSIVELY OBTAIN ALL SOURCE FILES FOR THE PROJECT
file(GLOB_RECURSE PROJECT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"                                         # PROJECT SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"                                           # PROJECT HEADERS                      
)

# SOURCE FILES FOR IMGUI, SDL2_GFX, AND THE PROJECT
file(GLOB SOURCES 
    ${PROJECT_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/*.cpp"                                   # IMGUI SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/*.h"                                     # IMGUI HEADERS  
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends/imgui_impl_sdl2.h"              # IMGUI SDL2 BACKEND
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends/imgui_impl_sdl2.cpp"            # IMGUI SDL2 BACKEND
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends/imgui_impl_opengl3.h"           # IMGUI OPENGL3 BACKEND
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends/imgui_impl_opengl3.cpp"         # IMGUI OPENGL3 BACKEND
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends/imgui_impl_sdlrenderer2.h"      # IMGUI SDLRENDERER2 BACKEND
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends/imgui_impl_sdlrenderer2.cpp"    # IMGUI SDLRENDERER2 BACKEND
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2_gfx-1.0.4/*.h"                            # SDL2_GFX HEADERS          
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2_gfx-1.0.4/*.cpp"                          # SDL2_GFX SOURCES    
)
##############################################################################################################
# ADD SDL2 AS A LIBRARY. IT ALREADY HAS ITS OWN CMAKELISTS.TXT SO WE CAN JUST STRAIGHT UP ADD THE SUBDIRECTORY
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2")

# ADD JSON AS A LIBRARY. IT ALREADY HAS ITS OWN CMAKELISTS.TXT SO WE CAN JUST STRAIGHT UP ADD THE SUBDIRECTORY
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/json-3.11.3")


##############################################################################################################
# ADD IMGUI AS A STATIC LIBRARY
add_library(imgui STATIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui.cpp")


##############################################################################################################
# ADD SDL2_GFX AS A STATIC LIBRARY AS WELL
file(GLOB SDL2GFX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2_gfx-1.0.4/*.cpp")
add_library(SDL2GFX STATIC ${SDL2GFX_SOURCES})


##############################################################################################################

# RECURSIVELY OBTAIN ALL DIRECTORIES THAT HAVE .H FILES FOR THE PROJECT
file(GLOB_RECURSE PROJECT_INCLUDE_DIRS LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*"                         # PROJECT INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/json-3.11.3/include/*"         # JSON INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/json-3.11.3/single_include/*"  # JSON SINGLE INCLUDES
)

# ONLY KEEP THE PATHS TO THE DIRECTORIES AND NOT FILES THEMSELVES
foreach(header_file IN LISTS PROJECT_INCLUDE_DIRS)
     get_filename_component(dir_path ${header_file} DIRECTORY)
     list(APPEND PROJECT_INCLUDES ${dir_path})
endforeach()

# REMOVE DUPLICATE DIRECTORY PATHS
list(REMOVE_DUPLICATES PROJECT_INCLUDES) 


# LET CMAKE KNOW OF ALL INCLUDE DIRECTORIES FOR SDL2, IMGUI, AND THE PROJECT
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2/include"                      # SDL2
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2_gfx-1.0.4"                    # SDL2_GFX
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui"                             # IMGUI
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends"                    # IMGUI BACKENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/examples/libs/emscripten"    # IMGUI EMSCRIPTEN
    ${PROJECT_INCLUDES}                                                 # THE PROJECT ITSELF

    # WHEN WE RECURSIVELY SEARCHED FOR DIRECTORIES INSIDE OF /INCLUDE, 
    # FOR BOTH JSON-3.11.3 AND THE PROJECT, WE ONLY GOT THE INNER DIRECTORIES, 
    # WE DID NOT INCLUDE THE /INCLUDE DIRECTORIES THEMSELVES. SO WE NEED TO ADD IT MANUALLY
    "${CMAKE_CURRENT_SOURCE_DIR}/include"           
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/json-3.11.3/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/json-3.11.3/single_include"                         
)


##############################################################################################################
# ADD THE ALL SOURCES TO THE EXECUTABLE THAT WILL BE CREATED
add_executable(${PROJECT_NAME} ${SOURCES}) 


##############################################################################################################
# FIND OPENGL
find_package(OpenGL REQUIRED)


##############################################################################################################

# IF WE ARE BUILDING TO RUN ON A WEB BROWSER, WE NEED TO USE THE EMCC COMPILER.
if(MY_COMPILER STREQUAL "emcc")

    # SET THE COMPILER TO EMCC
    set(CMAKE_CXX_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/lib/emsdk/upstream/emscripten/emcc")
    # SET THE EXECUTABLE SUFFIX TO .JS
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    # SET THE COMPILER FLAGS

    # -s WASM=1: ENABLES WEB ASSEMBLY
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sWASM=1")   

    # -s USE_SDL=2: ENABLES SUPPORT FOR SDL2                                        
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_SDL=2")

    # -s ASYNCIFY ENABLES THE ASYNCIFY FEATURE IN EMSCRIPTEN
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sASYNCIFY")

    # -s ASYNCIFY_IMPORTS=EM_JS_GET,EM_JS_POST: ENABLES THE USE OF "EM_JS_GET" AND "EM_JS_POST" FUNCTIONS DEFINED INSIDE OF HERMES
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sASYNCIFY_IMPORTS=EM_JS_GET,EM_JS_POST")

    # -s FETCH: ENABLES THE FETCH API IN EMSCRIPTEN
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sFETCH")
    
    # -s PTHREAD_POOL_SIZE=navigator.hardwareConcurrency: ENABLES MULTITHREADING
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency")

    # -s TOTAL_MEMORY=512MB: SETS THE TOTAL HEAP MEMORY TO 512MB
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sTOTAL_MEMORY=512MB")

    # --preload-file: PRELOADS FILES INTO THE VIRTUAL FILESYSTEM
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets/Roboto@assets/Roboto")

    # -O3: ENABLES OPTIMIZATION LEVEL 3 (OPTIONAL)
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    
    link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2")
    link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/json-3.11.3")

    target_link_libraries(${PROJECT_NAME} "imgui" "SDL2GFX")

elseif((MY_COMPILER STREQUAL "gcc")) 
    target_link_libraries(${PROJECT_NAME} "SDL2" "SDL2GFX" "imgui" "OpenGL::GL" "nlohmann_json")
endif() 

##############################################################################################################