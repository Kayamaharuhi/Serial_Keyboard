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
    cycle::dt = 0.01f;

    Timer timer;
    float elapsed_time = 0.0f;
    timer.reset();
    timer.start();


    DigitalOut led(GPIOB, GPIO_PIN_15);

    SerialKeyManager key_mgr(&hlpuart1);

    KeyListener key_a('a'); 

    KeyStickAxis stick_pitch('w', 's', 8 /* 変化速度 */, true /* 自動で0復帰 */);


    key_mgr.register_listener(&key_a);
    key_mgr.register_axis(&stick_pitch);

    while (true)
    {
        key_mgr.poll();

        if (key_a.pressed())
        {
            if (key_a.is_toggled()) {
                led.turn_on();
                printf("Key 'a' [ON]\r\n");
            } else {
                led.turn_off();
                printf("Key 'a' [OFF]\r\n");
            }
        }


        int16_t pitch = stick_pitch.get_value();
        static int16_t prev_pitch = 0;
        if (pitch != prev_pitch) {
            printf("Stick : %+4d / 256\r\n", pitch);
            prev_pitch = pitch;
        }

        key_mgr.tick(10);

        while(timer.read() - elapsed_time < cycle::dt){};
        elapsed_time = timer.read();
    }

    return 0;
}