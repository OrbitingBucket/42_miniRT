/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dispatch.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/05 10:00:00 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/19 12:51:33 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

static void	unknown_id(char *token)
{
	ft_putstr_fd("Error\nunknown identifier: ", 2);
	ft_putstr_fd(token, 2);
	ft_putstr_fd("\n", 2);
	exit(1);
}

void	dispatch(char **tokens, t_scene *scene)
{
	if (!tokens || !tokens[0])
		return ;
	if (tokens[0][0] == '#')
		return ;
	if (!ft_strcmp(tokens[0], "A"))
		parse_ambient(tokens, scene);
	else if (!ft_strcmp(tokens[0], "C"))
		parse_camera(tokens, scene);
	else if (!ft_strcmp(tokens[0], "L"))
		parse_light(tokens, scene);
	else if (!ft_strcmp(tokens[0], "sp"))
		parse_sphere(tokens, scene);
	else if (!ft_strcmp(tokens[0], "pl"))
		parse_plane(tokens, scene);
	else if (!ft_strcmp(tokens[0], "cy"))
		parse_cylinder(tokens, scene);
	else
		unknown_id(tokens[0]);
}
