/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keys.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/08 07:00:00 by azaytsev          #+#    #+#             */
/*   Updated: 2026/09/08 07:00:00 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

static int	key_bit(int keycode)
{
	if (keycode == KEY_W)
		return (MOVE_BIT_W);
	if (keycode == KEY_A)
		return (MOVE_BIT_A);
	if (keycode == KEY_S)
		return (MOVE_BIT_S);
	if (keycode == KEY_D)
		return (MOVE_BIT_D);
	return (0);
}

void	press_move_key(t_app *app, int keycode)
{
	int	bit;

	bit = key_bit(keycode);
	if (!bit)
		return ;
	if (!app->keys)
		app->move_clock = now_seconds();
	app->keys |= bit;
	app->pending &= ~bit;
}

int	key_release(int keycode, void *param)
{
	t_app	*app;

	app = (t_app *)param;
	app->pending |= key_bit(keycode);
	return (0);
}

int	focus_out(void *param)
{
	t_app	*app;

	app = (t_app *)param;
	app->keys = 0;
	app->pending = 0;
	return (0);
}
