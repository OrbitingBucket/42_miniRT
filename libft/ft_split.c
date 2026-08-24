/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 12:17:48 by azaytsev          #+#    #+#             */
/*   Updated: 2025/11/14 18:30:48 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_countwords(char const *s, char c)
{
	size_t	words;
	size_t	i;

	words = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] != c && (i == 0 || s[i - 1] == c))
			words++;
		i++;
	}
	return (words);
}

static size_t	ft_word_len(char const *s, char c)
{
	size_t	len;

	len = 0;
	while (s[len] && s[len] != c)
		len++;
	return (len);
}

static void	ft_free_all(char **split, size_t filled)
{
	while (filled > 0)
	{
		filled--;
		free(split[filled]);
	}
	free(split);
}

static char	*ft_word_dup(char const *s, size_t len)
{
	size_t	i;
	char	*dst;

	dst = malloc(len + 1);
	if (!dst)
		return (NULL);
	i = 0;
	while (i < len)
	{
		dst[i] = s[i];
		i++;
	}
	dst[i] = '\0';
	return (dst);
}

char	**ft_split(char const *s, char c)
{
	char	**split;
	size_t	i;
	size_t	len;

	if (!s)
		return (NULL);
	split = malloc(sizeof(char *) * (ft_countwords(s, c) + 1));
	if (!split)
		return (NULL);
	i = 0;
	while (*s)
	{
		while (*s == c)
			s++;
		if (*s == '\0')
			break ;
		len = ft_word_len(s, c);
		split[i] = ft_word_dup(s, len);
		if (!split[i++])
			return (ft_free_all(split, i - 1), (char **) NULL);
		s += len;
	}
	split[i] = NULL;
	return (split);
}
