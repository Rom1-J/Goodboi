cmake_minimum_required(VERSION 3.10)
project(KernelHooker NONE)

include(ProcessorCount)
ProcessorCount(NPROC)

set(KERNEL_DIR /lib/modules/${CMAKE_SYSTEM_VERSION}/build)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/kmod)

file(MAKE_DIRECTORY ${BUILD_DIR})
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)
foreach(F ${SRC_FILES})
    file(RELATIVE_PATH REL_PATH ${SRC_DIR} ${F})
    configure_file(${F} ${BUILD_DIR}/${REL_PATH} COPYONLY)
endforeach()

add_custom_target(build ALL
        COMMAND make -C ${KERNEL_DIR} M=${BUILD_DIR} -j${NPROC} modules
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Building kernel module in ${BUILD_DIR}"
)

add_custom_target(modclean
        COMMAND make -C ${KERNEL_DIR} M=${BUILD_DIR} clean
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMENT "Cleaning kernel module"
)
