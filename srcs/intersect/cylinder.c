/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 14:28:05 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/19 12:51:33 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

static int	hit_body(t_ray ray, t_object *obj, t_hit *hit)
{
	t_quad	q;
	double	sqrt_disc;

	if (!cylinder_quadratic(ray, obj, &q))
		return (0);
	sqrt_disc = sqrt(q.disc);
	if (check_body_hit(ray, obj, hit, (-q.b - sqrt_disc) / (2.0 * q.a)))
	{
		hit->normal = body_normal(obj, hit->point);
		return (1);
	}
	if (check_body_hit(ray, obj, hit, (-q.b + sqrt_disc) / (2.0 * q.a)))
	{
		hit->normal = body_normal(obj, hit->point);
		return (1);
	}
	return (0);
}

static void	cap_plane(t_object *obj, int top, t_vec3 *center, t_vec3 *normal)
{
	if (top)
	{
		*center = obj->cylinder.top_center;
		*normal = obj->cylinder.axis;
	}
	else
	{
		*center = obj->cylinder.bottom_center;
		*normal = vec3_negate(obj->cylinder.axis);
	}
}

static int	hit_cap(t_ray ray, t_object *obj, t_hit *hit, int top)
{
	double	denom;
	t_vec3	p;
	t_vec3	center;
	t_vec3	normal;

	cap_plane(obj, top, &center, &normal);
	denom = vec3_dot(ray.dir, normal);
	if (fabs(denom) < EPSILON)
		return (0);
	hit->t = vec3_dot(vec3_sub(center, ray.origin), normal) / denom;
	if (hit->t < EPSILON)
		return (0);
	p = ray_at(ray, hit->t);
	if (vec3_dot(vec3_sub(p, center), vec3_sub(p,
				center)) > obj->cylinder.radius * obj->cylinder.radius)
		return (0);
	set_cap_hit(ray, obj, hit, normal);
	return (1);
}

int	intersect_cylinder(t_ray ray, t_object *obj, t_hit *hit)
{
	t_hit	tmp;
	double	best;

	best = INFINITY;
	if (hit_body(ray, obj, &tmp) && tmp.t < best)
	{
		best = tmp.t;
		*hit = tmp;
	}
	if (hit_cap(ray, obj, &tmp, 1) && tmp.t < best)
	{
		best = tmp.t;
		*hit = tmp;
	}
	if (hit_cap(ray, obj, &tmp, 0) && tmp.t < best)
	{
		best = tmp.t;
		*hit = tmp;
	}
	return (best < INFINITY);
}
