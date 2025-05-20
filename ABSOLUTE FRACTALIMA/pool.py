import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from matplotlib.widgets import Slider, Button, RectangleSelector
from matplotlib.patches import Patch
from numba import jit

resolution = 1000


@jit(nopython=True)
def newton_pool(reZ, imZ, reC, imC, iterN, R, reRoots, imRoots):
    height, width = reZ.shape
    basin = np.zeros((height, width), dtype=np.int32)

    for i in range(height):
        for j in range(width):
            curReZ = reZ[i, j]
            curImZ = imZ[i, j]
            converged = False

            for _ in range(iterN):
                """
                f(z) = z^3 + c
                z = x + iy
                (x+iy)^2 = x^2 - y^2 + 2ixy
                (x+iy)^3 = (x^2 - y^2 + 2ixy)(x+iy) = x^3 + 3ixy - 3xy^2 iy^3
                """
                reZSq = curReZ * curReZ
                imZSq = curImZ * curImZ

                reF = curReZ * (reZSq - 3 * imZSq) + reC
                imF = curImZ * (3 * reZSq - imZSq) + imC

                """
                f'(z) = 3z^2
                z = x + iy
                (x+iy)^2 = x^2 - y^2 + 2ixy
                f'(z) = 3x^2-3y^2+6ixy
                """
                reFd = 3 * (reZSq - imZSq)
                imFd = 6 * curReZ * curImZ
                # f'(z) * -f'(z) = -f'(z)/|f'(z)^2|
                normFd = reFd * reFd + imFd * imFd

                if normFd < 1e-10:
                    break

                # Обновление по методу Ньютона
                reDelta = (reF * reFd + imF * imFd) / normFd
                imDelta = (imF * reFd - reF * imFd) / normFd
                curReZ -= reDelta
                curImZ -= imDelta

                # Проверка сходимости к корням
                for k in range(len(reRoots)):
                    dx = curReZ - reRoots[k]
                    dy = curImZ - imRoots[k]
                    if dx * dx + dy * dy < R * R:
                        basin[i, j] = k + 1
                        converged = True
                        break
                if converged:
                    break
            else:
                basin[i, j] = 0
    return basin


