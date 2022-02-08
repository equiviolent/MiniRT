/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   op_vec.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msegrans <msegrans@student.42lausanne      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/26 23:37:05 by msegrans          #+#    #+#             */
/*   Updated: 2022/02/07 20:50:11 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OP_VEC_H
# define OP_VEC_H

# include <types.h>

t_vec3	min_vec(t_vec3 a, t_vec3 b);

t_vec3	plus_vec(t_vec3 a, t_vec3 b);

double	dot(t_vec3 a, t_vec3 b);

double	len3(t_vec3 a);

t_vec3	normalize(t_vec3 a);

t_vec3 	cross_prod(t_vec3 a, t_vec3 b);

#endif //OP_VEC_H
