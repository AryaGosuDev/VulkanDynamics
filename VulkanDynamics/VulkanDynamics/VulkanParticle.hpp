#ifndef __VK_PARTICLE_HPP__
#define __VK_PARTICLE_HPP__

namespace VkApplication {

	std::vector<Particle> particles;
	std::vector<Particle> fixedParticles;
	std::vector<std::unordered_map<int, Spring>> springs;
	std::vector<Spring> fixedSprings;
	extern std::vector<InstanceData> instanceData;
	float particleTimeStep = 0.01f;
	int elapsedTicks = 0;
	int totalSprings = 0;

	float UNIVERSAL_SPRING_CONSTANT = 1.0f;
	float UNIVERSAL_SPRING_DAMPENING = 0.51f;

	float FIXED_SPRING_CONSTANT = 0.5f;
	float FIXED_SPRING_DAMPENING = 0.1f;

	// TODO : restoritive forces. Eventually bring the particles back to their original locations graudally after
	// force dwindles to 0 again. Pull the particles back into place.

	glm::vec3 calculateSpringForce(Spring & spring) {
		Particle & p1 = particles[spring.particle1Indx];
		Particle & p2 = particles[spring.particle2Indx];

		glm::vec3 delta = p1.position - p2.position;
		float dist = glm::length(delta);
		glm::vec3 delta_normalized = glm::normalize(delta);

		glm::vec3 springForce = -spring.stiffness * (dist - spring.restLength) * delta_normalized;
		glm::vec3 dampingForce = -spring.dampening * (p1.velocity - p2.velocity);

		//return (springForce);
		return (springForce + dampingForce);

	}

	glm::vec3 calculateSpringForceFixed(Spring& spring) {
		Particle& p1      = particles[spring.particle1Indx];
		Particle& p2Fixed = fixedParticles[spring.particle2Indx];

		glm::vec3 delta = p1.position - p2Fixed.position;
		float dist = glm::length(delta);
		glm::vec3 delta_normalized = glm::normalize(delta);

		glm::vec3 springForce = -spring.stiffness * (dist - spring.restLength) * delta_normalized;
		glm::vec3 dampingForce = -spring.dampening * (p1.velocity);

		//return (springForce);
		return (springForce + dampingForce);

	}

	//verlet integration
	void integrate(int indx) {
		glm::vec3 temp = particles[indx].position;
		glm::vec3 acceleration = particles[indx].force / particles[indx].mass;
		glm::vec3 newPosition = 2.0f * particles[indx].position - particles[indx].oldPosition + acceleration * particleTimeStep * particleTimeStep;
		
		particles[indx].oldPosition = temp;
		particles[indx].position = newPosition;
		particles[indx].velocity = (particles[indx].position - particles[indx].oldPosition) / particleTimeStep;
	}

	//Semi-Implicit Euler Integration
	void semiImplicitEuler(int indx) {
		glm::vec3 acceleration = particles[indx].force / particles[indx].mass;
		particles[indx].velocity += acceleration * particleTimeStep; // Update velocity
		particles[indx].position += particles[indx].velocity * particleTimeStep; // Update position
	}

	void handleCollisionsAndConstraints() {
		
		//constraint for center of mass of spring. The center of mass of a spring should never move
		std::vector<bool> springUsed(totalSprings, false);
		for (int i = 0; i < particles.size(); ++i) {
			for (auto& s : springs[i]) {
				if (springUsed[s.second.springID] == false) {
					glm::vec3 centerOfMass = (particles[s.second.particle1Indx].position + particles[s.second.particle2Indx].position) / 2.0f;
					glm::vec3 displacementFromCenter = centerOfMass - s.second.CenterOfMass; // initialCenterOfMass is a constant

					particles[s.second.particle1Indx].position -= displacementFromCenter;
					particles[s.second.particle2Indx].position -= displacementFromCenter;
					springUsed[s.second.springID] = true;
				}
			}
		}
	}

