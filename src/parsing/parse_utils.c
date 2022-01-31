/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msegrans <msegrans@student.42lausanne      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/26 23:34:52 by msegrans          #+#    #+#             */
/*   Updated: 2022/01/26 23:34:55 by msegrans         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <types.h>

void	skip_spaces(char **line)
{
	while (ft_isspace(**line))
		(*line)++;
}

int	new_elem(t_list **head)
{
	t_list	*elem;

	elem = ft_lstnew(NULL);
	if (!elem)
		return (1);
	ft_lstadd_front(head, elem);
	return (0);
}

char	*chop_word(char **line, int (fn)(int))
{
	char	*word;

	skip_spaces(line);
	word = *line;
	while (**line && !fn(**line))
		(*line)++;
	if (**line)
	{
		**line = 0;
		(*line)++;
	}
	return (word);
}

int	is_comma(int c)
{
	return ((char)c == ',');
}

int	parse_vec3(char **line, t_vec3 *elem)
{
	char	*word;

	word = chop_word(line, is_comma);
	if (ft_atof(word, &(elem->x)))
		return (1);
	word = chop_word(line, is_comma);
	if (ft_atof(word, &(elem->y)))
		return (1);
	word = chop_word(line, ft_isspace);
	return (ft_atof(word, &(elem->z)));
}

int	parsing_error(void *elem, char *error)
{
	ft_printf(2, "%s\n", error);
	free(elem);
	return (1);
}
