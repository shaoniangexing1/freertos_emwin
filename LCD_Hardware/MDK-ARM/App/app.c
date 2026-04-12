#include "app.h"

void LCD_task(void *pvParameters);
TaskHandle_t LCD_Task_Handler;
#define LCD_TASK_STACK_SIZE 2048
#define LCD_TASK_PRIORITY 1

void app_start(void)
{
    
    printf("Starting application...\n");
    //LCD刷新任务
    xTaskCreate(LCD_task, "LCD_Task", LCD_TASK_STACK_SIZE, NULL, LCD_TASK_PRIORITY, &LCD_Task_Handler);

    vTaskStartScheduler();
}

void LCD_task(void *pvParameters)
{ // 初始化 emWin
    int ret = GUI_Init();
    WM_HWIN hDlg=EditModule_Create();
    WM_InvalidateWindow(hDlg);
    GUI_Exec();
    LCD_X_DisplayDriver(0, LCD_X_SHOWBUFFER, NULL);
    while (1)
    {
        // 运行 GUI 窗口管理器
        GUI_Exec();
        vTaskDelay(100);
        HAL_IWDG_Refresh(&hiwdg); // 刷新独立看门狗
    }
}
