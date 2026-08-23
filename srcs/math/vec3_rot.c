/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3_rot.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 09:24:59 by azaytsev          #+#    #+#             */
/*   Updated: 2026/08/23 09:24:59 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

t_vec3	vec3_rotate(t_vec3 v, t_vec3 axis, double angle)
{
	t_vec3	radial;
	t_vec3	tangent;
	t_vec3	axial;

	radial = vec3_scale(v, cos(angle));
	tangent = vec3_scale(vec3_cross(axis, v), sin(angle));
	axial = vec3_scale(axis, vec3_dot(axis, v) * (1.0 - cos(angle)));
	return (vec3_add(vec3_add(radial, tangent), axial));
}
