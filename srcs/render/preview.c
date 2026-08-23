/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   preview.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 10:00:00 by azaytsev          #+#    #+#             */
/*   Updated: 2026/08/22 14:00:00 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

static t_color	half_at(t_color *half, int i, int j)
{
	if (i < 0)
		i = 0;
	if (i > WIDTH / 2 - 1)
		i = WIDTH / 2 - 1;
	if (j < 0)
		j = 0;
	if (j > HEIGHT / 2 - 1)
		j = HEIGHT / 2 - 1;
	return (half[j * (WIDTH / 2) + i]);
}

static double	spread4(double a, double b, double c, double d)
{
	double	lo;
	double	hi;

	lo = fmin(fmin(a, b), fmin(c, d));
	hi = fmax(fmax(a, b), fmax(c, d));
	return (hi - lo);
}

static int	is_edge(t_color *half, int i, int j)
{
	t_color	a;
	t_color	b;
	t_color	c;
	t_color	d;

	a = half_at(half, i, j);
	b = half_at(half, i + 1, j);
	c = half_at(half, i, j + 1);
	d = half_at(half, i + 1, j + 1);
	if (spread4(a.r, b.r, c.r, d.r) > EDGE_SPREAD)
		return (1);
	if (spread4(a.g, b.g, c.g, d.g) > EDGE_SPREAD)
		return (1);
	return (spread4(a.b, b.b, c.b, d.b) > EDGE_SPREAD);
}

static t_color	sample_half(t_color *half, double sx, double sy)
{
	int		i;
	int		j;
	t_color	top;
	t_color	bot;

	i = (int)sx;
	j = (int)sy;
	top = color_lerp(half_at(half, i, j), half_at(half, i + 1, j), sx - i);
	bot = color_lerp(half_at(half, i, j + 1), half_at(half, i + 1, j + 1),
			sx - i);
	return (color_lerp(top, bot, sy - j));
}

void	upsample_frame(t_app *app)
{
	t_camera_basis	basis;
	t_color			color;
	int				x;
	int				y;

	basis = build_camera_basis(&app->scene.camera);
	y = -1;
	while (++y < HEIGHT)
	{
		x = -1;
		while (++x < WIDTH)
		{
			if (is_edge(app->half, x / 2, y / 2))
				color = ray_color(get_ray(&basis, (x + 0.5) / WIDTH, 1.0 - (y
								+ 0.5) / HEIGHT), &app->scene, MAX_DEPTH);
			else
				color = sample_half(app->half, x * 0.5, y * 0.5);
			mlx_put_pixel(&app->mlx, x, y, color);
		}
	}
}
