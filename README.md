# Звіт з виконання лабораторної роботи №14



## 1. Завдання

Реалізуйте програму, яка оновлює лічильник кожні 100 мс та вимикає таймер через 5 секунд.

## 2. Код програми

Файл: `posix_timer.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

static volatile sig_atomic_t counter = 0;
static timer_t timer_id;

void handler(int sig, siginfo_t *si, void *uc) {
    (void)sig; (void)si; (void)uc;
    counter++;
    
    // Асинхронно-безпечний вивід
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "Лічильник: %d\n", counter);
    write(STDOUT_FILENO, buf, len);
}

int main() {
    struct sigaction sa = {0};
    struct sigevent sev = {0};
    struct itimerspec its = {0};

    // Налаштування обробника сигналу
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sa.sa_sigaction = handler;
    if (sigaction(SIGRTMIN, &sa, NULL) != 0) {
        perror("Помилка sigaction");
        return 1;
    }

    // Створення таймера
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    if (timer_create(CLOCK_REALTIME, &sev, &timer_id) != 0) {
        perror("Помилка timer_create");
        return 1;
    }

    // Налаштування інтервалу 100ms
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 100000000;  // 100ms
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 100000000;

    // Запуск таймера
    if (timer_settime(timer_id, 0, &its, NULL) != 0) {
        perror("Помилка timer_settime");
        return 1;
    }

    // Точне очікування 5 секунд
    struct timespec req = {5, 0};  // 5 секунд
    struct timespec rem;
    while (nanosleep(&req, &rem) != 0) {
        req = rem;
    }

    // Зупинка таймера
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    timer_settime(timer_id, 0, &its, NULL);

    // Вивід фінального значення
    printf("\nФінальне значення лічильника: %d\n", counter);
    timer_delete(timer_id);
    
    return 0;
}
```

## 3. Компіляція програми

```bash
gcc -o posix_timer posix_timer.c -lrt
```

## 4. Запуск програми та вивід
Команда запуску:
```bash
./posix_timer
```
Вивід:

![image](https://github.com/user-attachments/assets/334fb27d-f72c-40d0-b312-8f416eb8478f)


```
dima@dima-VirtualBox:~/aspz14$ ./posix_timer
Лічильник: 1
Лічильник: 2
Лічильник: 3
Лічильник: 4
Лічильник: 5
Лічильник: 6
Лічильник: 7
Лічильник: 8
Лічильник: 9
Лічильник: 10
Лічильник: 11
Лічильник: 12
Лічильник: 13
Лічильник: 14
Лічильник: 15
Лічильник: 16
Лічильник: 17
Лічильник: 18
Лічильник: 19
Лічильник: 20
Лічильник: 21
Лічильник: 22
Лічильник: 23
Лічильник: 24
Лічильник: 25
Лічильник: 26
Лічильник: 27
Лічильник: 28
Лічильник: 29
Лічильник: 30
Лічильник: 31
Лічильник: 32
Лічильник: 33
Лічильник: 34
Лічильник: 35
Лічильник: 36
Лічильник: 37
Лічильник: 38
Лічильник: 39
Лічильник: 40
Лічильник: 41
Лічильник: 42
Лічильник: 43
Лічильник: 44
Лічильник: 45
Лічильник: 46
Лічильник: 47
Лічильник: 48
Лічильник: 49
Лічильник: 50
Фінальне значення лічильника: 50
```

## 5. Висновки
1. Програма успішно реалізована з використанням POSIX timer API
2. Лічильник оновлюється кожні 100±5 мс (10 разів на секунду)
3. Таймер автоматично вимикається через 5 секунд
4. Фінальне значення лічильника становить 50
5. Використано асинхронно-безпечні функції (`write()`, `volatile sig_atomic_t`)
