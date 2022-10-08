#pragma once
#ifndef CATA_SRC_BEHAVIOR_STRATEGY_H
#define CATA_SRC_BEHAVIOR_STRATEGY_H

#include <unordered_map>
#include <vector>
#include <string>

namespace behavior
{

class node_t;
class oracle_t;

enum status_t : char;
struct behavior_return;

class strategy_t
{
    public:
        virtual ~strategy_t() = default;
        virtual auto evaluate( const oracle_t *subject,
                                          std::vector<const node_t *> children ) const -> behavior_return = 0;
};

class sequential_t : public strategy_t
{
        auto evaluate( const oracle_t *subject,
                                  std::vector<const node_t *> children ) const -> behavior_return override;
};

class fallback_t : public strategy_t
{
        auto evaluate( const oracle_t *subject,
                                  std::vector<const node_t *> children ) const -> behavior_return override;
};

class sequential_until_done_t : public strategy_t
{
        auto evaluate( const oracle_t *subject,
                                  std::vector<const node_t *> children ) const -> behavior_return override;
};

extern std::unordered_map<std::string, const strategy_t *> strategy_map;

} // namespace behavior

#endif // CATA_SRC_BEHAVIOR_STRATEGY_H
