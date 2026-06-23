#include "codexion.h"

static int  init_threads(t_sim *sim, t_coder *coders, pthread_t *threads)
{
    int i;

    i = 0;
    while (i < sim->nb_coders)
    {
        coders[i].id = i + 1;
        coders[i].sim = sim;
        pthread_create(&threads[i], NULL, coder_routine, &coders[i]);
        i++;
    }
    return (1);
}

static void join_threads(t_sim *sim, pthread_t *threads)
{
    int i;

    i = 0;
    while (i < sim->nb_coders)
    {
        pthread_join(threads[i], NULL);
        i++;
    }
}

static void cleanup_all(t_sim *sim, t_coder *c, pthread_t *t)
{
    cleanup_sim(sim);
    free(c);
    free(t);
}

int main(int argc, char **argv)
{
    t_sim       sim;
    t_coder     *coders;
    pthread_t   *threads;
    pthread_t   monitor_thread;
    
    memset(&sim, 0, sizeof(t_sim));
    if (!arg_parser(argc, argv, &sim))
        return (1);
    if (!sim_init(&sim))
        return(fprintf(stderr, "Error: initialization failed\n"), 1);
    coders = malloc(sizeof(t_coder) * sim.nb_coders);
    threads = malloc(sizeof(pthread_t) * sim.nb_coders);
    if (!coders || !threads)
    {
        fprintf(stderr, "Error: malloc failed\n");
        cleanup_all(&sim, coders, threads);
        return (1);
    }
    pthread_create(&monitor_thread, NULL, monitor_routine, &sim);
    init_threads(&sim, coders, threads);
    pthread_join(monitor_thread, NULL);
    join_threads(&sim, threads);
    cleanup_all(&sim, coders, threads);
    return (0);
}
