#ifndef TOUCH_GESTURE_H
#define TOUCH_GESTURE_H

#include "stdint.h"
#include "stdbool.h"
#include "Inf_FT6236.h"

/*********************************************************************
 *
 *  手势类型定义
 *
 *********************************************************************/
typedef enum {
    GESTURE_NONE = 0,      // 无手势
    GESTURE_SHORT_PRESS,   // 短按
    GESTURE_LONG_PRESS,    // 长按
    GESTURE_SWIPE_LEFT,    // 左滑
    GESTURE_SWIPE_RIGHT,   // 右滑
    GESTURE_SWIPE_UP,      // 上滑
    GESTURE_SWIPE_DOWN     // 下滑
} GestureType;

/*********************************************************************
 *
 *  手势回调函数类型
 *  参数：
 *    type: 检测到的手势类型
 *    x:    手势发生的X坐标
 *    y:    手势发生的Y坐标
 *
 *********************************************************************/
typedef void (*GestureCallback)(GestureType type, int x, int y);

/*********************************************************************
 *
 *  公共函数声明
 *
 *********************************************************************/
/**
 * @brief  初始化手势检测模块
 * @param  callback: 手势事件回调函数
 */
void Gesture_Init(GestureCallback callback);

/**
 * @brief  更新手势检测状态
 * @note   必须在主循环中每10ms调用一次，与触摸扫描同步
 * @param  pressed: 1=触摸按下，0=触摸释放
 * @param  x:       当前触摸X坐标（已修正后的坐标）
 * @param  y:       当前触摸Y坐标（已修正后的坐标）
 */
void Gesture_Update(bool pressed, int x, int y);

/**
 * @brief  注册手势回调函数
 * @param  callback: 新的回调函数
 */
void Gesture_RegisterCallback(GestureCallback callback);

#endif /* TOUCH_GESTURE_H */