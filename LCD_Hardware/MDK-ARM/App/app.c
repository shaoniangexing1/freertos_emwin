#include "app.h"

void LCD_task(void *pvParameters);
TaskHandle_t LCD_Task_Handler;
#define LCD_TASK_STACK_SIZE 8194
#define LCD_TASK_PRIORITY 2

void Touch_task(void *pvParameters);
TaskHandle_t TOUCH_Task_Handler;
#define LTOUCH_TASK_STACK_SIZE 512
#define TOUCH_TASK_PRIORITY 3

extern SPI_HandleTypeDef hspi2;


static void OnGestureDetected(GestureType type, int x, int y);

void app_start(void)
{

    // printf("Starting application...\n");
    // LCD刷新任务
    xTaskCreate(LCD_task, "LCD_Task", LCD_TASK_STACK_SIZE, NULL, LCD_TASK_PRIORITY, &LCD_Task_Handler);//LCD刷新任务
		xTaskCreate(Touch_task, "Touch_Task", LTOUCH_TASK_STACK_SIZE, NULL, TOUCH_TASK_PRIORITY, &TOUCH_Task_Handler);//电容按键任务

    vTaskStartScheduler();
}

void LCD_task(void *pvParameters)
{ // 初始化emWin
    LCD_X_Config();
     HAL_IWDG_Refresh(&hiwdg); // 刷新看门狗
    int ret = GUI_Init();
    if (ret != 0)
        printf("GUI_Init failed with error code: %d\n", ret);
    Gesture_Init(OnGestureDetected); // 初始化手势检测，注册回调函数    
    
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
        

        vTaskDelay(100);
        HAL_IWDG_Refresh(&hiwdg); // 刷新标立看门狗
    }
}

void Touch_task(void *pvParameters){
	//FT6236_Init();
	uint32_t ulNotificationValue;
    
	/**==============================demo测试触摸================================== */
        // if (TPR_Structure.TouchSta & TP_COORD_UD) // 触摸按下
        // {
        //     TPR_Structure.TouchSta &= ~TP_COORD_UD; // 清除标标
        //     FT6236_Scan();                          // 读取坐标
        //     printf("x轴坐标:\t%d\r\n", TPR_Structure.x[0]);
        //     printf("Y轴坐标:\t%d\r\n", TPR_Structure.y[0]);
        // }
        /**==============================emwin 测试触摸,增加触摸状态================================== */
	while(1){
		// 阻塞等待通知，portMAX_DELAY表示永远等待
        // 第一个参数pdTRUE表示收到通知后将通知值清0（二值信号量模式）
        ulNotificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		
		if (TPR_Structure.TouchSta & TP_COORD_UD){
            TPR_Structure.TouchSta &= ~TP_COORD_UD;
            FT6236_Scan();
            Gesture_Update(TPR_Structure.TouchSta & TP_PRES_DOWN, TPR_Structure.x[0], TPR_Structure.y[0]);
        }
		vTaskDelay(10);
     HAL_IWDG_Refresh(&hiwdg); // 刷新标立看门狗
	}
        
}

// 你的触摸读取函数
static void OnGestureDetected(GestureType type, int x, int y)
{
    switch (type) {
        case GESTURE_SHORT_PRESS:// 短按
            printf("短按\n");
            break;

        case GESTURE_LONG_PRESS:// 长按
            printf("长按\n");   
            break;

        case GESTURE_SWIPE_LEFT:// 左滑
            printf("左滑\n");   
            break;

        case GESTURE_SWIPE_RIGHT:// 右滑
        printf("右滑\n");
            break;

        case GESTURE_SWIPE_UP:// 上滑
            printf("上滑\n");
            break;

        case GESTURE_SWIPE_DOWN:// 下滑
           printf("下滑\n");
            break;

        default:
            break;
    }
}