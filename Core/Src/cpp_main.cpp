#include "main.h"
#include "usart.h"
#include "Keylistener.hpp"
#include <cstdio>
#include "DigitalOut.hpp" 
#include "Timer.hpp"
#include "Config.hpp"

int cpp_main()
{
    main_timer::activate();
    cycle::dt = 0.05f;

    Timer timer;
    float elapsed_time = 0.0f;
    timer.reset();
    timer.start();

    DigitalOut led(GPIOB, GPIO_PIN_15);

    // 1. UART受信マネージャを作成
    SerialKeyManager key_mgr(&hlpuart1);

    // 2. 監視したい各キーリスナーを作成して登録
    KeyListener key('a');
    KeyListener key1('s');
    key_mgr.register_listener(&key);
    key_mgr.register_listener(&key1);

    while (true)
    {
        // シリアル受信をまとめてチェック＆各キーへ振り分け
        key_mgr.poll();

        if (key.pressed())
        {
            led.turn_on();
            printf("on\n");
        }
        else if (key1.pressed())
        {
            led.turn_off();
            printf("off\n");
        }

        while(timer.read() - elapsed_time < cycle::dt){};
        elapsed_time = timer.read();
    }

    return 0;
}