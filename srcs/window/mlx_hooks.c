/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_hooks.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/15 14:15:43 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/19 12:51:32 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

int	mouse_hook(int button, int x, int y, void *param)
{
	t_app	*app;

	app = (t_app *)param;
	(void)x;
	(void)y;
	if (app->is_locked)
		return (0);
	if (button == 4)
	{
		app->scene.camera.fov -= 5;
		if (app->scene.camera.fov < 1)
			app->scene.camera.fov = 1;
	}
	else if (button == 5)
	{
		app->scene.camera.fov += 5;
		if (app->scene.camera.fov > 179)
			app->scene.camera.fov = 179;
	}
	else
		return (0);
	app->fast = 1;
	app->needs_render = 1;
	return (0);
}

int	key_handler(int keycode, void *param)
{
	t_app	*app;

	app = (t_app *)param;
	if (keycode == KEY_ESC)
		close_handler(app);
	if (keycode == KEY_SPACE)
	{
		app->is_locked = !app->is_locked;
		if (app->is_locked)
			ft_putstr_fd("🔒 Camera Locked\n", 1);
		else
			ft_putstr_fd("🔓 Camera Unlocked\n", 1);
		return (0);
	}
	if (app->is_locked)
		return (0);
	handle_move(app, keycode);
	return (0);
}

int	close_handler(t_app *app)
{
	mlx_destroy_image(app->mlx.ptr, app->mlx.img);
	mlx_destroy_image(app->mlx.ptr, app->mlx.back);
	mlx_destroy_window(app->mlx.ptr, app->mlx.win);
	mlx_destroy_display(app->mlx.ptr);
	free(app->mlx.ptr);
	free(app->half);
	free_scene(&app->scene);
	exit(0);
}

int	expose_handler(t_app *app)
{
	mlx_put_image_to_window(app->mlx.ptr, app->mlx.win, app->mlx.img, 0, 0);
	return (0);
}

int	loop_hook(t_app *app)
{
	int	end;

	if (app->needs_render)
	{
		app->needs_render = 0;
		app->row = 0;
	}
	if (app->row >= HEIGHT)
		return (0);
	end = app->row + SLICE_ROWS;
	if (end > HEIGHT)
		end = HEIGHT;
	render_span(app, app->row, end);
	app->row = end;
	if (app->row >= HEIGHT)
		present_frame(app);
	return (0);
}
