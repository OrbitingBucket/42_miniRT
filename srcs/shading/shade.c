/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shade.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/15 15:02:01 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:06 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

t_color	shade(t_hit *hit, t_scene *scene, t_ray ray)
{
	t_color	final;
	t_light	*light;

	(void)ray;
	final = ambient_light(hit, &scene->ambient);
	light = scene->lights;
	while (light)
	{
		if (!in_shadow(scene, hit, light))
		{
			final = color_add(final, diffuse_light(hit, light));
		}
		light = light->next;
	}
	return (color_clamp(final));
}
