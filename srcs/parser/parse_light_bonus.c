/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_light_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/05 10:00:00 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:04 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT_bonus.h"

static void	append_light(t_scene *scene, t_light *light)
{
	t_light	*cursor;

	if (!scene->lights)
	{
		scene->lights = light;
		return ;
	}
	cursor = scene->lights;
	while (cursor->next)
		cursor = cursor->next;
	cursor->next = light;
}

void	parse_light(char **tokens, t_scene *scene)
{
	t_light	*light;

	if (token_count(tokens) != 4)
		error_exit("light: expected 'L <x,y,z> <brightness> <r,g,b>'");
	light = malloc(sizeof(t_light));
	if (!light)
		error_exit("malloc failed");
	light->pos = parse_vec3(tokens[1]);
	light->brightness = parse_double(tokens[2]);
	if (light->brightness < 0.0 || light->brightness > 1.0)
		error_exit("light brightness must be in [0.0, 1.0]");
	light->color = parse_color(tokens[3]);
	light->kc = 1.0;
	light->kl = 0.0;
	light->kq = 0.0;
	light->next = NULL;
	append_light(scene, light);
	scene->has_light = 1;
}