class NewtonFractal:
    def __init__(self):
        self.iterNum = None
        self.yMax = None
        self.yMin = None
        self.xMax = None
        self.xMin = None
        self.R = None
        self.c = None
        self.roots = None
        self.zoom_stack = []
        self.init_params()
        self.setup_ui()
        self.update_plot()
        plt.show()

    def init_params(self):
        self.c = complex(-1, 0)
        self.R = 1e-3
        self.xMin, self.xMax = -3, 3
        self.yMin, self.yMax = -3, 3
        self.iterNum = 50
        self.roots = None

    def setup_ui(self):
        self.fig, self.ax = plt.subplots(figsize=(8, 8))
        plt.subplots_adjust(left=0.25, bottom=0.35)

        # Элементы управления
        self.ax_re = plt.axes([0.25, 0.25, 0.65, 0.03])
        self.ax_im = plt.axes([0.25, 0.20, 0.65, 0.03])
        self.ax_R = plt.axes([0.25, 0.15, 0.65, 0.03])
        self.ax_iter = plt.axes([0.25, 0.10, 0.65, 0.03])

        self.s_re = Slider(self.ax_re, 'Re(c)', -3.0, 3.0, valinit=self.c.real)
        self.s_im = Slider(self.ax_im, 'Im(c)', -3.0, 3.0, valinit=self.c.imag)
        self.s_R = Slider(self.ax_R, 'R', 1e-5, 1e-2, valinit=self.R, valfmt='%1.0e')
        self.s_iter = Slider(self.ax_iter, 'Iterations', 1, 100, valinit=self.iterNum, valfmt='%d')

        self.ax_save = plt.axes([0.05, 0.85, 0.1, 0.04])
        self.btn_save = Button(self.ax_save, 'Save')
        self.ax_reset = plt.axes([0.05, 0.80, 0.1, 0.04])
        self.btn_reset = Button(self.ax_reset, 'Reset')
        self.ax_zoom = plt.axes([0.05, 0.75, 0.1, 0.04])
        self.btn_zoom = Button(self.ax_zoom, 'Zoom')
        self.ax_back = plt.axes([0.05, 0.70, 0.1, 0.04])
        self.btn_back = Button(self.ax_back, 'Back')

        # Селектор для масштабирования
        self.selector = RectangleSelector(
            self.ax, self.zoom_to_rect,
            useblit=True,
            button=[1],
            minspanx=5, minspany=5,
            spancoords='pixels',
            interactive=True,
            props=dict(alpha=0.3, facecolor='red', edgecolor='black'))
        self.selector.set_active(False)

        # Привязка событий
        self.s_re.on_changed(self.update)
        self.s_im.on_changed(self.update)
        self.s_R.on_changed(self.update)
        self.s_iter.on_changed(self.update)

        self.btn_save.on_clicked(self.save_image)
        self.btn_reset.on_clicked(self.reset)
        self.btn_zoom.on_clicked(self.toggle_selector)
        self.btn_back.on_clicked(self.zoom_back)

    def zoom_to_rect(self, eclick, erelease):
        x1, y1 = eclick.xdata, eclick.ydata
        x2, y2 = erelease.xdata, erelease.ydata
        self.zoom_stack.append((self.xMin, self.xMax, self.yMin, self.yMax))
        self.xMin, self.xMax = sorted([x1, x2])
        self.yMin, self.yMax = sorted([y1, y2])
        self.update_plot()
        self.selector.set_active(False)

    def toggle_selector(self, event):
        self.selector.set_active(not self.selector.get_active())

    def zoom_back(self, event):
        if self.zoom_stack:
            self.xMin, self.xMax, self.yMin, self.yMax = self.zoom_stack.pop()
            self.update_plot()

    def update(self, val=None):
        self.c = complex(self.s_re.val, self.s_im.val)
        self.R = self.s_R.val
        self.iterNum = int(self.s_iter.val)
        self.update_plot()

    def reset(self, event):
        self.zoom_stack = []
        self.init_params()
        self.s_re.set_val(self.c.real)
        self.s_im.set_val(self.c.imag)
        self.s_R.set_val(self.R)
        self.s_iter.set_val(self.iterNum)
        self.update_plot()

    def compute_basin(self):
        x = np.linspace(self.xMin, self.xMax, resolution)
        y = np.linspace(self.yMin, self.yMax, resolution)
        X, Y = np.meshgrid(x, y)
        # поиск корней
        self.roots = np.roots([1, 0, 0, self.c])
        reRoots = self.roots.real
        imRoots = self.roots.imag

        basin = newton_pool(
            X, Y,
            self.c.real, self.c.imag,
            self.iterNum, self.R,
            reRoots, imRoots
        )
        return basin

    def update_plot(self):
        basin = self.compute_basin()
        cmap = ListedColormap(['gray', 'red', 'green', 'blue'])

        self.ax.clear()
        img = self.ax.imshow(basin, cmap=cmap,
                             extent=[self.xMin, self.xMax, self.yMin, self.yMax],
                             origin='lower',
                             aspect='auto')

        legend_elements = []
        if self.roots is not None:
            colors = ['red', 'green', 'blue']
            for k, (root, color) in enumerate(zip(self.roots, colors)):
                legend_text = f'Root {k + 1}: {root.real:.3f}'
                if root.imag >= 0:
                    legend_text += f' + {root.imag:.3f}i'
                else:
                    legend_text += f' - {abs(root.imag):.3f}i'
                legend_elements.append(Patch(facecolor=color, label=legend_text))

        legend_elements.append(Patch(facecolor='gray', label='No convergence'))

        self.ax.legend(handles=legend_elements, loc='upper right', fontsize=8,
                       bbox_to_anchor=(1.05, 1), borderaxespad=0.)

        # Обновляем заголовок
        self.ax.set_title(
            f"Бассейны Ньютона для $z^3 + ({self.c.real:.2f}{self.c.imag:+.2f}i) = 0$\n"
            f"R={self.R:.0e}, Итерации: {self.iterNum}"
        )
        self.ax.set_xlabel("Re(z)")
        self.ax.set_ylabel("Im(z)")
        self.fig.canvas.draw_idle()

    def save_image(self, event):
        filename = f"newton_c_{self.c.real:.2f}_{self.c.imag:.2f}i_R_{self.R:.0e}.jpg"
        plt.savefig(filename, dpi=300, bbox_inches='tight', format='jpeg')
        print(f"Изображение сохранено как {filename}")


if __name__ == "__main__":
    app = NewtonFractal()
    plt.show()
