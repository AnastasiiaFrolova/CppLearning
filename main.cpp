#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
std::vector<std::string> split(const std::string& str, char d)
{
	std::vector<std::string> r;

	std::string::size_type start = 0;
	std::string::size_type stop = str.find_first_of(d);
	while (stop != std::string::npos)
	{
		r.push_back(str.substr(start, stop - start));

		start = stop + 1;
		stop = str.find_first_of(d, start);
	}

	r.push_back(str.substr(start));

	return r;
}

void reverseSort(std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>>& data) {
	for (size_t idx_i = 0; idx_i + 1 < data.size(); ++idx_i) {
		for (size_t idx_j = 0; idx_j + 1 < data.size() - idx_i; ++idx_j) {
			if (std::get<0>(data[idx_j + 1]) > std::get<0>(data[idx_j])) {
				swap(data[idx_j], data[idx_j + 1]);
			}
			if (std::get<0>(data[idx_j + 1]) == std::get<0>(data[idx_j]) &&
				(std::get<1>(data[idx_j + 1]) > std::get<1>(data[idx_j]))) {
				swap(data[idx_j], data[idx_j + 1]);
			}
			if (std::get<0>(data[idx_j + 1]) == std::get<0>(data[idx_j]) &&
				std::get<1>(data[idx_j + 1]) == std::get<1>(data[idx_j]) &&
				std::get<2>(data[idx_j + 1]) > std::get<2>(data[idx_j])) {
				swap(data[idx_j], data[idx_j + 1]);
			}
			if (std::get<0>(data[idx_j + 1]) == std::get<0>(data[idx_j]) &&
				std::get<1>(data[idx_j + 1]) == std::get<1>(data[idx_j]) &&
				std::get<2>(data[idx_j + 1]) == std::get<2>(data[idx_j]) &&
				std::get<3>(data[idx_j + 1]) > std::get<3>(data[idx_j])) {
				swap(data[idx_j], data[idx_j + 1]);
			}
		}
	}
}

int main(int argc, char const* argv[])
{
	try
	{
		std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>> ip_pool;
		auto parseStr = [](const std::string& str, char d)
		{ std::vector<std::string> v = split(str, d); return std::make_tuple(v); };

		for (std::string line; std::getline(std::cin, line);)
		{
			if (line.empty()) break;

			auto ip = std::get<0>(parseStr(line, '\t'))[0];
			auto ipParts = std::get<0>(parseStr(ip, '.'));
			ip_pool.push_back(std::make_tuple(stoi(ipParts[0]), stoi(ipParts[1]), stoi(ipParts[2]), stoi(ipParts[3])));
		}
		reverseSort(ip_pool);


		// TODO reverse lexicographically sort

		for (std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>>::const_iterator ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
		{
			std::cout << std::to_string(std::get<0>(*ip)) << "." << std::to_string(std::get<1>(*ip)) << "." << std::to_string(std::get<2>(*ip)) << "." << std::to_string(std::get<3>(*ip)) << std::endl;
		}
		std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>> ip_pool_filtered_1; // 1 байт = 1
		std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>> ip_pool_filtered_2; // 1 байт = 46, 2 байт = 70
		std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>> ip_pool_filtered_3; // 1 байт = 46, 2 байт = 70

		std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(ip_pool_filtered_1), [](std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> i) {
			return std::get<0>(i) == 1;
		});
		std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(ip_pool_filtered_2), [](std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> i) {
			return std::get<0>(i) == 46 && std::get<1>(i) == 70;
		});
		std::copy_if(ip_pool.begin(), ip_pool.end(), std::back_inserter(ip_pool_filtered_3), [](std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> i) {
			return std::get<0>(i) == 46 || std::get<1>(i) == 46 || std::get<2>(i) == 46 || std::get<3>(i) == 46;
		});

		for (std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>>::const_iterator ip = ip_pool_filtered_1.cbegin(); ip != ip_pool_filtered_1.cend(); ++ip)
		{
			std::cout << std::to_string(std::get<0>(*ip)) << "." << std::to_string(std::get<1>(*ip)) << "." << std::to_string(std::get<2>(*ip)) << "." << std::to_string(std::get<3>(*ip)) << std::endl;
		}

		for (std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>>::const_iterator ip = ip_pool_filtered_2.cbegin(); ip != ip_pool_filtered_2.cend(); ++ip)
		{
			std::cout << std::to_string(std::get<0>(*ip)) << "." << std::to_string(std::get<1>(*ip)) << "." << std::to_string(std::get<2>(*ip)) << "." << std::to_string(std::get<3>(*ip)) << std::endl;
		}

		for (std::vector<std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>>::const_iterator ip = ip_pool_filtered_3.cbegin(); ip != ip_pool_filtered_3.cend(); ++ip)
		{
			std::cout << std::to_string(std::get<0>(*ip)) << "." << std::to_string(std::get<1>(*ip)) << "." << std::to_string(std::get<2>(*ip)) << "." << std::to_string(std::get<3>(*ip)) << std::endl;
		}
		// 222.173.235.246
		// 222.130.177.64
		// 222.82.198.61
		// ...
		// 1.70.44.170
		// 1.29.168.152
		// 1.1.234.8

		// TODO filter by first byte and output
		// ip = filter(1)

		// 1.231.69.33
		// 1.87.203.225
		// 1.70.44.170
		// 1.29.168.152
		// 1.1.234.8

		// TODO filter by first and second bytes and output
		// ip = filter(46, 70)

		// 46.70.225.39
		// 46.70.147.26
		// 46.70.113.73
		// 46.70.29.76

		// TODO filter by any byte and output
		// ip = filter_any(46)

		// 186.204.34.46
		// 186.46.222.194
		// 185.46.87.231
		// 185.46.86.132
		// 185.46.86.131
		// 185.46.86.131
		// 185.46.86.22
		// 185.46.85.204
		// 185.46.85.78
		// 68.46.218.208
		// 46.251.197.23
		// 46.223.254.56
		// 46.223.254.56
		// 46.182.19.219
		// 46.161.63.66
		// 46.161.61.51
		// 46.161.60.92
		// 46.161.60.35
		// 46.161.58.202
		// 46.161.56.241
		// 46.161.56.203
		// 46.161.56.174
		// 46.161.56.106
		// 46.161.56.106
		// 46.101.163.119
		// 46.101.127.145
		// 46.70.225.39
		// 46.70.147.26
		// 46.70.113.73
		// 46.70.29.76
		// 46.55.46.98
		// 46.49.43.85
		// 39.46.86.85
		// 5.189.203.46
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
