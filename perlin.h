#ifndef PERLIN_H
#define PERLIN_H

#include "rtweekend.h"

class perlin {
    public:
        perlin() {
            for (int i = 0; i < point_count; i++) {
                randfloat[i] = random_float();
                randvec[i] = glm::normalize(glm::vec3(random_float(-1,1), random_float(-1,1), random_float(-1,1)));
            }

            perlin_generate_perm(perm_x);
            perlin_generate_perm(perm_y);
            perlin_generate_perm(perm_z);
        }

        double noise_unsmoothed(const glm::vec3& p) const {
            // ----- Unsmoothed noise -----
            // - A point p is hashed based on its component values, and the hash is used as an index into randfloat

            int i = int(4 * p.x) & 255;
            int j = int(4 * p.y) & 255;
            int k = int(4 * p.z) & 255;

            int hash = perm_x[i] ^ perm_y[j] ^ perm_z[k];
            return randfloat[hash];
        }

        double noise_trilinear_interp(const glm::vec3& p) {
            // ----- Trilinearly interpolated noise -----
            // - The voxel (i,j,k) in which point p is contained is computed using std::floor
            // - Each voxel corresponds to some random float value given by its hashed index into randfloat
            // - The values of the 2x2x2 array of voxels (c) surrounding point p's voxel are trilinearly interpolated
            // based on p's subvoxel location to obtain the result
            
            // Subvoxel location to sample
            double u = p.x - std::floor(p.x);
            double v = p.y - std::floor(p.y);
            double w = p.z - std::floor(p.z);

            // Hermitian smoothing (to eliminate obvious grid features)
            u = u*u*(3-2*u);
            v = v*v*(3-2*v);
            w = w*w*(3-2*w);

            // Center of voxel in which p is contained
            int i = std::floor(p.x);
            int j = std::floor(p.y);
            int k = std::floor(p.z);

            // Obtain noise coefficients
            double c[2][2][2];
            for (int di = 0; di < 2; di++)
                for (int dj = 0; dj < 2; dj++)
                    for (int dk = 0; dk < 2; dk++) {
                        c[di][dj][dk] = randfloat[
                            perm_x[(i+di) & 255] ^
                            perm_y[(j+dj) & 255] ^
                            perm_z[(k+dk) & 255]
                        ];
                    }

            // Interpolate noise coefficients using subvoxel location
            return trilinear_interp(c, u, v, w);
        }

        double noise_perlin_interp(const glm::vec3& p) const {
            // ----- Perlin interpolated noise -----
            // - The voxel (i,j,k) in which point p is contained is computed using std::floor
            // - Each voxel corresponds to some random unit vec3 given by its hashed index into randvec
            // - The vec3 values of the 2x2x2 array of voxels (c) surrounding point p's voxel are Perlin-interpolated
            // based on p's subvoxel location to obtain the result
            
            // Subvoxel location to sample
            double u = p.x - std::floor(p.x);
            double v = p.y - std::floor(p.y);
            double w = p.z - std::floor(p.z);

            // Center of voxel in which p is contained
            int i = std::floor(p.x);
            int j = std::floor(p.y);
            int k = std::floor(p.z);

            // Obtain noise vectors
            glm::vec3 c[2][2][2];
            for (int di = 0; di < 2; di++)
                for (int dj = 0; dj < 2; dj++)
                    for (int dk = 0; dk < 2; dk++) {
                        int hash = perm_x[(i+di) & 255] ^ perm_y[(j+dj) & 255] ^ perm_z[(k+dk) & 255];
                        c[di][dj][dk] = randvec[hash];
                    }

            // Interpolate noise coefficients using subvoxel location
            return perlin_interp(c, u, v, w);
        }

        double turb(const glm::vec3& p, int depth) const {
            // ----- Turbulence -----
            // - Generate composite noise by summing noise at multiple frequencies

            double accum = 0.0;
            glm::vec3 temp_p = p;
            double weight = 1.0;

            for (int i = 0; i < depth; i++) {
                accum += weight * noise_perlin_interp(temp_p);
                weight *= 0.5;
                temp_p *= 2;
            }

            return std::fabs(accum);
        }
    
    private:
        static const int point_count = 256;
        double randfloat[point_count];
        glm::vec3 randvec[point_count];
        int perm_x[point_count];
        int perm_y[point_count];
        int perm_z[point_count];

        static void perlin_generate_perm(int* p) {
            // Fill p
            for (int i = 0; i < point_count; i++)
                p[i] = i;
            // Randomly permute p's elements
            permute(p, point_count);
        }

        static void permute(int* p, int n) {
            // Randomly swap elements of array p up from elements n-1 to 0
            for (int i = n-1; i > 0; i--) {
                int target = random_int(0, i);
                int tmp = p[i];
                p[i] = p[target];
                p[target] = tmp;
            }
        }

        static double trilinear_interp(double c[2][2][2], double u, double v, double w) {
            // c: array of random floats
            // u,v,w: decimal portions of p's x,y,z components
            
            // Accumulate values from each voxel based on:
            // - The voxel's float value weighted by the subvoxel's distance from the voxel center
            // - Note: No normalization factor needed at the end because distances (which are < 1) accumulate to a factor of 1/8
            double accum = 0.0;
            for (int i = 0; i < 2; i++)
                for (int j = 0; j < 2; j++)
                    for (int k = 0; k < 2; k++) {
                        accum += (i*u + (1-i)*(1-u))
                               * (j*v + (1-j)*(1-v))
                               * (k*w + (1-k)*(1-w))
                               * c[i][j][k];
                    }
            return accum;
        }

        static double perlin_interp(glm::vec3 c[2][2][2], double u, double v, double w) {
            // c: array of random unit vec3
            // u,v,w: decimal portions of p's x,y,z components

            // Hermitian smoothing (to eliminate obvious grid features)
            double uu = u*u*(3-2*u);
            double vv = v*v*(3-2*v);
            double ww = w*w*(3-2*w);
            
            // Accumulate values from each voxel based on:
            // - A dot product between the voxel's random unit vec3 and a weight vector based on the 
            // position of the subvoxel center relative to the voxel center
            // - The subvoxel's cubic Hermite distance from the voxel center
            // - Note: No normalization factor needed at the end because distances (which are < 1) accumulate to a factor of 1/8 
            double accum = 0.0;
            for (int i = 0; i < 2; i++)
                for (int j = 0; j < 2; j++)
                    for (int k = 0; k < 2; k++) {
                        glm::vec3 weight_v(u-i, v-j, w-k);
                        accum += (i*uu + (1-i)*(1-uu))
                               * (j*vv + (1-j)*(1-vv))
                               * (k*ww + (1-k)*(1-ww))
                               * glm::dot(c[i][j][k], weight_v);
                    }
            return accum;
        }
};

#endif