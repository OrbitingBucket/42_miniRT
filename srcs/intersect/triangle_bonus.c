/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triangle_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 17:38:13 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:02 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT_bonus.h"

static int	tri_solve(t_ray ray, t_object *obj, double *uvt)
{
	t_vec3	e[2];
	t_vec3	hsq[3];
	double	det;
	double	inv_det;

	e[0] = vec3_sub(obj->triangle.v1, obj->triangle.v0);
	e[1] = vec3_sub(obj->triangle.v2, obj->triangle.v0);
	hsq[0] = vec3_cross(ray.dir, e[1]);
	det = vec3_dot(e[0], hsq[0]);
	if (fabs(det) < EPSILON)
		return (0);
	inv_det = 1.0 / det;
	hsq[1] = vec3_sub(ray.origin, obj->triangle.v0);
	uvt[0] = vec3_dot(hsq[1], hsq[0]) * inv_det;
	if (uvt[0] < 0.0 || uvt[0] > 1.0)
		return (0);
	hsq[2] = vec3_cross(hsq[1], e[0]);
	uvt[1] = vec3_dot(ray.dir, hsq[2]) * inv_det;
	if (uvt[1] < 0.0 || uvt[0] + uvt[1] > 1.0)
		return (0);
	uvt[2] = vec3_dot(e[1], hsq[2]) * inv_det;
	uvt[3] = det;
	return (1);
}

int	intersect_triangle(t_ray ray, t_object *obj, t_hit *hit)
{
	double	uvt[4];

	if (!tri_solve(ray, obj, uvt))
		return (0);
	if (uvt[2] < EPSILON)
		return (0);
	hit->t = uvt[2];
	hit->point = ray_at(ray, uvt[2]);
	hit->mat = &obj->mat;
	hit->front_face = uvt[3] > 0;
	if (uvt[3] > 0)
		hit->normal = obj->triangle.normal;
	else
		hit->normal = vec3_negate(obj->triangle.normal);
	return (1);
}
