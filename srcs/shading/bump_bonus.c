/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bump_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 13:30:41 by azaytsev          #+#    #+#             */
/*   Updated: 2026/08/28 13:30:41 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT_bonus.h"

static double	bump_at(t_material *m, int x, int y)
{
	x = ((x % m->bump_w) + m->bump_w) % m->bump_w;
	if (y < 0)
		y = 0;
	if (y >= m->bump_h)
		y = m->bump_h - 1;
	return (m->bump[y * m->bump_w + x] / 255.0);
}

static void	sphere_uv(t_vec3 n, double *u, double *v)
{
	double	ny;

	ny = n.y;
	if (ny > 1.0)
		ny = 1.0;
	if (ny < -1.0)
		ny = -1.0;
	*u = 0.5 + atan2(n.z, n.x) / 6.28318530717959;
	*v = 0.5 - asin(ny) / 3.14159265358979;
}

static t_vec3	tangent_of(t_vec3 n)
{
	t_vec3	t;

	t = vec3_cross(vec3(0, 1, 0), n);
	if (vec3_len(t) < 1e-6)
		t = vec3(1, 0, 0);
	return (vec3_norm(t));
}

void	apply_bump(t_hit *hit)
{
	t_material	*m;
	t_vec3		axes[2];
	double		uv[2];
	double		grad[2];
	int			px[2];

	m = hit->mat;
	if (!m->bump)
		return ;
	sphere_uv(hit->normal, &uv[0], &uv[1]);
	px[0] = (int)(uv[0] * m->bump_w);
	px[1] = (int)(uv[1] * m->bump_h);
	grad[0] = bump_at(m, px[0] + 1, px[1]) - bump_at(m, px[0] - 1, px[1]);
	grad[1] = bump_at(m, px[0], px[1] + 1) - bump_at(m, px[0], px[1] - 1);
	axes[0] = tangent_of(hit->normal);
	axes[1] = vec3_cross(hit->normal, axes[0]);
	hit->normal = vec3_norm(vec3_add(hit->normal,
				vec3_add(vec3_scale(axes[0], grad[0] * 2.2),
					vec3_scale(axes[1], grad[1] * 2.2))));
}
