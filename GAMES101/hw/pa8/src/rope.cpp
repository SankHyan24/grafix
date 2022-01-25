#include <iostream>
#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

namespace CGL
{

    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, vector<int> pinned_nodes)
    {
        Vector2D spring_dist = (end - start) / (num_nodes - 1);
        for (int i = 0; i < num_nodes; i++)
        {
            Vector2D position = start + i * spring_dist;
            auto wuhu = new Mass(position, node_mass, false);
            masses.push_back(wuhu);
        }
        for (int i = 1; i < num_nodes; i++)
            springs.push_back(new Spring(masses[i - 1], masses[i], k));
        for (auto &i : pinned_nodes)
            masses[i]->pinned = true;
    }

    void Rope::simulateEuler(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            if (s->m1->pinned && s->m2->pinned)
                continue;
            auto vdelta_x = s->m2->position - s->m1->position;
            auto delta_x = vdelta_x.norm() - s->rest_length;
            auto delta_v_v = dot(s->m1->velocity - s->m2->velocity, vdelta_x / vdelta_x.norm()) * vdelta_x / vdelta_x.norm2();
            auto delta_v_p = delta_v_v - (s->m1->velocity - s->m2->velocity);
            // Internal Damping for Spring
            if (!s->m1->pinned)
                s->m1->forces += s->k * vdelta_x / vdelta_x.norm() * delta_x - delta_v_v * 0.05 + delta_v_p * 0.05;
            if (!s->m2->pinned)
                s->m2->forces -= s->k * vdelta_x / vdelta_x.norm() * delta_x - delta_v_v * 0.05 + delta_v_p * 0.05;
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                // For Explicit method 确实Unstable。直接原地起飞了
                // m->forces += gravity * m->mass;
                // m->position += delta_t * m->velocity;
                // m->velocity += delta_t * (m->forces / m->mass);
                // For Semi-implicit method
                m->forces += gravity * m->mass;
                m->velocity += delta_t * (m->forces / m->mass);
                m->position += delta_t * m->velocity;
            }
            m->forces = Vector2D(0, 0);
        }
    }

    void Rope::simulateVerlet(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            if (s->m1->pinned && s->m2->pinned)
                continue;
            auto vdelta_x = s->m2->position - s->m1->position;
            auto delta_x = vdelta_x.norm() - s->rest_length;
            auto delta_v_v = dot(s->m1->velocity - s->m2->velocity, vdelta_x / vdelta_x.norm()) * vdelta_x / vdelta_x.norm2(); // v
            auto delta_v_p = delta_v_v - (s->m1->velocity - s->m2->velocity);
            if (delta_x > 0.2f * s->rest_length)
                delta_x = s->rest_length;
            if (!s->m1->pinned)
                s->m1->forces += s->k * vdelta_x / vdelta_x.norm() * delta_x - delta_v_v * 0.05 + delta_v_p * 0.05;
            if (!s->m2->pinned)
                s->m2->forces -= s->k * vdelta_x / vdelta_x.norm() * delta_x - delta_v_v * 0.05 + delta_v_p * 0.05;
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                auto temp = m->position;
                float damping_factor = 0.00005;
                m->forces += gravity * m->mass;
                m->position = m->position + (1 - damping_factor) * (m->position - m->last_position) + (m->forces / m->mass) * delta_t * delta_t;
                m->velocity += delta_t * (m->forces / m->mass);
                m->last_position = temp;
            }
            m->forces = Vector2D(0, 0);
        }
    }
}
