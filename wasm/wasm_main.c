#include "miniRT_bonus.h"
#include <stdlib.h>

#ifdef __EMSCRIPTEN__
# include <emscripten.h>
#else
# define EMSCRIPTEN_KEEPALIVE
#endif

void	build_wonderland(t_scene *s);

static t_app	g_app;

void	error_exit(const char *msg)
{
	ft_putstr_fd("Error\n", 2);
	ft_putstr_fd((char *)msg, 2);
	ft_putstr_fd("\n", 2);
	exit(1);
}

void	mlx_put_pixel(t_mlx *mlx, int x, int y, t_color color)
{
	unsigned char	*dst;

	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
		return ;
	dst = (unsigned char *)mlx->back_addr + (y * mlx->line_len + x * 4);
	dst[0] = (int)(color.r * 255.999);
	dst[1] = (int)(color.g * 255.999);
	dst[2] = (int)(color.b * 255.999);
	dst[3] = 255;
}

EMSCRIPTEN_KEEPALIVE unsigned char	*rt_init(void)
{
	build_wonderland(&g_app.scene);
	g_app.mlx.bpp = 32;
	g_app.mlx.line_len = WIDTH * 4;
	g_app.mlx.back_addr = malloc((size_t)WIDTH * HEIGHT * 4);
	g_app.mlx.addr = g_app.mlx.back_addr;
	if (!g_app.mlx.back_addr)
		error_exit("malloc failed");
	g_app.fast = 0;
	g_app.half = NULL;
	return ((unsigned char *)g_app.mlx.back_addr);
}

EMSCRIPTEN_KEEPALIVE void	rt_set_cam(double px, double py, double pz,
		double dx, double dy, double dz)
{
	g_app.scene.camera.pos = vec3(px, py, pz);
	g_app.scene.camera.dir = vec3_norm(vec3(dx, dy, dz));
}

EMSCRIPTEN_KEEPALIVE void	rt_set_fov(double fov)
{
	if (fov < 25)
		fov = 25;
	if (fov > 120)
		fov = 120;
	g_app.scene.camera.fov = fov;
}

static void	fill_block(int x0, int y0, int step, t_color c)
{
	int	x;
	int	y;

	y = y0 - 1;
	while (++y < y0 + step && y < HEIGHT)
	{
		x = x0 - 1;
		while (++x < x0 + step && x < WIDTH)
			mlx_put_pixel(&g_app.mlx, x, y, c);
	}
}

static void	render_blocks(int y_start, int y_end, int step)
{
	t_camera_basis	basis;
	t_color			c;
	double			u;
	double			v;
	int				xy[2];

	basis = build_camera_basis(&g_app.scene.camera);
	xy[1] = y_start;
	while (xy[1] < y_end && xy[1] < HEIGHT)
	{
		xy[0] = 0;
		while (xy[0] < WIDTH)
		{
			u = (xy[0] + step * 0.5) / (double)WIDTH;
			v = 1.0 - (xy[1] + step * 0.5) / (double)HEIGHT;
			c = ray_color(get_ray(&basis, u, v), &g_app.scene, MAX_DEPTH);
			c = color_clamp(c);
			fill_block(xy[0], xy[1], step, c);
			xy[0] += step;
		}
		xy[1] += step;
	}
}

EMSCRIPTEN_KEEPALIVE void	rt_render_band(int y0, int y1, int quality)
{
	if (quality == 0)
	{
		g_app.fast = 0;
		render_span(&g_app, y0, y1);
	}
	else if (quality == 1)
		render_blocks(y0, y1, 2);
	else if (quality == 2)
		render_blocks(y0, y1, 4);
	else
		render_blocks(y0, y1, 8);
}
