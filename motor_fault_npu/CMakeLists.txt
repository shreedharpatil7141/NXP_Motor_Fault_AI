cmake_minimum_required(VERSION 3.22.0)

include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

project(motor_fault_npu
    LANGUAGES C CXX ASM
    PROJECT_BOARD_PORT_PATH ${board_root}/${board}/eiq_examples/tflm_cifar10
)

mcux_add_include(
    INCLUDES .
    pcq_npu
    ${SdkRootDirPath}/middleware/eiq/neutron/common/include
    ${SdkRootDirPath}/middleware/eiq/neutron/driver/include
    ${SdkRootDirPath}/examples/eiq_examples/common/tflm/model
    ${SdkRootDirPath}/examples/eiq_examples/common
)

mcux_add_source(
    BASE_PATH ${SdkRootDirPath}

    examples/eiq_examples/common/tflm/model/get_top_n.cpp
    examples/eiq_examples/common/tflm/model/get_top_n.h
    examples/eiq_examples/common/tflm/model.cpp
    examples/eiq_examples/common/tflm/model/model.h
    examples/eiq_examples/common/tflm/demo_info.cpp
    examples/eiq_examples/common/demo_info.h
    examples/eiq_examples/common/timer.c
    examples/eiq_examples/common/timer.h

    middleware/eiq/neutron/mcxn/libNeutronFirmware.a
    middleware/eiq/neutron/mcxn/libNeutronDriver.a
)

mcux_add_source(
    SOURCES
    main.cpp
    pcq_npu/model_data.h
    pcq_npu/model_motor_fault_ops_npu.cpp
)

include(${SdkRootDirPath}/CMakeLists.txt)

include(
    ${SdkRootDirPath}/${board_root}/${board}/eiq_examples/tflm_cifar10/reconfig.cmake
    OPTIONAL
)

mcux_convert_binary(
    BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin
)
