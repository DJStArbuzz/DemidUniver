"""
Modeling the three smokers problem using threads and semaphores.
"""

import threading
import time
import random

# Semaphores for component pairs
TOBACCO_PAPER: threading.Semaphore = threading.Semaphore(0)
TOBACCO_FILTER: threading.Semaphore = threading.Semaphore(0)
PAPER_FILTER: threading.Semaphore = threading.Semaphore(0)
BARTENDER_SEM: threading.Semaphore = threading.Semaphore(1)


def bartender_process() -> None:
    """The flow of the bartender placing the ingredients on the table."""
    combinations: list[tuple[threading.Semaphore, str, str]] = [
        (TOBACCO_PAPER, "Табак", "Бумагу"),
        (TOBACCO_FILTER, "Табак", "Фильтр"),
        (PAPER_FILTER, "Бумагу", "Фильтр")
    ]
    while True:
        BARTENDER_SEM.acquire()
        comb_sem, item_1, item_2 = random.choice(combinations)
        print(f"\033[94mБармен положил: {item_1} и {item_2}\033[0m\n")
        comb_sem.release()


def smoker_process(name: str, has_item: str,
                   needed_sem: threading.Semaphore, needed_items: tuple[str]) -> None:
    """The flow of a smoker picking up components and smoking.

    Params:
        name (str): Smoker's name
        has_item (str): A component that the smoker already has
        needed_sem (Semaphore): A semaphore for the desired pair of components
        needed_items (tuple): Essential ingredients for smoking
    """
    while True:
        needed_sem.acquire()
        print(f"\033[92m{name} (имеет {has_item}) "
              f"взял {needed_items[0]} и {needed_items[1]}\033[0m\n")
        BARTENDER_SEM.release()

        # The smoking process
        print(f"\033[93m{name} начинает курить...\033[0m\n")
        smoke_time = random.randint(2, 4)
        time.sleep(smoke_time)

        print(f"\033[93m{name} закончил курить ({smoke_time} сек.)\033[0m\n")


if __name__ == "__main__":
    threads: list[threading.Thread] = [
        threading.Thread(target=bartender_process),
        threading.Thread(
            target=smoker_process,
            args=("Курильщик Т", "Табак", PAPER_FILTER, ("Бумагу", "Фильтр")),
            daemon=True
        ),
        threading.Thread(
            target=smoker_process,
            args=("Курильщик Б", "Бумагу", TOBACCO_FILTER, ("Табак", "Фильтр")),
            daemon=True
        ),
        threading.Thread(
            target=smoker_process,
            args=("Курильщик Ф", "Фильтр", TOBACCO_PAPER, ("Табак", "Бумагу")),
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
