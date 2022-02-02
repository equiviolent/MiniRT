/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsabir <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/22 14:19:03 by hsabir            #+#    #+#             */
/*   Updated: 2022/02/02 18:07:32 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing/parser.h"
#include <mlx.h>
#include <libft.h>
#include <validate.h>
#include <keycodes.h>
#include <types.h>
#include <op_vec_double.h>
#include <op_vec.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <constants.h>
#include <rotate_vec.h>

double	solve_quadratic(double a, double b, double c)
{
	double	d;
	double	t0;
	double	t1;
	double	q;

	d = b * b - 4 * a * c;
	if (d < 0)
		return (-1);
	if (d == 0)
		return (-0.5 * b / a);
	if (b > 0)
		q = -0.5 * (b + sqrt(d));
	else
		q = -0.5 * (b - sqrt(d));
	t0 = q / a;
	t1 = c / q;
	if (t0 > t1)
	{
		q = t1;
		t1 = t0;
		t0 = q;
	}
	if (t0 < 0)
		return (t1);
	return (t0);
}

double	hit_sphere(const t_sphere *sphere, t_ray r)
{
	t_vec3	oc;
	double	radius;
	double	a;
	double	b;
	double	c;

	radius = sphere->diameter / 2;
	oc = min_vec(r.origin, sphere->coordinates);
	a = dot(r.direction, r.direction);
	b = 2.0 * dot(r.direction, oc);
	c = dot(oc, oc) - radius * radius;
	return (solve_quadratic(a, b, c));
}

double plane_hit(t_plane *plane, t_ray r)
{
    double denom;
	denom = dot(plane->orientation, r.direction);
	double t;
	if (denom > 1e-6)
	{
		t_vec3 p0l0 = min_vec(plane->coordinates, r.origin);
		t = dot(p0l0, plane->orientation);
		return (t);
	}
	return (-1);
}

int	clip_colour(double c)
{
	if (c < 0)
		return (0);
	if (c > 255)
		return (255);
	return ((int)c);
}

int	rgb_to_int(t_colour c)
{
	int	rgb;

	rgb = clip_colour(c.x);
	rgb = (rgb << 8) + clip_colour(c.y);
	rgb = (rgb << 8) + clip_colour(c.z);
	return (rgb);
}

int	get_ray_luminosity(t_light L, t_ambient A, t_vec3 hit_point, t_sphere *sp)
{
	t_colour c = {0, 0, 0};

	c = plus_vec(c, mult3(sp->colour, A.brightness));
	c = plus_vec(c, mult3(sp->colour, L.brightness)); // todo: add dot
	return (rgb_to_int(c));
}

int	cast_ray(t_list **head, t_ray r, t_list *obj, t_data img)
{
	t_list	*elem;
	double	t;

	elem = *head;
	if (elem == obj)
		elem = elem->next;
	t = -1;
	while (elem != NULL)
	{
		if (elem == obj)
		{
			elem = elem->next;
			continue ;
		}
		if (elem->type == 's')
			t = hit_sphere(((t_sphere *) elem->content), r);
		else if (elem->type == 'p')
			t = plane_hit(((t_plane *) elem->content), r);
		if (t > 0)
			return (0); // todo: return ambient light?
		elem = elem->next;
	}
	return (get_ray_luminosity(img.light, img.ambient, r.origin, obj->content));
}

int	cast_ray2(t_light L, t_ambient A, t_vec3 hit_point, t_plane *pl)
{
	t_vec3	normal;
	t_vec3	ld;
	double	intensity;
	int		ret;

	return (rgb_to_int(pl->colour));
	normal = min_vec(pl->coordinates,hit_point);
	ld = normalize(min_vec(L.coordinates, hit_point));
	//hit_point = normalize(hit_point);
	normal = normalize(normal);
	//TODO maybe multiply by light ratio * L.brightness;
	intensity = -dot(normal, ld) * L.brightness;
	ret = rgb_to_int(mult3(pl->colour, intensity));
	if (ret <= 0) {
//		intensity = dot(normal, ld) * A.brightness;
//		ret = rgb_to_int(mult3(A.colour, intensity));
		ret = 0;
	}
	return (ret);
}

int ray_color(t_ray r, t_list **head, t_data img) {
	double	distance;
	double	t;
	t_list	*elem;
	t_list	*hit_elem;

	hit_elem = NULL;
	distance = DBL_MAX;
	elem = *head;
	t = -1;
	while (elem != NULL) {
		if (elem->type == 's')
			t = hit_sphere(((t_sphere *) elem->content), r);
		else if (elem->type == 'p')
			t = plane_hit(((t_plane *) elem->content), r);
		if (t >= 0 && t < distance)
		{
			distance = t;
			t = -1;
			hit_elem = elem;
		}
		elem = elem->next;
	}
	if (hit_elem != NULL) {
		if (hit_elem->type == 's') {
			r.origin = plus_vec(r.origin, mult3(r.direction, distance));
			r.origin = plus_vec(r.origin,
								mult3(normalize(min_vec(r.origin, ((t_sphere *) hit_elem->content)->coordinates)),
									  1e-5));
			r.direction = normalize(min_vec(img.light.coordinates, r.origin));
			return (cast_ray(head, r, hit_elem, img));
		} else
			return (cast_ray2(img.light, img.ambient, plus_vec(r.origin, mult3(r.direction, distance)), hit_elem->content));

	}
	return (0);
}

void	init_image(t_data *img)
{
	img->mlx = mlx_init();
	img->mlx_win = mlx_new_window(img->mlx, IMG_W, IMG_H, "MiniRT!");
	img->img = mlx_new_image(img->mlx, IMG_W, IMG_H);
	img->addr = (int *) mlx_get_data_addr(img->img, &(img->bbp), &(img->line_len), &(img->endian));
	// convert fov from degrees to radiant
	img->camera.fov = img->camera.fov * M_PI / 180;
	img->viewport_width = tan(img->camera.fov / 2) * 2;
	img->viewport_height = img->viewport_width / ASPECT_RATIO;
	// pi/2 is 90 degrees
	img->horizontal = mult3(rotate_y_axis(-M_PI_2, img->camera.orientation), img->viewport_width);
	img->vertical = mult3(rotate_x_axis(M_PI_2, img->camera.orientation), img->viewport_height);
	img->top_left_corner = plus_vec(plus_vec(min_vec(img->camera.view_point, div3(img->horizontal, 2)), div3(img->vertical, 2)), img->camera.orientation);
	mlx_hook(img->mlx_win, 17, 1L, ft_exit, img);
	mlx_hook(img->mlx_win, 2, 1L, key_hook, img);
}

void	create_img(t_list **objects, t_data	*img)
{
	int		y;
	int		x;
	t_ray	ray;
	int		px;

	y = -1;
	ray.origin = img->camera.view_point;
	ray.direction = img->top_left_corner;
	px = 0;
	while (++y < IMG_H)
	{
		x = -1;
		while (++x < IMG_W)
		{
			ray.direction = normalize(min_vec(plus_vec(plus_vec(img->top_left_corner,
							mult3(img->horizontal, ((double) x + .5) / IMG_W)),
						mult3(img->vertical, -((double) y + .5) / IMG_H)),
					ray.origin));
			img->addr[px++] = ray_color(ray, objects, *img);
		}
	}
}

int	main(int argc, char **argv)
{
	t_list		**objects;
	t_data		img;

	if (argc != 2)
		return (ft_printf(2, "Usage: ./miniRT (scene)\n"));
	objects = parser(argv[1], &(img.camera), &(img.light), &(img.ambient));
	if (objects == NULL)
		return (ft_printf(2, "Error\nFile scene corrupted\n"));
	else if (check_list_values(*objects, &(img.ambient), &(img.light), &(img.camera)))
	{
		ft_lstclear(objects, free);
		free(objects);
		return (ft_printf(2, "Error\nInvalid value in scene\n"));
	}
	init_image(&img);
	create_img(objects, &img);
	mlx_put_image_to_window(img.mlx, img.mlx_win, img.img, 0, 0);
	mlx_loop(img.mlx);
}
