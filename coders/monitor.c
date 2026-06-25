#include "codexion.h"

int check_all_done(t_sim *sim)
{
    int done;

    pthread_mutex_lock(&sim->running_mutex);
    done = (sim->finished >= sim->nb_coders);
    pthread_mutex_unlock(&sim->running_mutex);
    return (done);
}

static void stop_sim(t_sim *sim)
{
    pthread_mutex_lock(&sim->running_mutex);
    sim->running = 0;
    pthread_mutex_unlock(&sim->running_mutex);
}

void    mark_done(t_sim *sim)
{
    pthread_mutex_lock(&sim->running_mutex);
    sim->finished++;
    pthread_mutex_unlock(&sim->running_mutex);
}

static int  check_burnout_loop(t_sim *sim, long long now)
{
    int         i;
    long long   since_last;

    i = 0;
    while (i < sim->nb_coders)
    {
        since_last = now - sim->last_compile_start[i];
        if (since_last >= sim->time_to_burnout)
        {
            ft_log_burnout(sim, i + 1);
            return (1);
        }
        i++;
    }
    return (0);
}

void    *monitor_routine(void *arg)
{
    t_sim       *sim;
    long long   now;

    sim = (t_sim *)arg;
    while (1)
    {
        usleep(1000);
        if (!is_running(sim))
            break ;
        now = get_time_ms();
        if (check_burnout_loop(sim, now))
            return (NULL);
        if (check_all_done(sim))
        {
            stop_sim(sim);
            break ;
        }
    }
    return (NULL);
}