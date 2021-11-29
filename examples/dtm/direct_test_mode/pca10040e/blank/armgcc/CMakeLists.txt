set(PROJECT_NAME direct_test_mode_pca10040e)

set(SDK_ROOT ../../../../../..)

set(PROJ_DIR ../../..)

set(SRC_FILES ../../../../../../modules/nrfx/mdk/gcc_startup_nrf52810.S ../../../../../../components/libraries/log/src/nrf_log_frontend.c ../../../../../../components/libraries/log/src/nrf_log_str_formatter.c ../../../../../../components/libraries/util/app_error.c ../../../../../../components/libraries/util/app_error_handler_gcc.c ../../../../../../components/libraries/util/app_error_weak.c ../../../../../../components/libraries/fifo/app_fifo.c ../../../../../../components/libraries/uart/app_uart_fifo.c ../../../../../../components/libraries/util/app_util_platform.c ../../../../../../components/libraries/util/nrf_assert.c ../../../../../../components/libraries/atomic/nrf_atomic.c ../../../../../../components/libraries/balloc/nrf_balloc.c ../../../../../../external/fprintf/nrf_fprintf.c ../../../../../../external/fprintf/nrf_fprintf_format.c ../../../../../../components/libraries/memobj/nrf_memobj.c ../../../../../../components/libraries/ringbuf/nrf_ringbuf.c ../../../../../../components/libraries/strerror/nrf_strerror.c ../../../../../../components/boards/boards.c ../../../../../../integration/nrfx/legacy/nrf_drv_uart.c ../../../../../../modules/nrfx/soc/nrfx_atomic.c ../../../../../../modules/nrfx/drivers/src/prs/nrfx_prs.c ../../../../../../modules/nrfx/drivers/src/nrfx_uart.c ../../../../../../modules/nrfx/drivers/src/nrfx_uarte.c ../../../main.c ../../../../../../components/ble/ble_dtm/ble_dtm.c ../../../../../../components/ble/ble_dtm/ble_dtm_hw_nrf52.c ../../../../../../modules/nrfx/mdk/system_nrf52810.c )

set(INC_FOLDERS ../../../../../../components ../../../../../../modules/nrfx/mdk ../../.. ../../../../../../components/libraries/fifo ../../../../../../components/libraries/strerror ../../../../../../components/toolchain/cmsis/include ../../../../../../components/ble/ble_dtm ../../../../../../components/libraries/util ../config ../../../../../../components/libraries/balloc ../../../../../../components/libraries/ringbuf ../../../../../../modules/nrfx/hal ../../../../../../components/libraries/bsp ../../../../../../components/libraries/uart ../../../../../../components/libraries/log ../../../../../../modules/nrfx ../../../../../../components/libraries/experimental_section_vars ../../../../../../integration/nrfx/legacy ../../../../../../integration/nrfx ../../../../../../components/drivers_nrf/nrf_soc_nosd ../../../../../../components/libraries/atomic ../../../../../../components/boards ../../../../../../components/libraries/memobj ../../../../../../modules/nrfx/drivers/include ../../../../../../external/fprintf ../../../../../../components/libraries/log/src )

set(CFLAGS -O3 -g3 -DBOARD_PCA10040 -DBSP_DEFINES_ONLY -DCONFIG_GPIO_AS_PINRESET -DDEVELOP_IN_NRF52832 -DFLOAT_ABI_SOFT -DNRF52810_XXAA -DNRF52_PAN_74 -DNRFX_COREDEP_DELAY_US_LOOP_CYCLES=3 -mcpu=cortex-m4 -mthumb -mabi=aapcs -Wall -Werror -mfloat-abi=soft -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums)

set(CXXFLAGS -O3 -g3)

cmake_minimum_required(VERSION 2.4.0)
project(${PROJECT_NAME})
list(APPEND CFLAGS "-undef" "-D__GNUC__")
list(FILTER CFLAGS EXCLUDE REGEX mcpu)
string(REPLACE ";" " " CFLAGS "${CFLAGS}")
set(CMAKE_C_FLAGS ${CFLAGS})
include_directories(${INC_FOLDERS})
add_executable(${PROJECT_NAME} ${SRC_FILES})
