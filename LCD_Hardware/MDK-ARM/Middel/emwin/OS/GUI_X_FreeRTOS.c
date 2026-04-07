#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "GUI.h"

static SemaphoreHandle_t _Semaphore;
static StaticSemaphore_t _Semaphore_Memory;

// 时间函数：返回当前系统滴答数（单位 ms）
int GUI_X_GetTime(void) {
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

void GUI_X_Delay(int Period) {
    vTaskDelay(pdMS_TO_TICKS(Period));
}

void GUI_X_ExecIdle(void) {
    // 空闲时可以让出 CPU
    vTaskDelay(1);
}

// 多任务支持：初始化信号量（计数型，初始值为 1，最大为 1 即互斥量）
void GUI_X_InitOS(void) {
    _Semaphore = xSemaphoreCreateCountingStatic(1, 1, &_Semaphore_Memory);
}

void GUI_X_Lock(void) {
    xSemaphoreTake(_Semaphore, portMAX_DELAY);   // 阻塞等待，直到获得锁
}

void GUI_X_Unlock(void) {
    xSemaphoreGive(_Semaphore);
}

U32 GUI_X_GetTaskId(void) {
    return (U32)xTaskGetCurrentTaskHandle();
}

// 日志函数（调试用，空实现即可）
void GUI_X_Log(const char *s) { (void)s; }
void GUI_X_Warn(const char *s) { (void)s; }
void GUI_X_ErrorOut(const char *s) { (void)s; }
void GUI_X_Init(void) {}