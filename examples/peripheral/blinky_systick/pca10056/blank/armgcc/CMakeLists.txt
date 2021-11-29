set(PROJECT_NAME blinky_systick_pca10056)

set(SDK_ROOT ../../../../../..)

set(PROJ_DIR ../../..)

set(SRC_FILES ../../../../../../modules/nrfx/mdk/gcc_startup_nrf52840.S ../../../../../../components/libraries/log/src/nrf_log_frontend.c ../../../../../../components/libraries/log/src/nrf_log_str_formatter.c ../../../../../../components/boards/boards.c ../../../../../../components/libraries/util/app_error.c ../../../../../../components/libraries/util/app_error_handler_gcc.c ../../../../../../components/libraries/util/app_error_weak.c ../../../../../../components/libraries/util/app_util_platform.c ../../../../../../components/libraries/util/nrf_assert.c ../../../../../../components/libraries/atomic/nrf_atomic.c ../../../../../../components/libraries/balloc/nrf_balloc.c ../../../../../../external/fprintf/nrf_fprintf.c ../../../../../../external/fprintf/nrf_fprintf_format.c ../../../../../../components/libraries/memobj/nrf_memobj.c ../../../../../../components/libraries/ringbuf/nrf_ringbuf.c ../../../../../../components/libraries/strerror/nrf_strerror.c ../../../../../../modules/nrfx/soc/nrfx_atomic.c ../../../../../../modules/nrfx/drivers/src/nrfx_systick.c ../../../main.c ../../../../../../modules/nrfx/mdk/system_nrf52840.c )

set(INC_FOLDERS ../../../../../../components/libraries/util ../../../../../../modules/nrfx ../../../../../../components/libraries/experimental_section_vars ../../../../../../modules/nrfx/mdk ../../../../../../integration/nrfx/legacy ../../../../../../components/libraries/balloc ../../../../../../components/libraries/strerror ../../../../../../external/fprintf ../../../../../../components/libraries/ringbuf ../../.. ../../../../../../integration/nrfx ../../../../../../components/drivers_nrf/nrf_soc_nosd ../config ../../../../../../modules/nrfx/drivers/include ../../../../../../components/libraries/atomic ../../../../../../components/boards ../../../../../../components/libraries/log ../../../../../../components/toolchain/cmsis/include ../../../../../../modules/nrfx/hal ../../../../../../components/libraries/memobj ../../../../../../components/libraries/log/src )

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
