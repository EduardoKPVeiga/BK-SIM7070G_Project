#include "../include/main.h"

static const char TAG[] = "MAIN";

extern "C"
{
    void app_main(void)
    {
#ifdef DEBUG
        esp_log_level_set("*", ESP_LOG_INFO);
#endif
        uart2_task_init();
        gsm_routine_task_init();
    }
}
