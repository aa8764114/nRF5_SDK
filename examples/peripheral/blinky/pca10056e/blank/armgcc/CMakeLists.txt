set(PROJECT_NAME blinky_pca10056e)

set(SDK_ROOT ../../../../../..)

set(PROJ_DIR ../../..)

set(SRC_FILES ../../../../../../modules/nrfx/mdk/gcc_startup_nrf52811.S ../../../../../../components/libraries/log/src/nrf_log_frontend.c ../../../../../../components/libraries/log/src/nrf_log_str_formatter.c ../../../../../../components/boards/boards.c ../../../../../../components/libraries/util/app_error.c ../../../../../../components/libraries/util/app_error_handler_gcc.c ../../../../../../components/libraries/util/app_error_weak.c ../../../../../../components/libraries/util/app_util_platform.c ../../../../../../components/libraries/util/nrf_assert.c ../../../../../../components/libraries/atomic/nrf_atomic.c ../../../../../../components/libraries/balloc/nrf_balloc.c ../../../../../../external/fprintf/nrf_fprintf.c ../../../../../../external/fprintf/nrf_fprintf_format.c ../../../../../../components/libraries/memobj/nrf_memobj.c ../../../../../../components/libraries/ringbuf/nrf_ringbuf.c ../../../../../../components/libraries/strerror/nrf_strerror.c ../../../../../../modules/nrfx/soc/nrfx_atomic.c ../../../main.c ../../../../../../modules/nrfx/mdk/system_nrf52811.c )

set(INC_FOLDERS ../../../../../../components ../../../../../../modules/nrfx/mdk ../../.. ../../../../../../components/libraries/strerror ../../../../../../components/toolchain/cmsis/include ../../../../../../components/libraries/util ../config ../../../../../../components/libraries/balloc ../../../../../../components/libraries/ringbuf ../../../../../../modules/nrfx/hal ../../../../../../components/libraries/bsp ../../../../../../components/libraries/log ../../../../../../modules/nrfx ../../../../../../components/libraries/experimental_section_vars ../../../../../../components/libraries/delay ../../../../../../integration/nrfx ../../../../../../components/drivers_nrf/nrf_soc_nosd ../../../../../../components/libraries/atomic ../../../../../../components/boards ../../../../../../components/libraries/memobj ../../../../../../external/fprintf ../../../../../../components/libraries/log/src )

set(CFLAGS -O3 -g3 -DBOARD_PCA10056 -DBSP_DEFINES_ONLY -DCONFIG_GPIO_AS_PINRESET -DDEVELOP_IN_NRF52840 -DFLOAT_ABI_SOFT -DNRF52811_XXAA -DNRFX_COREDEP_DELAY_US_LOOP_CYCLES=3 -mcpu=cortex-m4 -mthumb -mabi=aapcs -Wall -Werror -mfloat-abi=soft -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums)

set(CXXFLAGS -O3 -g3)

cmake_minimum_required(VERSION 2.4.0)
project(${PROJECT_NAME})
list(APPEND CFLAGS "-undef" "-D__GNUC__")
list(FILTER CFLAGS EXCLUDE REGEX mcpu)
string(REPLACE ";" " " CFLAGS "${CFLAGS}")
set(CMAKE_C_FLAGS ${CFLAGS})
include_directories(${INC_FOLDERS})
add_executable(${PROJECT_NAME} ${SRC_FILES})
