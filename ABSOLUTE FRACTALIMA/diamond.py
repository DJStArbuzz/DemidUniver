import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
from numba import njit


@njit(cache=True)
def diamond_square(n_iter, roughness=0.5):
    size = 2 ** n_iter + 1
    terrain = np.zeros((size, size))
    terrain[0::size - 1, 0::size - 1] = np.random.uniform(-1, 1, (2, 2))

    step_size = size - 1
    current_roughness = roughness

    for _ in range(n_iter):
        half = step_size // 2

        # Diamond step
        for x in range(0, size - step_size, step_size):
            for y in range(0, size - step_size, step_size):
                avg = (terrain[x, y] + terrain[x + step_size, y] +
                       terrain[x, y + step_size] + terrain[x + step_size, y + step_size]) / 4
                terrain[x + half, y + half] = avg + np.random.uniform(-1, 1) * current_roughness

        # Square step
        for x in range(0, size, half):
            for y in range((x + half) % step_size, size, step_size):
                total = 0.0
                count = 0
                if x >= half:
                    total += terrain[x - half, y]
                    count += 1
                if x + half < size:
                    total += terrain[x + half, y]
                    count += 1
                if y >= half:
                    total += terrain[x, y - half]
                    count += 1
                if y + half < size:
                    total += terrain[x, y + half]
                    count += 1
                if count > 0:
                    terrain[x, y] = total / count + np.random.uniform(-1, 1) * current_roughness

        step_size = half
        current_roughness *= 0.5

    return terrain


@njit(cache=True)
def progressive_generate(base, new_iter, roughness):
    current_size = base.shape[0]
    current_iter = int(np.log2(current_size - 1))
    new_size = 2 ** new_iter + 1

    terrain = np.zeros((new_size, new_size))
    step = (new_size - 1) // (current_size - 1)
    terrain[::step, ::step] = base

    for iter_num in range(current_iter, new_iter):
        step_size = 2 ** (new_iter - iter_num)
        half = step_size // 2
        current_rough = roughness * (0.5 ** iter_num)

        # Diamond step
        for x in range(0, new_size - step_size, step_size):
            for y in range(0, new_size - step_size, step_size):
                avg = (terrain[x, y] + terrain[x + step_size, y] +
                       terrain[x, y + step_size] + terrain[x + step_size, y + step_size]) / 4
                terrain[x + half, y + half] = avg + np.random.uniform(-1, 1) * current_rough

        # Square step
        for x in range(0, new_size, half):
            for y in range((x + half) % step_size, new_size, step_size):
                total = 0.0
                count = 0
                if x >= half:
                    total += terrain[x - half, y]
                    count += 1
                if x + half < new_size:
                    total += terrain[x + half, y]
                    count += 1
                if y >= half:
                    total += terrain[x, y - half]
                    count += 1
                if y + half < new_size:
                    total += terrain[x, y + half]
                    count += 1
                if count > 0:
                    terrain[x, y] = total / count + np.random.uniform(-1, 1) * current_rough

    return terrain


fig, ax = plt.subplots(figsize=(10, 8))
plt.subplots_adjust(left=0.1, right=0.8, bottom=0.25)

current_iter = 5
current_roughness = 0.5
terrain = diamond_square(current_iter, current_roughness)
img = ax.imshow(terrain, cmap='terrain', origin='lower',
                extent=[0, terrain.shape[1], 0, terrain.shape[0]])
ax.set_title('Diamond-Square Fractal Landscape')
ax.set_aspect('equal')

cax = fig.add_axes([0.85, 0.15, 0.03, 0.7])
fig.colorbar(img, cax=cax, label='Height')

ax_iter = fig.add_axes([0.2, 0.15, 0.6, 0.03])
iter_slider = Slider(ax_iter, 'Iterations', 1, 10, valinit=current_iter, valstep=1)

ax_rough = fig.add_axes([0.2, 0.1, 0.6, 0.03])
rough_slider = Slider(ax_rough, 'Roughness', 0.1, 2.0, valinit=current_roughness, valstep=0.1)


def update(val):
    global terrain, current_iter
    new_iter = int(iter_slider.val)
    new_rough = rough_slider.val

    if new_iter > current_iter:
        terrain = progressive_generate(terrain, new_iter, new_rough)
    else:
        terrain = diamond_square(new_iter, new_rough)

    current_iter = new_iter
    img.set_data(terrain)
    img.set_extent([0, terrain.shape[1], 0, terrain.shape[0]])
    img.set_clim(terrain.min(), terrain.max())
    ax.set_xlim(0, terrain.shape[1])
    ax.set_ylim(0, terrain.shape[0])
    fig.canvas.draw_idle()


iter_slider.on_changed(update)
rough_slider.on_changed(update)

plt.show()
