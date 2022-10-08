#pragma once
#ifndef CATA_SRC_DEPENDENCY_TREE_H
#define CATA_SRC_DEPENDENCY_TREE_H

#include <map>
#include <stack>
#include <vector>
#include <string>

#include "type_id.h"

enum class node_error_type : int {
    missing_dep = 0,
    conflicting_deps,
    cyclic_dep,
    num
};

class dependency_node
{
    public:
        std::vector<dependency_node *> parents;
        std::vector<dependency_node *> children;
        std::vector<const dependency_node *> conflicts;
        std::map<node_error_type, std::vector<std::string> > all_errors;
        mod_id key;
        bool availability;

        // cyclic check variables
        int index;
        int lowlink;
        bool on_stack;

        dependency_node();
        dependency_node( mod_id _key );

        void add_parent( dependency_node *parent );
        void add_child( dependency_node *child );
        void add_conflict( const dependency_node *conflict );

        auto is_available() -> bool;
        auto has_errors() -> bool;
        auto errors() -> std::map<node_error_type, std::vector<std::string > >;
        auto s_errors() -> std::string;

        // Tree traversal
        // Upward towards head(s)
        auto get_dependencies_as_strings() -> std::vector<mod_id>;
        auto get_dependencies_as_nodes() -> std::vector<dependency_node * >;
        // Downward towards leaf(ves)
        auto get_dependents_as_strings() -> std::vector<mod_id>;
        auto get_dependents_as_nodes() -> std::vector< dependency_node * >;

        void inherit_errors();
};

class dependency_tree
{
    public:
        dependency_tree();

        void init(
            std::map<mod_id, std::vector<mod_id> > key_dependency_map,
            std::map<mod_id, std::vector<mod_id> > key_conflict_map
        );

        void clear();

        // tree traversal
        // Upward by key
        auto get_dependencies_of_X_as_strings( mod_id key ) -> std::vector<mod_id >;
        auto get_dependencies_of_X_as_nodes( mod_id key ) -> std::vector<dependency_node * >;
        // Downward by key
        auto get_dependents_of_X_as_strings( mod_id key ) -> std::vector< mod_id >;
        auto get_dependents_of_X_as_nodes( mod_id key ) -> std::vector< dependency_node * >;

        auto is_available( mod_id key ) -> bool;
        auto get_node( mod_id key ) -> dependency_node *;

        std::map<mod_id, dependency_node, mod_id::LexCmp> master_node_map;
    private:
        // Don't need to be called directly. Only reason to call these are during initialization phase.
        void build_node_map( std::map<mod_id, std::vector<mod_id > > key_dependency_map );
        void build_connections(
            std::map<mod_id, std::vector<mod_id > > key_dependency_map,
            std::map<mod_id, std::vector<mod_id > > key_conflict_map
        );

        /*
        Cyclic Dependency checks using Tarjan's Strongly Connected Components algorithm
        Order is O(N+E) and linearly increases with number of nodes and number of connections between them.
        References:
            http://en.wikipedia.org/wiki/Tarjan's_strongly_connected_components_algorithm
            http://www.cosc.canterbury.ac.nz/tad.takaoka/alg/graphalg/sc.txt
        */
        void check_for_strongly_connected_components();
        void strong_connect( dependency_node *dnode );

        std::vector<std::vector<dependency_node * > > strongly_connected_components;
        std::stack<dependency_node *> connection_stack;
        std::vector<dependency_node *> nodes_on_stack;
        int open_index;

        void check_for_conflicting_dependencies();
};

#endif // CATA_SRC_DEPENDENCY_TREE_H
