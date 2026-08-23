/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/15 14:16:21 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:06 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

void	mlx_setup(t_app *app)
{
	app->mlx.ptr = mlx_init();
	if (!app->mlx.ptr)
		error_exit("mlx_init failed");
	app->mlx.win = mlx_new_window(app->mlx.ptr, WIDTH, HEIGHT, "miniRT");
	if (!app->mlx.win)
		error_exit("mlx_new_window failed");
	app->mlx.img = mlx_new_image(app->mlx.ptr, WIDTH, HEIGHT);
	if (!app->mlx.img)
		error_exit("mlx_new_image failed");
	app->mlx.addr = mlx_get_data_addr(app->mlx.img, &app->mlx.bpp,
			&app->mlx.line_len, &app->mlx.endian);
	app->mlx.back = mlx_new_image(app->mlx.ptr, WIDTH, HEIGHT);
	if (!app->mlx.back)
		error_exit("mlx_new_image failed");
	app->mlx.back_addr = mlx_get_data_addr(app->mlx.back, &app->mlx.bpp,
			&app->mlx.line_len, &app->mlx.endian);
	ft_bzero(app->mlx.addr, (size_t)HEIGHT * app->mlx.line_len);
	app->half = malloc(sizeof(t_color) * (WIDTH / 2) * (HEIGHT / 2));
	if (!app->half)
		error_exit("malloc failed");
	app->is_locked = 1;
	app->needs_render = 1;
	app->fast = 1;
	app->row = HEIGHT;
}
