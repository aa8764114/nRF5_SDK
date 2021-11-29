set(PROJECT_NAME hkdf_pca10056)

set(SDK_ROOT ../../../../../../..)

set(PROJ_DIR ../../..)

set(SRC_FILES ../../../../../../../modules/nrfx/mdk/gcc_startup_nrf52840.S ../../../../../../../components/libraries/log/src/nrf_log_backend_rtt.c ../../../../../../../components/libraries/log/src/nrf_log_backend_serial.c ../../../../../../../components/libraries/log/src/nrf_log_backend_uart.c ../../../../../../../components/libraries/log/src/nrf_log_default_backends.c ../../../../../../../components/libraries/log/src/nrf_log_frontend.c ../../../../../../../components/libraries/log/src/nrf_log_str_formatter.c ../../../../../../../components/boards/boards.c ../../../../../../../components/libraries/util/app_error.c ../../../../../../../components/libraries/util/app_error_handler_gcc.c ../../../../../../../components/libraries/util/app_error_weak.c ../../../../../../../components/libraries/util/app_util_platform.c ../../../../../../../components/libraries/util/nrf_assert.c ../../../../../../../components/libraries/atomic/nrf_atomic.c ../../../../../../../components/libraries/balloc/nrf_balloc.c ../../../../../../../external/fprintf/nrf_fprintf.c ../../../../../../../external/fprintf/nrf_fprintf_format.c ../../../../../../../components/libraries/memobj/nrf_memobj.c ../../../../../../../components/libraries/ringbuf/nrf_ringbuf.c ../../../../../../../components/libraries/strerror/nrf_strerror.c ../../../../../../../integration/nrfx/legacy/nrf_drv_uart.c ../../../../../../../modules/nrfx/soc/nrfx_atomic.c ../../../../../../../modules/nrfx/drivers/src/prs/nrfx_prs.c ../../../../../../../modules/nrfx/drivers/src/nrfx_uart.c ../../../../../../../modules/nrfx/drivers/src/nrfx_uarte.c ../../../integration_test_ssi_hkdf_vectors.c ../../../main.c ../../../../../../../external/segger_rtt/SEGGER_RTT.c ../../../../../../../external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c ../../../../../../../external/segger_rtt/SEGGER_RTT_printf.c ../../../../../../../external/nrf_cc310/common/integration_test_plat_utils.c ../../../../../../../modules/nrfx/mdk/system_nrf52840.c )

set(INC_FOLDERS ../../../../../../../external/fprintf ../../.. ../../../../../../../components/libraries/experimental_section_vars ../../../../../../../external/nrf_cc310/include ../../../../../../../components/libraries/atomic ../../../../../../../components/libraries/delay ../config ../../../../../../../components/toolchain/cmsis/include ../../../../../../../components/libraries/balloc ../../../../../../../components/libraries/log ../../../../../../../components/libraries/memobj ../../../../../../../components ../../../../../../../modules/nrfx/mdk ../../../../../../../components/libraries/strerror ../../../../../../../integration/nrfx ../../../../../../../modules/nrfx/drivers/include ../../../../../../../components/libraries/ringbuf ../../../../../../../modules/nrfx ../../../../../../../components/drivers_nrf/nrf_soc_nosd ../../../../../../../components/libraries/log/src ../../../../../../../external/segger_rtt ../../../../../../../modules/nrfx/hal ../../../../../../../components/libraries/bsp ../../../../../../../components/boards ../../../../../../../integration/nrfx/legacy ../../../../../../../components/libraries/util ../../../../../../../external/nrf_cc310/common )

set(CFLAGS -O3 -g3 -DBOARD_PCA10056 -DBSP_DEFINES_ONLY -DCONFIG_GPIO_AS_PINRESET -DDEBUG -DDEBUG_NRF -DDX_CC_TEE -DFLOAT_ABI_HARD -DHKDF_INTEGTEST -DNRF52840_XXAA -DNRF_SDK_PRESENT -mcpu=cortex-m4 -mthumb -mabi=aapcs -Wall -Werror -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums)

set(CXXFLAGS -O3 -g3)

cmake_minimum_required(VERSION 2.4.0)
project(${PROJECT_NAME})
list(APPEND CFLAGS "-undef" "-D__GNUC__")
list(FILTER CFLAGS EXCLUDE REGEX mcpu)
string(REPLACE ";" " " CFLAGS "${CFLAGS}")
set(CMAKE_C_FLAGS ${CFLAGS})
include_directories(${INC_FOLDERS})
add_executable(${PROJECT_NAME} ${SRC_FILES})
