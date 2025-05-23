"""
Моделирование проблемы трех курильщиков с использованием потоков и семафоров.
"""

import threading
import time
import random

# Семафоры для пар компонентов
TOBACCO_PAPER = threading.Semaphore(0)
TOBACCO_FILTER = threading.Semaphore(0)
PAPER_FILTER = threading.Semaphore(0)
BARTENDER_SEM = threading.Semaphore(1)

# Мьютекс для стола и вывода
TABLE_MUTEX = threading.Lock()
PRINT_MUTEX = threading.Lock()


def bartender_process():
    """Поток бармена, размещающего компоненты на столе."""
    combinations = [
        (TOBACCO_PAPER, "Табак", "Бумагу"),
        (TOBACCO_FILTER, "Табак", "Фильтр"),
        (PAPER_FILTER, "Бумагу", "Фильтр")
    ]
    while True:
        BARTENDER_SEM.acquire()
        with TABLE_MUTEX:
            comb_sem, item1, item2 = random.choice(combinations)
            with PRINT_MUTEX:
                print(f"\n\033[94mБармен положил: {item1} и {item2}\033[0m")
            comb_sem.release()


def smoker_process(name, has_item, needed_sem, needed_items):
    """Поток курильщика, забирающего компоненты и курящего.

    Params:
        name (str): Имя курильщика
        has_item (str): Компонент, который уже есть у курильщика
        needed_sem (Semaphore): Семафор для нужной пары компонентов
        needed_items (tuple): Необходимые компоненты для курения
    """
    while True:
        needed_sem.acquire()
        with TABLE_MUTEX:
            print(f"\n\033[92m{name} (имеет {has_item}) "
                  f"взял {needed_items[0]} и {needed_items[1]}\033[0m")
            BARTENDER_SEM.release()

        # Процесс курения
        print(f"\033[93m{name} начинает курить...\033[0m")
        smoke_time = random.randint(2, 5)
        time.sleep(smoke_time)

        print(f"\033[93m{name} закончил курить ({smoke_time} сек.)\033[0m")


if __name__ == "__main__":
    threads = [
        threading.Thread(target=bartender_process, daemon=True),
        threading.Thread(
            target=smoker_process,
            args=("Курильщик 1", "Табак", PAPER_FILTER, ("Бумагу", "Фильтр")),
            daemon=True
        ),
        threading.Thread(
            target=smoker_process,
            args=("Курильщик 2", "Бумагу", TOBACCO_FILTER, ("Табак", "Фильтр")),
            daemon=True
        ),
        threading.Thread(
            target=smoker_process,
            args=("Курильщик 3", "Фильтр", TOBACCO_PAPER, ("Табак", "Бумагу")),
            daemon=True
        )
    ]

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    try:
        while True:
            time.sleep(3600)
    except KeyboardInterrupt:
        print("\nПрограмма остановлена")
