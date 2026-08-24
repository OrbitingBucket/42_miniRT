/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: azaytsev <azaytsev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 11:49:56 by azaytsev          #+#    #+#             */
/*   Updated: 2025/11/14 16:57:26 by azaytsev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_is_set(char c, char const *set)
{
	if (!set)
		return (0);
	while (*set)
	{
		if (c == *set)
			return (1);
		set++;
	}
	return (0);
}

static size_t	ft_trimstart(char const *s1, char const *set)
{
	size_t	start;

	start = 0;
	while (s1[start] && ft_is_set(s1[start], set))
		start++;
	return (start);
}

static size_t	ft_trimend(char const *s1, char const *set)
{
	size_t	end;

	end = 0;
	while (s1[end])
		end++;
	while (end > 0 && ft_is_set(s1[end - 1], set))
		end--;
	return (end);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	start;
	size_t	end;

	if (!s1)
		return (NULL);
	start = ft_trimstart(s1, set);
	end = ft_trimend(s1, set);
	return (ft_substr(s1, start, end - start));
}
