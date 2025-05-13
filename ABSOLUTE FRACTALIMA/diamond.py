import pygame
import numpy as np
import random

# Настройки
WIDTH, HEIGHT = 800, 600
INIT_SIZE = 257
COLORS = [
    (0, 0, 150), (0, 0, 255), (245, 245, 220),
    (34, 139, 34), (139, 69, 19), (255, 255, 255)
]


class TerrainGenerator:
    def __init__(self, roughness=0.5):
        self.size = INIT_SIZE
        self.base_roughness = roughness
        self.current_roughness = roughness
        self.heightmap = np.zeros((self.size, self.size), dtype=np.float32)
        self.steps = []
        self.init_corners()
        self.prepare_steps()

    def init_corners(self):
        self.heightmap[0, 0] = random.random()
        self.heightmap[0, -1] = random.random()
        self.heightmap[-1, 0] = random.random()
        self.heightmap[-1, -1] = random.random()

    def prepare_steps(self):
        step = self.size - 1
        self.steps = []
        while step > 1:
            self.steps.append(('diamond', step))
            self.steps.append(('square', step))
            step //= 2

    def generate_next(self):
        if not self.steps:
            return False

        step_type, step = self.steps.pop(0)
        half = step // 2
        iterations = int(np.log2(self.size)) - int(np.log2(step))
        current_roughness = self.base_roughness * (0.7 ** iterations)

        if step_type == 'diamond':
            for y in range(0, self.size - 1, step):
                for x in range(0, self.size - 1, step):
                    avg = np.mean([
                        self.heightmap[y, x],
                        self.heightmap[y, x + step],
                        self.heightmap[y + step, x],
                        self.heightmap[y + step, x + step]
                    ])
                    self.heightmap[y + half, x + half] = avg + random.uniform(-1, 1) * current_roughness
        else:
            for y in range(0, self.size, half):
                for x in range((y + half) % step, self.size, step):
                    neighbors = []
                    if y >= half:
                        neighbors.append(self.heightmap[y - half, x])
                    if y + half < self.size:
                        neighbors.append(self.heightmap[y + half, x])
                    if x >= half:
                        neighbors.append(self.heightmap[y, x - half])
                    if x + half < self.size:
                        neighbors.append(self.heightmap[y, x + half])

                    if neighbors:
                        avg = np.mean(neighbors)
                        self.heightmap[y, x] = avg + random.uniform(-1, 1) * current_roughness

        return True

    def get_surface(self):
        normalized = (self.heightmap - np.min(self.heightmap)) / np.ptp(self.heightmap)
        indices = (normalized * (len(COLORS) - 1)).astype(np.uint8)
        rgb_array = np.array(COLORS)[indices]
        return pygame.surfarray.make_surface(rgb_array.repeat(2, axis=0).repeat(2, axis=1))


# Инициализация Pygame
pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
clock = pygame.time.Clock()
font = pygame.font.Font(None, 24)

generator = TerrainGenerator()
running = True
dirty = True

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_r:
                generator = TerrainGenerator(generator.base_roughness)
                dirty = True
            elif event.key == pygame.K_UP:
                generator.base_roughness = min(generator.base_roughness + 0.1, 2.0)
            elif event.key == pygame.K_DOWN:
                generator.base_roughness = max(generator.base_roughness - 0.1, 0.05)

    if dirty:
        if generator.generate_next():
            surf = generator.get_surface()
            screen.blit(pygame.transform.scale(surf, (WIDTH, HEIGHT)), (0, 0))

            # Отображение уровня шероховатости
            text = font.render(
                f"Шероховатость: {generator.base_roughness:.2f}",
                True, (255, 255, 255), (0, 0, 0)
            )
            screen.blit(text, (10, 10))

            pygame.display.flip()
            clock.tick(10)
        else:
            dirty = False

pygame.quit()
