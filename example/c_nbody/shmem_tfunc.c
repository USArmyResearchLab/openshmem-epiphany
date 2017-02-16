/*
 * Copyright (c) 2015-2017, James A. Ross
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* JAR */

#include "coprthr.h"
#include "shmem.h"
#include "shmemx.h"
#include "nbody.h"

void __entry
nbody_thread( my_args_t* args )
{
	int n = args->n;
	int iter = args->iter;
	float dt = args->dt;
	float es = args->es;
	Particle *particles = args->p;
	ParticleV *state = args->v;

	shmem_init();
	int me = shmem_my_pe();
	int npes = shmem_n_pes();

	int npart = n / npes;
	size_t nszp = npart*sizeof(Particle);
	size_t nszv = npart*sizeof(ParticleV);

	// Allocate/Load initial values with high performance memcpy
	Particle* my_particles = (Particle*)shmem_malloc(nszp);
	ParticleV* my_state = (ParticleV*)shmem_malloc(nszv);
	shmemx_memcpy(my_particles, particles + npart*me, nszp);
	shmemx_memcpy(my_state, state + npart*me, nszv);

	// iteration loop
	while (iter--) {
		// Block for all particle positions to be updated
		shmem_barrier_all();
		// Looping over all processing elements
		for (int i = 0; i < npes; i++) {
			// Demonstrates RDMA feature of Epiphany/SHMEM
			Particle* others = shmem_ptr(my_particles, i);
			// Compute acceleration on local particles due to neighboring particles
			ComputeAccel(my_particles, others, my_state, npart, es);
		}
		shmem_barrier_all();
		// Update positions and velocities due to accelerations
		ComputeNewPos(my_particles, my_state, npart, dt);
	}

	// Copying local results to DRAM
	shmemx_memcpy(particles + npart*me, my_particles, nszp);
	shmemx_memcpy(state + npart*me, my_state, nszv);

	shmem_free(my_state);
	shmem_free(my_particles);

	shmem_finalize();
}
