/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_light.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 16:51:23 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/19 12:51:34 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

void	parse_light(char **tokens, t_scene *scene)
{
	t_light	*light;

	if (scene->has_light)
		error_exit("light (L) defined more than once");
	if (token_count(tokens) != 4)
		error_exit("light: expected 'L <x,y,z> <brightness> <r,g,b>'");
	light = malloc(sizeof(t_light));
	if (!light)
		error_exit("malloc failed");
	light->next = NULL;
	scene->lights = light;
	scene->has_light = 1;
	light->pos = parse_vec3(tokens[1]);
	light->brightness = parse_double(tokens[2]);
	if (light->brightness < 0.0 || light->brightness > 1.0)
		error_exit("light brightness must be in [0.0, 1.0]");
	light->color = parse_color(tokens[3]);
	light->kc = 1.0;
	light->kl = 0.0;
	light->kq = 0.0;
}
