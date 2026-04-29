import numpy as np
import matplotlib.pyplot as plt
import sympy as sp

# 1. Определение системы
def P(x, y):
    """dx/dt = (x+y)^2 - 1"""
    return (x + y)**2 - 1.0

def Q(x, y):
    """dy/dt = -y^2 - x + 1"""
    return -y*y - x + 1.0

def system(t, state):
    x, y = state
    return np.array([P(x, y), Q(x, y)])

# 2. Нахождение всех особых точек (аналитически через sympy)
def find_fixed_points():
    x, y = sp.symbols('x y')
    eq1 = (x + y)**2 - 1
    eq2 = -y**2 - x + 1
    solutions = sp.solve([eq1, eq2], [x, y], real=True)
    return [(float(s[0]), float(s[1])) for s in solutions]

# 3. Анализ типа особой точки (линеаризация)
def analyze_fixed_point(x0, y0):
    a = 2.0 * (x0 + y0)      # dP/dx
    b = 2.0 * (x0 + y0)      # dP/dy
    c = -1.0                 # dQ/dx
    d = -2.0 * y0            # dQ/dy

    trace = a + d
    det = a*d - b*c
    disc = trace*trace - 4*det

    if disc > 1e-12:
        l1 = (trace + np.sqrt(disc)) / 2.0
        l2 = (trace - np.sqrt(disc)) / 2.0
        if l1 * l2 < 0:
            return "седло (неустойчиво)"
        elif l1 > 0 and l2 > 0:
            return "неустойчивый узел"
        elif l1 < 0 and l2 < 0:
            return "устойчивый узел"
        else:
            return "узел (граничный случай)"
    elif abs(disc) < 1e-12:
        l = trace / 2.0
        if l > 0:
            return "неустойчивый вырожденный узел"
        elif l < 0:
            return "устойчивый вырожденный узел"
        else:
            return "центр / седло-узел?"
    else:
        real = trace / 2.0
        imag = np.sqrt(-disc) / 2.0
        if abs(real) < 1e-12:
            return "центр"
        elif real > 0:
            return "неустойчивый фокус"
        else:
            return "устойчивый фокус"

# 4. Численное интегрирование (Рунге-Кутта 4-го порядка)
def rk4_step(f, t, y, h):
    y = np.asarray(y)
    k1 = np.asarray(f(t, y))
    k2 = np.asarray(f(t + h/2, y + h/2 * k1))
    k3 = np.asarray(f(t + h/2, y + h/2 * k2))
    k4 = np.asarray(f(t + h, y + h * k3))
    return y + h/6 * (k1 + 2*k2 + 2*k3 + k4)

def integrate_trajectory(x0, y0, t_max, h=0.05):
    t = 0.0
    x, y = x0, y0
    ts = [t]
    xs = [x]
    ys = [y]
    while t < t_max:
        state = np.array([x, y])
        new_state = rk4_step(system, t, state, h)
        x, y = new_state
        t += h
        if abs(x) > 100 or abs(y) > 100:
            break
        ts.append(t)
        xs.append(x)
        ys.append(y)
    return np.array(ts), np.array(xs), np.array(ys)

# 5. Построение графиков для одной особой точки (4 траектории)
def plot_phase_portrait(point, eps=0.1, t_max=6.0, h=0.05):
    x0, y0 = point
    initials = [
        (x0 + eps, y0, "вправо"),
        (x0 - eps, y0, "влево"),
        (x0, y0 + eps, "вверх"),
        (x0, y0 - eps, "вниз")
    ]
    colors = ['red', 'blue', 'green', 'orange']

    plt.figure(figsize=(8, 6))
    for (ix, iy, label), color in zip(initials, colors):
        t, x, y = integrate_trajectory(ix, iy, t_max, h)
        plt.plot(x, y, lw=2, color=color, label=f'старт {label}')
        plt.plot(x[0], y[0], 'o', color=color, markersize=6)   # начальная точка
        plt.plot(x[-1], y[-1], 's', color=color, markersize=6) # конечная точка

    plt.plot(x0, y0, 'k*', markersize=12, label='Особая точка')
    plt.title(f"Особая точка ({x0}, {y0}) : {analyze_fixed_point(x0, y0)}")
    plt.xlabel('x')
    plt.ylabel('y')
    plt.grid(True)
    plt.legend(loc='best')
    plt.axis('equal')
    plt.tight_layout()

    safe_x = str(x0).replace('.', '_').replace('-', 'minus_')
    safe_y = str(y0).replace('.', '_').replace('-', 'minus_')
    filename = f"phase_{safe_x}_{safe_y}.png"
    plt.savefig(filename, dpi=150)
    print(f"  График сохранён: {filename}")
    plt.show()

# 6. Основная программа
def main():
    print("="*60)
    print("Лабораторная работа №5. Вариант 12")
    print("Система:")
    print("  dx/dt = (x+y)^2 - 1")
    print("  dy/dt = -y^2 - x + 1")
    print("="*60 + "\n")

    points = find_fixed_points()
    print("Особые точки и их тип (аналитически):")
    for (x, y) in points:
        typ = analyze_fixed_point(x, y)
        print(f"  ({x}, {y}) : {typ}")
    print()

    for (x, y) in points:
        print(f"\n--- Особая точка ({x}, {y}) ---")
        print(f"Ожидаемый тип: {analyze_fixed_point(x, y)}")
        plot_phase_portrait((x, y), eps=0.1, t_max=6.0, h=0.05)
        print("  Сгенерировано 4 траектории для этой точки.")

    print("\n" + "="*60)
    print("Численное интегрирование завершено. Все графики сохранены в PNG.")
    print("Аналитические выводы и численные результаты должны совпадать.")
    print("="*60)

if __name__ == "__main__":
    main()
