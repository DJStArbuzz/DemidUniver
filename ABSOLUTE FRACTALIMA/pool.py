import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from matplotlib.widgets import Slider, Button
from numba import jit


@jit(nopython=True)
def newton_iteration_vec(reZ, imZ, reC, imC, iterN, R, reRoots, imRoots):
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
                z_re_sq = curReZ * curReZ
                z_im_sq = curImZ * curImZ

                f_re = curReZ * (z_re_sq - 3 * z_im_sq) + reC
                f_im = curImZ * (3 * z_re_sq - z_im_sq) + imC

                """
                f'(z) = 3z^2
                z = x + iy
                (x+iy)^2 = x^2 - y^2 + 2ixy
                f'(z) = 3x^2-3y^2+6ixy
                """
                fp_re = 3 * (z_re_sq - z_im_sq)
                fp_im = 6 * curReZ * curImZ
                # f'(z) * -f'(z) = -f'(z)/|f'(z)^2|
                fp_norm = fp_re * fp_re + fp_im * fp_im

                if fp_norm < 1e-10:
                    break

                # Обновление по методу Ньютона
                delta_re = (f_re * fp_re + f_im * fp_im) / fp_norm
                delta_im = (f_im * fp_re - f_re * fp_im) / fp_norm
                curReZ -= delta_re
                curImZ -= delta_im

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
                basin[i, j] = 0  # Не сошлось
    return basin


class NewtonFractal:
    def __init__(self):
        self.max_iter = None
        self.resolution = None
        self.ymax = None
        self.xmax = None
        self.ymin = None
        self.xmin = None
        self.R = None
        self.c = None
        self.roots = None
        self.init_params()
        self.setup_ui()
        self.update_plot()

    def init_params(self):
        self.c = complex(-1, 0)
        self.R = 1e-3
        self.xmin, self.xmax = -2, 2
        self.ymin, self.ymax = -2, 2
        self.resolution = 512
        self.max_iter = 100

    def setup_ui(self):
        self.fig, self.ax = plt.subplots(figsize=(8, 8))
        plt.subplots_adjust(left=0.25, bottom=0.35)

        # Элементы управления
        self.ax_re = plt.axes([0.25, 0.25, 0.65, 0.03])
        self.ax_im = plt.axes([0.25, 0.20, 0.65, 0.03])
        self.ax_R = plt.axes([0.25, 0.15, 0.65, 0.03])
        self.ax_iter = plt.axes([0.25, 0.10, 0.65, 0.03])
        self.ax_res = plt.axes([0.25, 0.05, 0.65, 0.03])

        self.s_re = Slider(self.ax_re, 'Re(c)', -3.0, 3.0, valinit=self.c.real)
        self.s_im = Slider(self.ax_im, 'Im(c)', -3.0, 3.0, valinit=self.c.imag)
        self.s_R = Slider(self.ax_R, 'R', 1e-5, 1e-2, valinit=self.R, valfmt='%1.0e')
        self.s_iter = Slider(self.ax_iter, 'Iterations', 10, 200, valinit=self.max_iter, valfmt='%d')
        self.s_res = Slider(self.ax_res, 'Resolution', 256, 2048, valinit=self.resolution, valfmt='%d')

        self.ax_save = plt.axes([0.05, 0.85, 0.1, 0.04])
        self.btn_save = Button(self.ax_save, 'Save')

        self.ax_reset = plt.axes([0.05, 0.80, 0.1, 0.04])
        self.btn_reset = Button(self.ax_reset, 'Reset')

        self.s_re.on_changed(self.update)
        self.s_im.on_changed(self.update)
        self.s_R.on_changed(self.update)
        self.s_iter.on_changed(self.update)
        self.s_res.on_changed(self.update)
        self.btn_save.on_clicked(self.save_image)
        self.btn_reset.on_clicked(self.reset)

    def update(self, val=None):
        self.c = complex(self.s_re.val, self.s_im.val)
        self.R = self.s_R.val
        self.max_iter = int(self.s_iter.val)
        self.resolution = int(self.s_res.val)
        self.update_plot()

    def reset(self, event):
        self.init_params()
        self.s_re.set_val(self.c.real)
        self.s_im.set_val(self.c.imag)
        self.s_R.set_val(self.R)
        self.s_iter.set_val(self.max_iter)
        self.s_res.set_val(self.resolution)
        self.update_plot()

    def compute_basin(self):
        x = np.linspace(self.xmin, self.xmax, self.resolution)
        y = np.linspace(self.ymin, self.ymax, self.resolution)
        X, Y = np.meshgrid(x, y)

        # поиск корней
        self.roots = np.roots([1, 0, 0, self.c])
        roots_real = self.roots.real
        roots_imag = self.roots.imag

        basin = newton_iteration_vec(
            X, Y,
            self.c.real, self.c.imag,
            self.max_iter, self.R,
            roots_real, roots_imag
        )
        return basin

    def update_plot(self):
        basin = self.compute_basin()
        cmap = ListedColormap(['gray', 'red', 'green', 'blue'])
        self.ax.clear()
        self.ax.imshow(basin, cmap=cmap,
                       extent=[self.xmin, self.xmax, self.ymin, self.ymax],
                       origin='lower')
        self.ax.set_title(f"$z^3 + ({self.c.real:.2f}{self.c.imag:+.2f}i) = 0$")
        self.fig.canvas.draw_idle()

    def save_image(self, event):
        filename = f"newton_c_{self.c.real:.2f}_{self.c.imag:.2f}i_R_{self.R:.0e}.png"
        plt.savefig(filename, dpi=300, bbox_inches='tight')
        print(f"Saved to {filename}")


if __name__ == "__main__":
    app = NewtonFractal()
    plt.show()
