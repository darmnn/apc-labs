#include <io.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
 
char date[6];
unsigned int delayTime = 0;
const unsigned int registers[] = { 0x00, 0x02, 0x04, 0x07, 0x08, 0x09 };
 
void interrupt(*lastTime)(...); 
void interrupt(*lastAlarm) (...); 
void resetAlarm();
int getValueBetweenAsBCD(int min, int max);
void enterTime();
int BSDtoDecimal(int BCD);

void interrupt newTime(...) // новый обработчик прерываний часов
{
    delayTime++;
    outp(0x70, 0xC);
    inp(0x71);

    outp(0x20, 0x20);
    outp(0xA0, 0x20);
}

void interrupt newAlarm(...) // новый обработчик прерываний будильника
{
    puts("ALARM IS RINGING........");
    lastAlarm();
    resetAlarm();
}
 
void getTime() 
{
    for (int i = 0; i < 6; i++)
    {
        outp(0x70, registers[i]);
        date[i] = inp(0x71);
    }

    int decTime[6];
    for (int j = 0; j < 6; j++)
        decTime[j] = BSDtoDecimal(date[j]);
   

    printf("%2d:%2d:%2d\n", decTime[2], decTime[1], decTime[0]); // час, минута, секунда
    printf("%2d %s 20%2d\n", decTime[3], decTime[4], decTime[5]); // день, месяц, год
}

void setTime()
{
    enterTime();
    disable();

    do
    {
        outp(0x70, 0xA); // выбор регистра А
    } while (inp(0x71) & 0x80); // Если 7й бит установлен в 0, то это значит, что данные готовы для чтения

    //Остановка счётчика RTC
    outp(0x70, 0xB);
    outp(0x71, inp(0x71) | 0x80); // Если 7й бит установлен в 1, то счётчик остановлен

    for (int i = 0; i < 6; i++)
    {
        outp(0x70, registers[i]);
        outp(0x71, date[i]);
    }

    outp(0x70, 0xB);
    outp(0x71, inp(0x71) & 0x7F); // Если 7й бит установлен в 0, то счётчик запущен

    enable();
    system("cls");
}

void delay(unsigned int ms)
{
    disable();
    lastTime = getvect(0x70);
    setvect(0x70, newTime);
    enable();

    outp(0xA1, inp(0xA1) & 0xFE); 

    outp(0x70, 0xB); 
    outp(0x71, inp(0x71) | 0x40); 

    delayTime = 0;
    while (delayTime <= ms);
    puts("delay is over");
    setvect(0x70, lastTime);
}

void setAlarm()
{
    enterTime();
    disable();

    do {
        outp(0x70, 0xA);
    } while (inp(0x71) & 0x80); // Если 7й бит установлен в 0, то это значит, что данные готовы для чтения

    // установка часов в регистр будильника
    outp(0x70, 0x05);
    outp(0x71, date[2]);

    // установка минут в регистр будильника
    outp(0x70, 0x03);
    outp(0x71, date[1]);

    // установка секунд в регистр будильника
    outp(0x70, 0x01);
    outp(0x71, date[0]);

    outp(0x70, 0xB); // выбор регистра B
    outp(0x71, (inp(0x71) | 0x20)); //Если 5й бит установлен в 1, то разрешаем прерывания будильника

    // переопределение прерывания будильника
    lastAlarm = getvect(0x4A);
    setvect(0x4A, newAlarm);
    outp(0xA1, (inp(0xA0) & 0xFE)); // Если 0й бит в 0, то разрешаем прерывания от RTC

    enable();
    puts("alarm is enabled");
}

void resetAlarm()
{
    if (lastAlarm == NULL)
        return;

    disable();

    // возврат старого прерывания
    setvect(0x4A, lastAlarm);
    outp(0xA1, (inp(0xA0) | 0x01));

    do 
    {
        outp(0x70, 0xA);
    } while (inp(0x71) & 0x80); // Если 7й бит установлен в 0, то это значит, что данные готовы для чтения

    outp(0x70, 0x05); // 0x05 - часы
    outp(0x71, 0x00);

    outp(0x70, 0x03); // 0x03 - минуты
    outp(0x71, 0x00);

    outp(0x70, 0x01); // 0x01 - секунды
    outp(0x71, 0x00);

    outp(0x70, 0xB);
    outp(0x71, (inp(0x71) & 0xDF)); //Если 5й бит установлен в 0, то запрещаем прерывания будильника

    enable();
}

int getValueBetweenAsBCD(int min, int max)
{
    int value;
    do
    {
        rewind(stdin);
        scanf("%d", &value);
    } while (value > max || value < min);

    return value / 10 * 16 + value % 10;
}

void enterTime()
{
    printf("\nset year: ");
    date[5] = getValueBetweenAsBCD(0, 2100);
    printf("\nset month: ");
    date[4] = getValueBetweenAsBCD(1, 12);
    printf("\nset day: ");
    date[3] = getValueBetweenAsBCD(1, 365);
    printf("\nset hours: ");
    date[2] = getValueBetweenAsBCD(0, 23);
    printf("\nset minutes: ");
    date[1] = getValueBetweenAsBCD(0, 59);
    printf("\nset seconds: ");
    date[0] = getValueBetweenAsBCD(0, 59);
}

int BSDtoDecimal(int BCD)
{
    return ((BCD / 16 * 10) + (BCD % 16));
}

 
int main()
{
    while (1)
    {
        printf("1 - show time\n2 - set delay\n3 - create alarm\n4 - set time\n0 - exit\n");

        switch (getch())
        {
            case '1':
            {
                system("cls");
                getTime();
                break;
            }
 
            case '2':
            {
                system("cls");
                printf("set time for delay(in ms): ");
                delay(BSDtoDecimal(getValueBetweenAsBCD(0, 10000)));
                break;
            }

            case '3':
            {
                system("cls");
                setAlarm();
                break;
            }
            case '4':
            {
                system("cls");
                setTime();
                break;
            }
            case '0':
            {
                return 0;
            }
            default:
            {
                system("cls");
                break;
            }
        }
    }
}