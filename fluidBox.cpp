#include "fluidBox.h"

#include "include\freeglut\freeglut.h"
#include <algorithm>

using namespace std;

FluidBox::FluidBox() : sizeX(1), sizeY(1), sizeZ(1), diffusion(0.0), viscosity(0.1) {
	this->transform = Transform();

	this->s = new float[sizeX * sizeY * sizeZ];
	this->density = new float[sizeX * sizeY * sizeZ];

	this->Vx = new float[sizeX * sizeY * sizeZ];
	this->Vy = new float[sizeX * sizeY * sizeZ];
	this->Vz = new float[sizeX * sizeY * sizeZ];

	this->Vx0 = new float[sizeX * sizeY * sizeZ];
	this->Vy0 = new float[sizeX * sizeY * sizeZ];
	this->Vz0 = new float[sizeX * sizeY * sizeZ];
}

FluidBox::FluidBox(int sizeX, int sizeY, int sizeZ, float diffusion, float viscosity) : sizeX(sizeX), sizeY(sizeY), sizeZ(sizeZ), diffusion(diffusion), viscosity(viscosity) {
	this->transform = Transform();

	this->s = new float[sizeX * sizeY * sizeZ];
	this->density = new float[sizeX * sizeY * sizeZ];

	this->Vx = new float[sizeX * sizeY * sizeZ];
	this->Vy = new float[sizeX * sizeY * sizeZ];
	this->Vz = new float[sizeX * sizeY * sizeZ];

	this->Vx0 = new float[sizeX * sizeY * sizeZ];
	this->Vy0 = new float[sizeX * sizeY * sizeZ];
	this->Vz0 = new float[sizeX * sizeY * sizeZ];

	//TODO find a better way to initialize everything as 0?
	for (int k = 0; k < sizeZ; k++) {
		for (int j = 0; j < sizeY; j++) {
			for (int i = 0; i < sizeX; i++) {
				this->s[IX(i, j, k)] = 0.0;
				this->density[IX(i, j, k)] = 0.0;

				this->Vx[IX(i, j, k)] = 0.0;
				this->Vy[IX(i, j, k)] = 0.0;
				this->Vz[IX(i, j, k)] = 0.0;

				this->Vx0[IX(i, j, k)] = 0.0;
				this->Vy0[IX(i, j, k)] = 0.0;
				this->Vz0[IX(i, j, k)] = 0.0;
			}
		}
	}

}

FluidBox::~FluidBox() {
	delete[] this->s;
	delete[] this->density;

	delete[] this->Vx;
	delete[] this->Vy;
	delete[] this->Vz;

	delete[] this->Vx0;
	delete[] this->Vy0;
	delete[] this->Vz0;
}

Transform* FluidBox::getTransform() {
	return &(this->transform);
}

void FluidBox::setTransform(Transform t) {
	this->transform = t;
}

void FluidBox::render() {
	glPushMatrix();
	{
		for (int k = 0; k < sizeZ; k++) {
			for (int j = 0; j < sizeY; j++) {
				for (int i = 0; i < sizeX; i++) {
					float d = density[IX(i, j, k)] / 255;
					d = std::max(0.0f, std::min(1.0f, d));
					transform.setMatrix();
					glTranslatef(i - sizeX / 2, j - sizeY / 2, k - sizeZ / 2);
					glutSolidSphere(d, 8, 8);
				}
			}
		}
	}
	glPopMatrix();
}

void FluidBox::addDensity(ivec3 cell, float amount) {
	density[IV(cell)] += amount;
}

void FluidBox::addVelocity(ivec3 cell, vec3 amount) {
	int index = IV(cell);

	Vx[index] += amount.x;
	Vy[index] += amount.y;
	Vz[index] += amount.z;

}

