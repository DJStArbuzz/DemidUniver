import pygame
import random
import sys

# Инициализация графического модуля
pygame.init()
screen_width, screen_height = 1280, 720
display_surface = pygame.display.set_mode((screen_width, screen_height))
pygame.display.set_caption("НЕ бесконечный скроллинг ландшафта")

# Цветовая схема
BACKGROUND_COLOR = (28, 33, 39)  # Темно-серый фон
PATH_COLOR = (76, 175, 80)  # Зеленый цвет пути
TEXT_COLOR = (240, 240, 240)  # Светло-серый текст

# Параметры алгоритма
BASE_HEIGHT = screen_height // 2
MAX_ITERATIONS = 15
INITIAL_RANDOM_RANGE = 65


class TerrainGenerator:
    def __init__(self):
        self.roughness_factor = None
        self.current_step = None
        self.vertices = None
        self.reset_terrain()

    def reset_terrain(self):
        """Инициализирует или сбрасывает состояние ландшафта"""
        self.vertices = [(0, BASE_HEIGHT), (screen_width, BASE_HEIGHT)]
        self.current_step = 0
        self.roughness_factor = 0.45  # Начальное значение шероховатости

    def calculate_next_stage(self):
        """Выполняет одну итерацию алгоритма"""
        if self.current_step >= MAX_ITERATIONS:
            return

        new_vertices = []
        for i in range(len(self.vertices) - 1):
            new_vertices.append(self.vertices[i])

            x1, y1 = self.vertices[i]
            x2, y2 = self.vertices[i + 1]
            midpoint = ((x1 + x2) // 2, (y1 + y2) // 2)

            # Применение смещения с текущими параметрами
            displacement_scale = INITIAL_RANDOM_RANGE * (self.roughness_factor ** self.current_step)
            offset = random.uniform(-displacement_scale, displacement_scale)
            adjusted_midpoint = (midpoint[0], midpoint[1] + offset)

            new_vertices.append(adjusted_midpoint)

        new_vertices.append(self.vertices[-1])
        self.vertices = new_vertices
        self.current_step += 1

    def render(self, surface):
        """Отрисовывает текущее состояние ландшафта"""
        if len(self.vertices) >= 2:
            pygame.draw.lines(surface, PATH_COLOR, False, self.vertices, 3)


def create_info_panel(generator, font):
    """Создает текстовые поверхности с информацией"""
    lines = [
        f"№ итерации: {generator.current_step}/{MAX_ITERATIONS}",
        f"Точек: {len(generator.vertices)}",
        f"Шероховатость: {generator.roughness_factor:.2f}",
    ]

    rendered_lines = []
    for line in lines:
        text_surface = font.render(line, True, TEXT_COLOR)
        rendered_lines.append(text_surface)

    return rendered_lines


def main():
    clock = pygame.time.Clock()
    terrain = TerrainGenerator()
    ui_font = pygame.font.Font(None, 32)
    auto_advance = False

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_UP:
                    terrain.roughness_factor = min(terrain.roughness_factor + 0.05, 1.0)
                elif event.key == pygame.K_DOWN:
                    terrain.roughness_factor = max(terrain.roughness_factor - 0.05, 0.1)
                elif event.key == pygame.K_SPACE:
                    auto_advance = not auto_advance
                elif event.key == pygame.K_r:
                    terrain.reset_terrain()
                elif event.key == pygame.K_RIGHT:
                    terrain.calculate_next_stage()

        if auto_advance and not terrain.current_step >= MAX_ITERATIONS:
            terrain.calculate_next_stage()

        display_surface.fill(BACKGROUND_COLOR)
        terrain.render(display_surface)

        info_text = create_info_panel(terrain, ui_font)
        y_position = 20
        for text_surface in info_text:
            display_surface.blit(text_surface, (20, y_position))
            y_position += text_surface.get_height() + 8

        pygame.display.update()
        clock.tick(4 if auto_advance else 60)


if __name__ == "__main__":
    main()
