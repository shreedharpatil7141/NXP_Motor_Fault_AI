# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "flash_debug")
  file(REMOVE_RECURSE
  "clean_files-NOTFOUND"
  "motor_fault_npu_cm33_core0.bin"
  )
endif()