	void MainVulkApplication::prepareParticleSystem() {

		particles.resize(instanceData.size());
		springs.resize(instanceData.size());
		fixedSprings.resize(8);
		for (int i = 0; i < particles.size(); ++i) {
			particles[i].position = particles[i].oldPosition = instanceData[i].pos;
			particles[i].mass = 1.0f;
			particles[i].force = glm::vec3(0.0, 0.0, 0.0);
			particles[i].velocity = glm::vec3(0.0, 0.0, 0.0);
		}
		float Lrest = glm::distance(particles[0].position, particles[1].position);
		int springID = 0;
		
		for (int i = 0; i < particles.size();  ++i) {
			
			//only these have next pointers
			if ((i + 1) % 4 != 0) {
				glm::vec3 COM = (particles[i].position + particles[i+1].position) / 2.0f;
				springs[i].insert({ i + 1 , Spring(i  , i + 1,springID ,Lrest, UNIVERSAL_SPRING_CONSTANT, UNIVERSAL_SPRING_DAMPENING,COM) });
				springs[i+1].insert({ i  ,  Spring(i+1, i ,springID,Lrest, UNIVERSAL_SPRING_CONSTANT, UNIVERSAL_SPRING_DAMPENING, COM) });
				springID++;
			}
			//only these have up pointers
			if ((i) % 16 <= 11) {
				glm::vec3 COM = (particles[i].position + particles[i + 4].position) / 2.0f;
				springs[i].insert({ i + 4 ,  Spring(i    , i + 4,springID,Lrest, UNIVERSAL_SPRING_CONSTANT, UNIVERSAL_SPRING_DAMPENING,COM) });
				springs[i + 4].insert({ i  , Spring(i + 4, i ,springID,Lrest, UNIVERSAL_SPRING_CONSTANT, UNIVERSAL_SPRING_DAMPENING,COM) });
				springID++;
			}
			//these have across pointers
			if ((i) % 64 <= 47) {
				glm::vec3 COM = (particles[i].position + particles[i + 16].position) / 2.0f;
				springs[i].insert({ i + 16 ,  Spring(i     , i + 16,springID,Lrest, UNIVERSAL_SPRING_CONSTANT, UNIVERSAL_SPRING_DAMPENING,COM) });
				springs[i + 16].insert({ i  , Spring(i + 16, i ,springID,Lrest, UNIVERSAL_SPRING_CONSTANT, UNIVERSAL_SPRING_DAMPENING,COM) });
				springID++;
			}
		}
		totalSprings = springID;
		springID = 0;
		fixedParticles.push_back(particles[0]); fixedParticles.push_back(particles[3]);
		fixedParticles.push_back(particles[12]); fixedParticles.push_back(particles[15]);
		fixedParticles.push_back(particles[48]); fixedParticles.push_back(particles[51]);
		fixedParticles.push_back(particles[60]); fixedParticles.push_back(particles[63]);
		fixedParticles[0].position.x--; fixedParticles[0].position.y--; fixedParticles[0].position.z--;
		fixedParticles[1].position.x--; fixedParticles[1].position.y--; fixedParticles[1].position.z++;
		fixedParticles[2].position.x--; fixedParticles[2].position.y++; fixedParticles[2].position.z--;
		fixedParticles[3].position.x--; fixedParticles[3].position.y++; fixedParticles[3].position.z++;
		fixedParticles[4].position.x++; fixedParticles[4].position.y--; fixedParticles[4].position.z--;
		fixedParticles[5].position.x++; fixedParticles[5].position.y--; fixedParticles[5].position.z++;
		fixedParticles[6].position.x++; fixedParticles[6].position.y++; fixedParticles[6].position.z--;
		fixedParticles[7].position.x++; fixedParticles[7].position.y++; fixedParticles[7].position.z++;
		Lrest = glm::distance(particles[0].position, fixedParticles[0].position);
		fixedSprings[springID++] = Spring(0, 0, springID, Lrest, FIXED_SPRING_CONSTANT, FIXED_SPRING_DAMPENING);
		fixedSprings[springID++] = Spring(3, 1, springID, Lrest, FIXED_SPRING_CONSTANT, FIXED_SPRING_DAMPENING);
		fixedSprings[springID++] = Spring(12, 2, springID, Lrest, FIXED_SPRING_CONSTANT, FIXED_SPRING_DAMPENING);
		fixedSprings[springID++] = Spring(15, 3, springID, Lrest, FIXED_SPRING_CONSTANT, FIXED_SPRING_DAMPENING);
		fixedSprings[springID++] = Spring(48, 4, springID, Lrest, FIXED_SPRING_CONSTANT, FIXED_SPRING_DAMPENING);
		fixedSprings[springID++] = Spring(51, 5, springID, Lrest, FIXED_SPRING_CONSTANT, FIXED_SPRING_DAMPENING);
		fixedSprings[springID++] = Spring(60, 6, springID, Lrest, FIXED_SPRING_CONSTANT, FIXED_SPRING_DAMPENING);
		fixedSprings[springID++] = Spring(63, 7, springID, Lrest, FIXED_SPRING_CONSTANT, FIXED_SPRING_DAMPENING);
	}

