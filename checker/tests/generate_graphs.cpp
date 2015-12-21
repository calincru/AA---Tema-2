#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>

int main() {
    // Data to be generated: (how_many, number_vertices, {p0, p1})
    struct data_t {
        int count;
        int vertices;
        std::initializer_list<double> weights;
    } data[] = {
        {3, 4, {.50, .50}},
        {6, 4, {.40, .60}},

        {3, 5, {.50, .50}},
        {6, 5, {.40, .60}},

        {2, 6, {.45, .55}},
        {1, 6, {.40, .60}},
        {1, 6, {.35, .65}},
        {1, 6, {.30, .70}},

        {1, 7, {.35, .65}},
        {1, 7, {.30, .70}},
    };

    // Random generator settings
    std::random_device rd{};
    std::mt19937 mt{rd()};

    for (auto k = 0u, i = 0u; i < sizeof data/sizeof *data; ++i) {
        auto dist = std::discrete_distribution<int>(data[i].weights);

        for (auto j = 0; j < data[i].count; ++j) {
            auto edges = std::vector<std::pair<int, int>>{};

            // Generate random edges.
            for (auto u = 1; u <= data[i].vertices; ++u) {
                for (auto v = u + 1; v <= data[i].vertices; ++v) {
                    if (v != u && !dist(mt)) {
                        auto d = dist(mt);
                        edges.push_back({d ? u : v, d ? v : u});
                    }
                }
            }
            std::shuffle(edges.begin(), edges.end(), mt);

            std::ofstream fout{"test" + std::to_string(++k) + ".in"};
            fout << data[i].vertices << " " << edges.size() << "\n";
            for (auto& p: edges) {
                fout << p.first << " " << p.second << "\n";
            }
        }
    }
}
