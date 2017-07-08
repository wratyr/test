/*
 
 */


#ifndef F_CPU
#define F_CPU 2000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>


#ifndef EEMEM
#define EEMEM__attribute__ ((section (".eeprom")))
#endif

//макросы для для удобства написания дальнейшего кода
//#define LED1_ON PORTD |= (1 << PD0)
//#define LED1_OFF PORTD &= ~(1 << PD0)
//#define LED2_ON PORTD |= (1 << PD1)
//#define LED2_OFF PORTD &= ~(1 << PD1)

//uint8_t EEMEM count;

/*unsigned char seg[] = {
    0b00000000, //
    0b00000001, //1
    0b00000010, //2
    0b00000100, //3
    0b00001000 //4
}; */

uint16_t EEPCount EEMEM = 0;
uint16_t EEPOil EEMEM = 0;
unsigned char test = 0;
unsigned char test2 = 0;

void digit_print(int dig) {
    unsigned char local_count = 0;
    unsigned char seg[] = {0, 0, 0, 0};
    unsigned char seg_temp = 1;
    unsigned char num[] = {
        0b01000000, //0
        0b01111001, //1
        0b00100100, //2
        0b00110000, //3
        0b00011001, //4
        0b00010010, //5
        0b00000010, //6
        0b01111000, //7
        0b00000000, //8
        0b00010000, //9
        0b01111111 //пустота
    };

    if (dig < 10) // Для заполнения нулями левых разрядов
    {
        seg[0] = 0;
        seg[1] = 0;
        seg[2] = 0;
    }

    if (dig < 100) // Для заполнения нулями левых разрядов
    {
        seg[0] = 0;
        seg[1] = 0;
    }

    if (dig < 1000) // Для заполнения нулями левых разрядов
    {
        seg[0] = 0;
    }

    while (dig > 999) // Получение количества тысяч
    {
        seg[0]++;
        dig -= 1000;
    }

    while (dig > 99) // Получение количества сотен
    {
        seg[1]++;
        dig -= 100;
    }

    while (dig > 9) // Получение числа десятков
    {
        seg[2]++;
        dig -= 10;
    }

    seg[3] = dig;

    while (seg_temp <= 0x10) // Крутимся пока не заполним все 4 разряда
    {
        PORTC = seg_temp; // Выбираем разряд
        PORTD = num[seg[local_count]]; // Выводим цифру
        seg_temp = seg_temp << 1; // Сдвигаемся на следующий разряд
        local_count++; // Увеличиваем индекс следующей цифры
        _delay_ms(1); // Задержка 1 мс
    }

}

int main(void) {

    // -- настроим пины порта --
    //DDRD |= (1 << PD0); // пин PD0 порта DDRD на вывод led1
    //DDRD |= (1 << PD1); // пин PD1 порта DDRD на вывод led2
    DDRD = 0b01111111; //настраиваем разряды 1-7 на выход, 8 на вход (цифры)
    PORTD = 0b11111111; //настраиваем разряды 1-7 в лог. 1 (5в), 8 в лог.0(gnd)
    DDRB = 0b00000000; //весь порт на вход
    PORTB = 0b11111111; //логическая 1 на весь порт.
    DDRC = 0b00001111; // 4 разряда на выход (сегменты)
    PORTC = 0b00000000; //при старте везде gnd
    //DDRD &= ~(1 << PD2); // пин PD2 порта DDRD на ввод button1
    //можно выставить пины на асме :
    //PORTD |= (1 << PD2); //включение встроенного ограничительного резистора для порта

    uint16_t count;
    count = eeprom_read_word(&EEPCount);
    //Проверка при запуске

    while (1) {
        if ((PINB & (1 << PB0)) == 0 && test == 1) { //если кнопка нажата будет выполнено
            if (count < 9999) {
                count++;
                test = 0;
                // eeprom_update_word(&EEPCount, count);
            } else {
                count = 0;
                test = 0;
            }

        } else if ((PINB & (1 << PB0)) == 1) {
            test = 1;
        }


        if ((PINB & (1 << PB1)) == 0) { //reset
            count = 0;
            //eeprom_update_word(&EEPCount, count);
        }

        if ((PINB & (1 << PB7)) == 0) { //отслеживание пропадания питания
            eeprom_update_word(&EEPCount, count);//сохраняем текущее значение переменной в энергонезависимую память
        }

        if ((PINB & (1 << PB2)) == 0) {
            unsigned char menu = 1;
            test2 = 0;

        } else if ((PINB & (1 << PB2)) == 1) {
            test2 = 1;
        }

        digit_print(count);

    } // -- основной цикл программы --    
}