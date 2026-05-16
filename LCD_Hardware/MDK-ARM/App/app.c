#include "app.h"

void LCD_task(void *pvParameters);
TaskHandle_t LCD_Task_Handler;
#define LCD_TASK_STACK_SIZE 4096
#define LCD_TASK_PRIORITY 1

extern SPI_HandleTypeDef hspi2;

void app_start(void)
{

    // printf("Starting application...\n");
    // LCD刷新任务
    xTaskCreate(LCD_task, "LCD_Task", LCD_TASK_STACK_SIZE, NULL, LCD_TASK_PRIORITY, &LCD_Task_Handler);

    vTaskStartScheduler();
}

void LCD_task(void *pvParameters)
{ // 初始化 emWin
     LCD_X_Config();
     HAL_IWDG_Refresh(&hiwdg); // 刷新独立看门狗
    int ret = GUI_Init();
    if (ret != 0)
        printf("GUI_Init failed with error code: %d\n", ret);

    // ret = HAL_SPI_DeInit(&hspi2); // 先反初始化 SPI，确保干净的状态
    // //HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15); // 反初始化 SPI2 相关 GPIO
    // if (ret != HAL_OK)
    //     printf("HAL_SPI_DeInit failed with error code: %d\n", ret);

    // SPI2_Init_Set_Prescaler (SPI_BAUDRATEPRESCALER_8); // 设置 SPI 时钟分频，确保与 LCD 兼容
    // printf("SPI2 initialized with new prescaler.\n");

    // GUI_DispString("TEST");
    set_fatherwin(); // 设置父窗口背景

    WM_HWIN hDlg = EditModule_Create();
    WM_InvalidateWindow(hDlg);
    GUI_Exec();
    LCD_X_DisplayDriver(0, LCD_X_SHOWBUFFER, NULL);

    while (1)
    {
        // // 运行 GUI 窗口管理器
        GUI_Exec();
        // 只处理消息，不主动刷新！！！
        // if (GUI_Exec() == 0)
        // {
        //     // 只有界面真的变了，才刷新
        //     static int last_tick = 0;
        //     if (xTaskGetTickCount() - last_tick > 50)
        //     {
        //         LCD_X_DisplayDriver(0, LCD_X_SHOWBUFFER, NULL);
        //         last_tick = xTaskGetTickCount();
        //     }
        // }

        vTaskDelay(40);
        HAL_IWDG_Refresh(&hiwdg); // 刷新独立看门狗
    }
}
