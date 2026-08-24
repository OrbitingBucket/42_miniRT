/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   diffuse.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/15 15:01:54 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:05 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

t_color	diffuse_light(t_hit *hit, t_light *light)
{
	t_vec3	to_light;
	double	intensity;

	to_light = vec3_norm(vec3_sub(light->pos, hit->point));
	intensity = vec3_dot(hit->normal, to_light);
	if (intensity < 0.0)
		intensity = 0.0;
	intensity *= light->brightness;
	intensity *= attenuation(light, hit->point);
	return (color_scale(hit->mat->color, intensity));
}
