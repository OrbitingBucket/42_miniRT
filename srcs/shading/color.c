/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 15:52:29 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/05 23:48:05 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

t_color	color_scale(t_color c, double t)
{
	return ((t_color){c.r * t, c.g * t, c.b * t});
}

t_color	color_add(t_color a, t_color b)
{
	return ((t_color){a.r + b.r, a.g + b.g, a.b + b.b});
}

t_color	color_multiply(t_color a, t_color b)
{
	return ((t_color){a.r * b.r, a.g * b.g, a.b * b.b});
}

t_color	color_clamp(t_color c)
{
	t_color	out;

	out.r = c.r;
	out.g = c.g;
	out.b = c.b;
	if (out.r < 0.0)
		out.r = 0;
	if (out.r > 1.0)
		out.r = 1.0;
	if (out.g < 0.0)
		out.g = 0;
	if (out.g > 1.0)
		out.g = 1.0;
	if (out.b < 0.0)
		out.b = 0;
	if (out.b > 1.0)
		out.b = 1.0;
	return (out);
}

t_color	color_lerp(t_color a, t_color b, double t)
{
	return (color_clamp(color_add(color_scale(a, 1.0 - t),
				color_scale(b, t))));
}
