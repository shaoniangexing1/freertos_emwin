#include "app.h"

void LCD_task(void *pvParameters);
TaskHandle_t LCD_Task_Handler;
#define LCD_TASK_STACK_SIZE 8194
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
{ // 初始化emWin
    LCD_X_Config();
    // HAL_IWDG_Refresh(&hiwdg); // 刷新看门狗
    int ret = GUI_Init();
    if (ret != 0)
        printf("GUI_Init failed with error code: %d\n", ret);
    FT6236_Init();

    // ret = HAL_SPI_DeInit(&hspi2); // 先反初标化 SPI，确保干净的状标
    // //HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15); // 反初始化 SPI2 相关 GPIO
    // if (ret != HAL_OK)
    //     printf("HAL_SPI_DeInit failed with error code: %d\n", ret);

    // SPI2_Init_Set_Prescaler (SPI_BAUDRATEPRESCALER_8); // 设置 SPI 时钟分频
    // printf("SPI2 initialized with new prescaler.\n");

    // GUI_DispString("TEST");
    set_fatherwin(); // 设置父窗标

    WM_HWIN hDlg = EditModule_Create();
    WM_InvalidateWindow(hDlg);
    GUI_Exec();
    LCD_X_DisplayDriver(0, LCD_X_SHOWBUFFER, NULL);
    while (1)
    {
        char image_src[]="C:\\Users\\Administrator\\STM32Cube\\Repository\\STM32Cube_FW_F4_V1.27.1\\Utilities\\Media\\Pictures\\BMP_240x240\\24-bits\\image04.bmp";
        ImageType type = BMP;
        // // 运GUI 窗口管理
        // GUI_Exec();
        // 处理消息，不主动刷新！！
        // if (GUI_Exec() == 0)
        // {
        //     // 有界面真的变了，才刷
        //     static int last_tick = 0;
        //     if (xTaskGetTickCount() - last_tick > 50)
        //     {
        //         LCD_X_DisplayDriver(0, LCD_X_SHOWBUFFER, NULL);
        //         last_tick = xTaskGetTickCount();
        //     }
        // }

        if (TPR_Structure.TouchSta & TP_COORD_UD) // 触摸按下
        {
            TPR_Structure.TouchSta &= ~TP_COORD_UD; // 清除标标
            FT6236_Scan();                          // 读取坐标
            // while((USART1->SR&0X40)==0);			//
            printf("x轴坐标:\t%d\r\n", TPR_Structure.x[0]);
            // while((USART1->SR&0X40)==0);
            printf("Y轴坐标:\t%d\r\n", TPR_Structure.y[0]);
            //strcpy(image_src,"");
						//EDIT_SetText(hDlg, "aaaaaa"); // <-- 设置编辑框内容
					//GUI_Exec();
					//LCD_X_DisplayDriver(0, LCD_X_SHOWBUFFER, NULL);
            //Emwin_Show_Image(image_src, BMP, 0, 0); // 显示图片
        }

        vTaskDelay(10);
        HAL_IWDG_Refresh(&hiwdg); // 刷新标立看门狗
    }
}
