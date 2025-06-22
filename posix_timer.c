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
