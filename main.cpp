#include <iostream>
#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std::literals;

void PrintPercent(std::ostream& out, double percent) {
	auto default_precision = out.precision(6);
	out << percent << std::endl;
	out.precision(default_precision);
}

class EBookStats {
public:
	using UserId = std::size_t;
	using Page = std::size_t;

	EBookStats() : pages_stats_(1000 + 1, 0) {}

	void SetPagesRead(UserId user_id, Page pages_read) {
		if (user_id >= users_stats_.size()) {
			users_stats_.resize(user_id + 1);
		}

		for (std::size_t i = users_stats_[user_id] + 1; i <= pages_read; ++i) {
			pages_stats_[i] += 1;
		}

		if (users_stats_[user_id] == 0) {
			++users_count_;
		}
		users_stats_[user_id] = pages_read;
	}

	double GetUserStat(UserId user_id) {
		const bool is_user_exists = (user_id < users_stats_.size()) &&
			users_stats_[user_id] > 0;

		if (!is_user_exists) {
			return 0.;
		} else if (users_count_ == 1) {
			return 1.;
		}

		const auto pages_read = users_stats_[user_id];
		// percent of other users who DO read up to page *pages_read*
		const double percent = (static_cast<double>(pages_stats_[pages_read] - 1) /
				(users_count_ - 1));

		// percent of other users who DON'T read up to page *pages_read*
		return 1. - percent;
	}
private:
	std::size_t users_count_ = 0;

	// map page_num -> users count (who read up to page_num)
	std::vector<std::size_t> pages_stats_;
	// map user_id -> pages read
	std::vector<Page> users_stats_;
};

std::size_t GetRequestCount(std::istream& input) {
	std::string line;
	std::getline(input, line);
	std::istringstream line_stream(line);

	std::size_t request_count{};
	line_stream >> request_count;

	return request_count;
}

namespace Token {
	const std::string_view READ = "READ"sv;
	const std::string_view CHEER = "CHEER"sv;
}

void ProcessRequests(EBookStats& stats, std::size_t request_count, std::istream& input, std::ostream& output) {
	std::string line;
	for (std::size_t i = 0; i < request_count; ++i) {
		if (!std::getline(input, line)) {
			break;
		}

		std::istringstream line_stream(line);
		std::string command_str;
		line_stream >> command_str;

		if (command_str == Token::READ) {
			EBookStats::UserId user;
			EBookStats::Page page;

			line_stream >> user >> page;

			stats.SetPagesRead(user, page);

			continue;
		}

		if (command_str == Token::CHEER) {
			EBookStats::UserId user;

			line_stream >> user;

			PrintPercent(output, stats.GetUserStat(user));

			continue;
		}
	}
}

void Test() {
	std::istringstream input{
		"12\n"
		"CHEER 5\n"
		"READ 1 10\n"
		"CHEER 1\n"
		"READ 2 5\n"
		"READ 3 7\n"
		"CHEER 2\n"
		"CHEER 3\n"
		"READ 3 10\n"
		"CHEER 3\n"
		"READ 3 11\n"
		"CHEER 3\n"
		"CHEER 1\n"
	};

	std::ostringstream output;

	EBookStats stats;
	ProcessRequests(stats, GetRequestCount(input), input, output);

	assert(output.str() == "0\n1\n0\n0.5\n0.5\n1\n0.5\n"s);
}

int main() {
	//EBookStats stats;
	//ProcessRequests(stats, GetRequestCount(std::cin), std::cin, std::cout);
	Test();

	return EXIT_SUCCESS;
}
