/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_scene.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinliang <jinliang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 16:51:41 by jinliang          #+#    #+#             */
/*   Updated: 2026/08/19 12:58:32 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT.h"

static void	die_line(char *line, const char *msg)
{
	free(line);
	error_exit(msg);
}

static char	*read_line(int fd, long *total)
{
	char	*line;
	char	c;
	int		i;
	int		ret;

	line = malloc(4096);
	if (!line)
		error_exit("malloc failed");
	i = 0;
	while (i < 4095)
	{
		ret = read(fd, &c, 1);
		if (ret < 0)
			die_line(line, "read error");
		if (ret == 0 || c == '\n')
			break ;
		if (++(*total) > 1048576)
			die_line(line, "scene file too large");
		line[i++] = c;
	}
	line[i] = '\0';
	if (i > 0 || ret > 0)
		return (line);
	free(line);
	return (NULL);
}

static int	has_rt_extension(const char *file)
{
	size_t	len;

	if (!file)
		return (0);
	len = ft_strlen(file);
	if (len < 4)
		return (0);
	return (ft_strcmp(file + len - 3, ".rt") == 0);
}

static void	read_all(int fd, t_scene *scene)
{
	long	total;
	char	*line;
	char	**tokens;

	total = 0;
	while (1)
	{
		line = read_line(fd, &total);
		if (!line)
			break ;
		tokens = split_line(line);
		free(line);
		cleanup_slot()->tokens = tokens;
		if (tokens && tokens[0])
			dispatch(tokens, scene);
		free_tokens(tokens);
		cleanup_slot()->tokens = NULL;
	}
}

void	parse_scene(const char *file, t_scene *scene)
{
	int	fd;

	if (!has_rt_extension(file))
		error_exit("scene file must have .rt extension");
	fd = open(file, O_RDONLY);
	if (fd < 0)
		error_exit("cannot open scene file");
	ft_memset(scene, 0, sizeof(t_scene));
	cleanup_slot()->fd = fd;
	cleanup_slot()->scene = scene;
	read_all(fd, scene);
	close(fd);
	cleanup_slot()->fd = 0;
	if (!scene->has_ambient || !scene->has_camera || !scene->has_light)
		error_exit("scene missing mandatory element (A, C, or L)");
}
