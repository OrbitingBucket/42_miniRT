/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rotation.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 09:24:59 by azaytsev          #+#    #+#             */
/*   Updated: 2026/08/23 09:24:59 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

void	handle_rotate(t_app *app, int dx, int dy)
{
	t_camera	*cam;
	t_vec3		right;
	t_vec3		dir;
	t_vec3		pitched;

	if (dx == 0 && dy == 0)
		return ;
	cam = &app->scene.camera;
	dir = vec3_rotate(cam->dir, vec3(0, 1, 0), -dx * ROT_SPEED);
	if (fabs(dir.y) < 0.99)
	{
		right = vec3_norm(vec3_cross(dir, vec3(0, 1, 0)));
		pitched = vec3_rotate(dir, right, -dy * ROT_SPEED);
		if (fabs(pitched.y) <= 0.97)
			dir = pitched;
	}
	cam->dir = vec3_norm(dir);
	app->fast = 1;
	app->needs_render = 1;
}
