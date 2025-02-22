"""
 program
"""


def my_first_function():
    """
    info

    :return:
    """
    print("Hello, world")


def my_add_function(a, b):
    """
    d
    :param a:
    :param b:
    :return: a + b
    """
    return a + b


def my_and_add_function(a, b, c=1):
    """
    a, b  - *args
    c - ** kwargs
    :param c: именованные
    :param a: позиционные
    :param b: позиционные
    :return:
    """
    return a + b, a - b + c


def tmp(*args, **kwargs):
    """
    :param args: <- позиционные
    :param kwargs: <- именованные
    :return:
    """
    print(args)
    print(kwargs)


for i in "Hello, world":
    print(i)  # comm

my_first_function()
print(my_add_function(1, 2))
print(my_and_add_function(1, 2, 3))
tmp(1, 2, 3, c=4, d=5)

"""
списки 
1) a = [1, 2, 3]
2) a = list() - пустой
В списке хранятся ссылки на элементы
При этом:
a = [1, 2, 3], b = [3, 4, a]
Ссылка на 3 в a равна ссылке из b
Если a.append(5), то b = [3, 4, [1, 2, 3, 5]]
"""
# import copy

a = [1, 2, 3, 1]
"""
b = [3, 4, a]
c = copy.deepcopy(b)  # копирование, deepcopy - глубокое копирование
a.append(5)  # добавляет в конец списка новый элемент, нихрена не возвращает
b.append(11)
print(c)
print(b)
"""
a.remove(1)  # удаление первого вхождения числа
print(a)

a.insert(0, 5)  # на позицию под номером 0 поставили 5
print(a)

a[1] = 0  # замена значения
print(a)
print(a.index(0))  # выводится индекс элемента. Нет элемента - привет ошибке
if 89 in a:  # проверка на наличие элемента
    print(a.index(89))

a.sort()  # сортировка
a.reverse()  # разворот
print(a)

b = a.pop()  # удаление последнего элемента по умолчанию (или по индексу) + функция возвращает значение (которое
# удалили).
print(a)
print(b)

a = [1, 2, 3, 1]
b = [5, 6, 7, 8]
a.extend(b)  # слияние
print(a)

a = [1, 2, 3, 4, 5, 6, 7, 8]
b = a[1:8:2]  # срез
print(b)

# еще вид функций (однострочные)
j = lambda x: x ** 2
print(j(2))

a = [1, 2, 3]
b = list(map(lambda x: x ** 2, a))
print(b)

n = int(input())
a = []
for i in range(n):
    a.append(i)
print(a)

a = [i for i in range(n)]
print(a)

# фильтр
n = int(input())
a = [i for i in range(10)]
b = list(filter(lambda x: x % 2 == 0, a))  # только когда четные, когда выполняется условие
print(b)

# кортеж
a = (1, 2, 3)  # изменить нельзя, a[i] - запрещено
b = list(a)


def change_list(a=[]):
    a.append(2)
    print(a)


change_list()  # [2]
change_list()  # [2,2]
change_list()  # [2,2,2]
