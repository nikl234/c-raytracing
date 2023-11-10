#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "stl.h"
#include "ray.h"
#include <pthread.h>
#include <sys/sysinfo.h>
#include "matrix.h"
#include <math.h>

typedef struct Job{
    Hit *hits;
    Object* obj;
    bool* hitted;
    int rayStart, rayEnd;
    Ray *rays;
}Job;

void depthShader (Job* job, int index){
    Ray ray = job->rays[index];
    
    Hit hit = job->hits[index];
    job->hitted[index] = rayIntersectsObject(ray, *job->obj, &hit);
}

void* render (void* jobPtr){
    Job* job = (Job*)jobPtr;

    for (int i = job->rayStart; i<job->rayEnd; i+=1){
        depthShader(job, i);
    }
    free(job);
    return NULL;
}

int main(int argc, char* argv[]){
    int numThreads = get_nprocs();
    int ray_count = 1024;

    pthread_t inc_x_thread[numThreads];

    STLFile stl = loadSTL(argv[1]);
    Object obj;

    obj.Pos = (V3f){0,0,0};
    obj.STL = &stl;
    obj.Color = (V3f){255,255,255}; // white

    Hit (*hits) = malloc(ray_count * sizeof(Hit));
    bool (*hitted) = malloc(ray_count);
    Ray (*rays) = malloc(ray_count * sizeof(Ray));

    // initiallize random rays
    for (int i = 0; i < ray_count; i++) {
        // make the rays random
    }

    // split the frame into numThreads horizontally.
    for (int i=0; i<numThreads; i++){
        Job* job = malloc(sizeof(Job));
        job->obj = &obj;
        job->hits = hits;
        job->hitted = hitted;
        job->rays = rays;

        job->rayStart = i * ray_count / numThreads;
        job->rayEnd = (i + 1) * ray_count / numThreads;

        if(pthread_create(&inc_x_thread[i], NULL, render, job)) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    for (int i=0; i<numThreads; i++){
        if(pthread_join(inc_x_thread[i], NULL)) {

            fprintf(stderr, "Error joining thread\n");
            return 2;

        }
    }
    
    free (stl.triangle);

    free(hits);
    free(hitted);
    free(rays);
}
