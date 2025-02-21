def first_task():
    while True:
        first = input("Write first number: ")
        second = input("Write second number: ")

        if first.isdigit():
            first = int(first)
        else:
            print("Error. Not number")
            break

        if second.isdigit():
            second = int(second)
        else:
            print("Error. Not number")
            break

        print("Sum: " + str(first + second))

        flag = int(input("Continue? 1 - Yes, other - No: "))
        if flag != 1:
            break


def second_task():
    for size in range(3, 10, 2):
        res = "*" * size + "\n"

        numLoop = (size - 3) // 2

        for i in range(numLoop):
            res += (i + 1) * " " + "*" + (size - 2 - 2 * (i + 1)) * " " + "*" + (i + 1) * " " + "\n"

        res += " " * (size // 2) + "*" + " " * (size // 2) + "\n"
        for i in range(numLoop - 1, -1, -1):
            res += (i + 1) * " " + "*" + (size - 2 - 2 * (i + 1)) * " " + "*" + (i + 1) * " " + "\n"

        res += "*" * size + "\n"
        print(res)


def third_task():
    for ind in range(12, 20, 3):
        res = ""
        flag = 0
        curr = 1

        while curr <= ind:
            if flag % 2 == 1:
                for indOdd in range(5):
                    if indOdd % 2 == 0:
                        res += str(curr) + " "
                        curr += 1
                    else:
                        res += "* "
                res += "\n"
            else:
                for indEven in range(5):
                    if indEven % 2 == 1:
                        res += str(curr) + " "
                        curr += 1
                    else:
                        res += "* "
                res += "\n"
            flag += 1

        print(res)


while True:
    num = input()

    if num == "1":
        first_task()
    elif num == "2":
        second_task()
    else:
        third_task()
