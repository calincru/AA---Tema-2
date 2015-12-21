#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <algorithm>

using graph_t = std::vector<std::unordered_set<int>>;

int main() {
    std::ifstream fin{"test.in"};

    int N, M;
    fin >> N >> M;

    auto graph = graph_t(N);
    while (M--) {
        int u, v;
        fin >> u >> v, --u, --v;

        graph[u].emplace(v);
        graph[v].emplace(u);
    }

    auto perm = std::vector<int>(N);
    for (auto i = 0; i < N; ++i)
        perm[i] = i;

    do {
        bool is_ham = true;

        for (auto i = 0; i < N - 1; ++i) {
            if (!graph[perm[i]].count(perm[i + 1])) {
                is_ham = false;
                break;
            }
        }
        if (is_ham) {
            std::cout << "1" << std::endl;
            return 0;
        }
    } while (std::next_permutation(perm.begin(), perm.end()));

    std::cout << "0" << std::endl;
}
