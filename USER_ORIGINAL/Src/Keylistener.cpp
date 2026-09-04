#include "Keylistener.hpp"

KeyListener::KeyListener(char target_key, int16_t stick_step, int8_t stick_direction)
    : target_key_(target_key),
      is_pressed_(false),
      toggle_state_(false),
      press_count_(0),
      stick_value_(0),
      stick_step_(stick_step > 0 ? stick_step : 16),
      stick_direction_(stick_direction >= 0 ? 1 : -1),
      is_currently_down_(false),
      time_since_last_rx_ms_(9999) {
}

void KeyListener::update(char received_char) {
    if (received_char == target_key_) {
        is_pressed_ = true;
        toggle_state_ = !toggle_state_; // 押すたびにON/OFF反転
        press_count_++;

        is_currently_down_ = true;
        time_since_last_rx_ms_ = 0;
    }
}

void KeyListener::tick(uint32_t dt_ms) {
    time_since_last_rx_ms_ += dt_ms;
    if (time_since_last_rx_ms_ > 150) {
        is_currently_down_ = false;
    }

    // スティック値の計算 (-256 ~ +256)
    if (is_currently_down_) {
        if (stick_direction_ > 0) {
            stick_value_ += stick_step_;
            if (stick_value_ > 256) stick_value_ = 256;
        } else {
            stick_value_ -= stick_step_;
            if (stick_value_ < -256) stick_value_ = -256;
        }
    } else {
        // 離したら自動でニュートラルに戻る
        if (stick_value_ > 0) {
            stick_value_ -= stick_step_;
            if (stick_value_ < 0) stick_value_ = 0;
        } else if (stick_value_ < 0) {
            stick_value_ += stick_step_;
            if (stick_value_ > 0) stick_value_ = 0;
        }
    }
}

bool KeyListener::pressed() {
    bool res = is_pressed_;
    is_pressed_ = false;
    return res;
}

KeyStickAxis::KeyStickAxis(char positive_key, char negative_key, int16_t step_speed, bool auto_return)
    : positive_key_(positive_key),
      negative_key_(negative_key),
      step_speed_(step_speed > 0 ? step_speed : 16),
      auto_return_(auto_return),
      axis_value_(0),
      pos_down_(false),
      neg_down_(false),
      pos_rx_timer_ms_(9999),
      neg_rx_timer_ms_(9999) {
}

void KeyStickAxis::update(char received_char) {
    if (received_char == positive_key_) {
        pos_down_ = true;
        pos_rx_timer_ms_ = 0;
    } else if (received_char == negative_key_) {
        neg_down_ = true;
        neg_rx_timer_ms_ = 0;
    }
}

void KeyStickAxis::tick(uint32_t dt_ms) {
    pos_rx_timer_ms_ += dt_ms;
    if (pos_rx_timer_ms_ > 150) pos_down_ = false;

    neg_rx_timer_ms_ += dt_ms;
    if (neg_rx_timer_ms_ > 150) neg_down_ = false;

    // 長押しによる値の加減算 
    if (pos_down_ && !neg_down_) {
        axis_value_ += step_speed_;
        if (axis_value_ > 256) axis_value_ = 256;
    } else if (neg_down_ && !pos_down_) {
        axis_value_ -= step_speed_;
        if (axis_value_ < -256) axis_value_ = -256;
    } else if (auto_return_) {
        // キーを離したら 0 に戻る
        if (axis_value_ > 0) {
            axis_value_ -= step_speed_;
            if (axis_value_ < 0) axis_value_ = 0;
        } else if (axis_value_ < 0) {
            axis_value_ += step_speed_;
            if (axis_value_ > 0) axis_value_ = 0;
        }
    }
}

SerialKeyManager::SerialKeyManager(UART_HandleTypeDef* huart)
    : huart_(huart), listener_count_(0), axis_count_(0) {
    for (uint8_t i = 0; i < MAX_LISTENERS; ++i) listeners_[i] = nullptr;
    for (uint8_t i = 0; i < MAX_AXES; ++i) axes_[i] = nullptr;
}

void SerialKeyManager::register_listener(KeyListener* listener) {
    if (listener != nullptr && listener_count_ < MAX_LISTENERS) {
        listeners_[listener_count_++] = listener;
    }
}

void SerialKeyManager::register_axis(KeyStickAxis* axis) {
    if (axis != nullptr && axis_count_ < MAX_AXES) {
        axes_[axis_count_++] = axis;
    }
}

void SerialKeyManager::poll() {
    uint8_t rx_byte = 0;
    while (HAL_UART_Receive(huart_, &rx_byte, 1, 0) == HAL_OK) {
        char c = static_cast<char>(rx_byte);
        for (uint8_t i = 0; i < listener_count_; ++i) {
            if (listeners_[i] != nullptr) listeners_[i]->update(c);
        }
        for (uint8_t i = 0; i < axis_count_; ++i) {
            if (axes_[i] != nullptr) axes_[i]->update(c);
        }
    }
}

void SerialKeyManager::tick(uint32_t dt_ms) {
    for (uint8_t i = 0; i < listener_count_; ++i) {
        if (listeners_[i] != nullptr) listeners_[i]->tick(dt_ms);
    }
    for (uint8_t i = 0; i < axis_count_; ++i) {
        if (axes_[i] != nullptr) axes_[i]->tick(dt_ms);
    }
}