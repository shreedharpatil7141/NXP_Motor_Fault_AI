# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "flash_debug")
  file(REMOVE_RECURSE
  "clean_files-NOTFOUND"
  "frdmmcxn947_tflm_cifar10_cm33_core0_cm33_core0.bin"
  )
endif()
