#ifndef KEY_LISTENER_HPP_
#define KEY_LISTENER_HPP_

#include "usart.h"
#include <cstdint>


class KeyListener {
public:
    /**
     * @param target_key 判定対象の文字
     * @param stick_step 1周期あたりのスティック値増減量
     * @param stick_direction +1なら正, -1なら負へ加算
     */
    explicit KeyListener(char target_key, int16_t stick_step = 16, int8_t stick_direction = 1);

    void update(char received_char);
    void tick(uint32_t dt_ms = 50);

    bool pressed();

    bool is_toggled() const { return toggle_state_; }
    void set_toggle(bool state) { toggle_state_ = state; }

    int16_t get_stick_value() const { return stick_value_; }
    void reset_stick(int16_t val = 0) { stick_value_ = val; }

    uint32_t get_press_count() const { return press_count_; }
    char get_key() const { return target_key_; }

private:
    char target_key_;
    bool is_pressed_;
    bool toggle_state_;
    uint32_t press_count_;

    int16_t stick_value_;           // -256 ~ +256
    int16_t stick_step_;
    int8_t stick_direction_;
    bool is_currently_down_;
    uint32_t time_since_last_rx_ms_;
};


class KeyStickAxis {
public:
    /**
     * @param positive_key 正方向キー
     * @param negative_key 負方向キー
     * @param step_speed 変化スピード
     * @param auto_return 離したときに自動で0に戻すか
     */
    KeyStickAxis(char positive_key, char negative_key, int16_t step_speed = 16, bool auto_return = true);

    void update(char received_char);
    void tick(uint32_t dt_ms = 50);

    int16_t get_value() const { return axis_value_; }
    void reset() { axis_value_ = 0; }

private:
    char positive_key_;
    char negative_key_;
    int16_t step_speed_;
    bool auto_return_;
    int16_t axis_value_;

    bool pos_down_;
    bool neg_down_;
    uint32_t pos_rx_timer_ms_;
    uint32_t neg_rx_timer_ms_;
};
class SerialKeyManager {
public:
    explicit SerialKeyManager(UART_HandleTypeDef* huart);

    void register_listener(KeyListener* listener);
    void register_axis(KeyStickAxis* axis);

    void poll();
    void tick(uint32_t dt_ms = 50);

private:
    UART_HandleTypeDef* huart_;
    static constexpr uint8_t MAX_LISTENERS = 16;
    static constexpr uint8_t MAX_AXES = 4;

    KeyListener* listeners_[MAX_LISTENERS];
    uint8_t listener_count_;

    KeyStickAxis* axes_[MAX_AXES];
    uint8_t axis_count_;
};

#endif // KEY_LISTENER_HPP_