void FluidBox::step(float dt) {
	//TODO REMOVE THIS IS JUST A TESTER
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			for (int k = -1; k < 2; k++) {
				float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				addDensity(ivec3(sizeX / 2 + i, sizeY / 2 + j, sizeZ / 2 + k), r * 50);
				float rx = -1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - (-1))));
				float ry = -1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - (-1))));
				float rz = -1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - (-1))));
				addVelocity(ivec3(sizeX / 2 + i, sizeY / 2 + j, sizeZ / 2 + k), vec3(0, -1, 0));
			}
		}
	}

	/*for (int i = 2; i < sizeX - 2; i++) {
		for (int j = 2; j < sizeY - 2; j++) {
			for (int k = 2; k < sizeZ - 2; k++) {
				float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				if (density[IX(i,j,k)] > 0.01)
				addVelocity(ivec3(i,j,k), vec3(0, -1 * dt, 0));
			}
		}
	} */

	diffuse(1, Vx0, Vx, viscosity, dt);
	diffuse(2, Vy0, Vy, viscosity, dt);
	diffuse(3, Vz0, Vz, viscosity, dt);

	project(Vx0, Vy0, Vz0, Vx, Vy);

	advect(1, Vx, Vx0, Vx0, Vy0, Vz0, dt);
	advect(2, Vy, Vy0, Vx0, Vy0, Vz0, dt);
	advect(3, Vz, Vz0, Vx0, Vy0, Vz0, dt);

	project(Vx, Vy, Vz, Vx0, Vy0);

	diffuse(0, s, density, diffusion, dt);
	advect(0, density, s, Vx, Vy, Vz, dt);

	fade();
}

void FluidBox::set_bnd(int b, float* x) {
	for (int j = 1; j < sizeY - 1; j++) {
		for (int i = 1; i < sizeX - 1; i++) {
			x[IX(i, j, 0)] = b == 3 ? -x[IX(i, j, 1)] : x[IX(i, j, 1)];
			x[IX(i, j, sizeZ - 1)] = b == 3 ? -x[IX(i, j, sizeZ - 2)] : x[IX(i, j, sizeZ - 2)];
		}
	}
	for (int k = 1; k < sizeZ - 1; k++) {
		for (int i = 1; i < sizeX - 1; i++) {
			x[IX(i, 0, k)] = b == 2 ? -x[IX(i, 1, k)] : x[IX(i, 1, k)];
			x[IX(i, sizeY - 1, k)] = b == 2 ? -x[IX(i, sizeY - 2, k)] : x[IX(i, sizeY - 2, k)];
		}
	}
	for (int k = 1; k < sizeZ - 1; k++) {
		for (int j = 1; j < sizeY - 1; j++) {
			x[IX(0, j, k)] = b == 1 ? -x[IX(1, j, k)] : x[IX(1, j, k)];
			x[IX(sizeX - 1, j, k)] = b == 1 ? -x[IX(sizeX - 2, j, k)] : x[IX(sizeX - 2, j, k)];
		}
	}

	x[IX(0, 0, 0)] = 0.33f * (x[IX(1, 0, 0)]
		+ x[IX(0, 1, 0)]
		+ x[IX(0, 0, 1)]);
	x[IX(0, sizeY - 1, 0)] = 0.33f * (x[IX(1, sizeY - 1, 0)]
		+ x[IX(0, sizeY - 2, 0)]
		+ x[IX(0, sizeY - 1, 1)]);
	x[IX(0, 0, sizeZ - 1)] = 0.33f * (x[IX(1, 0, sizeZ - 1)]
		+ x[IX(0, 1, sizeZ - 1)]
		+ x[IX(0, 0, sizeZ - 2)]); //changed from size to size -2 as I think this was a mistake
	x[IX(0, sizeY - 1, sizeZ - 1)] = 0.33f * (x[IX(1, sizeY - 1, sizeZ - 1)]
		+ x[IX(0, sizeY - 2, sizeZ - 1)]
		+ x[IX(0, sizeY - 1, sizeZ - 2)]);
	x[IX(sizeX - 1, 0, 0)] = 0.33f * (x[IX(sizeX - 2, 0, 0)]
		+ x[IX(sizeX - 1, 1, 0)]
		+ x[IX(sizeX - 1, 0, 1)]);
	x[IX(sizeX - 1, sizeY - 1, 0)] = 0.33f * (x[IX(sizeX - 2, sizeY - 1, 0)]
		+ x[IX(sizeX - 1, sizeY - 2, 0)]
		+ x[IX(sizeX - 1, sizeY - 1, 1)]);
	x[IX(sizeX - 1, 0, sizeZ - 1)] = 0.33f * (x[IX(sizeX - 2, 0, sizeZ - 1)]
		+ x[IX(sizeX - 1, 1, sizeZ - 1)]
		+ x[IX(sizeX - 1, 0, sizeZ - 2)]);
	x[IX(sizeX - 1, sizeY - 1, sizeZ - 1)] = 0.33f * (x[IX(sizeX - 2, sizeY - 1, sizeZ - 1)]
		+ x[IX(sizeX - 1, sizeY - 2, sizeZ - 1)]
		+ x[IX(sizeX - 1, sizeY - 1, sizeZ - 2)]);
}

