#include "miniRT_bonus.h"
#include <stdlib.h>

#ifdef __EMSCRIPTEN__
# include <emscripten.h>
#else
# define EMSCRIPTEN_KEEPALIVE
#endif

static t_app	g_app;
static int		g_frame_ready;

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

void	present_frame(t_app *app)
{
	if (app->fast)
		upsample_frame(app);
	g_frame_ready = 1;
	if (app->fast)
	{
		app->fast = 0;
		app->row = 0;
	}
}

EMSCRIPTEN_KEEPALIVE unsigned char	*rt_init(void)
{
	parse_scene("scene.rt", &g_app.scene);
	g_app.mlx.bpp = 32;
	g_app.mlx.line_len = WIDTH * 4;
	g_app.mlx.back_addr = malloc((size_t)WIDTH * HEIGHT * 4);
	g_app.mlx.addr = g_app.mlx.back_addr;
	g_app.half = malloc(sizeof(t_color) * (WIDTH / 2) * (HEIGHT / 2));
	if (!g_app.mlx.back_addr || !g_app.half)
		error_exit("malloc failed");
	g_app.is_locked = 0;
	g_app.needs_render = 1;
	g_app.fast = 1;
	g_app.row = HEIGHT;
	g_app.drag = 0;
	g_app.last_x = 0;
	g_app.last_y = 0;
	return ((unsigned char *)g_app.mlx.back_addr);
}

EMSCRIPTEN_KEEPALIVE int	rt_tick(void)
{
	int	end;

	g_frame_ready = 0;
	if (g_app.needs_render)
	{
		g_app.needs_render = 0;
		g_app.row = 0;
	}
	if (g_app.row >= HEIGHT)
		return (0);
	end = g_app.row + SLICE_ROWS;
	if (end > HEIGHT)
		end = HEIGHT;
	render_span(&g_app, g_app.row, end);
	g_app.row = end;
	if (g_app.row >= HEIGHT)
		present_frame(&g_app);
	return (g_frame_ready);
}

EMSCRIPTEN_KEEPALIVE int	rt_idle(void)
{
	return (!g_app.needs_render && g_app.row >= HEIGHT);
}

EMSCRIPTEN_KEEPALIVE void	rt_key(int keycode)
{
	handle_move(&g_app, keycode);
}

EMSCRIPTEN_KEEPALIVE void	rt_press(int x, int y)
{
	g_app.drag = 1;
	g_app.last_x = x;
	g_app.last_y = y;
}

EMSCRIPTEN_KEEPALIVE void	rt_move(int x, int y)
{
	mouse_move(x, y, &g_app);
}

EMSCRIPTEN_KEEPALIVE void	rt_release(void)
{
	g_app.drag = 0;
}

EMSCRIPTEN_KEEPALIVE void	rt_wheel(int dir)
{
	g_app.scene.camera.fov += 5 * dir;
	if (g_app.scene.camera.fov < 1)
		g_app.scene.camera.fov = 1;
	if (g_app.scene.camera.fov > 179)
		g_app.scene.camera.fov = 179;
	g_app.fast = 1;
	g_app.needs_render = 1;
}
