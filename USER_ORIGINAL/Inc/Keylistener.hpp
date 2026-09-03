#ifndef KEY_LISTENER_HPP_
#define KEY_LISTENER_HPP_

#include "usart.h"
#include <cstdint>

/**
 * @brief 単一のキー押下判定を行うクラス
 */
class KeyListener {
public:
    /**
     * @param target_key 判定対象の文字 ('a', 'b' など)
     */
    explicit KeyListener(char target_key);

    /**
     * @brief シリアル受信した文字を渡して内部状態を更新
     * @param received_char 受信した文字
     */
    void update(char received_char);

    /**
     * @brief キーが押されたかを判定（1度呼ぶとフラグは消費されて false に戻ります）
     * @return true 押下された / false 押下されていない
     */
    bool pressed();

    /**
     * @brief 登録されているキー文字を取得
     */
    char get_key() const { return target_key_; }

private:
    char target_key_;
    bool is_pressed_;
};

/**
 * @brief 複数キーリスナーを束ねてシリアル受信を一括ディスパッチするマネージャ
 */
class SerialKeyManager {
public:
    explicit SerialKeyManager(UART_HandleTypeDef* huart);

    /**
     * @brief リスナーを追加登録
     */
    void register_listener(KeyListener* listener);

    /**
     * @brief 受信チェック（メインループから定期的に呼び出す）
     *        データを受信した場合、登録された全リスナーに通知します
     */
    void poll();

private:
    UART_HandleTypeDef* huart_;
    static constexpr uint8_t MAX_LISTENERS = 16;
    KeyListener* listeners_[MAX_LISTENERS];
    uint8_t listener_count_;
};

#endif // KEY_LISTENER_HPP_