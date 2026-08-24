/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 15:11:02 by jinliang          #+#    #+#             */
/*   Updated: 2026/07/20 15:11:22 by jinliang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

int	quad_solve(t_quad *q)
{
	q->disc = q->b * q->b - 4.0 * q->a * q->c;
	if (q->disc < 0.0)
		return (0);
	q->sqrt_disc = sqrt(q->disc);
	return (1);
}

double	quad_root1(t_quad *q)
{
	return ((-q->b - q->sqrt_disc) / (2.0 * q->a));
}

double	quad_root2(t_quad *q)
{
	return ((-q->b + q->sqrt_disc) / (2.0 * q->a));
}

int	quad_nearest(t_quad *q, double *t)
{
	*t = quad_root1(q);
	if (*t >= EPSILON)
		return (1);
	*t = quad_root2(q);
	if (*t >= EPSILON)
		return (1);
	return (0);
}