	void MainVulkApplication::updateParticleSystem() {

		//kick off an impulse force
		if ( keyControl.kickParticle == true && keyControl.objectPicked != -1 ) {
			//particles[0].force = glm::vec3(150.0, 20.0, 150.0);
			//integrate(0);
			particles[keyControl.objectPicked].force = glm::vec3(150.0, 20.0, 150.0);
			integrate(keyControl.objectPicked);
			//semiImplicitEuler(0);
			keyControl.kickParticle = false;
		}

		for (int i = 0; i < particles.size(); ++i) {
			particles[i].force.x = particles[i].force.y = particles[i].force.z = 0.0;
		}

		std::vector<bool> springUsed(totalSprings, false);
		int springsUsed = 0;
		
		for (int i = 0; i < particles.size(); ++i) {
			for (auto & s : springs[i]) {
				if (springUsed[s.second.springID] == false) {
					glm::vec3 springForce = calculateSpringForce(s.second);
					
					if (elapsedTicks % 100 == 0) {
						//std::cout << "Spring force : " << springForce.x << ", " << springForce.y << ", " << springForce.z << std::endl;
					}

					particles[i].force += springForce;
					particles[s.first].force -= springForce;
					springUsed[s.second.springID] = true;
					springsUsed++;
				}
			}
		}

		for (int i = 0; i < fixedSprings.size(); ++i) {
			glm::vec3 springForce = calculateSpringForceFixed(fixedSprings[i]);
			particles[fixedSprings[i].particle1Indx].force += springForce;
		}
		
		for (int i = 0; i < particles.size(); ++i) {
			integrate(i); // Update position and velocity
			//semiImplicitEuler(i);
		}
		
		//if (elapsedTicks % 500 == 0) {
			//handleCollisionsAndConstraints();
		//}
		/*
		if (elapsedTicks % 100 == 0) {
			std::cout << "Particle 0 position: " << particles[0].position.x << ", " 
				                                 << particles[0].position.y << ", "  
				                                 << particles[0].position.z << std::endl;
			std::cout << "Particle 0 velocity: " << particles[0].velocity.x << ", "
												 << particles[0].velocity.y << ", "
												 << particles[0].velocity.z << std::endl;
			std::cout << "Particle 0 force: "    << particles[0].force.x << ", "
												 << particles[0].force.y << ", "
												 << particles[0].force.z << std::endl;
			std::cout << std::endl;
		}
		*/
		//update particle positions
		for (int i = 0; i < particles.size(); ++i)
			instanceData[i].pos = particles[i].position;
		elapsedTicks++;
	}
}

#endif 