void FluidBox::lin_solve(int b, float* x, float* x0, float a, float c) {
	float cRecip = 1.0 / c;
	for (int m = 0; m < iter; m++) {
		for (int k = 1; k < sizeZ - 1; k++) {
			for (int j = 1; j < sizeY - 1; j++) {
				for (int i = 1; i < sizeX - 1; i++) {
					x[IX(i, j, k)] =
						(x0[IX(i, j, k)]
							+ a * (x[IX(i + 1, j, k)]
								+ x[IX(i - 1, j, k)]
								+ x[IX(i, j + 1, k)]
								+ x[IX(i, j - 1, k)]
								+ x[IX(i, j, k + 1)]
								+ x[IX(i, j, k - 1)]
								)) * cRecip;
				}
			}
		}
		set_bnd(b, x);
	}
}

void FluidBox::diffuse(int b, float* x, float* x0, float diff, float dt) {
	float mult; //Edited for variable size dimensions
	switch (b) {
	case 1:
		mult = (sizeY - 2) * (sizeZ - 2);
		break;
	case 2:
		mult = (sizeX - 2) * (sizeZ - 2);
		break;
	case 3:
		mult = (sizeX - 2) * (sizeY - 2);
	default:
		mult = (sizeX - 2) * (sizeY - 2) * (sizeZ - 2);
		break;
	}

	float a = dt * diff * mult;
	lin_solve(b, x, x0, a, 1 + 6 * a);
}

void FluidBox::project(float* velocX, float* velocY, float* velocZ, float* p, float* div) {
	for (int k = 1; k < sizeZ - 1; k++) {
		for (int j = 1; j < sizeY - 1; j++) {
			for (int i = 1; i < sizeX - 1; i++) {
				div[IX(i, j, k)] = -0.5f * (
					velocX[IX(i + 1, j, k)]
					- velocX[IX(i - 1, j, k)]
					+ velocY[IX(i, j + 1, k)]
					- velocY[IX(i, j - 1, k)]
					+ velocZ[IX(i, j, k + 1)]
					- velocZ[IX(i, j, k - 1)]
					) / (sizeX + sizeY + sizeZ); //Edited from just size to take account for all directions
				p[IX(i, j, k)] = 0;
			}
		}
	}
	set_bnd(0, div);
	set_bnd(0, p);
	lin_solve(0, p, div, 1, 6);

	for (int k = 1; k < sizeZ - 1; k++) {
		for (int j = 1; j < sizeY - 1; j++) {
			for (int i = 1; i < sizeX - 1; i++) {
				velocX[IX(i, j, k)] -= 0.5f * (p[IX(i + 1, j, k)]
					- p[IX(i - 1, j, k)]) * sizeX;
				velocY[IX(i, j, k)] -= 0.5f * (p[IX(i, j + 1, k)]
					- p[IX(i, j - 1, k)]) * sizeY;
				velocZ[IX(i, j, k)] -= 0.5f * (p[IX(i, j, k + 1)]
					- p[IX(i, j, k - 1)]) * sizeZ;
			}
		}
	}
	set_bnd(1, velocX);
	set_bnd(2, velocY);
	set_bnd(3, velocZ);
}

