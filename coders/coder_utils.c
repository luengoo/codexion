/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alluengo <alluengo@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 13:59:02 by alluengo          #+#    #+#             */
/*   Updated: 2026/09/16 15:42:45 by alluengo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	init_requests(t_coder *c, t_request *l, t_request *r)
{
	t_sim	*sim;

	sim = c->sim;
	pthread_cond_init(&l->cond, NULL);
	pthread_cond_init(&r->cond, NULL);
	l->coder_id = c->id;
	l->timestamp = get_time_ms();
	l->deadline = sim->last_compile_start[c->id - 1]
		+ sim->time_to_burnout;
	r->coder_id = c->id;
	r->timestamp = get_time_ms();
	r->deadline = sim->last_compile_start[c->id - 1]
		+ sim->time_to_burnout;
}

int	one_coder_case(t_sim *sim, t_request *l)
{
	dongle_acquire(sim, 0, l);
	ft_log(sim, l->coder_id, "has taken a dongle");
	while (is_running(sim))
		ft_usleep(1);
	dongle_release(sim, 0);
	return (0);
}

void	do_compile(t_coder *c)
{
	t_sim	*sim;

	sim = c->sim;
	pthread_mutex_lock(&sim->running_mutex);
	sim->last_compile_start[c->id - 1] = get_time_ms();
	pthread_mutex_unlock(&sim->running_mutex);
	ft_log(sim, c->id, "is compiling");
	ft_usleep(sim, sim->time_to_compile);
	pthread_mutex_lock(&sim->running_mutex);
	sim->compile_count[c->id - 1]++;
	pthread_mutex_unlock(&sim->running_mutex);
}

int	take_dongles_normal(t_coder *c, t_request *l, t_request *r)
{
	t_sim	*sim;
	int		left;
	int		right;

	sim = c->sim;
	left = c->id - 1;
	right = c->id % sim->nb_coders;
	dongle_acquire(sim, left, l);
	if (!is_running(sim))
		return (dongle_release(sim, left), 0);
	ft_log(sim, c->id, "has taken a dongle");
	dongle_acquire(sim, right, r);
	if (!is_running(sim))
	{
		dongle_release(sim, right);
		dongle_release(sim, left);
		return (0);
	}
	ft_log(sim, c->id, "has taken a dongle");
	return (1);
}

int	take_dongles_last(t_coder *c, t_request *l, t_request *r)
{
	t_sim	*sim;
	int		left;
	int		right;

	sim = c->sim;
	left = c->id - 1;
	right = c->id % sim->nb_coders;
	dongle_acquire(sim, right, r);
	if (!is_running(sim))
		return (dongle_release(sim, right), 0);
	ft_log(sim, c->id, "has taken a dongle");
	dongle_acquire(sim, left, l);
	if (!is_running(sim))
	{
		dongle_release(sim, left);
		dongle_release(sim, right);
		return (0);
	}
	ft_log(sim, c->id, "has taken a dongle");
	return (1);
}
