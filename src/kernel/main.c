#include <hal/hal.h>
#include <boot/params.h>

void start(boot_params_t* params) {
  hal_init();
}