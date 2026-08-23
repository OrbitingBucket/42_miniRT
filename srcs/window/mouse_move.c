/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mouse_move.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 09:24:59 by azaytsev          #+#    #+#             */
/*   Updated: 2026/08/23 09:24:59 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

int	mouse_move(int x, int y, void *param)
{
	t_app	*app;

	app = (t_app *)param;
	if (!app->drag || app->is_locked)
		return (0);
	if (app->fast && (app->needs_render || app->row < HEIGHT))
		return (0);
	handle_rotate(app, x - app->last_x, y - app->last_y);
	app->last_x = x;
	app->last_y = y;
	return (0);
}

int	mouse_release(int button, int x, int y, void *param)
{
	t_app	*app;

	app = (t_app *)param;
	(void)x;
	(void)y;
	if (button == 1)
		app->drag = 0;
	return (0);
}
