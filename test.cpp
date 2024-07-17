#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>

int width = 1000;
int height = 700;
int num_particles = 200;
float gravity = 0.5;
float t = 1;
float restitution = 0.9;
int time_elapsed = 0;

struct Particle {
    float x_velocity;
    float y_velocity;
    int x;
    int y;
    int vy0;
    int radius;
    sf::Color color;

    Particle(int x_, int y_, float x_velocity_, float y_velocity_, int radius_, int c1_, int c2_, int c3_, int c4_) { // Constructor
        sf::Color temp(c1_, c2_, c3_, c4_);
        x = x_;
        y = y_;
        x_velocity = x_velocity_ * t;
        y_velocity = y_velocity_ * t;
        vy0 = y_velocity_ * t;
        radius = radius_;
        color = temp;
    }
};

void resolveCollision(Particle &a, Particle &b, float restitution) {
    // Calculate the normal vector
    float nx = b.x - a.x;
    float ny = b.y - a.y;
    float distance = sqrt(nx * nx + ny * ny);
    nx /= distance;
    ny /= distance;

    // Calculate relative velocity
    float vx_rel = b.x_velocity - a.x_velocity;
    float vy_rel = b.y_velocity - a.y_velocity;

    // Calculate relative velocity in terms of the normal direction
    float vel_rel_norm = vx_rel * nx + vy_rel * ny;

    // If velocities are separating, do nothing
    if (vel_rel_norm > 0) return;

    // Calculate impulse scalar
    float impulse = (-(1 + restitution) * vel_rel_norm) / (1 / a.radius + 1 / b.radius);

    // Apply impulse to the particles
    a.x_velocity -= impulse * nx / a.radius;
    a.y_velocity -= impulse * ny / a.radius;
    b.x_velocity += impulse * nx / b.radius;
    b.y_velocity += impulse * ny / b.radius;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(width, height), "p2"); // Create the window
    window.setFramerateLimit(60); // Set the frame rate

    std::random_device rd; // Random number generator
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(2, 10);
    std::uniform_int_distribution<> orientation(0, 1);
    std::uniform_int_distribution<> r(5, 20);
    std::uniform_int_distribution<> c(0, 255);

    std::vector<Particle> particles;

    for (int i = 0; i < num_particles; i++) {
        if (orientation(gen) % 2 == 0) {
            Particle particle(width / 2, 0, distr(gen), distr(gen), r(gen), c(gen), c(gen), c(gen), 255);
            particles.push_back(particle);
        } else {
            Particle particle(width / 2, 0, -distr(gen), -distr(gen), r(gen), c(gen), c(gen), c(gen), 255);
            particles.push_back(particle);
        }
    }

    std::vector<sf::CircleShape> circles;

    for (int i = 0; i < particles.size(); i++) {
        sf::CircleShape circle(particles[i].radius);
        circle.setFillColor(particles[i].color);
        circles.push_back(circle);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        for (int i = 0; i < circles.size(); i++) {
            circles[i].setPosition(particles[i].x, particles[i].y);
            if (particles[i].x < 0 || particles[i].x > width - 2 * particles[i].radius) {
                particles[i].x_velocity = -restitution * particles[i].x_velocity;
            }
            if (particles[i].y < 0 || particles[i].y > height - 2 * particles[i].radius) {
                particles[i].y_velocity = -restitution * particles[i].y_velocity;
            }

            particles[i].x += particles[i].x_velocity * t;
            particles[i].y += particles[i].y_velocity * t;
            particles[i].y_velocity += gravity;

            window.draw(circles[i]);
        }

        if (time_elapsed >= 60) {
            for (int i = 0; i < particles.size(); i++) {
                for (int k = i + 1; k < particles.size(); k++) {
                    float dx = particles[i].x - particles[k].x;
                    float dy = particles[i].y - particles[k].y;
                    float distance = sqrt(dx * dx + dy * dy);
                    if (distance <= particles[i].radius + particles[k].radius) {
                        resolveCollision(particles[i], particles[k], restitution);
                    }
                }
            }
        } else {
            time_elapsed++;
        }

        window.display();
    }

    return 0;
}
