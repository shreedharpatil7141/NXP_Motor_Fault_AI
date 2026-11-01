cmake_minimum_required(VERSION 3.22.0)

include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

project(motor_fault_npu
    LANGUAGES C CXX ASM
    PROJECT_BOARD_PORT_PATH ${board_root}/${board}/eiq_examples/tflm_cifar10
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}/examples/eiq_examples
    common
    common/tflm/model
)

mcux_add_include(
    INCLUDES .
    pcq_npu
    neutron/common/include
    neutron/driver/include
)

mcux_add_source(
    BASE_PATH ${SdkRootDirPath}

    examples/eiq_examples/common/tflm/model/get_top_n.cpp
    examples/eiq_examples/common/tflm/model/get_top_n.h
    examples/eiq_examples/common/tflm/model.cpp
    examples/eiq_examples/common/tflm/model/model.h
    examples/eiq_examples/common/tflm/demo_info.cpp
    examples/eiq_examples/common/demo_info.h
    examples/eiq_examples/common/board_init.h
    examples/eiq_examples/common/timer.c
    examples/eiq_examples/common/timer.h

)

mcux_add_source(
    SOURCES
    main.cpp
    alert.cpp
    alert.h
    preprocess.cpp
    preprocess.h
    raw_vectors.h
    pcq_npu/model_data.h
    pcq_npu/model_motor_fault_ops_npu.cpp
    neutron/mcxn/libNeutronFirmware.a
    neutron/mcxn/libNeutronDriver.a
)

mcux_add_source(
    BASE_PATH ${SdkRootDirPath}/arch/arm/CMSIS
    SOURCES
    DSP/Source/CommonTables/arm_common_tables.c
    DSP/Source/CommonTables/arm_const_structs.c
    DSP/Source/TransformFunctions/arm_bitreversal2.c
    DSP/Source/TransformFunctions/arm_cfft_f32.c
    DSP/Source/TransformFunctions/arm_cfft_init_f32.c
    DSP/Source/TransformFunctions/arm_cfft_radix8_f32.c
    DSP/Source/TransformFunctions/arm_rfft_fast_f32.c
    DSP/Source/TransformFunctions/arm_rfft_fast_init_f32.c
)

include(${SdkRootDirPath}/CMakeLists.txt)

include(
    ${SdkRootDirPath}/${board_root}/${board}/eiq_examples/tflm_cifar10/reconfig.cmake
    OPTIONAL
)

get_target_property(MOTOR_FAULT_SOURCES ${MCUX_SDK_PROJECT_NAME} SOURCES)
list(FILTER MOTOR_FAULT_SOURCES EXCLUDE REGEX "model_cifarnet_ops_npu\\.cpp$")
list(FILTER MOTOR_FAULT_SOURCES EXCLUDE REGEX
    "middleware/eiq/neutron/mcxn/libNeutron(Driver|Firmware)\\.a$")
set_property(
    TARGET ${MCUX_SDK_PROJECT_NAME}
    PROPERTY SOURCES ${MOTOR_FAULT_SOURCES}
)

get_target_property(MOTOR_FAULT_LINK_LIBRARIES ${MCUX_SDK_PROJECT_NAME} LINK_LIBRARIES)
if(MOTOR_FAULT_LINK_LIBRARIES)
    list(FILTER MOTOR_FAULT_LINK_LIBRARIES EXCLUDE REGEX
        "middleware/eiq/neutron/mcxn/libNeutron(Driver|Firmware)\\.a$")
    set_property(
        TARGET ${MCUX_SDK_PROJECT_NAME}
        PROPERTY LINK_LIBRARIES ${MOTOR_FAULT_LINK_LIBRARIES}
    )
endif()

target_include_directories(
    ${MCUX_SDK_PROJECT_NAME}
    PRIVATE
    ${SdkRootDirPath}/examples/eiq_examples/common
    ${SdkRootDirPath}/examples/eiq_examples/common/tflm/model
    ${SdkRootDirPath}/drivers/powerquad
    ${SdkRootDirPath}/drivers/ctimer
    ${SdkRootDirPath}/arch/arm/CMSIS/DSP/Include
)

target_sources(
    ${MCUX_SDK_PROJECT_NAME}
    PRIVATE
    ${SdkRootDirPath}/examples/eiq_examples/common/tflm/model.cpp
    ${SdkRootDirPath}/examples/eiq_examples/common/timer.c
    ${SdkRootDirPath}/drivers/powerquad/fsl_powerquad_basic.c
    ${SdkRootDirPath}/drivers/powerquad/fsl_powerquad_transform.c
    ${SdkRootDirPath}/drivers/ctimer/fsl_ctimer.c
)

target_link_libraries(
    ${MCUX_SDK_PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/neutron/mcxn/libNeutronDriver.a
    ${CMAKE_CURRENT_SOURCE_DIR}/neutron/mcxn/libNeutronFirmware.a
)

target_compile_definitions(
    ${MCUX_SDK_PROJECT_NAME}
    PRIVATE
    MODEL_INPUT_MEAN=0
    MODEL_INPUT_STD=1
)

mcux_convert_binary(
    BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin
)
