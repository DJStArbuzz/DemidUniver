import pygame
import random
import sys

# Инициализация графики
pygame.init()
screen_width, screen_height = 1280, 720
display_surface = pygame.display.set_mode((screen_width, screen_height))
pygame.display.set_caption("Бесконечный скроллинг ландшафта")

# Цветовая схема
BACKGROUND_COLOR = (28, 33, 39)  # Темный фон
PATH_COLOR = (76, 175, 80)  # Зеленый путь
TEXT_COLOR = (240, 240, 240)  # Светлый текст

# Параметры генерации
BASE_HEIGHT = screen_height // 2
INITIAL_SEGMENT = 200
MAX_ITERATIONS = 5
SCROLL_SPEEDS = [1, 3, 5, 10]


class InfiniteLandscape:
    def __init__(self):
        self.points = []
        self.roughness = 0.5
        self.scroll_speed = 2  # Индекс в SCROLL_SPEEDS
        self.offset_x = 0
        self._generate_initial()

    def _generate_initial(self):
        # Генерация начального сегмента
        start = (0, BASE_HEIGHT)
        end = (INITIAL_SEGMENT, BASE_HEIGHT)
        self.points = self._process_segment([start, end])

    def _midpoint_displacement(self, points):
        # Алгоритм смещения средней точки
        new_points = []
        for i in range(len(points) - 1):
            left = points[i]
            right = points[i + 1]

            mid_x = (left[0] + right[0]) // 2
            mid_y = (left[1] + right[1]) // 2
            displacement = random.uniform(-1, 1) * self.roughness * 50
            new_points.extend([left, (mid_x, mid_y + displacement)])

        new_points.append(points[-1])
        return new_points

    def _process_segment(self, segment):
        # Обработка сегмента за несколько итераций
        processed = segment.copy()
        for _ in range(MAX_ITERATIONS):
            processed = self._midpoint_displacement(processed)
        return processed

    def update(self):
        # Обновление позиций и генерация новых сегментов
        speed = SCROLL_SPEEDS[self.scroll_speed]
        self.offset_x += speed

        # Сдвиг всех точек
        self.points = [(x - speed, y) for x, y in self.points]

        # Удаление невидимых точек
        self.points = [p for p in self.points if p[0] > -INITIAL_SEGMENT]

        # Генерация новых сегментов
        while self.points[-1][0] < screen_width + INITIAL_SEGMENT:
            last_x = self.points[-1][0]
            new_end = (last_x + INITIAL_SEGMENT, BASE_HEIGHT)
            new_segment = self._process_segment([self.points[-1], new_end])
            self.points.extend(new_segment[1:])

    def render(self, surface):
        # Отрисовка ландшафта
        if len(self.points) >= 2:
            pygame.draw.lines(surface, PATH_COLOR, False, self.points, 3)


def create_info_panel(landscape, font):
    # Создание информационной панели
    lines = [
        f"Шероховатость: {landscape.roughness:.2f}",
        f"Скорость: {SCROLL_SPEEDS[landscape.scroll_speed]} px/кадр",
    ]

    rendered = []
    for line in lines:
        text = font.render(line, True, TEXT_COLOR)
        rendered.append(text)

    return rendered


def main():
    clock = pygame.time.Clock()
    landscape = InfiniteLandscape()
    ui_font = pygame.font.Font(None, 32)
    running = True

    while running:
        # Обработка событий
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    running = False
                elif event.key == pygame.K_UP:
                    landscape.roughness = min(landscape.roughness + 0.1, 1.5)
                elif event.key == pygame.K_DOWN:
                    landscape.roughness = max(landscape.roughness - 0.1, 0.1)
                elif event.key == pygame.K_LEFT:
                    landscape.scroll_speed = max(landscape.scroll_speed - 1, 0)
                elif event.key == pygame.K_RIGHT:
                    landscape.scroll_speed = min(landscape.scroll_speed + 1, len(SCROLL_SPEEDS) - 1)
                elif event.key == pygame.K_r:
                    landscape = InfiniteLandscape()

        # Обновление и отрисовка
        landscape.update()
        display_surface.fill(BACKGROUND_COLOR)
        landscape.render(display_surface)

        # Отображение информации
        info = create_info_panel(landscape, ui_font)
        y_pos = 20
        for text_surface in info:
            display_surface.blit(text_surface, (20, y_pos))
            y_pos += text_surface.get_height() + 8

        pygame.display.flip()
        clock.tick(60)

    pygame.quit()
    sys.exit()


if __name__ == "__main__":
    main()
