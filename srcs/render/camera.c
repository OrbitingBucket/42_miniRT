/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/15 14:13:58 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/19 12:51:33 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

static void	get_camera_axes(t_vec3 dir, t_vec3 *right, t_vec3 *up)
{
	t_vec3	world_up;

	if (fabs(dir.y) < 0.99)
		world_up = vec3(0, 1, 0);
	else
		world_up = vec3(0, 0, 1);
	*right = vec3_norm(vec3_cross(dir, world_up));
	*up = vec3_norm(vec3_cross(*right, dir));
}

static void	compute_basis(t_camera *cam, t_camera_basis *b)
{
	t_vec3	right;
	t_vec3	up;
	double	half_h;
	double	half_w;

	get_camera_axes(cam->dir, &right, &up);
	half_w = tan((cam->fov * M_PI / 180.0) / 2.0);
	half_h = half_w * ((double)HEIGHT / (double)WIDTH);
	b->origin = cam->pos;
	b->horizontal = vec3_scale(right, 2.0 * half_w);
	b->vertical = vec3_scale(up, 2.0 * half_h);
	b->lower_left = vec3_sub(vec3_sub(
				vec3_add(cam->pos, cam->dir),
				vec3_scale(right, half_w)),
			vec3_scale(up, half_h));
}

t_camera_basis	build_camera_basis(t_camera *cam)
{
	t_camera_basis	b;

	compute_basis(cam, &b);
	return (b);
}

t_ray	get_ray(t_camera_basis *b, double u, double v)
{
	t_ray	ray;
	t_vec3	target;

	target = vec3_add(b->lower_left,
			vec3_add(vec3_scale(b->horizontal, u),
				vec3_scale(b->vertical, v)));
	ray.origin = b->origin;
	ray.dir = vec3_norm(vec3_sub(target, b->origin));
	return (ray);
}