void FluidBox::advect(int b, float* d, float* d0, float* velocX, float* velocY, float* velocZ, float dt) {
	float i0, i1, j0, j1, k0, k1;

	float dtx = dt * (sizeX - 2);
	float dty = dt * (sizeY - 2);
	float dtz = dt * (sizeZ - 2);

	float s0, s1, t0, t1, u0, u1;
	float tmp1, tmp2, tmp3, x, y, z;

	float NfloatX = sizeX;
	float NfloatY = sizeY;
	float NfloatZ = sizeZ;
	float ifloat, jfloat, kfloat;
	int i, j, k;

	for (k = 1, kfloat = 1; k < sizeZ - 1; k++, kfloat++) {
		for (j = 1, jfloat = 1; j < sizeY - 1; j++, jfloat++) {
			for (i = 1, ifloat = 1; i < sizeX - 1; i++, ifloat++) {
				tmp1 = dtx * velocX[IX(i, j, k)];
				tmp2 = dty * velocY[IX(i, j, k)];
				tmp3 = dtz * velocZ[IX(i, j, k)];
				x = ifloat - tmp1;
				y = jfloat - tmp2;
				z = kfloat - tmp3;

				if (x < 0.5f) x = 0.5f;
				if (x > NfloatX + 0.5f) x = NfloatX + 0.5f;
				i0 = floorf(x);
				i1 = i0 + 1.0f;
				if (y < 0.5f) y = 0.5f;
				if (y > NfloatY + 0.5f) y = NfloatY + 0.5f;
				j0 = floorf(y);
				j1 = j0 + 1.0f;
				if (z < 0.5f) z = 0.5f;
				if (z > NfloatZ + 0.5f) z = NfloatZ + 0.5f;
				k0 = floorf(z);
				k1 = k0 + 1.0f;

				s1 = x - i0;
				s0 = 1.0f - s1;
				t1 = y - j0;
				t0 = 1.0f - t1;
				u1 = z - k0;
				u0 = 1.0f - u1;

				int i0i = i0;
				int i1i = i1;
				int j0i = j0;
				int j1i = j1;
				int k0i = k0;
				int k1i = k1;

				//This is an ugly fix, maybe try to figure out math and fix
				i0i = std::max(0, std::min(sizeX - 1, i0i));
				i1i = std::max(0, std::min(sizeX - 1, i1i));
				j0i = std::max(0, std::min(sizeY - 1, j0i));
				j1i = std::max(0, std::min(sizeY - 1, j1i));
				k0i = std::max(0, std::min(sizeZ - 1, k0i));
				k1i = std::max(0, std::min(sizeZ - 1, k1i));

				d[IX(i, j, k)] =

					s0 * (t0 * (u0 * d0[IX(i0i, j0i, k0i)]
						+ u1 * d0[IX(i0i, j0i, k1i)])
						+ (t1 * (u0 * d0[IX(i0i, j1i, k0i)]
							+ u1 * d0[IX(i0i, j1i, k1i)])))
					+ s1 * (t0 * (u0 * d0[IX(i1i, j0i, k0i)]
						+ u1 * d0[IX(i1i, j0i, k1i)])
						+ (t1 * (u0 * d0[IX(i1i, j1i, k0i)]
							+ u1 * d0[IX(i1i, j1i, k1i)])));
			}
		}
	}
	set_bnd(b, d);
}

void FluidBox::fade() {
	for (int k = 0; k < sizeZ; k++) {
		for (int j = 0; j < sizeY; j++) {
			for (int i = 0; i < sizeX; i++) {
				density[IX(i, j, k)] *= (1 - fadeAmt);
			}
		}
	}
}