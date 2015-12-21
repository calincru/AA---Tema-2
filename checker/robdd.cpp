///
// A ROBDD based implementation of boolean functions equivalence.
//
// Calin Cruceru <calin.cruceru@cti.pub.ro> (courtesy to MP)
///
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <memory>
#include <algorithm>
#include <cassert>

#ifdef DEBUG
# define BDD_ASSERT(cond) assert(cond)
#else
# define BDD_ASSERT(cond)
#endif

namespace bdd {

///
// BDD type
///
struct bdd_t;
using bdd_ptr = std::shared_ptr<const bdd_t>;

struct bdd_t {
    const bdd_ptr low;
    const bdd_ptr high;
    const std::string id;

    bdd_t(bdd_ptr low_, bdd_ptr high_, std::string id_)
        : low(std::move(low_))
        , high(std::move(high_))
        , id(std::move(id_))
    {}
}; // struct bdd_t

namespace detail {
    namespace consts {
        static const bdd_ptr zero
            = std::make_shared<bdd_t>(nullptr, nullptr, "0");
        static const bdd_ptr one
            = std::make_shared<bdd_t>(nullptr, nullptr, "1");
    } // namespace consts

    template<class T>
    inline void hash_combine(std::size_t& seed, const T& v) {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    struct bdd_ptr_hash_t {
        std::size_t operator()(const bdd_ptr& bdd_p) const {
            std::size_t seed = 0;

            hash_combine(seed, bdd_p->id);
            hash_combine(seed, bdd_p->low);
            hash_combine(seed, bdd_p->high);

            return seed;
        }
    }; // struct bdd_ptr_hash_t

    struct bdd_ptr_equal_t {
        bool operator()(const bdd_ptr& lhs, const bdd_ptr& rhs) const {
            return lhs->id == rhs->id
                && lhs->low == rhs->low
                && lhs->high == rhs->high;
        }
    }; // struct bdd_ptr_equal_t

    bdd_ptr find_or_create_bdd(const bdd_ptr& low,
                               const bdd_ptr& high,
                               const std::string& id) {
        static auto bdd_cache = std::unordered_set<
                                    bdd_ptr,
                                    bdd_ptr_hash_t,
                                    bdd_ptr_equal_t
                                >{consts::zero, consts::one};

        if (low == high) {
            return low;
        }

        auto node = std::make_shared<bdd_t>(low, high, id);
        auto it = bdd_cache.find(node);

        if (it != bdd_cache.end()) {
            return *it;
        } else {
            bdd_cache.emplace(node);
            return node;
        }
    }

    ///
    // If-then-else type
    ///
    struct if_then_else_t {
        const bdd_ptr if_;
        const bdd_ptr then_;
        const bdd_ptr else_;
    }; // struct if_then_else_t

    struct if_then_else_hash_t {
        std::size_t operator()(const if_then_else_t& ite) const {
            std::size_t seed = 0;

            hash_combine(seed, ite.if_);
            hash_combine(seed, ite.then_);
            hash_combine(seed, ite.else_);

            return seed;
        }
    }; // struct if_then_else_hash_t

    struct if_then_else_equal_t {
        bool operator()(const if_then_else_t& lhs,
                        const if_then_else_t& rhs) const {
            return lhs.if_ == rhs.if_
                && lhs.then_ == rhs.then_
                && lhs.else_ == rhs.else_;
        }
    }; // struct if_then_else_equal_t

    std::string smallest_id(const bdd_ptr& x,
                            const bdd_ptr& y,
                            const bdd_ptr& z) {
        return std::min({x, y, z},
                        [](const bdd_ptr& lhs, const bdd_ptr& rhs) {
                            return lhs != consts::zero
                                    && lhs != consts::one
                                    && lhs->id < rhs->id;
                        })->id;
    }

    bdd_ptr restrict_bdd(const bdd_ptr& bdd_p,
                         const std::string& id,
                         bool truth) {
        if (bdd_p->id > id || bdd_p == consts::zero || bdd_p == consts::one) {
            return bdd_p;
        }

        BDD_ASSERT(bdd_p->id == id);
        return truth ? bdd_p->high : bdd_p->low;
    }

    bdd_ptr if_then_else(const bdd_ptr& if_,
                         const bdd_ptr& then_,
                         const bdd_ptr& else_) {
        // ITE cache
        static auto ite_cache = std::unordered_map<
                                    if_then_else_t,
                                    bdd_ptr,
                                    if_then_else_hash_t,
                                    if_then_else_equal_t
                                >{};

        // Check base cases
        if (if_ == consts::one) { return then_; }
        if (if_ == consts::zero) { return else_; }
        if (then_ == consts::one && else_ == consts::zero) { return if_; }
        if (then_ == else_) { return then_; }

        // Check cache hit
        auto ite_node = if_then_else_t{if_, then_, else_};
        auto it = ite_cache.find(ite_node);

        if (it != ite_cache.end()) {
            return it->second;
        }

        // Cache miss
        auto id = smallest_id(if_, then_, else_);
        auto false_restricted = if_then_else(restrict_bdd(if_, id, false),
                                             restrict_bdd(then_, id, false),
                                             restrict_bdd(else_, id, false));
        auto true_restricted = if_then_else(restrict_bdd(if_, id, true),
                                            restrict_bdd(then_, id, true),
                                            restrict_bdd(else_, id, true));
        auto result = find_or_create_bdd(false_restricted, true_restricted, id);

        ite_cache.emplace(ite_node, result);
        return result;
    }
} // namespace detail

// Public BDD functions
bdd_ptr make_leaf(const std::string& id) {
    return detail::find_or_create_bdd(detail::consts::zero,
                                      detail::consts::one,
                                      id);
}

bdd_ptr bdd_and(const bdd_ptr& lhs, const bdd_ptr& rhs) {
    return detail::if_then_else(lhs, rhs, detail::consts::zero);
}

bdd_ptr bdd_or(const bdd_ptr& lhs, const bdd_ptr& rhs) {
    return detail::if_then_else(lhs, detail::consts::one, rhs);
}

bdd_ptr bdd_not(const bdd_ptr& lhs) {
    return detail::if_then_else(lhs, detail::consts::zero, detail::consts::one);
}

} // namespace bdd


namespace consts {
    static const bdd::bdd_ptr open_paren
        = std::make_shared<bdd::bdd_t>(nullptr, nullptr, "(");
    static const bdd::bdd_ptr close_paren
        = std::make_shared<bdd::bdd_t>(nullptr, nullptr, ")");
    static const bdd::bdd_ptr op_and
        = std::make_shared<bdd::bdd_t>(nullptr, nullptr, "^");
    static const bdd::bdd_ptr op_or
        = std::make_shared<bdd::bdd_t>(nullptr, nullptr, "V");
    static const bdd::bdd_ptr op_not
        = std::make_shared<bdd::bdd_t>(nullptr, nullptr, "~");
} // namespace consts

void consume(std::stack<bdd::bdd_ptr>& st) {
    BDD_ASSERT(!st.empty());

    while (1) {
        auto lhs = st.top();
        st.pop();

        auto op = st.empty() ? static_cast<bdd::bdd_ptr>(nullptr) : st.top();
        if (!op || (op != consts::op_not && op != consts::op_or
                    && op != consts::op_and)) {
            st.emplace(lhs);
            break;
        }

        if (op == consts::op_not) {
            st.pop();
            st.emplace(bdd::bdd_not(lhs));
        } else if (op == consts::op_or) {
            st.pop();
            auto rhs = st.top();
            st.pop();
            st.emplace(bdd::bdd_or(rhs, lhs));
        } else {
            st.pop();
            auto rhs = st.top();
            st.pop();
            st.emplace(bdd::bdd_and(rhs, lhs));
        }
    }
}

bdd::bdd_ptr build_bdd(const std::string& s) {
    auto st = std::stack<bdd::bdd_ptr>{};
    auto is_valid_id = [](char c) {
        return c == 'x'
                || c == 'X'
                || std::isdigit(c);
    };
    auto is_valid_alphabet = [&is_valid_id](char c) {
        return is_valid_id(c)
                || std::isspace(c)
                || c == '('
                || c == ')'
                || c == '~'
                || c == 'V'
                || c == 'v'
                || c == '^';
    };

    for (auto i = 0u; i < s.size(); ++i) {
        if (!is_valid_alphabet(s[i])) {
            return nullptr;
        } else if (std::isspace(s[i])) {
            continue;
        } else if (s[i] == '(') {
            st.emplace(consts::open_paren);
        } else if (s[i] == ')') {
            auto lhs_bdd = st.top();
            st.pop();
            st.pop();
            st.emplace(lhs_bdd);
            consume(st);
        } else if (s[i] == '~') {
            st.emplace(consts::op_not);
        } else if (s[i] == 'V' || s[i] == 'v') {
            st.emplace(consts::op_or);
        } else if (s[i] == '^') {
            st.emplace(consts::op_and);
        } else {
            std::ostringstream oss;

            oss << s[i];
            for (; i < s.size() - 1 && is_valid_id(s[i + 1]); ++i) {
                oss << s[i + 1];
            }
            st.emplace(bdd::make_leaf(oss.str()));
            consume(st);
        }
    }

    return st.top();
}

bool are_equivalent(const std::string& e1, const std::string& e2) {
    auto lhs = build_bdd(e1);
    auto rhs = build_bdd(e2);

    return lhs && rhs && lhs == rhs;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage:\n";
        std::cerr << "\t./robdd <file_expr1> <file_expr2>" << std::endl;;
        return 1;
    }

    std::ifstream fin_e1{argv[1]};
    std::ifstream fin_e2{argv[2]};

    if (!fin_e1 || !fin_e2) {
        std::cerr << "[Error] Cannot open one of the input files.  Aborting..."
                  << std::endl;
        return 1;
    }

    std::string e1;
    std::getline(fin_e1, e1);

    std::string e2;
    std::getline(fin_e2, e2);

    std::cout << (are_equivalent(e1, e2) ? "1" : "0") << std::endl;
}
