/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   specular_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 14:10:23 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:06 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT_bonus.h"

t_color	specular_light(t_hit *hit, t_light *light, t_ray ray)
{
	t_vec3	to_light;
	t_vec3	reflect_dir;
	t_vec3	view_dir;
	double	spec;
	t_color	white;

	to_light = vec3_norm(vec3_sub(light->pos, hit->point));
	reflect_dir = vec3_reflect(vec3_negate(to_light), hit->normal);
	view_dir = vec3_norm(vec3_negate(ray.dir));
	spec = vec3_dot(reflect_dir, view_dir);
	if (spec < 0.0)
		return ((t_color){0.0, 0.0, 0.0});
	spec = pow(spec, hit->mat->shininess);
	spec *= hit->mat->ks * light->brightness;
	white = (t_color){1.0, 1.0, 1.0};
	return (color_scale(white, spec));
}
