/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shadow.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 14:06:30 by jinliang          #+#    #+#             */
/*   Updated: 2026/07/16 15:27:54 by jinliang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

static t_ray	make_shadow_ray(t_hit *hit, t_light *light)
{
	t_ray	ray;
	t_vec3	to_light;
	t_vec3	offset;

	to_light = vec3_norm(vec3_sub(light->pos, hit->point));
	offset = vec3_add(vec3_scale(hit->normal, SHADOW_BIAS), vec3_scale(to_light,
				SHADOW_BIAS));
	ray.origin = vec3_add(hit->point, offset);
	ray.dir = to_light;
	return (ray);
}

int	in_shadow(t_scene *scene, t_hit *hit, t_light *light)
{
	t_ray	shadow_ray;
	t_hit	tmp;
	double	light_dist;

	shadow_ray = make_shadow_ray(hit, light);
	light_dist = vec3_len(vec3_sub(light->pos, hit->point));
	if (intersect_scene(shadow_ray, scene, &tmp))
		return (tmp.t < light_dist - SHADOW_BIAS);
	return (0);
}
