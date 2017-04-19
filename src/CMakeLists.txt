cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(projName "MORSS")
project(${projName})

set(CMAKE_CXX_FLAGS -fopenmp)
set(CMAKE_C_FLAGS -fopenmp)

################################
# include ITK, to avoid setting it everytime, do:
# set the environment variable ITK_DIR to the dir containing ITKConfig.cmake in .bashrc
find_package (ITK)
if (ITK_FOUND)
   include(${USE_ITK_FILE})
endif(ITK_FOUND) 


set(Libraries ${ITK_LIBRARIES})


# boost
find_path(BOOST_PATH /boost/shared_ptr.hpp)
if(NOT BOOST_PATH)
   message(FATAL_ERROR "Please enter the path of boost")
endif(NOT BOOST_PATH)

include_directories(${BOOST_PATH})

option(build_SFLSMultiRobustStatSeg3DTest "build SFLSMultiRobustStatSeg3DTest?" OFF)
if (build_SFLSMultiRobustStatSeg3DTest)
    set(cexx SFLSMultiRobustStatSeg3DTest)
    ADD_EXECUTABLE(${cexx} ${cexx}.cxx)
    TARGET_LINK_LIBRARIES(${cexx} ${Libraries})
endif (build_SFLSMultiRobustStatSeg3DTest)
