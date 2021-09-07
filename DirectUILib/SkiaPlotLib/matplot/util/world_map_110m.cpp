//
// Created by Alan Freitas on 20/07/20.
//

#include <limits>
#include <matplot/util/geodata.h>
// http://www.gnuplotting.org/plotting-the-world-revisited/
namespace matplot {
    // (-?\d+\.\d+) +(-?\d+\.\d+) *\n
    // $1,\n
    // $2,\n
    // \n\n
    // \nstd::numeric_limits<double>::quiet_NaN(),\n
	std::pair<std::vector<double>, std::vector<double>> prepare_world_map_110m() {
		static constexpr double x[] = {0};
		static constexpr double y[] = {0};
		return std::make_pair(std::vector<double>(std::begin(x), std::end(x)), std::vector<double>(std::begin(y), std::end(y)));
	}

    std::pair<std::vector<double>, std::vector<double>>& world_map_110m() {
        static std::pair<std::vector<double>, std::vector<double>> world_map_110m_ = prepare_world_map_110m();
        return world_map_110m_;
    }
}
