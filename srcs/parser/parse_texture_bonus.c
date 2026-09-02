/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_texture_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/28 13:30:41 by azaytsev          #+#    #+#             */
/*   Updated: 2026/08/28 13:30:41 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "miniRT_bonus.h"

static int	is_pgm(const char *s)
{
	size_t	len;

	if (!s)
		return (0);
	len = ft_strlen(s);
	if (len < 5)
		return (0);
	return (ft_strcmp(s + len - 4, ".pgm") == 0);
}

static int	pgm_num(int fd, char *last)
{
	long	n;
	char	c;
	int		got;

	got = 0;
	n = 0;
	while (read(fd, &c, 1) == 1)
	{
		if (c >= '0' && c <= '9')
		{
			n = n * 10 + (c - '0');
			got = 1;
		}
		else if (got || (c != ' ' && c != '\t' && c != '\n' && c != '\r'))
			break ;
		if (n > 1000000)
			break ;
	}
	*last = c;
	if (!got || n < 1 || n > 4096)
		return (-1);
	return ((int)n);
}

static void	pgm_head(int fd, t_material *mat)
{
	char	c[3];
	char	last;
	int		maxval;

	if (read(fd, c, 3) != 3 || c[0] != 'P' || c[1] != '5'
		|| (c[2] != '\n' && c[2] != ' ' && c[2] != '\r'))
	{
		close(fd);
		error_exit("bump map must be a binary pgm (P5)");
	}
	mat->bump_w = pgm_num(fd, &last);
	mat->bump_h = pgm_num(fd, &last);
	maxval = pgm_num(fd, &last);
	if (mat->bump_w < 0 || mat->bump_h < 0 || maxval != 255)
	{
		close(fd);
		error_exit("bump map header is invalid");
	}
}

static void	pgm_data(int fd, t_material *mat)
{
	long	total;
	long	done;
	int		ret;

	total = (long)mat->bump_w * mat->bump_h;
	mat->bump = malloc(total);
	if (!mat->bump)
	{
		close(fd);
		error_exit("malloc failed");
	}
	done = 0;
	while (done < total)
	{
		ret = read(fd, mat->bump + done, total - done);
		if (ret <= 0)
		{
			close(fd);
			error_exit("bump map file is truncated");
		}
		done += ret;
	}
	close(fd);
}

int	material_token_count(char **tokens, t_material *mat)
{
	int	count;
	int	fd;

	count = token_count(tokens);
	if (count < 2 || !is_pgm(tokens[count - 1]))
		return (count);
	fd = open(tokens[count - 1], O_RDONLY);
	if (fd < 0)
		error_exit("cannot open bump map file");
	pgm_head(fd, mat);
	pgm_data(fd, mat);
	return (count - 1);
}
