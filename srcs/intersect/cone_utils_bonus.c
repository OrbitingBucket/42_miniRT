/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cone_utils_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/03 17:36:47 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:01 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT_bonus.h"

void	cone_quad(t_ray ray, t_object *obj, t_quad *q)
{
	t_vec3	oc;
	t_vec3	d_perp;
	t_vec3	oc_perp;
	double	k;

	k = obj->cone.radius / obj->cone.height;
	oc = vec3_sub(ray.origin, obj->cone.apex);
	q->m = vec3_dot(ray.dir, obj->cone.axis);
	q->n = vec3_dot(oc, obj->cone.axis);
	d_perp = vec3_sub(ray.dir, vec3_scale(obj->cone.axis, q->m));
	oc_perp = vec3_sub(oc, vec3_scale(obj->cone.axis, q->n));
	q->a = vec3_dot(d_perp, d_perp) - k * k * (q->m) * (q->m);
	q->b = 2.0 * (vec3_dot(d_perp, oc_perp) - k * k * (q->m) * (q->n));
	q->c = vec3_dot(oc_perp, oc_perp) - k * k * (q->n) * (q->n);
}

t_vec3	cone_body_normal(t_object *obj, t_vec3 point, double proj)
{
	t_vec3	perp;
	double	k;

	k = obj->cone.radius / obj->cone.height;
	perp = vec3_sub(point, vec3_add(obj->cone.apex,
				vec3_scale(obj->cone.axis, proj)));
	return (vec3_norm(vec3_sub(perp,
				vec3_scale(obj->cone.axis, k * k * proj))));
}

int	check_cone_body(t_ray ray, t_object *obj, t_hit *hit, t_quad *q)
{
	double	proj;
	t_vec3	outward;

	if (q->root < EPSILON)
		return (0);
	proj = q->n + q->root * q->m;
	if (proj < 0.0 || proj > obj->cone.height)
		return (0);
	hit->t = q->root;
	hit->point = ray_at(ray, q->root);
	hit->mat = &obj->mat;
	outward = cone_body_normal(obj, hit->point, proj);
	hit->front_face = vec3_dot(ray.dir, outward) < 0;
	if (hit->front_face)
		hit->normal = outward;
	else
		hit->normal = vec3_negate(outward);
	return (1);
}
