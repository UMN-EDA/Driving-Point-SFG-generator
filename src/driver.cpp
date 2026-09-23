// CLI and export adapter. The included legacy algorithms are kept byte-for-byte.
#include <cmath>
#include <stdexcept>
#include <iomanip>
#define main legacy_main
#if SFG_ENGINE == 1
#include "legacy/readCkt3.cpp"
#define GRAPH SFG
#elif SFG_ENGINE == 2
#include "legacy/readCkt3_SC_1.7.cpp"
#define GRAPH SFGFINAL
#elif SFG_ENGINE == 3
#include "legacy/readCkt3_SC_1.8_NEW.cpp"
#define GRAPH SFGFINAL
#else
#error Define SFG_ENGINE as 1 (ct), 2 (sc), or 3 (latest).
#endif
#undef main

void export_graph()
{
    std::ofstream edges("graph.tsv"), nodes("nodes.tsv");
    edges << "source\tweight\tdestination\n";
    nodes << "node\tmagic\n";
    for (auto &entry : GRAPH) {
        nodes << entry.first << '\t' << entry.second.getFlag() << '\n';
        for (const auto &edge : entry.second.return_output())
            edges << entry.first << '\t' << edge.edge << '\t' << edge.node << '\n';
    }
}

template <typename Paths>
void export_walks(const Paths &paths, const std::string &prefix, bool closed)
{
    std::ofstream nodes(prefix + "_nodes.tsv"), edges(prefix + "_edges.tsv"),
                  walks(prefix + ".tsv");
    for (const auto &path : paths) {
        if (path.empty()) continue;
        for (size_t n = 0; n < path.size(); ++n) {
            nodes << (n ? "\t" : "") << path[n];
            walks << path[n];
            if (n + 1 < path.size() || closed) {
                auto next = path[(n + 1) % path.size()];
                // Exports must not call returnSpecialEdge(): it mutates legacy taken flags.
                std::vector<std::string> candidates;
                for (const auto &edge : GRAPH.at(path[n]).return_output())
                    if (edge.node == next) candidates.push_back(edge.edge);
                std::string weight;
                if (candidates.size() == 1) weight = candidates.front();
                else {
                    weight = "alternatives[";
                    for (size_t k = 0; k < candidates.size(); ++k)
                        weight += (k ? " | " : "") + candidates[k];
                    weight += "]";
                }
                edges << (n ? "\t" : "") << weight;
                walks << '\t' << weight << '\t';
            }
        }
        if (closed) walks << path.front();
        nodes << '\n'; edges << '\n'; walks << '\n';
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: sfg-engine INPUT {graph|paths|loops|analysis|transfer}\n";
        return 2;
    }
    const std::string filename = argv[1], mode = argv[2];
    if (mode != "graph" && mode != "paths" && mode != "loops" &&
        mode != "analysis" && mode != "transfer") return 2;
    try {
        ckt_parse_1(filename);
#if SFG_ENGINE == 1
        add_magic_source();
        ckt_parse_2(filename);
        display_circuit();
        display_graph();
#else
        subcircuit(filename);
#if SFG_ENGINE == 3
        connectedTransistors();
#endif
        intra_branches(filename);
#if SFG_ENGINE == 3
        display_all_trans();
#endif
        update_VI_VO();
        inter_branches_2();
        update_input_output_node();
#if SFG_ENGINE == 3
        displayTransNodes();
        update_transbranches();
        modify_multi_edged_node_pair();
#endif
        display_final_graph();
#endif
        export_graph();
        const bool paths = mode == "paths" || mode == "analysis" || mode == "transfer";
        const bool loops = mode == "loops" || mode == "analysis" || mode == "transfer";
        if (paths) find_paths();
        if (loops) findCycles();
        if (paths) {
            displayForwardPaths();
            export_walks(All_Path, "paths", false);
            std::ofstream gains("path_gains.txt");
            for (const auto &gain : ForwardGains) gains << gain << '\n';
        }
        if (loops) {
            displayLoops();
            export_walks(FinalLoops, "loops", true);
            std::ofstream gains("loop_gains.txt");
            for (const auto &gain : LoopGains) gains << gain << '\n';
        }
        if (mode == "transfer") {
            std::cout << std::flush;
            findDELTA();
            findNumerator(); // Invoke once; legacy 1.7 main invoked it twice.
            std::ofstream result("transfer_function.txt");
            result << "Symbolic output from the original C++ transfer-function routines.\n"
                   << "NUMERATOR\n" << num << "\nDENOMINATOR\n" << Delta << '\n';
        }
        std::cout << "\nSFG adapter completed.\n";
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "Generator error: " << error.what() << '\n';
        return 1;
    }
}
