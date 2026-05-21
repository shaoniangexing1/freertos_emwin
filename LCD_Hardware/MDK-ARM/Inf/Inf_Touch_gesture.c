#include "Inf_touch_gesture.h"
#include <stdlib.h>

/*********************************************************************
 *
 *  手势阈值配置（根据你的屏幕和需求调整）
 *
 *********************************************************************/
#define SHORT_PRESS_MAX_MS    400    // 短按最大持续时间(ms)，超过则为长按
#define LONG_PRESS_MIN_MS     800    // 长按最小持续时间(ms)
#define SWIPE_MIN_DISTANCE    10     // 滑动最小距离(像素)，小于则认为是点击
#define DIRECTION_THRESHOLD   2   // 方向判断阈值，X/Y绝对值比大于此值认为是横向滑动

/*********************************************************************
 *
 *  内部状态结构体
 *
 *********************************************************************/
typedef struct {
    bool          pressed;          // 当前触摸状态
    uint32_t      start_time;       // 触摸按下的时间(ms)
    int           start_x;          // 触摸起始X坐标
    int           start_y;          // 触摸起始Y坐标
    int           current_x;        // 当前触摸X坐标
    int           current_y;        // 当前触摸Y坐标
    bool          long_press_triggered; // 长按是否已经触发
    GestureCallback callback;       // 手势回调函数
} GestureState;

static GestureState g_gesture_state = {0};

/*********************************************************************
 *
 *  公共函数实现
 *
 *********************************************************************/
void Gesture_Init(GestureCallback callback)
{
    FT6236_Init();
    g_gesture_state.pressed = false;
    g_gesture_state.start_time = 0;
    g_gesture_state.start_x = 0;
    g_gesture_state.start_y = 0;
    g_gesture_state.current_x = 0;
    g_gesture_state.current_y = 0;
    g_gesture_state.long_press_triggered = false;
    g_gesture_state.callback = callback;
}

void Gesture_RegisterCallback(GestureCallback callback)
{
    g_gesture_state.callback = callback;
}

void Gesture_Update(bool pressed, int x, int y)
{
    uint32_t current_time = HAL_GetTick();
    uint32_t duration = current_time - g_gesture_state.start_time;
    int delta_x = x - g_gesture_state.start_x;
    int delta_y = y - g_gesture_state.start_y;

    // --------------------------
    // 1. 触摸按下事件
    // --------------------------
    if (pressed && !g_gesture_state.pressed) {
        g_gesture_state.pressed = true;
        g_gesture_state.start_time = current_time;
        g_gesture_state.start_x = x;
        g_gesture_state.start_y = y;
        g_gesture_state.current_x = x;
        g_gesture_state.current_y = y;
        g_gesture_state.long_press_triggered = false;
        return;
    }

    // --------------------------
    // 2. 触摸保持事件
    // --------------------------
    if (pressed && g_gesture_state.pressed) {
        g_gesture_state.current_x = x;
        g_gesture_state.current_y = y;

        // 检测长按（实时触发，无需等待释放）
        if (!g_gesture_state.long_press_triggered && duration >= LONG_PRESS_MIN_MS) {
            // 只有移动距离小于滑动阈值时才认为是长按
            if (abs(delta_x) < SWIPE_MIN_DISTANCE && abs(delta_y) < SWIPE_MIN_DISTANCE) {
                g_gesture_state.long_press_triggered = true;
                if (g_gesture_state.callback != NULL) {
                    g_gesture_state.callback(GESTURE_LONG_PRESS, x, y);
                }
            }
        }
        return;
    }

    // --------------------------
    // 3. 触摸释放事件（核心手势判断）
    // --------------------------
    if (!pressed && g_gesture_state.pressed) {
        g_gesture_state.pressed = false;

        // 如果已经触发过长按，不再判断其他手势
        if (g_gesture_state.long_press_triggered) {
            return;
        }

        // 计算总移动距离
        int abs_delta_x = (delta_x >= 0) ? delta_x : -delta_x;
        int abs_delta_y = (delta_y >= 0) ? delta_y : -delta_y;

        // 情况1：移动距离小于阈值，判断为点击
        if (abs_delta_x < SWIPE_MIN_DISTANCE && abs_delta_y < SWIPE_MIN_DISTANCE) {
            if (duration < SHORT_PRESS_MAX_MS) {
                if (g_gesture_state.callback != NULL) {
                    g_gesture_state.callback(GESTURE_SHORT_PRESS, g_gesture_state.start_x, g_gesture_state.start_y);
                }
            }
            return;
        }

        // 情况2：移动距离大于阈值，判断为滑动
       // float ratio = (float)abs_delta_x / (float)abs_delta_y;

        // 横向滑动（左右滑）
        if (abs_delta_x > abs_delta_y*DIRECTION_THRESHOLD) {
            if (delta_x < 0) {
                // 向左滑动：X坐标减小
                if (g_gesture_state.callback != NULL) {
                    g_gesture_state.callback(GESTURE_SWIPE_LEFT, g_gesture_state.start_x, g_gesture_state.start_y);
                }
            } else {
                // 向右滑动：X坐标增大
                 if (g_gesture_state.callback != NULL) {
                    g_gesture_state.callback(GESTURE_SWIPE_RIGHT, g_gesture_state.start_x, g_gesture_state.start_y);
                }
            }
        }
        // 纵向滑动（上下滑）
        else if (abs_delta_y >abs_delta_x*DIRECTION_THRESHOLD) {
            if (delta_y < 0) {
                // 向上滑动：Y坐标减小（屏幕坐标系原点在左上角）
                if (g_gesture_state.callback != NULL) {
                    g_gesture_state.callback(GESTURE_SWIPE_UP, g_gesture_state.start_x, g_gesture_state.start_y);
                }
            } else {
                // 向下滑动：Y坐标增大
                if (g_gesture_state.callback != NULL) {
                    g_gesture_state.callback(GESTURE_SWIPE_DOWN, g_gesture_state.start_x, g_gesture_state.start_y);
                }
            }
        }
        // 斜向滑动：不触发任何手势
        else {
            // 可以在这里添加斜向滑动的处理
        }

        return;
    }
}