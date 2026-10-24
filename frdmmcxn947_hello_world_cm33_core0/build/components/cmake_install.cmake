# Install script for directory: C:/FRDM/mcuxsdk/mcuxsdk/components

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/frdmmcxn947_hello_world_cm33_core0")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Users/Shreedhargouda Patil/.mcuxpressotools/arm-gnu-toolchain-14.2.rel1-mingw-w64-x86_64-arm-none-eabi/bin/arm-none-eabi-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/assert/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/audio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/aws_iot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/button/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/common_task/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/coremark/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/crc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/debug_console/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/debug_console_lite/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/display/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/exception_handling/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/gpio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/i2c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/internal_flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/led/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/lists/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/log/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/mem_manager/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/messaging/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/misc_utilities/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/osa/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/panic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/pmic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/power_manager/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/pwm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/reset/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/reset1/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/rng/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/rpmsg/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/rtc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/sensor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/serial_manager/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/shell/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/spi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/str/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/timer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/timer_manager/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/time_stamp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/touch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/uart/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/video/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/phy/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/IS42SM16800H/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/adc_sensor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/conn_fwloader/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/ele_crypto/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/ele_hseb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/i3c_bus/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/imx_sm_crc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/imu_adapter/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/mt48lc2m32b2/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/mt48lc4m16a2/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/mx25_flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/mx25l_flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/mx25r_flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/rtt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/scmi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/sdu/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/slcd_engine/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/smt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/srtm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/sx1502/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/wifi_bt_module/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/smbus/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/debug_console_rtt/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/notifier/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/ele_base_api/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/format/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/mpi_loader/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/pinctrl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/clock/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/silicon_id/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/sm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/systick_timer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/timer_lptmr/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/unity/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/power/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/codec/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/expander/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/edgefast_wifi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/eeprom_emulation/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/storage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/debug/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/gen_hal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/lce/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/sgi_pkc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_dspi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_ecspi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_enet/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_flexcomm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_gpio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_i2c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_ii2c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_iuart/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_lpc_gpio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_lpc_i2c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_lpc_vspi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_lpc_vusart/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_lpi2c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_lpsci/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_lpspi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_lpuart/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_spi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/NXP_Motor_Fault_AI/frdmmcxn947_hello_world_cm33_core0/build/components/cmsis_drivers/cmsis_uart/cmake_install.cmake")
endif()

