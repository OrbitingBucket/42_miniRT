/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/15 14:15:20 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:05 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

static t_color	background(t_ray ray)
{
	double	t;
	t_color	white;
	t_color	blue;

	white = (t_color){1.0, 1.0, 1.0};
	blue = (t_color){0.5, 0.7, 1.0};
	t = (ray.dir.y + 1.0) * 0.5;
	return (color_lerp(white, blue, t));
}

t_color	ray_color(t_ray ray, t_scene *scene, int depth)
{
	t_hit	hit;

	if (depth <= 0)
		return ((t_color){0.0, 0.0, 0.0});
	if (!intersect_scene(ray, scene, &hit))
		return (background(ray));
	return (shade(&hit, scene, ray));
}

static t_color	get_sample_color(t_app *app, t_camera_basis *basis, double px,
		double py)
{
	double	u;
	double	v;
	t_ray	ray;

	u = px / (double)WIDTH;
	v = 1.0 - py / (double)HEIGHT;
	ray = get_ray(basis, u, v);
	return (ray_color(ray, &app->scene, MAX_DEPTH));
}

static void	render_pixel(t_app *app, t_camera_basis *basis, int x, int y)
{
	t_color	total;
	int		i;
	int		j;

	total = (t_color){0, 0, 0};
	i = -1;
	while (++i < 2)
	{
		j = -1;
		while (++j < 2)
		{
			total = color_add(total, get_sample_color(app, basis, x + (i + 0.5)
						/ 2.0, y + (j + 0.5) / 2.0));
		}
	}
	mlx_put_pixel(&app->mlx, x, y, color_scale(total, 0.25));
}

void	render(t_app *app)
{
	t_camera_basis	basis;
	int				x;
	int				y;

	basis = build_camera_basis(&app->scene.camera);
	y = -1;
	while (++y < HEIGHT)
	{
		x = -1;
		while (++x < WIDTH)
			render_pixel(app, &basis, x, y);
	}
	mlx_put_image_to_window(app->mlx.ptr, app->mlx.win, app->mlx.img, 0, 0);
}
