#include "miniRT_bonus.h"
#include <stdlib.h>

#ifdef __EMSCRIPTEN__
# include <emscripten.h>
#else
# define EMSCRIPTEN_KEEPALIVE
#endif

void	build_wonderland(t_scene *s);
void	build_chess(t_scene *s);
void	build_accel(t_scene *s);

static t_app	g_app;
static t_color	*g_coarse;

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

EMSCRIPTEN_KEEPALIVE unsigned char	*rt_init(int scene)
{
	if (scene == 1)
		build_wonderland(&g_app.scene);
	else
		build_chess(&g_app.scene);
	build_accel(&g_app.scene);
	g_app.mlx.bpp = 32;
	g_app.mlx.line_len = WIDTH * 4;
	g_app.mlx.back_addr = malloc((size_t)WIDTH * HEIGHT * 4);
	g_app.mlx.addr = g_app.mlx.back_addr;
	if (!g_app.mlx.back_addr)
		error_exit("malloc failed");
	g_app.fast = 0;
	g_app.half = NULL;
	g_coarse = malloc(sizeof(t_color) * (WIDTH / 2 + 2) * (HEIGHT / 2 + 2));
	if (!g_coarse)
		error_exit("malloc failed");
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

static int	is_edge4(t_color a, t_color b, t_color c, t_color d)
{
	double	lo;
	double	hi;
	int		k;
	double	v[4];

	k = 0;
	while (k < 3)
	{
		if (k == 0)
		{
			v[0] = a.r; v[1] = b.r; v[2] = c.r; v[3] = d.r;
		}
		else if (k == 1)
		{
			v[0] = a.g; v[1] = b.g; v[2] = c.g; v[3] = d.g;
		}
		else
		{
			v[0] = a.b; v[1] = b.b; v[2] = c.b; v[3] = d.b;
		}
		lo = v[0];
		hi = v[0];
		if (v[1] < lo) lo = v[1];
		if (v[2] < lo) lo = v[2];
		if (v[3] < lo) lo = v[3];
		if (v[1] > hi) hi = v[1];
		if (v[2] > hi) hi = v[2];
		if (v[3] > hi) hi = v[3];
		if (hi - lo > 0.13)
			return (1);
		k++;
	}
	return (0);
}

static t_color	lerp_c(t_color a, t_color b, double t)
{
	t_color	r;

	r.r = a.r + (b.r - a.r) * t;
	r.g = a.g + (b.g - a.g) * t;
	r.b = a.b + (b.b - a.b) * t;
	return (r);
}

static void	sample_grid(t_camera_basis *b, int y_start, int step, int gw,
		int gh)
{
	double	px;
	double	py;
	int		i;
	int		j;

	j = 0;
	while (j < gh)
	{
		py = y_start + step * (j - 1) + step * 0.5;
		i = 0;
		while (i < gw)
		{
			px = step * (i - 1) + step * 0.5;
			g_coarse[j * gw + i] = color_clamp(ray_color(get_ray(b,
							px / (double)WIDTH, 1.0 - py / (double)HEIGHT),
						&g_app.scene, MAX_DEPTH));
			i++;
		}
		j++;
	}
}

static void	render_blocks(int y_start, int y_end, int step)
{
	t_camera_basis	basis;
	int				gw;
	int				gh;
	int				x;
	int				y;

	basis = build_camera_basis(&g_app.scene.camera);
	gw = WIDTH / step + 2;
	gh = (y_end - y_start) / step + 2;
	sample_grid(&basis, y_start, step, gw, gh);
	y = y_start;
	while (y < y_end && y < HEIGHT)
	{
		double	gyf = (y - y_start + 0.5 + step * 0.5) / step;
		int		j0 = (int)gyf;
		double	ty = gyf - j0;
		x = 0;
		while (x < WIDTH)
		{
			double	gxf = (x + 0.5 + step * 0.5) / step;
			int		i0 = (int)gxf;
			double	tx = gxf - i0;
			t_color	c00 = g_coarse[j0 * gw + i0];
			t_color	c10 = g_coarse[j0 * gw + i0 + 1];
			t_color	c01 = g_coarse[(j0 + 1) * gw + i0];
			t_color	c11 = g_coarse[(j0 + 1) * gw + i0 + 1];
			if (step == 2 && is_edge4(c00, c10, c01, c11))
				mlx_put_pixel(&g_app.mlx, x, y, color_clamp(ray_color(
							get_ray(&basis, (x + 0.5) / (double)WIDTH,
								1.0 - (y + 0.5) / (double)HEIGHT),
							&g_app.scene, MAX_DEPTH)));
			else
				mlx_put_pixel(&g_app.mlx, x, y,
					lerp_c(lerp_c(c00, c10, tx), lerp_c(c01, c11, tx), ty));
			x++;
		}
		y++;
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
