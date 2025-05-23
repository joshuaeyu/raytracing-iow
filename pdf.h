#ifndef PDF_H
#define PDF_H

#include "rtweekend.h"
#include "hittable_list.h"
#include "onb.h"

class pdf {
    public:
        virtual ~pdf() {}

        virtual double value(const glm::vec3& direction) const = 0;
        virtual glm::vec3 generate() const = 0;
};

class sphere_pdf : public pdf {
    // Uniform density over unit sphere
    public:
        sphere_pdf() {}

        double value(const glm::vec3& direction) const override {
            return 1 / (4 * pi);
        }

        glm::vec3 generate() const override {
            return random_unit_vector();
        }
};

class cosine_pdf : public pdf {
    // Cosine density
    public:
        cosine_pdf(const glm::vec3& w)
         : uvw(w) {}

        double value(const glm::vec3& direction) const override {
            double cosine_theta = glm::dot(uvw.w(), glm::normalize(direction));
            return std::fmax(0, cosine_theta / pi);
        }

        glm::vec3 generate() const override {
            return uvw.transform(random_cosine_direction());
        }
    
    private:
        onb uvw;
};

class hittable_pdf : public pdf {
    // Sample directions towards a hittable (e.g., light)
    public:
        hittable_pdf(const hittable& objects, const glm::vec3& origin)
         : objects(objects), origin(origin) {}

        double value(const glm::vec3& direction) const override {
            return objects.pdf_value(origin, direction);
        }

        glm::vec3 generate() const override {
            return objects.random(origin);
        }
    
    private:
        const hittable& objects;
        glm::vec3 origin;
};

class mixture_pdf : public pdf {
    // Evenly weighted linear mixture of two PDFs
    public:
        mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1) {
            p[0] = p0;
            p[1] = p1;
        }

        double value(const glm::vec3& direction) const override {
            return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
        }

        glm::vec3 generate() const override {
            if (random_double() < 0.5)
                return p[0]->generate();
            else
                return p[1]->generate();
        }
    
    private:
        std::shared_ptr<pdf> p[2];
};



#endif 