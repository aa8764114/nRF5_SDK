set(PROJECT_NAME simple_timer_pca10056)

set(SDK_ROOT ../../../../../..)

set(PROJ_DIR ../../..)

set(SRC_FILES ../../../../../../modules/nrfx/mdk/gcc_startup_nrf52840.S ../../../../../../components/libraries/log/src/nrf_log_frontend.c ../../../../../../components/libraries/log/src/nrf_log_str_formatter.c ../../../../../../components/boards/boards.c ../../../../../../components/libraries/button/app_button.c ../../../../../../components/libraries/util/app_error.c ../../../../../../components/libraries/util/app_error_handler_gcc.c ../../../../../../components/libraries/util/app_error_weak.c ../../../../../../components/libraries/scheduler/app_scheduler.c ../../../../../../components/libraries/simple_timer/app_simple_timer.c ../../../../../../components/libraries/timer/app_timer2.c ../../../../../../components/libraries/util/app_util_platform.c ../../../../../../components/libraries/timer/drv_rtc.c ../../../../../../components/libraries/util/nrf_assert.c ../../../../../../components/libraries/atomic_fifo/nrf_atfifo.c ../../../../../../components/libraries/atomic/nrf_atomic.c ../../../../../../components/libraries/balloc/nrf_balloc.c ../../../../../../external/fprintf/nrf_fprintf.c ../../../../../../external/fprintf/nrf_fprintf_format.c ../../../../../../components/libraries/memobj/nrf_memobj.c ../../../../../../components/libraries/ringbuf/nrf_ringbuf.c ../../../../../../components/libraries/sortlist/nrf_sortlist.c ../../../../../../components/libraries/strerror/nrf_strerror.c ../../../../../../components/drivers_nrf/nrf_soc_nosd/nrf_nvic.c ../../../../../../components/drivers_nrf/nrf_soc_nosd/nrf_soc.c ../../../../../../modules/nrfx/soc/nrfx_atomic.c ../../../../../../modules/nrfx/drivers/src/nrfx_gpiote.c ../../../../../../modules/nrfx/drivers/src/nrfx_timer.c ../../../../../../components/libraries/bsp/bsp.c ../../../main.c ../../../../../../modules/nrfx/mdk/system_nrf52840.c )

set(INC_FOLDERS ../../../../../../components ../../../../../../modules/nrfx/mdk ../../../../../../components/libraries/scheduler ../../../../../../modules/nrfx ../../.. ../../../../../../components/libraries/timer ../../../../../../components/boards ../../../../../../components/libraries/strerror ../../../../../../components/toolchain/cmsis/include ../../../../../../components/libraries/simple_timer ../config ../../../../../../components/libraries/balloc ../../../../../../components/libraries/ringbuf ../../../../../../modules/nrfx/hal ../../../../../../components/libraries/bsp ../../../../../../components/libraries/log ../../../../../../components/libraries/button ../../../../../../components/libraries/util ../../../../../../components/libraries/experimental_section_vars ../../../../../../integration/nrfx/legacy ../../../../../../components/libraries/delay ../../../../../../integration/nrfx ../../../../../../components/libraries/atomic_fifo ../../../../../../components/drivers_nrf/nrf_soc_nosd ../../../../../../components/libraries/atomic ../../../../../../components/libraries/sortlist ../../../../../../components/libraries/memobj ../../../../../../modules/nrfx/drivers/include ../../../../../../external/fprintf ../../../../../../components/libraries/log/src )

set(CFLAGS -O3 -g3 -DAPP_TIMER_V2 -DAPP_TIMER_V2_RTC1_ENABLED -DBOARD_PCA10056 -DCONFIG_GPIO_AS_PINRESET -DFLOAT_ABI_HARD -DNRF52840_XXAA -mcpu=cortex-m4 -mthumb -mabi=aapcs -Wall -Werror -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums)

set(CXXFLAGS -O3 -g3)

cmake_minimum_required(VERSION 2.4.0)
project(${PROJECT_NAME})
list(APPEND CFLAGS "-undef" "-D__GNUC__")
list(FILTER CFLAGS EXCLUDE REGEX mcpu)
string(REPLACE ";" " " CFLAGS "${CFLAGS}")
set(CMAKE_C_FLAGS ${CFLAGS})
include_directories(${INC_FOLDERS})
add_executable(${PROJECT_NAME} ${SRC_FILES})
