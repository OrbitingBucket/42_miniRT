/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 17:10:53 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/19 12:51:32 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

int	main(int argc, char **argv)
{
	t_app	app;

	if (argc != 2)
		error_exit("usage: ./miniRT <scene.rt>");
	parse_scene(argv[1], &app.scene);
	mlx_setup(&app);
	mlx_mouse_hook(app.mlx.win, mouse_hook, &app);
	mlx_hook(app.mlx.win, 2, 1L << 0, (int (*)())(void *)key_handler, &app);
	mlx_hook(app.mlx.win, 17, 0, (int (*)())(void *)close_handler, &app);
	mlx_hook(app.mlx.win, 12, 1L << 15, (int (*)())(void *)expose_handler,
		&app);
	mlx_loop_hook(app.mlx.ptr, (int (*)())(void *)loop_hook, &app);
	mlx_loop(app.mlx.ptr);
	return (0);
}

void	error_exit(const char *msg)
{
	ft_putstr_fd("Error\n", 2);
	ft_putstr_fd((char *)msg, 2);
	ft_putstr_fd("\n", 2);
	exit(1);
}
