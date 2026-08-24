/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shade_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/05 10:00:00 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/19 14:07:30 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT_bonus.h"

t_color	shade(t_hit *hit, t_scene *scene, t_ray ray)
{
	t_color		final;
	t_color		contrib;
	t_light		*light;
	t_material	tmp;

	apply_checker(hit, &tmp);
	final = ambient_light(hit, &scene->ambient);
	light = scene->lights;
	while (light)
	{
		if (!in_shadow(scene, hit, light))
		{
			contrib = color_add(diffuse_light(hit, light),
					specular_light(hit, light, ray));
			final = color_add(final, color_multiply(contrib, light->color));
		}
		light = light->next;
	}
	return (color_clamp(final));
}
