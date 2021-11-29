set(PROJECT_NAME wireless_uart_raw_second_pca10056)

set(SDK_ROOT ../../../../../../../..)

set(PROJ_DIR ../../..)

set(SRC_FILES ../../../../../../../../modules/nrfx/mdk/gcc_startup_nrf52840.S ../../../../../../../../components/libraries/log/src/nrf_log_backend_rtt.c ../../../../../../../../components/libraries/log/src/nrf_log_backend_serial.c ../../../../../../../../components/libraries/log/src/nrf_log_default_backends.c ../../../../../../../../components/libraries/log/src/nrf_log_frontend.c ../../../../../../../../components/libraries/log/src/nrf_log_str_formatter.c ../../../../../../../../components/libraries/util/app_error.c ../../../../../../../../components/libraries/util/app_error_handler_gcc.c ../../../../../../../../components/libraries/util/app_error_weak.c ../../../../../../../../components/libraries/util/app_util_platform.c ../../../../../../../../components/libraries/util/nrf_assert.c ../../../../../../../../components/libraries/atomic/nrf_atomic.c ../../../../../../../../components/libraries/balloc/nrf_balloc.c ../../../../../../../../external/fprintf/nrf_fprintf.c ../../../../../../../../external/fprintf/nrf_fprintf_format.c ../../../../../../../../components/libraries/memobj/nrf_memobj.c ../../../../../../../../components/libraries/ringbuf/nrf_ringbuf.c ../../../../../../../../components/libraries/strerror/nrf_strerror.c ../../../../../../../../components/boards/boards.c ../../../../../../../../integration/nrfx/legacy/nrf_drv_clock.c ../../../../../../../../modules/nrfx/hal/nrf_ecb.c ../../../../../../../../components/drivers_nrf/nrf_soc_nosd/nrf_nvic.c ../../../../../../../../components/drivers_nrf/nrf_soc_nosd/nrf_soc.c ../../../../../../../../modules/nrfx/soc/nrfx_atomic.c ../../../../../../../../modules/nrfx/drivers/src/nrfx_clock.c ../../../../../assert.c ../../../../../fsm.c ../../../../../main.c ../../../../../security.c ../../../../../uart.c ../../../../../../../../external/segger_rtt/SEGGER_RTT.c ../../../../../../../../external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c ../../../../../../../../external/segger_rtt/SEGGER_RTT_printf.c ../../../../../../../../modules/nrfx/mdk/system_nrf52840.c )

set(INC_FOLDERS ../../../../../../../../integration/nrfx ../../../../../../../../components/802_15_4/api/RAL ../../../../../../../../components/802_15_4/api/SysAL ../config ../../../../../../../../components/libraries/experimental_section_vars ../../../../../../../../components/802_15_4/api/HAL/nrf52_soc ../../../../../../../../modules/nrfx/mdk ../../../../../../../../components/libraries/strerror ../../../../../../../../components/boards ../../../../../../../../components/802_15_4/api/HAL ../../../../../../../../components/802_15_4/raw ../../../../../../../../components/drivers_nrf/nrf_soc_nosd ../../../../../../../../components/libraries/log ../../../../../../../../modules/nrfx/hal ../../../../../../../../components/libraries/log/src ../../../../../../../../components/802_15_4/api/PHY ../../../../../../../../integration/nrfx/legacy ../../../../../../../../modules/nrfx/drivers/include ../../../../../../../../components/libraries/ringbuf ../../../../../../../../components/802_15_4 ../../../../../../../../external/fprintf ../../../../../../../../components/802_15_4/api/MAC ../../../../../../../../components/libraries/balloc ../../../../../../../../components/libraries/util ../../../../../../../../modules/nrfx ../../../../../../../../external/segger_rtt ../../../../../../../../components/libraries/memobj ../../../../.. ../../../../../../../../components/libraries/atomic ../../../../../../../../components/libraries/delay ../../../../../../../../components/802_15_4/api/SecAL ../../../../../../../../components/toolchain/cmsis/include ../../../../../../../../components/802_15_4/api/RAL/nrf52_soc )

set(CFLAGS -O3 -g3 -DBOARD_PCA10056 -DCONFIG_GPIO_AS_PINRESET -DFLOAT_ABI_HARD -DNRF52840_XXAA -mcpu=cortex-m4 -mthumb -mabi=aapcs -Wall -Werror -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums)

set(CXXFLAGS -O3 -g3)

cmake_minimum_required(VERSION 2.4.0)
project(${PROJECT_NAME})
list(APPEND CFLAGS "-undef" "-D__GNUC__")
list(FILTER CFLAGS EXCLUDE REGEX mcpu)
string(REPLACE ";" " " CFLAGS "${CFLAGS}")
set(CMAKE_C_FLAGS ${CFLAGS})
include_directories(${INC_FOLDERS})
add_executable(${PROJECT_NAME} ${SRC_FILES})
