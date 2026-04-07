#include "app.h"

void LCD_task(void *pvParameters);
TaskHandle_t LCD_Task_Handler;

void app_start(void)
{
    taskENTER_CRITICAL();
     printf("Starting application...\n");
    xTaskCreate(LCD_task, "LCD_Task", 2048, NULL, 1, &LCD_Task_Handler);

    taskEXIT_CRITICAL();
    vTaskStartScheduler();
}

void LCD_task(void *pvParameters)
{ // 初始化 emWin
    int ret = GUI_Init();
    printf("GUI_Init: %d\n", ret);
    WM_InvalidateWindow(WM_HBKWIN);
    GUI_Exec();
    GUI_SetColor(GUI_BLACK); // 黑色像素
    GUI_DrawPoint(10, 10);
    // 创建窗口、控件等（示例）
    GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
    GUI_DispStringAt("Hello emWin!", 4, 0);
		LCD_X_DisplayDriver(0, LCD_X_SHOWBUFFER, NULL);
    GUI_Exec();
    while (1)
    {
        // 运行 GUI 窗口管理器
        //GUI_Exec();
        vTaskDelay(100);
        HAL_IWDG_Refresh(&hiwdg); // 刷新独立看门狗
    }
}
