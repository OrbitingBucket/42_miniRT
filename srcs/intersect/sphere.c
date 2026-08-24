/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 14:25:19 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:02 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

static void	set_face_normal(t_hit *hit, t_ray ray, t_vec3 outward_normal)
{
	if (vec3_dot(ray.dir, outward_normal) < 0)
	{
		hit->front_face = 1;
		hit->normal = outward_normal;
	}
	else
	{
		hit->front_face = 0;
		hit->normal = vec3_negate(outward_normal);
	}
}

int	intersect_sphere(t_ray ray, t_object *obj, t_hit *hit)
{
	t_vec3	oc;
	t_quad	q;
	double	t;

	oc = vec3_sub(ray.origin, obj->sphere.center);
	q.a = vec3_dot(ray.dir, ray.dir);
	q.b = 2.0 * vec3_dot(oc, ray.dir);
	q.c = vec3_dot(oc, oc) - obj->sphere.radius * obj->sphere.radius;
	if (!quad_solve(&q) || !quad_nearest(&q, &t))
		return (0);
	hit->t = t;
	hit->point = ray_at(ray, t);
	hit->mat = &obj->mat;
	set_face_normal(hit, ray, vec3_norm(vec3_sub(hit->point,
				obj->sphere.center)));
	return (1);
}
