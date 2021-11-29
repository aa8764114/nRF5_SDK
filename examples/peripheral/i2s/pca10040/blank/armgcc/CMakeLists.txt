set(PROJECT_NAME i2s_pca10040)

set(SDK_ROOT ../../../../../..)

set(PROJ_DIR ../../..)

set(SRC_FILES ../../../../../../modules/nrfx/mdk/gcc_startup_nrf52.S ../../../../../../components/libraries/log/src/nrf_log_backend_rtt.c ../../../../../../components/libraries/log/src/nrf_log_backend_serial.c ../../../../../../components/libraries/log/src/nrf_log_backend_uart.c ../../../../../../components/libraries/log/src/nrf_log_default_backends.c ../../../../../../components/libraries/log/src/nrf_log_frontend.c ../../../../../../components/libraries/log/src/nrf_log_str_formatter.c ../../../../../../components/boards/boards.c ../../../../../../components/libraries/util/app_error.c ../../../../../../components/libraries/util/app_error_handler_gcc.c ../../../../../../components/libraries/util/app_error_weak.c ../../../../../../components/libraries/util/app_util_platform.c ../../../../../../components/libraries/util/nrf_assert.c ../../../../../../components/libraries/atomic/nrf_atomic.c ../../../../../../components/libraries/balloc/nrf_balloc.c ../../../../../../external/fprintf/nrf_fprintf.c ../../../../../../external/fprintf/nrf_fprintf_format.c ../../../../../../components/libraries/memobj/nrf_memobj.c ../../../../../../components/libraries/ringbuf/nrf_ringbuf.c ../../../../../../components/libraries/strerror/nrf_strerror.c ../../../../../../integration/nrfx/legacy/nrf_drv_uart.c ../../../../../../modules/nrfx/soc/nrfx_atomic.c ../../../../../../modules/nrfx/drivers/src/nrfx_i2s.c ../../../../../../modules/nrfx/drivers/src/prs/nrfx_prs.c ../../../../../../modules/nrfx/drivers/src/nrfx_uart.c ../../../../../../modules/nrfx/drivers/src/nrfx_uarte.c ../../../main.c ../../../../../../external/segger_rtt/SEGGER_RTT.c ../../../../../../external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c ../../../../../../external/segger_rtt/SEGGER_RTT_printf.c ../../../../../../modules/nrfx/mdk/system_nrf52.c )

set(INC_FOLDERS ../../../../../../components ../../../../../../modules/nrfx/mdk ../../.. ../../../../../../components/libraries/strerror ../../../../../../components/toolchain/cmsis/include ../../../../../../components/libraries/util ../config ../../../../../../components/libraries/balloc ../../../../../../components/libraries/ringbuf ../../../../../../modules/nrfx/hal ../../../../../../components/libraries/bsp ../../../../../../components/libraries/log ../../../../../../modules/nrfx ../../../../../../components/libraries/experimental_section_vars ../../../../../../integration/nrfx/legacy ../../../../../../components/libraries/delay ../../../../../../integration/nrfx ../../../../../../components/drivers_nrf/nrf_soc_nosd ../../../../../../external/segger_rtt ../../../../../../components/boards ../../../../../../components/libraries/memobj ../../../../../../modules/nrfx/drivers/include ../../../../../../components/libraries/log/src ../../../../../../external/fprintf ../../../../../../components/libraries/atomic )

set(CFLAGS -O3 -g3 -DBOARD_PCA10040 -DBSP_DEFINES_ONLY -DCONFIG_GPIO_AS_PINRESET -DFLOAT_ABI_HARD -DNRF52 -DNRF52832_XXAA -DNRF52_PAN_74 -mcpu=cortex-m4 -mthumb -mabi=aapcs -Wall -Werror -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums)

set(CXXFLAGS -O3 -g3)

cmake_minimum_required(VERSION 2.4.0)
project(${PROJECT_NAME})
list(APPEND CFLAGS "-undef" "-D__GNUC__")
list(FILTER CFLAGS EXCLUDE REGEX mcpu)
string(REPLACE ";" " " CFLAGS "${CFLAGS}")
set(CMAKE_C_FLAGS ${CFLAGS})
include_directories(${INC_FOLDERS})
add_executable(${PROJECT_NAME} ${SRC_FILES})
