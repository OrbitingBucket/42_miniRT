/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_opts_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/05 10:00:00 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:04 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT_bonus.h"

void	check_count(char **tokens, int n, char *msg)
{
	if (token_count(tokens) < n)
		error_exit(msg);
}

void	parse_material(char **tokens, int color_idx, t_material *mat)
{
	int	count;

	count = material_token_count(tokens, mat);
	if (count > color_idx + 1)
		mat->ks = parse_ratio(tokens[color_idx + 1]);
	if (count > color_idx + 2)
		mat->shininess = parse_double(tokens[color_idx + 2]);
	if (mat->shininess < 1.0)
		error_exit("Material shininess must be >= 1.0");
	if (count > color_idx + 3)
		mat->reflectivity = parse_ratio(tokens[color_idx + 3]);
	if (count > color_idx + 4)
		mat->transparency = parse_ratio(tokens[color_idx + 4]);
	if (count > color_idx + 5)
		mat->ior = parse_double(tokens[color_idx + 5]);
	if (mat->ior < 1.0)
		error_exit("Material index of refraction must be >= 1.0");
	if (count > color_idx + 6)
		mat->checkerboard = (int)parse_double(tokens[color_idx + 6]);
	if (count > color_idx + 7)
		mat->checker_size = parse_double(tokens[color_idx + 7]);
}
