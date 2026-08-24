/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 14:58:55 by jinliang          #+#    #+#             */
/*   Updated: 2026/07/21 16:10:56 by jinliang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

int	cylinder_quadratic(t_ray ray, t_object *obj, t_quad *q)
{
	t_vec3	oc;

	oc = vec3_sub(ray.origin, obj->cylinder.center);
	q->d = vec3_sub(ray.dir, vec3_scale(obj->cylinder.axis, vec3_dot(ray.dir,
					obj->cylinder.axis)));
	q->f = vec3_sub(oc, vec3_scale(obj->cylinder.axis, vec3_dot(oc,
					obj->cylinder.axis)));
	q->a = vec3_dot(q->d, q->d);
	if (fabs(q->a) < EPSILON)
		return (0);
	q->b = 2.0 * vec3_dot(q->d, q->f);
	q->c = vec3_dot(q->f, q->f) - obj->cylinder.radius * obj->cylinder.radius;
	q->disc = q->b * q->b - 4.0 * q->a * q->c;
	return (q->disc >= 0.0);
}

static int	valid_body_hit(t_ray ray, t_object *obj, double t)
{
	double	proj;

	if (t < EPSILON)
		return (0);
	proj = vec3_dot(vec3_sub(ray_at(ray, t), obj->cylinder.center),
			obj->cylinder.axis);
	return (proj >= -obj->cylinder.height / 2.0 && proj <= obj->cylinder.height
		/ 2.0);
}

t_vec3	body_normal(t_object *obj, t_vec3 point)
{
	double	proj;
	t_vec3	radial;

	proj = vec3_dot(vec3_sub(point, obj->cylinder.center), obj->cylinder.axis);
	radial = vec3_sub(point, vec3_add(obj->cylinder.center,
				vec3_scale(obj->cylinder.axis, proj)));
	return (vec3_norm(radial));
}

int	check_body_hit(t_ray ray, t_object *obj, t_hit *hit, double t)
{
	if (!valid_body_hit(ray, obj, t))
		return (0);
	hit->t = t;
	hit->point = ray_at(ray, t);
	hit->mat = &obj->mat;
	hit->front_face = (vec3_dot(ray.dir, body_normal(obj, hit->point)) < 0.0);
	if (hit->front_face)
		hit->normal = body_normal(obj, hit->point);
	else
		hit->normal = vec3_negate(body_normal(obj, hit->point));
	return (1);
}

void	set_cap_hit(t_ray ray, t_object *obj, t_hit *hit, t_vec3 normal)
{
	hit->point = ray_at(ray, hit->t);
	hit->mat = &obj->mat;
	hit->front_face = (vec3_dot(ray.dir, normal) < 0.0);
	if (hit->front_face)
		hit->normal = normal;
	else
		hit->normal = vec3_negate(normal);
}
