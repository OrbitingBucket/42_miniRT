/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/05 10:00:00 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:03 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

t_material	default_material(t_color color)
{
	t_material	m;

	m.color = color;
	m.ka = 0.1;
	m.kd = 0.9;
	m.ks = 0.3;
	m.shininess = 32.0;
	m.reflectivity = 0.0;
	m.transparency = 0.0;
	m.ior = 1.0;
	m.roughness = 0.0;
	m.checkerboard = 0;
	m.checker_size = 1.0;
	m.bump = NULL;
	m.bump_w = 0;
	m.bump_h = 0;
	return (m);
}
