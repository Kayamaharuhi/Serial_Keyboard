#include "Keylistener.hpp"

// -------------------------------------------------------------
// KeyListener 実装
// -------------------------------------------------------------
KeyListener::KeyListener(char target_key)
    : target_key_(target_key), is_pressed_(false) {
}

void KeyListener::update(char received_char) {
    if (received_char == target_key_) {
        is_pressed_ = true;
    }
}

bool KeyListener::pressed() {
    bool res = is_pressed_;
    is_pressed_ = false; // 1回読んだらリセット（エッジ検出）
    return res;
}

// -------------------------------------------------------------
// SerialKeyManager 実装
// -------------------------------------------------------------
SerialKeyManager::SerialKeyManager(UART_HandleTypeDef* huart)
    : huart_(huart), listener_count_(0) {
    for (uint8_t i = 0; i < MAX_LISTENERS; ++i) {
        listeners_[i] = nullptr;
    }
}

void SerialKeyManager::register_listener(KeyListener* listener) {
    if (listener != nullptr && listener_count_ < MAX_LISTENERS) {
        listeners_[listener_count_++] = listener;
    }
}

void SerialKeyManager::poll() {
    uint8_t rx_byte = 0;

    // タイムアウト 0ms（非ブロッキング）で受信バッファを確認
    while (HAL_UART_Receive(huart_, &rx_byte, 1, 0) == HAL_OK) {
        char c = static_cast<char>(rx_byte);
        // 登録されているすべてのキーリスナーに受信文字を通知
        for (uint8_t i = 0; i < listener_count_; ++i) {
            if (listeners_[i] != nullptr) {
                listeners_[i]->update(c);
            }
        }
    }
}