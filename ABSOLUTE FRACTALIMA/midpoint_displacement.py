import random
import matplotlib.pyplot as plt
import math


def show():
    """Отображение графика."""
    plt.show()


class TerrainGenerator:
    def __init__(self, start: tuple = (0, 0), end: tuple = (50, 0),
                 iterations: int = 10, roughness: float = 1.5):
        self.start = (start[0], random.uniform(-10, 10))
        self.end = (end[0], random.uniform(-10, 10))
        self.iterations = iterations
        self.roughness = roughness
        self.line_color = 'blue'
        self.fill_color = 'lightblue'

        self.current_terrain = self.midpoint_displacement(
            self.start, self.end, self.iterations, self.roughness
        )

        self.fig, self.ax = plt.subplots(figsize=(12, 6))
        self.line, = self.ax.plot([], [], color=self.line_color, linewidth=0.8)
        self.fill = None
        self.update_title()

        self.fig.canvas.mpl_connect('key_press_event', self.handle_key)
        self.update_plot()
        self.ax.set_xlim(self.start[0], self.end[0])

    def update_title(self):
        """Обновление заголовка с текущими параметрами"""
        self.ax.set_title(
            f"Midpoint Displacement\n"
            f"Шероховатость: {self.roughness:.1f} (Up/Down)")

    @staticmethod
    def midpoint_displacement(begin: tuple, end: tuple,
                              iterations: int, roughness: float) -> list:
        """Генерация ландшафта с использованием алгоритма midpoint displacement."""
        points = [begin, end]

        for _ in range(iterations):
            new_points = []
            for i in range(len(points) - 1):
                # Вычисление средней точки
                new_x = (points[i][0] + points[i + 1][0]) / 2
                new_y = (points[i][1] + points[i + 1][1]) / 2

                # Вычисление диапазона смещения
                segment_length = math.sqrt(
                    (points[i][0] - points[i + 1][0]) ** 2 +
                    (points[i][1] - points[i + 1][1]) ** 2
                )
                displacement_range = segment_length / (2 ** roughness)

                new_point = (new_x, new_y + random.uniform(-displacement_range, displacement_range))

                new_points.append(points[i])
                new_points.append(new_point)

            new_points.append(points[-1])
            points = new_points
        return points

    def update_plot(self):
        """Обновление графика с текущим состоянием ландшафта."""
        x = [p[0] for p in self.current_terrain]
        y = [p[1] for p in self.current_terrain]
        min_y = min(y) if y else 0

        self.line.set_data(x, y)

        # Удаление предыдущей заливки и создание новой
        if self.fill:
            self.fill.remove()
        self.fill = self.ax.fill_between(
            x, y, min_y - 0.1, color=self.fill_color, alpha=0.3
        )

        self.fig.canvas.draw()

    def handle_key(self, event):
        """Обработка нажатий клавиш для управления ландшафтом."""
        x_min, x_max = self.ax.get_xlim()
        view_width = x_max - x_min
        delta = view_width * 0.05

        if event.key == 'right':
            # Расширение вправо
            right_end = self.current_terrain[-1]
            new_end = (
                right_end[0] + delta,
                right_end[1] + random.uniform(-delta, delta)
            )
            new_segment = self.midpoint_displacement(
                right_end, new_end, self.iterations, self.roughness
            )[1:]
            self.current_terrain += new_segment
            self.ax.set_xlim(x_min + delta, x_max + delta)

        elif event.key == 'up':
            # Увеличение шероховатости
            self.roughness = min(self.roughness + 0.1, 2.5)
            self.update_title()

        elif event.key == 'down':
            # Уменьшение шероховатости
            self.roughness = max(self.roughness - 0.1, 0.1)
            self.update_title()

        self.update_plot()


if __name__ == "__main__":
    # Создание и запуск генератора ландшафта
    generator = TerrainGenerator(
        start=(0, 0),
        end=(50, 0),
        iterations=10,
        roughness=1.5
    )
    show()
