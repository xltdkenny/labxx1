#include <iostream>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int ready = 0; //инициализация ready которая служит флагом для синхронизации между поставщиком и потребителем

void* producer(void*) //поток поставщика
{
    while (true) 
    {
        pthread_mutex_lock(&lock); //захват мьютекса для обеспечения безопасности доступа к общим данным из нескольких потоков. когда захватывается мьютекст, это означает,
        // что он входит в критическую секцию кода, где происходит манипуляции с общими ресурсами или данными. (ready)
        if (ready) //если готово (нужно, если какой-то другой поток занял мьютекс)
        {
            pthread_mutex_unlock(&lock); // освобождению мьютекса и продолжение кода
            continue;
        }
        ready = 1; //=1, показывая, что данные готовы к использованию
        printf("Поставщик: отправлено\n"); 
        pthread_cond_signal(&cond); //отправка сигнала, что данные готовы
        pthread_mutex_unlock(&lock); //освобождение мьютекса
        sleep(1); //задержка в секунду (для имитации процесса отправки)
    }
    return nullptr; //завершение выполнения и возврат указателя на нулевую память.
}

void* consumer(void*) //потребитель
{
    while (true) 
    {
        pthread_mutex_lock(&lock); //лок мьютекса для потока
        while (!ready) //потребитель ждет, пока данные не будут готовы
        {
            pthread_cond_wait(&cond, &lock);//ожидание сигнала от поставщика, что данные не будут готовы
        }
        ready = 0; //сброс флага, после того, как данные были обработаны
        printf("Потребитель: принято\n");
        pthread_mutex_unlock(&lock); //анлок мьютекса, чтобы другие потоки могли обрабатывать информацию
    }
    return nullptr;
}

int main() 
{
    pthread_t producerThread, consumerThread; //объявление переменных для идентификаторов потоков

    pthread_create(&producerThread, nullptr, producer, nullptr); //создание потока для функции поставщика
    pthread_create(&consumerThread, nullptr, consumer, nullptr); //создание потока для функции потребителя

    return 0; //возвращаем нулик
}
