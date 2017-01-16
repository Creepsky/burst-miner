//  cryptoport.io Burst Pool Miner
//
//  Created by Uray Meiviar < uraymeiviar@gmail.com > 2014
//  donation :
//
//  [Burst  ] BURST-8E8K-WQ2F-ZDZ5-FQWHX
//  [Bitcoin] 1UrayjqRjSJjuouhJnkczy5AuMqJGRK4b

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <array>

namespace Burst
{
	template <typename T, size_t SZ>
	std::string byteArrayToStr(const std::array<T, SZ>& arr)
	{
		std::stringstream stream;
		for (size_t i = 0; i < SZ; i++)
		{
			stream << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << static_cast<size_t>(arr[i]);
		}
		return stream.str();
	}

	bool isNumberStr(const std::string& str);
	std::string getFileNameFromPath(const std::string& strPath);
	std::vector<std::string>& splitStr(const std::string& s, char delim, std::vector<std::string>& elems);
	std::vector<std::string> splitStr(const std::string& s, char delim);
	std::vector<std::string> splitStr(const std::string& s, const std::string& delim);
	bool isValidPlotFile(const std::string& filePath);
	std::string getAccountIdFromPlotFile(const std::string& path);
	std::string getStartNonceFromPlotFile(const std::string& path);
	std::string getNonceCountFromPlotFile(const std::string& path);
	std::string getStaggerSizeFromPlotFile(const std::string& path);
	std::string deadlineFormat(uint64_t seconds);
	uint64_t formatDeadline(const std::string& format);
	std::string gbToString(uint64_t size);
	std::string versionToString();
	std::string getInformationFromPlotFile(const std::string& path, uint8_t index);

	template <typename T, typename U>
	void transferSocket(T& from, U& to)
	{
		auto socket = from.close();

		if (socket != nullptr)
			to = std::move(socket);
	}
}
